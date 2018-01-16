/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : MainThread
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/


// include
#include <math.h>
#include "MainThread.h"
#include "Config.h"
#include "CBAGlobal.h"
#include "TcpSessionManagerBase.h"
#include "CBSessionManager.h"
#include "IVRDriverController.h"


// constructor && desctructor
CMainThread::CMainThread(void)
{ 
    m_pInstance = NULL;
}

CMainThread::~CMainThread(void) { }

// singleton
CMainThread* CMainThread::m_pInstance = NULL;
CMainThread* CMainThread::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CMainThread();
    return m_pInstance;
}

void CMainThread::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

// implementation
/****************************************************************************
 TITLE       : ThreadRun
 DESCRIPTION : main loop

 PARAMS      : 
 RETURN      : true / false

 WRITER      : KIMCG
 DATE        : 2014.03.13
****************************************************************************/
NPVOID CMainThread::ThreadRun(NPVOID pArg)
{
    iSleep(ONESEC);
    while(IsOn())
    {
        iSleep(ONESEC * 3);
        if(!CIVRDriverController::Inst()->IsRunning())
            break;

        if(!CCBSessionManager::Inst()->IsRunning())
            break;
    }
    
    return 0;
}

/****************************************************************************
 TITLE       : start main thread..
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/ 
bool CMainThread::StartUp()
{
    //CLogEx::INF("%-25s|Start main thread", PROC_NAME);
    Start();

    // start CBSessionManager
    CCBSessionManager::Inst()->StartUp(NULL);

    // start IServerSessionManager
    CIVRDriverController::Inst()->StartUp(NULL);

    return true;
}


/****************************************************************************
 TITLE       : Maint thread initialize..
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/
bool CMainThread::StopAll()
{
    Stop(true);
    return true;
}

