/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge interface library for Genesys IVR Server.

        TITLE   : Keep Alive Check thread
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

// icore lib..
#include <iCore/iLib.h>
#include <iCore/iThread.h>
#include <iCore/iSocket.h>


using namespace std;


class CKeepAlive : public iThread
{
    // constructor && destructor
    public:
        CKeepAlive(void);
        virtual ~CKeepAlive(void);

    // thread control..
    public:
        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    public:
        virtual NPVOID ThreadRun(NPVOID pArg);

    public:
        uint32 m_nInterval;
        bool m_bContinue;
};