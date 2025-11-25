#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <list>
#include <cstring>
#include <sys/uio.h>
#include <sys/types.h>
#include "socketAddr.h"
#include "InPackage.h"
#include "ReadCallBack.h"
#include "TcpReader.h"
#include "TcpWriter.h"
struct Info;

struct Iov
{
    Iov()
    {
        m_pCompleteBuffer = NULL;
        memset( &m_Iovec, 0, sizeof( m_Iovec ) );
    }

    Iov( char* pBuf, size_t len)
    {
        m_Iovec.iov_base = pBuf;   //缓冲区
        m_Iovec.iov_len = len;     //长度
        m_pCompleteBuffer = pBuf;  //回收指针？
    }

    virtual ~Iov()
    {
        m_Iovec.iov_base = NULL;
        m_Iovec.iov_len = 0;
        m_pCompleteBuffer = NULL;
    }

    struct iovec m_Iovec;
    char* m_pCompleteBuffer;  //析构Iov
};


class BufferManager
{
public:
    BufferManager();
    ~BufferManager();

    int readTCP();   //从套接字读头部和负载
    int writeToSocket();  //将发送队列的报文写到套接字
    void ToSendList(char* pBuf, ssize_t len);  //将报文添加到发送列表末尾
    int getSendBufferLength() const
    {
        return m_SendIovList.size();
    } //获取发送队列的大小

    void setReader( TCPReader* pReader );
    void setWriter( TCPWriter* pWriter );
    void setReadCallback( ReadCallback* pReadCallback );
    bool serialPackage(Info* fo);      //序列化发送队列：修改偏移量和长度
    ssize_t writen(int fd);
    bool changeRever(const TCPSocket &sock);
protected:
    void releaseSendBuffer();  //释放缓冲区资源
    int  readTCPHeader();   //读头部
    int  readTCPContent();  //读负载
private:
    std::list<Iov> m_SendIovList; //发送队列
    struct InReq m_inReq;  //保存新报文的头部信息和负载首址
    uint32_t m_nReadOffset;  //报文读取偏移量
    uint32_t m_nHeaderSize;  //头部大小
    uint32_t m_nContentLength;  //负载长度
    
    bool m_bReadHeader;   //是否已读头部
    bool m_bNewPackage;   //是否是新到报文
    
    TCPReader* m_pReader;  //从套接字读数据
    TCPWriter* m_pWriter;  //将报文写入套接字
    ReadCallback* m_pReadCallback;  //读回调函数
};
#endif /* bufferManager_h */
