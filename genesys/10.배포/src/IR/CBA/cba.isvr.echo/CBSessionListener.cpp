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

#include "CBSessionManager.h"
#include "CBSessionListener.h"
#include "CBSession.h"
#include "LogEx.h"



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
        CLogEx::ERR("%s|Server socket open failure.", GetName());
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
        
        CLogEx::ERR("%s|Server socket bind failure. ip:%s, port:%d, ErrNo:%d"
                    , GetName()
                    , GetIp()
                    , GetPort()
                    , m_socket.GetErrNo());

        return NULL;
    }

    if(!m_socket.Listen())
    {
        CLogEx::ERR("%s|Server socket listen failure. ip:%s, port:%d, ErrNo:%d"
                    , GetName()
                    , GetIp()
                    , GetPort()
                    , m_socket.GetErrNo());
        return NULL;
    }

    CLogEx::INF("%s|Server socket listen start !!. ip:%s, port:%d"
                    , GetName()
                    , GetIp()
                    , GetPort());

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
        
        CCBSession* pSession  = new CCBSession();
        if(!pSession->Init(szSessionName, szIpAddr, stAddr.Port(), sockFd))
        {
            CLogEx::ERR("%s|CBSession initialize failure. ip:%s, port:%d"
                    , pSession->GetName()
                    , pSession->GetIp()
                    , pSession->GetPort());
            
            delete pSession;
            continue;
        }
        
        if(!CCBSessionManager::Inst()->AddSession((CTcpSessionBase*)pSession))
        {
            CLogEx::ERR("%s|Failed to add session to CCBSessionManager. ip:%s, port:%d"
                    , pSession->GetName()
                    , pSession->GetIp()
                    , pSession->GetPort());

            delete pSession;
            continue;
        }

        CLogEx::INF("%s|Server accept socket !!. session_count:%d, name:%s, ip:%s, port:%d"
                    , GetName()
                    , CCBSessionManager::Inst()->GetSessionCount()
                    , pSession->GetName()
                    , pSession->GetIp()
                    , pSession->GetPort());

        // start cb session.
        pSession->StartUp(NULL);
        
    }

    CLogEx::ERR("%s|Exit thread. ip:%s, port:%d"
                    , GetName()
                    , GetIp()
                    , GetPort());
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




