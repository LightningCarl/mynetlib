#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "clientagent.h"

clientagent::clientagent(unsigned int Len)
{
    m_len = Len;
}

clientagent::clientagent(const SocketAddr &addr, unsigned int Len)
: TCPAgent(addr)
{
    m_len = Len;
}

clientagent::clientagent(const TCPSocket &sock, const SocketAddr &addr, unsigned int Len)
: TCPAgent(sock, addr)
{
    m_len = Len;
}

clientagent::~clientagent()
{
}

int clientagent::init()
{
    if (this->connect() != 0)
    {
        printf("in ServerAgent::init connect server error");
        return -1;
    }

    return 0;
}

int clientagent::connectAfter(bool bConnect)
{
    if (bConnect)
    {
        echo();    //发送数据
        return 0;
    }
    else
    {
        return -1;
    }
}

bool 
clientagent:: readFiles(int fd, char* buf, int len)
{
    int n_read = 0;
    int left = len;
    int flags = fcntl(fd, F_GETFL, 0); 
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    while (left > 0)
    {
        if( (n_read = read(fd, buf, left)) < 0 )
        {
            printf("read_file_error\n");   //.......
            return false;  //0
        }
        else if (n_read == 0)
        {
            return true;
        }else
        {
            left -= n_read;
        }
    }
    printf("发送的数据为: %s\n", buf);  //打印收到的数据
    return true; //～0
}

void
clientagent::echo()
{
    char *buf = new char[m_len + 1];
    memset(buf, 0, m_len + 1);
    struct timeval temp;
    int filefd = 0;
    filefd = open("./file.txt", O_RDONLY); 
    printf("Message: %s\n", strerror(errno));
    if(readFiles(filefd, buf, m_len))
    {
        MsgHeader msg;
        msg.length = m_len;
        gettimeofday(&temp, NULL);  //添加时间
	    msg.clientSend_t = temp;
        PackageMsg(msg, buf);  //将消息打包并放到发送队
    }
    delete[] buf;
    buf = NULL;
    close(filefd);
}

void
clientagent::readBack(InReq &req)  //客户端
{
    MsgHeader temp = req.m_msgHeader;
    uint32_t msgLength = temp.length; //负载长度
    char *buf = new char[msgLength + 1];  //临时缓冲区
    memset(buf, 0, msgLength + 1);   //清空
    memcpy(buf, req.ioBuf, msgLength);   //将负载复制到缓冲区
    printf("收到数据为: %s\n", buf);  //打印收到的数据
    PrintRTT(temp);  //输出往返时间
    echo();  //再次发送数据
}

int
clientagent::PackageMsg( MsgHeader &header, const char *dataStr )
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

void
clientagent::PrintRTT(const MsgHeader &msg)
{
    struct timeval  clientRecv_t;
    float rtt = 0;
    gettimeofday( &clientRecv_t, NULL );
    rtt = (long long)(clientRecv_t.tv_sec - msg.clientSend_t.tv_sec)*1000 + (clientRecv_t.tv_usec - msg.clientSend_t.tv_usec)/1000;
    printf("往返时间为: %.5f ms\n", rtt);
}
