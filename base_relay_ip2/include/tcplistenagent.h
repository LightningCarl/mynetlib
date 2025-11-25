#ifndef TCPLISTENAGENT_H
#define TCPLISTENAGENT_H
#include <netinet/tcp.h>
#include <iostream>
#include <cstring>
#include <errno.h>

#include "agent.h"
#include "agentManager.h"
#include "tcpSocket.h"
#include "epollEvent.h"
#include "socketAddr.h"
#include "singleton.h"

using namespace std;
const int LISTEN_NUM = 1024;

template <class ConcreteAgent>
class TCPListenAgent : public Agent
{
public:
    TCPListenAgent( const SocketAddr& addr ):
		m_addr(addr)
    {
    }
    ~TCPListenAgent() {}

    virtual int init( void );
    virtual int recvData( void );
    virtual int sendData( void );

private:
    TCPSocket    m_TCPListenSocket;
    SocketAddr   m_addr;
    epollEvent   m_epollEvent;
};

template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::init()
{
    if ( m_TCPListenSocket.generateFd() < 0
            || m_TCPListenSocket.disableLinger() < 0      //优雅断开
            || m_TCPListenSocket.disableNagle() < 0       //
            || m_TCPListenSocket.setNonBlocking() < 0        //非阻塞
            || m_TCPListenSocket.bindSockAddr( m_addr ) < 0
            || m_TCPListenSocket.listenFd( LISTEN_NUM ) < 0 )
    {
        printf("TCPListenAgent::init()");
        return -1;
    }

    m_epollEvent.setFd( m_TCPListenSocket.getFd() );
    m_epollEvent.setIndexID( getID());  //将agentID存入epollEvent
    m_epollEvent.registerRevent();
    return 0;
}

template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::recvData()
{
    SocketAddr oppositeAddr;
    int connfd;
    connfd = m_TCPListenSocket.acceptFd( oppositeAddr ); 
    if ( connfd < 0 )
    {
        if ( !( ( errno == EINTR ) || ( errno == EWOULDBLOCK ) ) )
        {
            printf("TCPListenAgent listen");
            return -1;
        }
    }
    TCPSocket connSock( connfd ); //构造函数
    if ( connSock.setNonBlocking() < 0               
            || connSock.disableLinger() < 0       
            || connSock.disableNagle() < 0 )      
    {
        printf( "TCPListenAgent::recvData" );
        connSock.closeFd();
        return -1;
    }
    
    //resigerREvent and initialize the Agent's member
    ConcreteAgent* agent =( AgentManager::getInstance() )->createAgent<ConcreteAgent>(connSock, oppositeAddr); //创建带有连接状态的已连接套接字的agent，由具体agent实现
    agent->init();

    return 0;
}

//when the write event arrive, do nothing
template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::sendData( void )
{
    return -1;
}

#endif
