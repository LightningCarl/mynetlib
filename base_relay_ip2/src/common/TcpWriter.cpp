#include <list>
#include <stdint.h>
#include <limits.h>
#include "TcpWriter.h"
#include "BufferManager.h"
using namespace std;

int
TCPWriter::doWrite( list<Iov>* pSendIovList )
{
    int iovcnt = (int)pSendIovList->size(); //获取发送列表长度
    if ( iovcnt > IOV_MAX )
    {
        iovcnt = IOV_MAX;
    }

    struct iovec* pIovec = new struct iovec[iovcnt];  //报文缓冲区指针及长度的数据结构 数组
    memset( pIovec, 0, sizeof( struct iovec )*iovcnt );

    int i = 0;
    for ( list<Iov>::iterator it = pSendIovList->begin();
            it != pSendIovList->end(); ++it, ++i )
    {
        if ( i < iovcnt )
        {
            pIovec[i] = it->m_Iovec;   //从发送队列获取指针和长度
        }
        else
        {
            break;
        }
    }

    ssize_t rt = doWriteInternal( pIovec, iovcnt );  //将数组中所有缓冲区的数据写入套接字

    delete [] pIovec;  //释放临时资源
    pIovec = NULL;

    if ( rt < 0 )
    {
        printf( "In Writer::doWrite, Writer::doWriteInternal error" );
        return -1;  //出错
    }

    uint32_t writen = ( uint32_t )rt;  //已发送字节数
    for ( list<Iov>::iterator it = pSendIovList->begin();
            it != pSendIovList->end(); ++it )
    {
        if ( writen > it->m_Iovec.iov_len )
        {
            writen -= it->m_Iovec.iov_len;  //更新剩下应该写的字节数
            it->m_Iovec.iov_base = NULL;    //将已写的数据结构置空
            it->m_Iovec.iov_len = 0;
        }
        else
        {
             //写的最后一个报文 没写完，更新要写的指针和文件大小
            it->m_Iovec.iov_base = static_cast<char*>( it->m_Iovec.iov_base ) + writen;
            it->m_Iovec.iov_len -= static_cast<size_t>( writen );
            rt = 0;  //为发送完说明对端下线。用0表示
            break;
        }
    }
    return rt;
}

void 
TCPWriter::changeRecver(const TCPSocket &sock)
{
    m_socket = sock;
} //修改报文的接收方
