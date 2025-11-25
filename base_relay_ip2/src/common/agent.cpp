#include <string>
#include "agent.h"
#include "MsgHeader.h"
#include "socketAddr.h"

Agent::Agent(): 
    m_bIsRecycler( false ), 
    m_iConnect( UNCONNECT ),
    m_iConnectTimes( -1 ),
    m_ID(0),
    m_len(0)
{
}

Agent::~Agent()
{
}

int Agent::recvData( void )
{
    return 0;
}

int Agent::sendData( void )
{
    return 0;
}


/*int Agent::connect( const SocketAddr& )
{
    return 0;
}*/

int Agent::connectAfter( bool )
{
    return 0;
}

int Agent::getState()const
{
    return m_iConnect;
}

void Agent::setState( bool st )
{
    m_iConnect = st;
}

void Agent::recycler( void )
{
    m_bIsRecycler = true;
}

bool Agent::isRecycler( void )
{
    return m_bIsRecycler;
}

int Agent::getErrno()
{
    return errno;
}

int Agent::allowReconnect()const
{
    if ( m_iConnectTimes < MAXRECONNECTTIMES )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Agent::setID( uint32_t id )
{
    m_ID = id;
    return;
}

uint32_t Agent::getID( void )const
{
    return m_ID;
}