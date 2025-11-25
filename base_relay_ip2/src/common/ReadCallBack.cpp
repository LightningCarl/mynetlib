#include <string>
#include "tcpagent.h" 
#include "ReadCallBack.h"
#include "InPackage.h"

int 
ReadCallback::run( InReq& req )
{
    if ( NULL == m_pAgent )
    {
        printf("In ReadCallback::run, m_pAgent == NULL" );
        return -1;
    }

    m_pAgent->readBack( req );
    return 0;
}