#ifndef READCALLBACK_H
#define READCALLBACK_H
#include <cstdlib>

class TCPAgent;
struct InReq;

class ReadCallback
{
public:
    explicit ReadCallback( TCPAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~ReadCallback()
    {
        m_pAgent = NULL;
    }
    int run(InReq& req);
private:
    TCPAgent* m_pAgent;
};
#endif /* ReadCallBack_h */
