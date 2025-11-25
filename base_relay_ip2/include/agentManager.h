#ifndef AGENTMANAGER_H
#define AGENTMANAGER_H

#include <map>          //agent与ID的匹配
#include <list>         //回收agent
#include <stdint.h>     //uint32_t
#include "singleton.h"

class Agent;
class SocketAddr;
class TCPSocket;

class AgentManager:public singleton<AgentManager>
{
    friend class singleton<AgentManager>;
public:
    uint32_t add( Agent* const );   //将agent加入到map中
    Agent* get( int ) const;        //返回由id匹配的agent
    bool find( int ) const;         //判断一个id是否存在,回收的时候用
    bool remove( int );             //将agent从map中删除
    void recycle( int );            //将agent回收到回收列表，并设置已回收标志
    void recycleAll( void );        //清除回收列表
	void clearAllAgent();           //将所有agent回收

    template<typename T>
    T* createAgent();

    template<typename T>
    T* createAgent( const SocketAddr& );

    template<typename T>
    T* createAgent( const SocketAddr&, int len);

    template<typename T>
    T* createAgent( const TCPSocket& , const SocketAddr& );

    void recyclerAgent( Agent* );
private:
    AgentManager();
    ~AgentManager();
private:
    typedef std::map<int, Agent*> AgentMap;
    typedef std::list<Agent*> AgentList;
    uint32_t generateID( void );

    uint32_t m_agentID;
    AgentMap m_map;  //查找agent的map
    AgentList m_recycleList;  //回收agent的list
};

template<typename T>
T* AgentManager::createAgent()
{
    T* agent;
    agent = new T();
    if ( agent != NULL )
    {
        add( agent );
    }
    return agent;
}

template<typename T>
T* AgentManager::createAgent( const SocketAddr& addr )
{
    T* agent;
    agent = new T( addr );
    if ( agent != NULL )
    {
        add( agent );
    }
    return agent;
}

template<typename T>
T* AgentManager::createAgent( const SocketAddr& addr, int len)
{
    T* agent;
    agent = new T( addr,len );
    if ( agent != NULL )
    {
        add( agent );
    }
    return agent;
}

template<typename T>
T* AgentManager::createAgent(const TCPSocket& sock, const SocketAddr& addr)
{
    T* agent;
    agent = new T( sock, addr );
    if ( agent != NULL )
    {
        add( agent );
    }
    return agent;
}
#endif
