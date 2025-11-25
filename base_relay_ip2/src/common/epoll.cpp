#include <string.h>
#include <cstdio>
#include <errno.h>

#include "Epoll.h"
#include "tcpagent.h"
#include "agentManager.h"
#include "epollEvent.h"

Epoll:: Epoll():
    m_events( NULL ),
    m_epollFd( -1 ),
    m_eventSize(-1),
    m_bFinished(false)
{
}

Epoll:: ~Epoll()
{
    if ( m_events != NULL )
    {
        delete [] m_events;
        m_events = NULL;
    }
}

bool 
Epoll:: initEpoll(int fd_size)
{
    m_eventSize = fd_size;
    m_epollFd = epoll_create(m_eventSize);
    if (m_epollFd < 0)
    {
        printf("epoll_creat_error ");
        return false;
    }
    m_events = new epoll_event[fd_size];
    memset(m_events, 0, sizeof(epoll_event)*fd_size);
    return true;
}//建立epoll机制，创建epoll活跃事件数组

bool 
Epoll:: doEvent(epollEvent  *ptr, int fd, int operate, unsigned int event)
{
    struct epoll_event ev; //新建一个用于红黑树的事件
    memset(&ev, 0, sizeof(epoll_event));  
    ev.events = event;
    ev.data.ptr = ptr;    //指向epollEvent的指针
    if (epoll_ctl(m_epollFd, operate, fd, &ev) < 0)
    {
        printf("epoll_ctl_error ");
        return false;
    }
    return true;
}

void Epoll::run( void )
{
    int nfds = 0;
    epollEvent* event = NULL; //指向类EpollEvent的指针

    for ( ;; )
    {
        if ( ( nfds = epoll_wait(m_epollFd, m_events, m_eventSize, 5) ) < 0 )
        {
            if ( EINTR  == errno )
            {
                continue;
            }
            else
            {
                printf( "Epoll:epoll_wait\n" );
            }
        }  //获取活跃事件

        for ( int i = 0; i < nfds; i++ )
        {
            event = ( epollEvent* )m_events[i].data.ptr; //从数组获取epoll事件
            if ( NULL == event )
            {
                printf( "Epoll::event == NULL\n" );
                continue;
            }
            
            Agent* agent = (AgentManager::getInstance())->get(event->getIndexID());
            if ( NULL == agent )
            {
                printf( "Epoll::agent == NULL\n" );  //。。。。。
                continue;
            }

            if ( m_events[i].events & EPOLLERR  //文件描述符错误
                    || m_events[i].events & EPOLLHUP )  //文件描述符被挂断
            {
                if ( agent->getState() == UNCONNECT ) //客户端未连接
                {
                    if ( agent->getErrno() == EISCONN ) //若已连接该套接字  --客户端
                    {
                        agent->setState( CONNECTED );  //正确标记已连接，然后正常发送数据
                        if ( agent->connectAfter( true ) < 0 )  //tcpagent 发送数据
                        {
                            AgentManager::getInstance()->recycle( agent->getID());
                            continue;
                        }
                        agent->resetConnect(); //将重连次数设置为0
                        continue;
                    }
                    else //其他连接错误 客户端
                    {
                        if ( agent->allowReconnect() )  //未超过重连次数限值
                        {
                            SocketAddr addr;
                            if ( 0 == ( agent->getOppAddr( addr ) ) )  //对端地址不为0，可以重连
                            {
                                if ( agent->connect() < 0 ) //重连
                                {
                                }
                            }
                            else //放弃连接，释放资源
                            {
                                printf( "OppAddr error" );
                                AgentManager::getInstance()->recycle( agent->getID());
                            }
                            continue;
                        }
                        else //超过重连次数限值   客户端
                        {
                            agent->resetConnect();  //将连接次数清0
                            agent->connectAfter( false );  //返回错误
                            printf( "connect error" );
                            AgentManager::getInstance()->recycle( agent->getID());
                        }
                    }
                }
                else
                {
                    if ( agent->recvData() < 0 )   //服务器：已连接的直接接收数据？
                    {
                        AgentManager::getInstance()->recycle( agent->getID());
                        continue;
                    }
                }
            }

            if ( m_events[i].events & EPOLLOUT )
            {
                if ( CONNECTED == ( agent->getState() ) )
                {
                    if ( agent->sendData() < 0 )          //已连接
                    {
                        AgentManager::getInstance()->recycle( agent->getID());
                        continue; //客户端和服务器都是从发送队列发送数据
                    }
                }
                else
                {
                    agent->setState( CONNECTED );
                    if ( agent->connectAfter( true ) < 0 ) //客户端侦测到未连接：将状态改正，发送第一份数据
                    {
                        AgentManager::getInstance()->recycle( agent->getID());
                    }
                    continue;
                }
            }

            if ( m_events[i].events & EPOLLIN )
            {
                if ( agent->recvData() < 0 ) //服务器：  tcpagent接收数据，listenagent创建新的tcpagent
                {
                    AgentManager::getInstance()->recycle( agent->getID());
                    continue;  //客户端和服务器在接收完整的报文后的反应由用户定义成不一样的
                }
            }
            
        }
        if ( doTask() < 0 )
        {
            printf( "Deal With Task Error" );
        }
        if ( m_bFinished )
        {
            break;
        }
    }
}

int
Epoll::doTask( void )
{
    if ( m_bFinished )
    {
        AgentManager::getInstance()->clearAllAgent();
    }
    AgentManager::getInstance()->recycleAll();
    return 0;
}

void
Epoll::finish()
{
    m_bFinished = true;
}
