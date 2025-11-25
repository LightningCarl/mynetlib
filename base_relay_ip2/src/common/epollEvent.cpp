#include <sys/epoll.h>
#include <cstdio>
#include "epollEvent.h"
#include "Epoll.h"   //用于调用epoll的doEvent

#define LEVELREVENT                  EPOLLIN
#define LEVELWEVENT                  EPOLLOUT
#define LEVELEVENTS                  (EPOLLIN |EPOLLOUT| EPOLLERR | EPOLLHUP)
#define EVENTADD                     EPOLL_CTL_ADD
#define EVENTDEL                     EPOLL_CTL_DEL
#define EVENTCHANGE                  EPOLL_CTL_MOD

int epollEvent::closeRevent(void)
{
    if ( -1 == m_fd )
    {
        return 0;
    }

    m_event = m_event &~LEVELREVENT;
    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTCHANGE, m_event ) < 0 )
    {
        printf( "EpollEvent::closeRevent" );
        return -1;
    }
    return 0;
}

int epollEvent::openRevent( void )
{
    m_event = m_event | LEVELREVENT;
    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTCHANGE, m_event ) < 0 )
    {
        printf( "EpollEvent::OpenRevent" );
        return -1;
    }

    return 0;
}

int epollEvent::closeWevent( void )
{
    if(-1 == m_fd)
    {
        return 0;
    }

    m_event = m_event &~LEVELWEVENT;
    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTCHANGE, m_event ) < 0 )
    {
        printf( "EpollEvent::closeWevent" );
        return -1;
    }

    return 0;
}

int 
epollEvent::openWevent( void )
{
    m_event = m_event | LEVELWEVENT;
    if ((Epoll::getInstance())->doEvent( this, m_fd, EVENTCHANGE, m_event ) < 0 )
    {
        printf( "EpollEvent::OpenWevent" );
        return -1;
    }
    return 0;
}

int 
epollEvent::registerRevent( void )
{
    m_event = LEVELREVENT;
    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTADD, m_event ) < 0 )
    {
        printf( "EpollEvent::registerREvent" );
        return -1;
    }

    return 0;
}

int epollEvent::registerWevent( void )
{
    m_event = LEVELWEVENT;

    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTADD, m_event ) < 0 )
    {
        printf( "EpollEvent::registerWEvent" );
        return -1;
    }
    return 0;
}

int epollEvent::registerRWevents( void )
{
    if ( -1 == m_fd )
    {
        return 0;
    }

    m_event = LEVELEVENTS;

    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTADD, m_event ) < 0 )
    {
        printf( "EpollEvent::registerRWEvent" );
        return -1;
    }

    return 0;
}

int epollEvent::unregisterRWevents( void )
{
    if ( -1 == m_fd )
    {
        return 0;
    }

    m_event = LEVELEVENTS;

    if ( (Epoll::getInstance())->doEvent( this, m_fd, EVENTDEL, m_event ) < 0 )
    {
        printf( "EpollEvent::UnregisterRWEvent" );
        return -1;
    }

    return 0;
}
