#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <string.h>
#include "socketAddr.h"
#include "tcpSocket.h"

TCPSocket:: TCPSocket()
   : m_fd(-1)
{
}

TCPSocket:: TCPSocket(int fd)
   :m_fd(fd)
{
}

TCPSocket:: ~TCPSocket()
{
}

int
TCPSocket:: generateFd(void)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0) {
        printf("socket_generate_error");
        return -1;
    }
    return m_fd;
}

bool TCPSocket:: bindSockAddr(const SocketAddr& servaddr)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    
    if (servaddr.ifAnyAddr())
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if ((inet_aton(servaddr.getIp(), ( in_addr* )&addr.sin_addr.s_addr )) == 0)
        {
            printf("invalid_ip_address");
            return false;
        }//将IP地址填入addr
    }
    
    if (servaddr.getPort() == 0)
    {
        printf("tcpsocket_invalid_bind_port");
        return false;
    }
    
    addr.sin_port = htons( servaddr.getPort() ); //将端口号填入addr
    if (::bind(m_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("bind_error\n");  //....
        return false;
    }//此处bind 前必须有::，否则报错为无效
    return true;
}

bool
TCPSocket:: listenFd(int listen_num)
{
    if (::listen(m_fd, listen_num) < 0)
    {
        printf("tcpsocket_listen_error");
        return false;
    }
    return true;
}

int
TCPSocket:: connectAddr(const SocketAddr& ipaddr)
{
    int ret = -1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof( sockaddr_in ));
    addr.sin_family = PF_INET;
    if ( ipaddr.ifAnyAddr() )
    {
        printf("TCPSocket::connect:ipaddr.ip NULL");
        return -1;
    }
    else
    {            
        if ( ( addr.sin_addr.s_addr = inet_addr( ipaddr.getIp() ) ) == INADDR_NONE )  //...
        {
            printf("TCPSocket::bind::inet_addr: IP Address Invalid" );
            return -1;
        }
    }
        
    if ( ipaddr.getPort() == 0 )
    {
        printf( "TCPSocket::connect:ipaddr.Port Invalid" );
        return -1;
    }
    addr.sin_port = htons( ipaddr.getPort() );
    ret = ::connect(m_fd, ( struct sockaddr* )&addr, sizeof( addr ) );
    return ret;
}

int
TCPSocket:: acceptFd(SocketAddr& addr)
{
    struct sockaddr_in cliAddr;
    unsigned int cliAddrLen = sizeof( sockaddr_in );
    memset( &cliAddr, 0, cliAddrLen );
    int fd = ::accept(m_fd, ( struct sockaddr* )&cliAddr, &cliAddrLen );
    addr.setAddr(inet_ntoa( cliAddr.sin_addr ), ntohs( cliAddr.sin_port ) );
    return fd;
}

int
TCPSocket::disableLinger( void )
{
    struct linger ling = {0, 0};
    if ( setsockopt( 
        m_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof( ling ) ) < 0 )
    {
        printf( "TCPSocket::disableLinger::setsockopt" );
        return -1;
    }
    return 0;
}

int
TCPSocket::disableNagle( void )
{
    int val = 1;
    if ( setsockopt( 
        m_fd,
        IPPROTO_TCP, 
        TCP_NODELAY, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        printf("TCPSocket::disableNagle::setsockopt");
        return -1;
    }
    return 0;
}


int
TCPSocket:: setNonBlocking(void)
{
    int val;
    if ((val = fcntl(m_fd, F_GETFL, 0)) < 0)
    {
        printf("TCPSocket::setnonblocking_error");
        return val;
    }
    val |= O_NONBLOCK;
    if (fcntl(m_fd, F_SETFL, val) < 0)
    {
        printf("TCPSocket::setNonBlock:fcntl");
        return -1;
    }
    return 0;
}

int
TCPSocket::enableReuseaddr( void )
{
    int val = 1;
    if ( setsockopt( 
        m_fd,
        SOL_SOCKET, 
        SO_REUSEADDR, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        printf("TCPSocket::enableReuseaddr::setsockopt");
        return -1;
    }

    return 0;
}

int
TCPSocket::setKeepAlive( void )
{
    int val = 1;
    if ( setsockopt( m_fd, SOL_SOCKET, SO_KEEPALIVE,
                     ( const void* )&val, sizeof( val ) ) < 0 )
    {
        printf("TCPSocket::setKeepAlive::setsockopt");
        return -1;
    }

    return 0;
}//心跳检测

int
TCPSocket::closeFd(void)
{
    if (-1 == m_fd) {
        return 0;
    }
    
    if (::close(m_fd) < 0)
    {
        printf("tcpsocket_close_error");
        return -1;
    }
    
    m_fd = -1;
    return 0;
}

int
TCPSocket:: getFd(void)
{
    return m_fd;
}

ssize_t
TCPSocket::read( char* buf, size_t len )
{
    ssize_t readNum = ::read( m_fd, buf, len );
    return readNum;
}

ssize_t
TCPSocket::write( const char* buf, size_t len )
{
    ssize_t writeNum = ::write( m_fd, buf, len );
    return writeNum;
}

ssize_t
TCPSocket::writev( const struct iovec* v, size_t c )
{
    ssize_t writeNum = ::writev( m_fd, v, c );
    return writeNum;
}
