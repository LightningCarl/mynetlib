#include "socketAddr.h"
#include <sys/socket.h>
#include <cstdio>
#include <string.h>
#include <arpa/inet.h> //inet_addr
using namespace std;

const size_t IPV4LEN = 15; //255.255.255.255
const char* ANYADDR = "0.0.0.0";

SocketAddr :: SocketAddr(): m_port(0)
{
    m_ip = ANYADDR; //0.0.0.0
    //m_str = ""
}

SocketAddr :: SocketAddr(const char* buf, unsigned short p)
{
    m_port = p;  //设置端口
    if(strlen(buf) > IPV4LEN || strcmp(buf, ANYADDR) == 0)
    {
        m_ip = ANYADDR; //若ip地址非法，将其设置为0.0.0.0
    }
    else
    {
        m_ip = string(buf); //从缓冲区中获取ip地址
    }
}//带有IP和端口的构造函数 可能用不上

SocketAddr:: ~SocketAddr()
{
}

void
SocketAddr:: setAddr(const char *buf, unsigned short p)
{
    m_port = p;
    m_ip = string(buf);
}

sockaddr_in SocketAddr:: getAddr()
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    return addr;
}

bool SocketAddr:: ifAnyAddr(void) const
{
    if (m_ip == ANYADDR)
    {
        return true;
    }
    return false;
}//用户不能访问私有数据成员，故通过此函数做判断

void
SocketAddr::setPort( unsigned short p )
{
    m_port = p;
}

SocketAddr&
SocketAddr::operator=( const SocketAddr& h )
{
    m_ip = h.m_ip;
    m_port = h.m_port;
    return *this;
}

bool
SocketAddr::operator == ( const SocketAddr& h )const
{
    if ( m_ip == h.m_ip && m_port == h.m_port )
    {
        return true;
    }
    else
    {
        return false;
    }
}

const char* 
SocketAddr::getIp(void)const
{
    return m_ip.c_str();  //将C++的string转化为C的字符串数组:生成一个const char *指针，指向字符串的首地址。
};