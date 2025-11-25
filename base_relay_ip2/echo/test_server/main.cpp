#include "comm.h"
#include "serveragent.h"
#include "tcplistenagent.h"
#include "agentManager.h"
#include "Epoll.h"

TCPListenAgent<serverAgent>  *g_pListenSA = NULL;
SocketAddr servaddr;
using comm::conv::conv; //类型转换

void doExit(int signo)
{
     {
    if (signo == SIGINT)
        printf("server sigint catch!");
        Epoll::getInstance()->finish();
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " Port" << endl;
        return -1;
    } //获取通信端口

    if (Epoll::getInstance()->initEpoll(EPOLLSIZE) == false)   //always false
    {
        printf("\nIn main: init epoll error!\n");
        return -1;
    } //建立epoll机制及其事件数组

    servaddr.setPort(conv<unsigned short,char*>(argv[1])); //设置通信端口

    g_pListenSA = (AgentManager::getInstance())->createAgent<TCPListenAgent<serverAgent>>(servaddr); //创建监听agent
    g_pListenSA->init();  //初始化监听agent

    if (signal(SIGINT, doExit) == SIG_ERR)  //收到出错信号
    {
        printf("\nIn main: set SIGINT(catch \"ctrl+c\") signal error!\n");
        return -1;
    }
    Epoll::getInstance()->run(); //执行epoll循环，会调用agent尽心各种事件下的请求处理

    //收尾工作
    AgentManager::deleteInstance();
    Epoll::deleteInstance();
    return 0;
}
