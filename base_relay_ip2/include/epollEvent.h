#ifndef EPOLLEVENT_H
#define EPOLLEVENT_H
#include <stdint.h>

class epollEvent
{
public:
    epollEvent() 
    {
        m_event = 0;
        m_indexId =-1;
        m_fd = -1;
    }
    ~epollEvent() {}

    int getFd(void);
    void setFd(int fd)
    {
        m_fd = fd;
        m_event = 0;
    }

    void setIndexID( uint32_t m_id )
    {
        m_indexId = m_id;
    }//设置索引ID

    uint32_t getIndexID( void )
    {
        return m_indexId;
    }//获取索引ID
    int openRevent( void );   //加入IN事件
    int closeRevent( void );  //关闭IN事件

    int openWevent( void );   //加入OUT事件
    int closeWevent( void );  //关闭OUT事件
    
    int registerRevent( void );  //将IN事件加到红黑树上
    int registerWevent( void );  //将OUT事件加到红黑树上

    int registerRWevents( void );  //加载所有事件到树上
    int unregisterRWevents( void ); //从树上卸载所有事件
private:
    int m_event;   //to change the epollevent
    int m_fd;      //to identify the epollevent
    uint32_t m_indexId;   //t identify agent
};

#endif