/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : Main entry
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

// include
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iLib.h>
#include <iLog.h>
#include <list>
#include <iSocket.h>

#include "CBAGlobal.h"
#include "CBSessionManager.h"
#include "CBSessionListener.h"
#include "CBSession.h"



// constructor && desctructor
CCBSessionListener::CCBSessionListener(void)
{
    Clear();
}

CCBSessionListener::~CCBSessionListener(void){}

/****************************************************************************
 TITLE       : ThreadRun
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : true / false

 WRITER      : KIMCG
 DATE        : 2014.03.13
****************************************************************************/
NPVOID CCBSessionListener::ThreadRun(NPVOID pArg)
{
    iFd         sockFd = 0;
    iSockAddr   stAddr;
    char        szIpAddr[30];
    char        szSessionName[30];

    if(!m_socket.Open(AF_INET, SOCK_STREAM))
    {
        LOG_WRITE(ILOG_ERR, "Server socket open failure.");
        return NULL;
    }

    m_socket.SetLinger(true, 0);

    bool bBindRet = false;
    if( strlen(GetIp()) <= 0 )
        bBindRet = m_socket.Bind(GetPort());
    else
        bBindRet = m_socket.Bind(GetPort(), GetIp());

    if(!bBindRet)
    {
        LOG_WRITE(ILOG_ERR, "Server socket bind failure. ip:%s, port:%d, ErrNo:%d"
                    , GetName()
                    , GetIp()
                    , GetPort()
                    , m_socket.GetErrNo());
        
        return NULL;
    }

    if(!m_socket.Listen())
    {
        LOG_WRITE(ILOG_ERR, "Server socket listen failure. ip:%s, port:%d, ErrNo:%d"
                    , GetName()
                    , GetIp()
                    , GetPort()
                    , m_socket.GetErrNo());

        return NULL;
    }

    LOG_WRITE(ILOG_INFO, "Server socket listen start !!. ip:%s, port:%d"                    
                    , GetIp()
                    , GetPort()
                    );

    while(IsOn())
    {
        iSleep(ONESEC);
        sockFd = m_socket.Accept(&stAddr);
        if(sockFd <= 0)
            continue;

        //create cb session.
        memset(szSessionName, 0x00, sizeof(szSessionName));
        sprintf(szSessionName, "CB_SESSION%d", sockFd);
        stAddr.IpAddr(szIpAddr, sizeof(szIpAddr));

        LOG_WRITE(ILOG_INFO, "Accept Server Socket !!. name:%s, ip:%s, port:%d"
                    , szSessionName
                    , szIpAddr
                    , stAddr.Port());
        
        CCBSession* pSession  = new CCBSession();
        if(!pSession->Init(szSessionName, szIpAddr, stAddr.Port(), sockFd))
        {
            LOG_WRITE(ILOG_ERR, "%s initialize failure. ip:%s, port:%d"
                    , pSession->GetName()
                    , pSession->GetIp()
                    , pSession->GetPort());

            delete pSession;
            continue;
        }
        
        if(!CCBSessionManager::Inst()->AddSession((CTcpSessionBase*)pSession))
        {
            LOG_WRITE(ILOG_ERR, "Failed to add session to CCBSessionManager. name:%s, ip:%s, port:%d"
                    , pSession->GetName()
                    , pSession->GetIp()
                    , pSession->GetPort());            

            delete pSession;
            continue;
        }

        // start cb session.
        pSession->StartUp(NULL);        
    }

    LOG_WRITE(ILOG_ERR, "Exit Thread.");

    return 0;
}


/****************************************************************************
 TITLE       : StartUp - override
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionListener::StartUp(NPVOID pArg)
{
    //CLogEx::INF("%s|Start !!", GetName());
    if(pArg == NULL) Start();
    else Start(pArg);

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
bool CCBSessionListener::StopAll()
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
void CCBSessionListener::LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...)
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

    g_iLog.Log(nLogLevel, szLog);
}

