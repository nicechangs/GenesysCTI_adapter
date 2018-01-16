/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : CBSessionListener
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
#include <iCore/iThread.h>
#include <iCore/iSocket.h>

#include "TcpSessionBase.h"

using namespace std;


// classes
class CCBSessionListener : public CTcpSessionBase
{
    public:
        // constructor && destructor
        CCBSessionListener(void);
        virtual ~CCBSessionListener(void);

    public:
        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    protected:
        virtual NPVOID  ThreadRun(NPVOID pArg);

    private:
        void LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...);
    
};

