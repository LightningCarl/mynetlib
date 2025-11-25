#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include <string>

class SocketAddr;//声明

class TCPSocket
{
public:
    TCPSocket();
    TCPSocket(int fd);
    ~TCPSocket();
    
    int  generateFd(void);      //生成监听套接字
    bool bindSockAddr(const SocketAddr& add);  //fd与socket绑定 
    bool listenFd(int listen_num);      //监听TCP连接请求
    int  connectAddr(const SocketAddr&);
    int  connectAfter( const SocketAddr& );
    int  enableReuseaddr( void ); //客户端
    int  acceptFd(SocketAddr&);
    int  disableLinger( void );
    int  disableNagle( void );
    int  setNonBlocking(void);      //设置非阻塞
    int  setKeepAlive( void );
    int  closeFd(void);
    int  getFd(void);
    ssize_t  read( char* buf, size_t len );
    ssize_t  write( const char* buf, size_t len );
    ssize_t  writev( const struct iovec*, size_t );
    
private:
    int m_fd;
};
#endif /* TCPSocket_h */
