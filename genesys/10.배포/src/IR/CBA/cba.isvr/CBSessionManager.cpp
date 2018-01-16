/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : CBSessionManager
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

// include
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iCore/iLib.h>
#include "LogEx.h"
#include "Config.h"
#include "CBSessionManager.h"
#include "CBSessionListener.h"
#include "QueueOfResponse.h"
#include "CBADefines.h"


// constructor && desctructor
CCBSessionManager::CCBSessionManager(void) {}
CCBSessionManager::~CCBSessionManager(void) {}


// singleton
CCBSessionManager* CCBSessionManager::m_pInstance = NULL;
CCBSessionManager* CCBSessionManager::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CCBSessionManager();
    return m_pInstance;
}

void CCBSessionManager::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


/****************************************************************************
 TITLE       : ThreadRun - override
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
NPVOID CCBSessionManager::ThreadRun(NPVOID pArg)
{
    int nLen = 0;
    while(IsOn())
    {
        // queue polling..
        iSleep(50);
        if(CQueueOfResponse::Inst()->GetCount() <= 0)
            continue;

        QueueItem qItem = CQueueOfResponse::Inst()->Dequeue();

        CTcpSessionBase* pSession = FindSession(qItem.szCBSessionName);
        if(pSession == NULL)
        {
            LOG_WRITE(ILOG_ERR, "Not found CBSession. SessionName:%s", qItem.szCBSessionName);
            continue;
        }
            
        nLen = pSession->Send(qItem.szResponsePacket, qItem.nResponsePacketLen);
        LOG_WRITE(ILOG_INFO, "%s:%d|%s <<< %s|REQ_KEY:%s|Data=(%d)%s"
                            , pSession->GetIp()
                            , pSession->GetPort()
                            , pSession->GetName()
                            , PROC_NAME
                            , qItem.szReqKey
                            , nLen
                            , qItem.szResponsePacket);
    }
    
    return 0;
}


/****************************************************************************
 TITLE       : AddSession - override
 DESCRIPTION : Add session
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionManager::AddSession(CTcpSessionBase *pTcpSession)
{
    // 같은 이름의 세션이 존재한다면 삭제후 추가
    CTcpSessionManagerBase::RemoveSession(pTcpSession->GetName());

    // log..
    if(!CTcpSessionManagerBase::AddSession(pTcpSession))
    {
        LOG_WRITE(ILOG_ERR, "Add CBSession Fail. name:%s, ip:%s, port:%d "
                            , pTcpSession->GetName()
                            , pTcpSession->GetIp()
                            , pTcpSession->GetPort());
        return false;
    }
    
    LOG_WRITE(ILOG_INFO, "Add CBSession success. name:%s, ip:%s, port:%d "
                            , pTcpSession->GetName()
                            , pTcpSession->GetIp()
                            , pTcpSession->GetPort());

    return true;
}


/****************************************************************************
 TITLE       : RemoveSession - override
 DESCRIPTION : RemoveSession session
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionManager::RemoveSession(NPCSTR szSessionName)
{
    if(!CTcpSessionManagerBase::RemoveSession(szSessionName))
    {
        CLogEx::WRN("%-20s|Remove CBSession fail. name:%s"
                            , GetName()
                            , szSessionName);
        return false;
    }

    CLogEx::INF("%-20s|Remove CBSession success. name:%s"
                            , GetName()
                            , szSessionName);
    return true;
}


/****************************************************************************
 TITLE       : StartUp - override
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionManager::StartUp(NPVOID pArg)
{
    // start CBSessionManager
    SetName("CBS_MGR");
    //CLogEx::INF("%-20s|Start !!", m_szName);
    if(pArg == NULL) Start();
    else Start(pArg);

    // start CBListener
    m_pCBListener = new CCBSessionListener();
    m_pCBListener->SetName("CBS_LSTNER");
    m_pCBListener->SetIp  (CConfig::Inst()->GetCBListenIp());
    m_pCBListener->SetPort(CConfig::Inst()->GetCBListenPort());
    m_pCBListener->StartUp(NULL);

    // start CBCleaner
    m_pCBCleaner = new CCBSessionCleaner();
    m_pCBCleaner->SetName("CBSessionCleaner");
    m_pCBCleaner->StartUp(this);

    return true;
}


/****************************************************************************
 TITLE       : StopAll - override
 DESCRIPTION : Stop thread
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionManager::StopAll()
{
    return Stop(true);
}


/****************************************************************************
 TITLE       : LOG_WRITE
 DESCRIPTION : 로그메시지에 추가정보 삽입.
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CCBSessionManager::LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    char szLog[2048];
    memset(szLog, 0x00, sizeof(szLog));
    sprintf(szLog, "%-12s|%s", GetName(), buf);

    CLogEx::WRITE(nLogLevel, szLog);
}

