/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : 프로세스 환경설정 클래스
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

using namespace std;


// class
class CConfig
{
    // constructor && destructor
    public:        
        CConfig(void);
        virtual ~CConfig(void);


    // for singleton
    public:
        static CConfig* Inst(); 
        static void FreeInst();
    protected:
        static CConfig *m_pInstance;

    // implementations
    protected:
        void Clear();

    public:
        bool Init(NPCSTR szProcessName, NPCSTR szPid);
        bool ReadConfig(NPCSTR szCFGFile);
        void Display();


    // inlines
    public:
        inline NPCSTR GetListenIp()          { return m_szListenIp;         }
        inline uint32 GetListenPort()        { return m_nListenPort;        }

        inline NPCSTR GetPrefix()              { return m_szLogPrefix;          }
        inline NPCSTR GetLogPath()             { return m_szLogPath;            }
        inline NPCSTR GetLogLinkPath()         { return m_szLogLinkPath;        }
        inline uint32 GetLogLevel()            { return m_nLogLevel;            }


    // fields..
    protected:
        char        m_szAppName[50];
        char        m_szProcessId[25];
        
        char        m_szListenIp[30]; 
        uint32      m_nListenPort;

        uint32      m_nLogLevel;
        char        m_szLogPrefix[50];
        char        m_szLogPath[128];
        char        m_szLogLinkPath[128];
};