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
#include "QueueOfResponse.h"

// constructor && desctructor
CQueueOfResponse::CQueueOfResponse(void){ }
CQueueOfResponse::~CQueueOfResponse(void) { }


// singleton
CQueueOfResponse* CQueueOfResponse::m_pInstance = NULL;
CQueueOfResponse* CQueueOfResponse::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CQueueOfResponse();
    return m_pInstance;
}

void CQueueOfResponse::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


