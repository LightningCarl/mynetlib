#include <cstdio>
#include <iostream>
#include "Center.h"
#include "comm.h"
#include "Epoll.h"
#include "tcplistenagent.h"

using comm::conv::conv; //类型转换
using namespace std;
SocketAddr servaddr;

void doExit(int signo)
{
    if (signo == SIGINT)
    {
        printf("client sigint catch!");
        Epoll::getInstance()->finish();
    }
}

int main(int argc, char *argv[])
{
    if ( argc != 5 )
    {
        cout << "Usage: " << argv[0] << " IP Port clientnum size" << endl;
        return -1;
    }

    if (Epoll::getInstance()->initEpoll(EPOLLSIZE) == false)
    {
        printf("\nIn main: init epoll error!\n");
        return -1;
    }//建立epoll机制及其事件数组
    
    servaddr.setAddr(argv[1],conv<unsigned short, char *>(argv[2]));  //设置服务器通信地址
    center m_center(servaddr);  //创建center类
    m_center.setNum(conv<unsigned short,char*>(argv[3]));  //连接数
    m_center.setSize(conv<unsigned short,char*>(argv[4]));  //报文大小
    m_center.createAgents();   //创建客户端agent
 
    if ( signal(SIGINT, doExit) == SIG_ERR ) //若连接失败，则退出
    {
        printf("\nIn main: set SIGINT(catch \"ctrl+c\") signal error!\n");
        return -1;
    }
    
    Epoll::getInstance()->run(); //执行epoll循环，不停发送数据
     
    //收尾工作
    AgentManager::deleteInstance();
    Epoll::deleteInstance();
    return 0;
}
