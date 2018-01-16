
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

using namespace std;

class CCBSessionCleaner : public iThread
{
    public:
        CCBSessionCleaner(void);
        virtual ~CCBSessionCleaner(void);

    public:
        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    protected:
        virtual NPVOID ThreadRun(NPVOID pArg);

    public:
        inline void SetName(NPCSTR szName) { strcpy(m_szName, szName); }
        inline NPCSTR GetName()            { return m_szName;          }

    protected:
        char m_szName[30];
};

