#ifndef TCPREADER_H
#define TCPREADER_H

#include <stdio.h>
#include <sys/types.h>
#include "tcpSocket.h"

class TCPReader
{
public:
    explicit TCPReader( const TCPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~TCPReader()
    {

    }
    
    ssize_t doRead( char* pBuf, size_t len );

protected:
    virtual ssize_t doReadInternal( char* pBuf, size_t len );
private:
    TCPSocket m_socket;
};
#endif /* TcpReader_h */
