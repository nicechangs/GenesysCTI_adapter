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
    // if(m_pCBListener != NULL)
    // {
    //     m_pCBListener->StopAll();
    //     delete m_pCBListener;
    //     m_pCBListener = NULL;
    // }

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
    while(IsOn())
    {
        // queue polling..
        iSleep(ONESEC * 5);
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
        CLogEx::WRN("%s|Add CBSession fail. name:%s, ip:%s, port:%d "
                            , GetName()
                            , pTcpSession->GetName()
                            , pTcpSession->GetIp()
                            , pTcpSession->GetPort());
        return false;
    }

    CLogEx::INF("%s|Add CBSession success. name:%s, ip:%s, port:%d "
                            , GetName()
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
        CLogEx::WRN("%s|Remove CBSession fail. name:%s"
                            , GetName()
                            , szSessionName);
        return false;
    }

    CLogEx::INF("%s|Remove CBSession success. name:%s"
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
    SetName("CBSessionManager");
    //CLogEx::INF("%s|Start !!", m_szName);
    if(pArg == NULL) Start();
    else Start(pArg);

    // start CBListener
    m_pCBListener = new CCBSessionListener();
    m_pCBListener->SetName("CBSessionListener");
    m_pCBListener->SetIp  (CConfig::Inst()->GetListenIp());
    m_pCBListener->SetPort(CConfig::Inst()->GetListenPort());
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

