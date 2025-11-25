#ifndef LGAGENTBUFFER_H
#define LGAGENTBUFFER_H
#include "BufferManager.h"
struct Info;

class logAgentBuffer:public BufferManager{
public:
    void Re_serialPackage(Info* fo);   //反序列化发送队列：将数据写回到发送队列
    void readTolist(int fd, Info* fo);
    bool readFiles(int fd, char* buf, int len);
    //void ToSendList(char* pBuf, ssize_t len);
    //int writeToSocket();  

    logAgentBuffer();
    ~logAgentBuffer();
};

#endif