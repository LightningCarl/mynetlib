#include "serveragent.h"
#include "log.h"
using namespace std;

serverAgent:: serverAgent()
{
}

serverAgent:: serverAgent(const SocketAddr &addr):
    TCPAgent(addr)
{
}

serverAgent:: serverAgent(const TCPSocket &sock, const SocketAddr &addr):
    TCPAgent(sock, addr)
{
}

serverAgent::~serverAgent()
{
}

void
serverAgent::readBack(InReq &req)  //服务器
{
   PackageMsg(req.m_msgHeader, req.ioBuf);  //将头部和负载写到发送队列
   string temp_ip = req.m_msgHeader.m_RecverIp; //对端ip

   if ( (Log::getInstance()->find(temp_ip)) )
   {
       Info* fo = (Log::getInstance()->getInfo(temp_ip) );  //获取对端信息
       m_buffer.changeRever(fo->m_RecvSocket);  //更新对端套接字
   }
   else
   {
       printf("未知通信方\n");  
       TCPSocket sock = -1;
       m_buffer.changeRever(sock);  //设置一个空的套接字：发送的数据会当作下线的形式处理
   }
}

int
serverAgent::PackageMsg(MsgHeader &header, const char *dataStr)
{
    uint32_t msgLength = header.length + sizeof(MsgHeader); //获取总长度
    char *sendBuf = new char[msgLength + 1];    //申请发送缓冲区
    memset(sendBuf, 0, msgLength + 1);   //置空
    memcpy(sendBuf, &header, sizeof(MsgHeader));  //将头部拷入发送缓冲区
    if (dataStr != NULL)         //指向缓冲区的指针
    {
        memcpy(sendBuf + sizeof(MsgHeader), dataStr, header.length); //将负载拷入发送缓冲区
    }

    if (this->ToSendList(sendBuf, msgLength) == -1)  //将发送缓冲写入发送队列
    {
        printf("\n In ClientAgent: sendPackage(): write data error! \n");
        return -1;
    }
    return 0;
}

int 
serverAgent::init()
{
    string temp_ip = m_addr.getIp();  //获取ip
    if ( (Log::getInstance()->find(temp_ip)) )
    {
        Info* fo = (Log::getInstance()->getInfo(temp_ip) ); 
        if (fo->m_offset)
        {
            fo->m_RecvSocket = m_socket;  //更新客户端套接字  
            Log::getInstance()->send(fo);  //使用logAgentBuffer反序列化
            //使用logAgentBuffer发送数据到此agent对应的客户端的套接字
        }
        else
        {
            fo->m_RecvSocket = m_socket;  //仅更新客户端套接字 
        }
    }
    else
    {
        //注册 
        Log::getInstance()->setIp(m_addr);  //注册ip
        Log::getInstance()->add();  //用IP创建info结构
        Log::getInstance()->setTcpSocket(m_socket, m_addr);  //把套接字填入info结构中
    }
    return 0;
}

int
serverAgent::sendData( void )
{
    if ( this->m_buffer.writeToSocket() < 0 )
    {
        string temp_ip = m_addr.getIp();  //获取ip
        //检查对方离线还是未注册
        
        if( !(Log::getInstance()->find(temp_ip))  )
        {
            //注册ip：缺少套接字的未激活状态
            Log::getInstance()->setIp(m_addr);
            Log::getInstance()->add();//创建Info结构
        }
        Info* fo = ( Log::getInstance() )->getInfo(temp_ip);
        m_buffer.serialPackage(fo); //序列化报文：填充其他信息
        return 0;
    }
    if ( this->m_buffer.getSendBufferLength() == 0 )
    {
        if ( this->m_epollEvent.closeWevent() < 0 )
        {
            printf( "this->m_epollEvent.closeWevent error" );
        }
    }
    return 0;
}
