/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionBase
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/


// include
#include <math.h>
#include <iLib.h>
#include <iLog.h>
#include "QueueOfRequest.h"

// constructor && desctructor
CQueueOfRequest::CQueueOfRequest(void){ }
CQueueOfRequest::~CQueueOfRequest(void) { }


// singleton
CQueueOfRequest* CQueueOfRequest::m_pInstance = NULL;
CQueueOfRequest* CQueueOfRequest::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CQueueOfRequest();
    return m_pInstance;
}

void CQueueOfRequest::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


