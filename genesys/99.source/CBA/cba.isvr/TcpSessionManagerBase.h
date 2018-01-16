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

#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>
#include <list>
#include <string.h>
#include <iostream>

// icore..
#include <iCore/iLib.h>
#include <iCore/iThread.h>
#include <iCore/iSocket.h>
#include <iCore/iMutex.h>

#include "TcpSessionBase.h"

using namespace std;

typedef map<string, CTcpSessionBase*> MAP_TCP_SESSION;

class CTcpSessionManagerBase : public iThread
{
    public:
        CTcpSessionManagerBase(void);
        virtual ~CTcpSessionManagerBase(void);

    // virtual..
    public:
    	virtual bool AddSession(CTcpSessionBase *pTcpSession);
    	virtual CTcpSessionBase* FindSession(NPCSTR strSessionName);
        virtual bool RemoveSession(NPCSTR strSessionName);
        virtual bool RemoveAllSessions();
        virtual MAP_TCP_SESSION GetTcpSessionMap();

        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    // inline..
    public:
        inline void SetName(NPCSTR szName)  { strcpy(m_szName, szName); }
        inline NPCSTR GetName()             { return m_szName; }



        inline int32 GetSessionCount()
        {
            return m_mapTcpSessions.size();
        }

    // fields..
    protected:
        char m_szName[30];
        iMutex m_mutexLock;
        MAP_TCP_SESSION m_mapTcpSessions;
        
    	
};


