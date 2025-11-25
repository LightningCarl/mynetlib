#ifndef SERVERAGENT_H
#define SERVERAGENT_H
#include "tcpagent.h"
#include "InPackage.h"

class MsgHeader;

class serverAgent:public TCPAgent
{
public:
    serverAgent();
    serverAgent(const SocketAddr &addr);
    serverAgent(const TCPSocket &sock, const SocketAddr &addr);
    ~serverAgent();

    int init();  //重写
    //接收完整报文后的操作
    void readBack(InReq &req);  //执行回调：将完整报文送到发送队列
    int PackageMsg(MsgHeader &header, const char *dataStr);  //打包报文


    //epoll端被调用的函数
    int sendData( void );  //服务器发送数据
    //int recvData( void );  //服务器接收数据
private:
};
#endif 
