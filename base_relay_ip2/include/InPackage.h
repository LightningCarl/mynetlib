#ifndef INPACKAGE_H
#define INPACKAGE_H

//#include <netinet/in.h>
#include "MsgHeader.h"

struct InReq
{
    MsgHeader m_msgHeader;  //头部
    char* ioBuf;            //指向接收新负载的缓冲区的指针
}; //服务器接收的每个消息都有一个此结构

#endif
