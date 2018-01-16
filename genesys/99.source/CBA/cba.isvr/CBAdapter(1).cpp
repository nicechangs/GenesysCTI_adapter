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
    CLogEx::INF(">> Process Exit. <name=%s, version=%s>", PROC_NAME, PROC_VERSION);
    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    exit(0);
}

int main(int32 argc, char* argv[])
{
    if (argc != 3) 
    {
        printf("usage: %s [PID] [CFG_FILE_FULL_NAME]\n", argv[0]);
        printf("example: %s 10001 ../cfg/cba.isvr.10001.cfg/\n", argv[0]);
        ExitProcess();
    }

    char szCfgFileFullName[128];
    memset(szCfgFileFullName, 0x00, sizeof(szCfgFileFullName));
    strcpy(szCfgFileFullName, argv[2]);

    // init config..
    if(!CConfig::Inst()->Init(PROC_NAME, argv[1], szCfgFileFullName))
    {
        printf("Fail initialize configuration. cfg:%s \n", szCfgFileFullName);
        exit(0);
    }

    if(!CConfig::Inst()->ReadConfig())
    {
        printf("Fail Read Config. cfg:%s \n", szCfgFileFullName);
        exit(0);
    }

    // init log..
    if(!CLogEx::Init( CConfig::Inst()->GetPrefix()
                     , CConfig::Inst()->GetLogPath()
                     , CConfig::Inst()->GetLogLinkPath()
                     , CConfig::Inst()->GetLogLevel())
                    )
    {
        printf("Fail Initialize Log");
        exit(0);
    }

    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    CLogEx::INF(">> Process startup. <PrcName=%s, version=%s, args=%s>"
                        , PROC_NAME 
                        , PROC_VERSION
                        , argv[1]);

    CConfig::Inst()->Display();
    CLogEx::INF(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf(">> Process startup. <PrcName=%s, version=%s, args=%s>", PROC_NAME , PROC_VERSION, argv[1]);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");


    // start main thread
     if(!CMainThread::Inst()->StartUp())
    {
        ExitProcess();
    }

    while(CMainThread::Inst()->IsRunning()) 
    {
        iSleep(ONESEC * 3);
    }

    ExitProcess();
    return 0;
}

