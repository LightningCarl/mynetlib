#ifndef CLIENTAGENT_H
#define CLIENTAGENT_H
#include "tcpagent.h"
#include "InPackage.h"

class SocketAddr;
class TCPSocket;

class clientagent : public TCPAgent
{
public:
    clientagent(unsigned int Len);
    clientagent( const SocketAddr &, unsigned int Len);
    clientagent( const TCPSocket &, const SocketAddr &, unsigned int Len);
    ~clientagent();

    void readBack( InReq & req);
    int PackageMsg(MsgHeader &header, const char *dataStr);  //打包报文
    int init();
    int connectAfter(bool bConnect); //for echo data
    void echo();  //回射数据    
private:
    void PrintRTT(const MsgHeader &msg);
    bool readFiles(int fd, char* buf, int len);
};

#endif 
