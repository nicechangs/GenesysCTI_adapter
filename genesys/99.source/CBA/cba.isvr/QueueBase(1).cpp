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
#include <string.h>
#include <math.h>
#include <iLib.h>
#include "QueueBase.h"


// constructor && desctructor
CQueueBase::CQueueBase(void){ }
CQueueBase::~CQueueBase(void) { }

// implementation
/****************************************************************************
 TITLE       : Enqueue
 DESCRIPTION : appent queue item
 PARAMS      : QueueItem 

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.07.10
****************************************************************************/
int CQueueBase::Enqueue(QueueItem stItem)
{
    m_mutexLock.Lock();
    m_queue.push(stItem);
    m_mutexLock.Unlock();
    
    return GetCount();
}


// implementation
/****************************************************************************
 TITLE       : Dequeue
 DESCRIPTION : appent queue item
 PARAMS      : QueueItem 

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.07.10
****************************************************************************/
QueueItem CQueueBase::Dequeue()
{
    QueueItem stItem;
    memset(&stItem, 0x00, sizeof(stItem));
    m_mutexLock.Lock();
    stItem = (QueueItem)m_queue.front();
    m_queue.pop();
    m_mutexLock.Unlock();

    return stItem;


    // QueueItem stDest;
    // memset(&stDest, 0x00, sizeof(stDest));

    // strcpy(stDest.szCBSessionName, stSrc.szCBSessionName);
    // strcpy(stDest.szCBSessionIp, stSrc.szCBSessionIp);
    // stDest.nCBSessionPort = stSrc.nCBSessionPort;

    // strcpy(stDest.szIServerSessionName, stSrc.szIServerSessionName);
    // strcpy(stDest.szIServerSessionIp, stSrc.szIServerSessionIp);
    // stDest.nIServerSessionPort = stSrc.nIServerSessionPort;

    // strcpy(stDest.szRequest, stSrc.szRequest);
    // strcpy(stDest.szResponse, stSrc.szResponse);

    // return stDest;
}


/****************************************************************************
 TITLE       : GetCount
 DESCRIPTION : appent queue item
 PARAMS      : QueueItem 

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.07.10
****************************************************************************/
int CQueueBase::GetCount()
{
    int nCnt = 0;
    m_mutexLock.Lock();
    nCnt = m_queue.size();
    m_mutexLock.Unlock();

    return nCnt;
}


