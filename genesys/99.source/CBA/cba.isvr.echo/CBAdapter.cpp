/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : Main entry
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#include <signal.h>
#include <iCore/iLib.h>
#include "CBADefines.h"
#include "Config.h"
#include "LogEx.h"
#include "MainThread.h"


void ExitProcess()
{
    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    CLogEx::INF(">> Process Exit. <PrcName=%s, version=%s>", PROC_NAME , PROC_VERSION);
    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    exit(0);
}

int main(int32 argc, char* argv[])
{
    if (argc != 2) 
    {
        printf("usage: %s [PID]\n", argv[0]);
        ExitProcess();
    }


    // init config..
    if(!CConfig::Inst()->Init(PROC_NAME, argv[1]))
        exit(0);

    if(!CConfig::Inst()->ReadConfig(CFG_FILE_FULL_NAME))
        exit(0);

    // init log..
    if(!CLogEx::Init( CConfig::Inst()->GetPrefix()
                     , CConfig::Inst()->GetLogPath()
                     , CConfig::Inst()->GetLogLinkPath()
                     , CConfig::Inst()->GetLogLevel())
                    )
    {
        exit(0);
    }

    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    CLogEx::INF(">> Process startup. <PrcName=%s, version=%s, args=%s>"
                        , PROC_NAME 
                        , PROC_VERSION
                        , argv[1]);

    CConfig::Inst()->Display();
    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    // start main thread
     if(!CMainThread::Inst()->StartUp())
    {
        // log ..
        ExitProcess();
    }

    while(CMainThread::Inst()->IsRunning()) 
    {
        iSleep(ONESEC * 3);
    }

    ExitProcess();
    return 0;
}