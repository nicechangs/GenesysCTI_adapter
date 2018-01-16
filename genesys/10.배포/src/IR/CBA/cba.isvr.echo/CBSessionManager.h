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

#include "TcpSessionManagerBase.h"
#include "CBSessionListener.h"
#include "CBSessionCleaner.h"

using namespace std;

class CCBSessionManager : public CTcpSessionManagerBase
{
    // constructor && destructor
    public:
        CCBSessionManager(void);
        virtual ~CCBSessionManager(void);

    // for sigleton ..
    public:
        static CCBSessionManager* Inst(); 
        static void FreeInst();
    protected:
        static CCBSessionManager *m_pInstance;
    
    // override
    public:
        bool AddSession(CTcpSessionBase *pTcpSession);        
        bool RemoveSession(NPCSTR szSessionName);
        bool StartUp(NPVOID pArg);
        bool StopAll();

    protected:
        virtual NPVOID ThreadRun(NPVOID pArg);

    // fields..
    protected:
        CCBSessionListener* m_pCBListener;
        CCBSessionCleaner* m_pCBCleaner;
};

