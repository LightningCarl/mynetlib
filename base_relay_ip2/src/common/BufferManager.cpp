#include <strings.h>
#include <cerrno>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "BufferManager.h"
#include "log.h"
using namespace std;

BufferManager::BufferManager()
{
    m_pReader = NULL;
    m_pWriter = NULL;
    m_pReadCallback = NULL;

    m_nReadOffset = 0;
    m_nHeaderSize = 0;
    m_nContentLength = 0;
    m_bReadHeader = true;
    m_bNewPackage = true;

    bzero((char*)&m_inReq, sizeof(m_inReq));
}

BufferManager::~BufferManager()
{
    try
    {
        releaseSendBuffer();

        if ( m_pReader != NULL )
        {
            delete m_pReader;
            m_pReader = NULL;
        }
        
        if ( m_pWriter != NULL )
        {
            delete m_pWriter;
            m_pWriter = NULL;
        }
        
        if ( m_pReadCallback != NULL )
        {
            delete m_pReadCallback;
            m_pReadCallback = NULL;
        }
        
        if ( m_inReq.ioBuf != NULL )
        {
            delete []m_inReq.ioBuf;
            m_inReq.ioBuf = NULL;
        }
    }
    catch ( ... )
    {
    }
}

void
BufferManager::releaseSendBuffer()
{
    for ( list<Iov>::iterator it = m_SendIovList.begin(); it != m_SendIovList.end();)
    {
        list<Iov>::iterator tempit = it;
        ++it;
        if ( tempit->m_pCompleteBuffer != NULL)
        {
            delete [] tempit->m_pCompleteBuffer;
            tempit->m_pCompleteBuffer = NULL;
        }

        m_SendIovList.erase( tempit );
    }
}

int
BufferManager::writeToSocket()
{
    if ( NULL == m_pWriter )  //tcpwrite 的指针
    {
        printf( "In BufferManager::write, m_pWriter == NULL" );
        return -1;
    }

    int rt = m_pWriter->doWrite( &m_SendIovList );  //调用tcpwrite

    if ( rt < 0 )
    {
        printf("In BufferManager::write, m_pWriter->doWrite error" );
        return -1;
    }

    list<Iov> cbIovList;  //回收队列

    for ( list<Iov>::iterator it = m_SendIovList.begin();
            it != m_SendIovList.end(); )
    {
        if ( 0 == it->m_Iovec.iov_len )
        {
            cbIovList.push_back( *it );  //回收
            m_SendIovList.erase( it++ );
        }
        else
        {
            break;
        }
    }

    for ( list<Iov>::iterator it = cbIovList.begin();
            it != cbIovList.end(); )
    {
        cbIovList.erase( it++ );
    }

    return 0;
}

void
BufferManager::ToSendList(char* pBuf, ssize_t len)
{
    m_SendIovList.push_back( Iov( pBuf, len) );
}

int
BufferManager::readTCP()
{
    if ( NULL == m_pReader )
    {
        printf( "In BufferManager::readTCP, m_pReader == NULL" );
        return -1;
    }

    if ( m_bNewPackage )
    {
        m_bNewPackage = false;
        m_bReadHeader = true;
        
        m_nReadOffset = 0;
        m_nContentLength = 0;
        
        m_nHeaderSize = HEADER_SIZE;
        m_inReq.ioBuf = NULL;
        memset( &m_inReq.m_msgHeader, 0 , m_nHeaderSize );
    }

    int rt = 0;

    if ( m_bReadHeader )
    {
        rt =  readTCPHeader();

        if ( rt < 0 )
        {
            return rt;
        }
    }

    if ( !m_bNewPackage && !m_bReadHeader )
    {
        rt = readTCPContent();

        if ( rt < 0 )
        {
            return rt;
        }
    }
    return 0;
}

int
BufferManager::readTCPHeader()
{
    int rt = m_pReader->doRead(
             ( ( char* )( &( m_inReq.m_msgHeader ) ) ) + m_nReadOffset,m_nHeaderSize - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) ) //让客户端重连
    {
        return 0;
    }
    else if ( rt < 0 )  //断开连接
    {
        // error
        printf("In BufferManager::readTCPHeader, m_pReader->doRead error" );
        return -1;
    }
    else if ( 0 == rt )  //关闭连接
    {
        // connection closed by opposition
        printf("In BufferManager::readTCPHeader, connection closed by opposition" );
        return -1;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nReadOffset == m_nHeaderSize )
    {
        m_nReadOffset = 0;
        m_bReadHeader = false;
        m_nContentLength = m_inReq.m_msgHeader.length;
        
        if ( 0 == m_nContentLength )
        {
            m_bNewPackage = true;

            if ( NULL == m_pReadCallback )
            {
                printf("In BufferManager::readTCPHeader, m_pReadCallback == NULL" );
                return 0;
            }

            // callback
            rt = m_pReadCallback->run( m_inReq );

            if ( rt < 0 )
            {
                printf("In BufferManager::readTCPHeader, m_pReadCallback->run error" );
                return -1;
            }
        }
        else if ( m_nContentLength > 0 )
        {
            m_inReq.ioBuf = new char[m_nContentLength];
            memset( m_inReq.ioBuf, 0 , m_nContentLength );
        }
    }
    return 0;
}

int
BufferManager::readTCPContent()
{
    int rt = m_pReader->doRead( m_inReq.ioBuf + m_nReadOffset,
                            m_nContentLength - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) )
    {
        return 0;
    }
    else if ( rt < 0 )
    {
        // error
        printf("In BufferManager::readTCPContent, m_pReader->doRead error" );
        return -1;

    }
    else if ( 0 == rt )
    {
        // connection closed by opposition
        printf("In BufferManager::readTCPContent, connection closed by opposition" );
        return -1;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nContentLength == m_nReadOffset )  //消息读完整后，调用回调函数，将数据写入发送队列
    {
        m_nReadOffset = 0;
        m_bNewPackage = true;

        // callback
        if ( NULL == m_pReadCallback )
        {
            printf("In BufferManager::readTCPContent, m_pReadCallback == NULL" );
            if( m_inReq.ioBuf != NULL )
            {
                delete [] m_inReq.ioBuf;
                m_inReq.ioBuf = NULL;
            }
            return 0;
        }

        rt = m_pReadCallback->run( m_inReq );
        if( m_inReq.ioBuf != NULL )
        {
            delete [] m_inReq.ioBuf;
            m_inReq.ioBuf = NULL;
        }
        if ( rt < 0 )
        {
            printf("In BufferManager::readContent, m_pReadCallback->run error" );
            return -1;
        }
    }
    return 0;
}

void 
BufferManager::setReader( TCPReader* pReader )
{
    if ( NULL != m_pReader )
    {
        delete m_pReader;
    }

    m_pReader = pReader;
}

void BufferManager::setWriter( TCPWriter* pWriter )
{
    if ( NULL != m_pWriter )
    {
        delete m_pWriter;
    }

    m_pWriter = pWriter;
}
    
void BufferManager::setReadCallback( ReadCallback* pReadCallback )
{
    if ( NULL != m_pReadCallback )
    {
        delete m_pReadCallback;
    }
    m_pReadCallback = pReadCallback;
}

bool
BufferManager::serialPackage(Info* fo)
{
    int filefd = open("./recvPackage.txt", O_WRONLY,  O_APPEND);
    fo->m_fileLen = writen(filefd);   //将发送队列中的数据序列化
    fo->m_bufLen = m_nContentLength; 
    fo->m_offset = ( Log::getInstance() )->getOffset();
    Log::getInstance()->setOffset(fo->m_fileLen); 
    close(filefd);
    return true;
}

ssize_t 
BufferManager:: writen(int fd)
{
    int i = 0;
    ssize_t len = 0;
    for ( list<Iov>::iterator it = m_SendIovList.begin(); it != m_SendIovList.end(); ++it, ++i )
    {
        ssize_t write_nums = 0;
        size_t left_nums = it->m_Iovec.iov_len;
        len = left_nums;
        char *ptr = (char*)it->m_Iovec.iov_base;

        while (left_nums > 0)
        {
            if ( (write_nums = write(fd, ptr, left_nums)) <= 0)
            {
                if( write_nums < 0 && (errno = EINTR))
                {
                    write_nums = 0;  //可重新调用
                }
                else
                {
                    return -1;  //error
                }
            }
            left_nums -= write_nums;
            ptr += write_nums;
        }
    }
    return len;
}

bool 
BufferManager::changeRever(const TCPSocket &sock)
{
    m_pWriter->changeRecver(sock);
    return true;
}