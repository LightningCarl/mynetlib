#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <sys/types.h>
#include <string>
#include <sys/time.h>

const int UNCONNECT = 0;  //客户端未连接
const int CONNECTED = 1;   //客户端已连接
const int CONNECTFAILED = -1;  //连接失败
const int MAXRECONNECTTIMES = 3;  //最大重连次数

const unsigned int HEADER_SIZE = sizeof( uint32_t ) + sizeof(timeval) + sizeof(string);  //头部20

struct MsgHeader
{
    uint32_t length;  //指示负载大小
    struct timeval  clientSend_t = {};
    string m_RecverIp;

    MsgHeader() :
        length( 0 ),
        m_RecverIp( "0" )
    {
    }

    ~MsgHeader()
    {
    }
};

#endif
