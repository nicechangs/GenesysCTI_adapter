/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : iLog 확장 클래스
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <string.h>
#include <iostream>

#include <iCore/iLog.h>
#include <iCore/iLib.h>
#include <iCore/iIniFile.h>
#include <iCore/iMutex.h>
//#include <iIniFile.h>
//#include <iMutex.h>

using namespace std;

class CLogEx
{
    // constructor && destructor
    public:        
        CLogEx(void);
        virtual ~CLogEx(void);

    // implmentations
    public:
        static bool Init(NPCSTR szPrefix, NPCSTR szLogPath, NPCSTR szLogLinkPath, uint32 nLogLevel);
        static void WRITE(uint32 nLogLvl, NPCSTR szLog);
        static void INF(NPCSTR szFmt, ...);
        static void WRN(NPCSTR szFmt, ...);
        //static void ERR(NPCSTR szFmt, ...);
        static void LV1(NPCSTR szFmt, ...);
        static void LV2(NPCSTR szFmt, ...);
        static void LV3(NPCSTR szFmt, ...);

    protected:
        static iLog     m_log;
};


