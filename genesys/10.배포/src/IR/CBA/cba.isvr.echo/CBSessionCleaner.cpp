/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : CBSessionCleaner
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

#include "TcpSessionManagerBase.h"
#include "CBSessionManager.h"
#include "TcpSessionBase.h"
#include "CBSessionCleaner.h"


// constructor && desctructor
CCBSessionCleaner::CCBSessionCleaner(void) 
{
    memset(m_szName, 0x00, sizeof(m_szName));

}

CCBSessionCleaner::~CCBSessionCleaner(void) {}

/****************************************************************************
 TITLE       : ThreadRun - override
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
NPVOID CCBSessionCleaner::ThreadRun(NPVOID pArg)
{
    CCBSessionManager *pMgr = (CCBSessionManager*)pArg;
    if(pMgr == NULL)
        return 0;

    list<CTcpSessionBase*> lstRemoveSessions;
    while(IsOn())
    {
        iSleep(ONESEC * 5);
        lstRemoveSessions.clear();

        MAP_TCP_SESSION tcpSessionsMap = pMgr->GetTcpSessionMap();
        MAP_TCP_SESSION::iterator itor;
        for(itor=tcpSessionsMap.begin(); itor != tcpSessionsMap.end(); itor++)
        {
            CTcpSessionBase *session = (CTcpSessionBase*)itor->second;
            if(session == NULL)
                continue;

            // reservated remove session
            if(!session->IsRunning() || !session->IsConnected())
                lstRemoveSessions.push_back(session);
        }

        if(lstRemoveSessions.size() <= 0)
            continue;

        list<CTcpSessionBase*>::iterator rItor;
        for(rItor=lstRemoveSessions.begin(); rItor != lstRemoveSessions.end(); rItor++)
        {
            CTcpSessionBase *session = *rItor;
            pMgr->RemoveSession(session->GetName());
        }
    }

    
    return 0;
}

/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionCleaner::StartUp(NPVOID pArg)
{
	//CLogEx::INF("%s|Start !!", GetName());

	//CLogEx::INF
    if(pArg == NULL)
        return Start();
    else
        return Start(pArg);
}


/****************************************************************************
 TITLE       : StopAll
 DESCRIPTION : Stop thread
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSessionCleaner::StopAll()
{
    return Stop(true);
}


