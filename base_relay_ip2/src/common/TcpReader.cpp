#include "TcpReader.h"
#include <cstring>

ssize_t TCPReader:: doRead( char* pBuf, size_t len )
{
    ssize_t rt = doReadInternal( pBuf, len );
    if ( rt < 0 )
    {
        printf( "In Reader::doRead, Reader::doReadInternal error" );
    }
    else if ( 0 == rt )
    {
        printf( "In Reader::doRead, EOF encountered" );
    }

    return rt;
}

ssize_t 
TCPReader::doReadInternal( char* pBuf, size_t len )
{
    return m_socket.read( pBuf, len );
}
