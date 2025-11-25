#include <cassert>
#include <algorithm>
#include <cstdio>
#include "agent.h"
#include "agentManager.h"
#include "socketAddr.h"
#include "tcpSocket.h"

namespace
{

struct AgentDeleter
{
    void operator()( Agent* agent )const
    {
        if ( agent != NULL)
        {
            delete agent;
            agent = NULL;
        }
    }
};

}

AgentManager::AgentManager():
    m_agentID( 0 ),
    m_map(),
    m_recycleList()
{
}

AgentManager::~AgentManager()
{
}

uint32_t 
AgentManager::add( Agent* agent )
{
    uint32_t id = generateID();
    m_map[id] = agent;
    agent->setID( id );
    return id;
}

Agent* 
AgentManager::get( int id )const
{
    Agent* agent = NULL;
    AgentMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        agent = it->second;
    }
    return agent;
}

bool 
AgentManager::find( int id )const
{
    AgentMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        return true;
    }
    return false;
}

bool 
AgentManager::remove( int id )
{
    AgentMap::size_type eraseCount = m_map.erase( id );
    return ( eraseCount != 0 );
}

void 
AgentManager::recycle( int id )
{
    Agent* agent = NULL;

    if ( find( id ) )
    {
        agent = get( id );
        if ( !remove( id ) )
        {
        }
        AgentList::iterator ait = 
            m_recycleList.insert( m_recycleList.end(), agent );
        if ( ait == m_recycleList.end() )
        {
        }

        agent->recycler();
    }
    return;
}

void AgentManager::recycleAll()
{
    if ( !m_recycleList.empty() )
    {
        for_each( 
            m_recycleList.begin(), m_recycleList.end(), AgentDeleter() );
        m_recycleList.clear();
    }
    return;
}

void
AgentManager::clearAllAgent()
{
    if ( !m_map.empty() )
    {
        for (AgentMap::iterator it = m_map.begin();
            it != m_map.end();)
        {
            AgentMap::iterator tempit = it;
            ++it;
            recycle( tempit->first );
        }
    }
}

uint32_t AgentManager::generateID()
{
    ++m_agentID;
    return m_agentID;
}

void AgentManager::recyclerAgent( Agent* agent )
{
    if ( agent != NULL )
    {
        agent->recycler();
        recycle( agent->getID() );
    }
    return;
}