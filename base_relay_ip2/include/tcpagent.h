#ifndef TCPAGENT_H
#define TCPAGENT_H

#include "agent.h"
#include "epollEvent.h"
#include "tcpSocket.h"
#include "MsgHeader.h"
#include "BufferManager.h"

struct InReq;
const SocketAddr InitAddr( "0", 0 );

class TCPAgent:public Agent
{
public:
    TCPAgent();
    TCPAgent( const SocketAddr& );
    TCPAgent( const SocketAddr& ,int len);
    TCPAgent( const TCPSocket&, const SocketAddr& );
    virtual ~TCPAgent(){}

    void setID( uint32_t );
    uint32_t getID();
    virtual int init(); //for client
    void setState( int st );
    int connect();  //客户端
    int connectAfter(bool m_iConnect); //客户端发送数据
    virtual int sendData( void );  //服务器发送数据
    virtual int recvData( void );  //服务器接收数据
    
    virtual void readBack(InReq &req);   //服务器：回调函数调用此函数，进行收到完整数据后的操作。客户端：计算往返时间
    virtual int PackageMsg(MsgHeader &header, const char *dataStr);  //打包消息
    int ToSendList(char* pBuf, unsigned int len);  //将Package写入发送队列

    virtual int getOppAddr( SocketAddr& sockAddr )
    {
        if ( InitAddr == m_addr )
        {
            return -1;
        }
        sockAddr = m_addr;
        return 0;
    } //用来判断接受的对端地址是否为0
protected:
    TCPSocket m_socket;
    SocketAddr m_addr;
    epollEvent m_epollEvent;
    BufferManager m_buffer; 
};
#endif
