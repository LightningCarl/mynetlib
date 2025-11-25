#ifndef AGENT_H
#define AGENT_H

#include <stdint.h>
#include "socketAddr.h"
#include "tcpSocket.h"

struct InReq;
class SocketAddr;

class Agent
{
public:
    Agent();
    virtual ~Agent();
    
    virtual void setID( uint32_t );
    virtual uint32_t getID()const;
    //virtual int connect( const SocketAddr& );
    virtual int connect() { return 0; }
    virtual int connectAfter( bool );
    int getState()const;
    virtual void setState(bool );
    virtual void readBack( InReq& req ) {}
    virtual int recvData( void );
    virtual int sendData( void );
    virtual void recycler( void );
    virtual bool isRecycler( void );
    virtual int getErrno();

    virtual int getOppAddr( SocketAddr& )
    {
        return 0;
    }

    void resetConnect()
    {
        m_iConnectTimes = 0;
    }
    int allowReconnect()const;
protected:
    bool m_bIsRecycler;
    bool m_iConnect;      //已连接标志
    int m_iConnectTimes;  //允许的连接次数
    uint32_t m_ID;        //用于查找，在epollEvent中也有一份
    unsigned int m_len;            //客户端存放报文长度
};
#endif
