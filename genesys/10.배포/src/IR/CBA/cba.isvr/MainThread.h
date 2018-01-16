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


#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

// icore..
#include <iCore/iThread.h>

#include "CBADefines.h"

using namespace std;

// classes
class CMainThread : public iThread
{
    public:
        // constructor && destructor
        CMainThread(void);
        virtual ~CMainThread(void);

    protected:
        virtual NPVOID  ThreadRun(NPVOID pArg);

    public:
        // sigleton ..
        static CMainThread* Inst(); 
        static void FreeInst();

        bool StartUp();
        bool StopAll();

    // fields
    private:
        static CMainThread        *m_pInstance;

};


