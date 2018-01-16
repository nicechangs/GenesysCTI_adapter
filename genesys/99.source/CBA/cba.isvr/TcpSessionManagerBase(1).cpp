/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionManagerBase
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

// constructor && desctructor
CTcpSessionManagerBase::CTcpSessionManagerBase(void) {}
CTcpSessionManagerBase::~CTcpSessionManagerBase(void) {}


/****************************************************************************
 TITLE       : AddSession
 DESCRIPTION : Add Tcp session
 PARAMS      : TcpSessionBase pointer
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionManagerBase::AddSession(CTcpSessionBase *pSession)
{
    m_mutexLock.Lock();
    m_mapTcpSessions.insert(make_pair(string(pSession->GetName()), pSession));
    m_mutexLock.Unlock();
    return true;
}


/****************************************************************************
 TITLE       : FindSession
 DESCRIPTION : find tcp session
 PARAMS      : session name
 RETURN      : tcp session
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
CTcpSessionBase* CTcpSessionManagerBase::FindSession(NPCSTR szSessionName)
{
    CTcpSessionBase *session = NULL;
    
    MAP_TCP_SESSION::iterator itor;
    m_mutexLock.Lock();
    itor = m_mapTcpSessions.find( string(szSessionName) );
    
    if(itor != m_mapTcpSessions.end())
        session = (CTcpSessionBase *)itor->second;
    
    m_mutexLock.Unlock();

    return session;
}


/****************************************************************************
 TITLE       : RemoveSession
 DESCRIPTION : remove tcp session
 PARAMS      : session name
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionManagerBase::RemoveSession(NPCSTR szSessionName)
{
    CTcpSessionBase *session = FindSession(szSessionName);
    if(session == NULL)
        return true;

    session->StopAll();
    delete session;

    m_mutexLock.Lock();
    m_mapTcpSessions.erase(string(szSessionName));
    m_mutexLock.Unlock();

    return true;
}


/****************************************************************************
 TITLE       : RemoveAllSession
 DESCRIPTION : remove all tcp session
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionManagerBase::RemoveAllSessions()
{
    return true;
}

/****************************************************************************
 TITLE       : GetTcpSessionMap
 DESCRIPTION : Get copied tcp session map.
 PARAMS      : 
 RETURN      : MAP_TCP_SESSION
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
MAP_TCP_SESSION CTcpSessionManagerBase::GetTcpSessionMap()
{
    MAP_TCP_SESSION ClonedMap;
    m_mutexLock.Lock();
    ClonedMap.insert(m_mapTcpSessions.begin(), m_mapTcpSessions.end());
    m_mutexLock.Unlock();

    return ClonedMap;
}


/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionManagerBase::StartUp(NPVOID pArg)
{
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
bool CTcpSessionManagerBase::StopAll()
{
    return Stop(true);
}


