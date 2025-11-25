#include "tcpagent.h"
#include "TcpReader.h"
#include "TcpWriter.h"
#include "ReadCallBack.h"

TCPAgent:: TCPAgent():
    m_addr(InitAddr)
{
}

TCPAgent:: TCPAgent(const SocketAddr& oppoAddr):
    m_addr(oppoAddr)
{
}

TCPAgent:: TCPAgent(const TCPSocket& sock, const SocketAddr& oppoAddr):
    m_socket(sock),m_addr(oppoAddr)
{
    setState( CONNECTED );
    m_epollEvent.setFd( m_socket.getFd() );
    m_buffer.setReader( new TCPReader( m_socket ) );       //从本端m_socket读
    m_buffer.setWriter( new TCPWriter( m_socket ) );       //向对端m_socket写
    m_buffer.setReadCallback( new ReadCallback( this ) );
    m_epollEvent.registerRevent();
}

void TCPAgent::setID( uint32_t id )
{
    m_ID = id;
    return;
}//将agentID给epollEvent

uint32_t TCPAgent::getID( void )
{
    return m_ID;
}//获取agentID

int TCPAgent::init()
{
    m_epollEvent.setIndexID(getID());
    return 0;
}//设置索引ID

int TCPAgent::connectAfter( bool )
{
    return 0;
}//客户端重写

int TCPAgent::recvData(void)
{
    if ( ( this->m_buffer.readTCP() ) < 0 )
    {
        printf( "TCPAgent::readData():m_buffer.read()" );
        return -1;
    }
    return 0;
}


int
TCPAgent::sendData( void )
{
    if ( this->m_buffer.writeToSocket() < 0 )
    {
        printf( "TCPAgent::writeData():m_buffer.write(TCPSocket)" );
        return -1;
    }
    if ( this->m_buffer.getSendBufferLength() == 0 )
    {
        if ( this->m_epollEvent.closeWevent() < 0 )
        {
            printf( "this->m_epollEvent.closeWevent error" );
        }
    }

    return 0;
}

void
TCPAgent::setState( int st )
{
    m_iConnect = CONNECTED;
    if ( st == CONNECTED )
    {
        if ( m_epollEvent.closeWevent() < 0 )
        {
            printf( "m_epollEvent.closeWevent error" );
        }
    }
}

int
TCPAgent::ToSendList( char* buf, unsigned int len )
{
    this->m_buffer.ToSendList( buf, len );

    if ( this->m_epollEvent.openWevent() < 0 )
    {
        printf( "m_epollEvent.openWevent error" );
        return -1;
    }
    return 0;
}//调用bufferManager的函数将报文写入发送列表

int
TCPAgent::PackageMsg(MsgHeader &header, const char *dataStr)
{
    uint32_t msgLength = header.length + sizeof(MsgHeader);
    char *sendBuf = new char[msgLength + 1];
    memset(sendBuf, 0, msgLength + 1);
    memcpy(sendBuf, &header, sizeof(MsgHeader));
    if (dataStr != NULL)
    {
        memcpy(sendBuf + sizeof(MsgHeader), dataStr, header.length);
    }

    if (this->ToSendList(sendBuf, msgLength) == -1)
    {
        printf("\n In ServerAgent: sendPackage(): write data error! \n");
        return -1;
    }
    return 0;
}//服务器将报文整合起来，放到发送队列尾部


int
TCPAgent::connect()
{
    m_iConnectTimes += 1;
    if ( m_socket.closeFd() < 0 )
    {
        printf( "m_socket.close error" );
    }

    if ( m_socket.generateFd() < 0
            || m_socket.disableNagle() < 0
            || m_socket.disableLinger() < 0
            || m_socket.setNonBlocking() < 0
            || m_socket.setKeepAlive() < 0 )
    {
        printf( "TCPAgent::connect():set the socket option error " );
        return -1;
    }
    m_epollEvent.setFd( m_socket.getFd() );
    m_epollEvent.setIndexID( getID());
    m_buffer.setReader( new TCPReader( m_socket ) );
    m_buffer.setWriter( new TCPWriter( m_socket ) );
    m_buffer.setReadCallback( new ReadCallback( this ) );

    if ( m_epollEvent.registerRWevents() < 0 )
    {
        return -1;
    }

    if (m_socket.connectAddr( m_addr ) < 0)
    {
        printf("connect_errror\n");
    }
    return 0;
}

void
TCPAgent::readBack(InReq &req)  
{
    string data(req.ioBuf, req.m_msgHeader.length); 
    
    MsgHeader msg;  //临时头部
    msg.length = req.m_msgHeader.length;  
    PackageMsg(msg, req.ioBuf);  //将头部和负载写到发送队列
    data.clear();
}