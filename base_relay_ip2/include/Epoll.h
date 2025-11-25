#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "singleton.h"

class epollEvent;   //doEvent()的参数要用到

class Epoll:public singleton<Epoll>
{
public:
    friend class singleton<Epoll>;
    bool initEpoll(int fd_size);  //建立epoll机制和活跃事件反馈数组
    bool doEvent(epollEvent *ptr, int fd, int operate, unsigned int event);               //更改事件
    void run(void);               //epoll循环
    void finish(void);            //结束本次活跃事件处理的循环
    virtual int doTask( void );   //清除本次活跃事件处理的agent
private:
    Epoll();
    ~Epoll();
    struct epoll_event*  m_events;     //指向epoll活跃事件数组
    int    m_epollFd;    //标识红黑树根节点
    int    m_eventSize;  //事件数组大小
    bool   m_bFinished;  //结束标识
};

#endif /* Epoll_h */
