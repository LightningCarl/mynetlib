#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include "log.h"
#include "logAgentBuffer.h"

logAgentBuffer::logAgentBuffer(/* args */)
{
}

logAgentBuffer::~logAgentBuffer()
{
}

void
logAgentBuffer::Re_serialPackage(Info* fo)
{
    int filefd = open("./recvPackage.txt", O_WRONLY,  O_APPEND);
    readTolist(filefd, fo);
    //将Info中的反序列化字段清空
    fo->m_fileLen = 0; 
    fo->m_bufLen = 0; 
    fo->m_offset = 0;
    close(filefd);
}

void
logAgentBuffer::readTolist(int fd, Info* fo)
{
    unsigned int bufSize = fo->m_bufLen;
    ssize_t count = 0;  //统计已读长度
    while (count < fo->m_fileLen)
    {
        char *buf = new char[bufSize];
        memset(buf, 0, bufSize + 1);
            
        if ( (readFiles(fd, buf, bufSize)) )
        {
            this->ToSendList(buf, bufSize);
        } //将已反序列化的报文（头部+负载）加载到发送队列
        count++;
    }//反序列化完成
}

bool 
logAgentBuffer:: readFiles(int fd, char* buf, int len)
{
    int n_read = 0;
    int left = len;
    while (left > 0)
    {
        if( (n_read = read(fd, buf, left)) < 0 )
        {
            printf("re_serial_error\n");   //.......
            return false;  //0
        }
        else
        {
            left -= n_read;
        }
    }
    printf("发送的数据为: %s\n", buf);  //打印收到的数据
    return true; //～0
}
