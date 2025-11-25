#ifndef SOCKETADDR_H
#define SOCKETADDR_H

#include <netinet/in.h>
#include <string>
using namespace std;

//套接口的类
class SocketAddr
{
public:
    SocketAddr(); //默认构造函数
    SocketAddr( const char* buf, unsigned short port); //能指定IP和端口的构造函数
    ~SocketAddr(); //析构函数
    
    void setAddr( const char* buf, unsigned short port); //设置套接字IP和端口的成员函数
    void setPort( unsigned short p );
    const char* getIp(void) const;
    unsigned short getPort(void) const
    {
        return m_port;
    };
    sockaddr_in getAddr(void); //函数实现需要在.cpp文件中完成
    SocketAddr& operator = ( const SocketAddr& );
    bool operator == ( const SocketAddr& )const;
    bool ifAnyAddr(void) const;
private:
    string          m_ip;
    unsigned short  m_port;
};
#endif /* SocketAddr_h */
