#ifndef TCPWRITER_H
#define TCPWRITER_H

#include <sys/uio.h>
#include <list>
#include "tcpSocket.h"

struct Iov;

class TCPWriter
{
public:
    explicit TCPWriter( const TCPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~TCPWriter()
    {

    }
    
    int doWrite( std::list<Iov>* pSendIovList );
    void changeRecver(const TCPSocket &sock); //由bufferManager读完头部后调用，修改 m_socket
protected:
    virtual ssize_t doWriteInternal(const struct iovec* pIovec, int iovcnt )
    {
        return m_socket.writev( pIovec, iovcnt );  //将数组中的报文写到套接字
    }

private:
    TCPSocket m_socket;  //将完整报文写入套接字    改为目标套接字
};
#endif /* TcpWriter_h */
