#include <string>
#include "log.h"
#include "socketAddr.h"

namespace
{

struct LogDeleter
{
    void operator()( Info* info )const
    {
        if ( info != NULL)
        {
            delete info;
            info = NULL;
        }
    }
};

}

Log::Log():
    m_map(),
    m_ip()
{
}

Log::~Log()
{
}

void 
Log::add()
{
    Info* info = new Info;
    string ip = getIp();
    m_map[ip] = info;
}

Info* 
Log::getInfo(string temp_ip)const
{
    Info* info = NULL;
    InfoMap::const_iterator it = m_map.find( temp_ip );
    if ( it != m_map.end() )
    {
        info = it->second;
    }
    return info;
}

bool 
Log::find( string temp_ip )const
{
    InfoMap::const_iterator it = m_map.find( temp_ip );
    if ( it != m_map.end() )
    {
        return true;
    }
    return false;
}

bool 
Log::remove( string temp_ip)
{
    InfoMap::size_type eraseCount = m_map.erase( temp_ip );
    return ( eraseCount != 0 );
}

string
Log::setIp(const SocketAddr& addr)
{
    string m_ip = addr.getIp();
    return m_ip;
}

string 
Log::getIp()
{
    return m_ip;
}

void
Log::setOffset(ssize_t n)
{
    m_NextOffset += n;
}

ssize_t
Log::getOffset()
{
    return m_NextOffset;
}

void 
Log::send(Info* fo)
{
    if (m_AgentBuffer)
    {
        m_AgentBuffer->Re_serialPackage(fo); //反序列化
        m_AgentBuffer->writeToSocket();    //将反序列化得到的发送队列中的数据发送给客户端套接字
    }
}

void 
Log::setTcpSocket(const TCPSocket& sock, const SocketAddr& addr)
{
    getInfo(addr.getIp())->m_RecvSocket = sock;
}