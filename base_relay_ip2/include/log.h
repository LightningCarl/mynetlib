#ifndef LOG_H
#define LOG_H
#include <list>
#include <map>
#include "singleton.h"
#include "tcpSocket.h"
#include "logAgentBuffer.h"

class SocketAddr;
using namespace std;

struct Info
{
    Info(){}
    ~Info(){}
    TCPSocket m_RecvSocket;  //接收离线消息的套接字
    ssize_t m_offset;     //序列化文件偏移量
    int m_fileLen;   //反序列化文件的长度
    int m_bufLen;      //发送队列单个缓冲区存储的数据长度
};

class Log : public singleton <Log>
{
    friend class singleton<Log>;
public:
    void add();   //info加入到map中
    Info* getInfo(string temp_ip)const;        //返回由id匹配的info
    bool remove( string temp_ip );             //将info从map中删除
    string setIp(const SocketAddr& addr);
    void setTcpSocket(const TCPSocket& sock, const SocketAddr& addr);
    bool find( string temp_ip )const;
    void setOffset(ssize_t n);
    ssize_t getOffset();
    void send(Info* fo); //将某个socket的数据反序列化
    //考虑什么时候将结构体析构
private:
    Log();
    ~Log();
private:
    string getIp();
    typedef std::map<string, Info*> InfoMap;   //通过端口来找到对应的套接字 m_socket
    InfoMap m_map;
    string m_ip; //临时存放
    ssize_t m_NextOffset; //下一个要序列化的数据集的偏移量
   logAgentBuffer* m_AgentBuffer; //用于反序列化并向套接字发送待接收的报文
};
#endif
