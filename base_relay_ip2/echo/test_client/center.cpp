#include "Center.h"
#include "agentManager.h"

void 
center::createAgents()
{
    for (int i = 0; i < m_agents; i++)
    {
        //bug:此文件：createAgent<clientagent>(m_servaddr, m_size);  不完整
        m_pClientAgent = (AgentManager::getInstance())->createAgent<clientagent>(m_servaddr, m_size);  //生成通信agent
        m_pClientAgent->init();  //连接到服务器
    }
}

void
center::setNum(unsigned int n)
{
    m_agents = n;
}

void
center::setSize(unsigned int s)
{
    m_size = s;
}

void 
center::setAddr(SocketAddr sa)
{
    m_servaddr = sa;
}