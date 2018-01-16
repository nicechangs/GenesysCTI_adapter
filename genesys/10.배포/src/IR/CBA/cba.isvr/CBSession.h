/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : CCBSession
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

#include "CBInterface.h"
#include "QueueBase.h"
#include "QueueOfRequest.h"


using namespace std;

class CCBSession : public CTcpSessionBase
{
    public:
        CCBSession(void);
        virtual ~CCBSession(void);

    // override
    public:
        bool StartUp(NPVOID pArg);
        bool StopAll();

    // implementation
    public:
        bool HandlePacket(QueueItem stQItem);
        bool HandleHeartBeat(QueueItem stQItem);
        bool HandleRegister(QueueItem stQItem);
        bool HandleReqData(QueueItem stQItem);

    private:
        void LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...);
        
    protected:
        virtual NPVOID ThreadRun(NPVOID pArg);
};

