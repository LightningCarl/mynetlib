#ifndef CENTER_H
#define CENTER_H
#include "clientagent.h"

class center{
public:
    center(const SocketAddr& addr)
    {
        m_servaddr = addr;
        m_agents = 0;
        m_size = 0;
        m_pClientAgent = NULL;
    }

    ~center(){}
    void setNum(unsigned int);
    void setSize(unsigned int);
    void setAddr(SocketAddr);
    void createAgents();
private:
    int           m_agents;
    int           m_size;
    SocketAddr    m_servaddr;
    clientagent*  m_pClientAgent;
};

#endif /* Center_h */
