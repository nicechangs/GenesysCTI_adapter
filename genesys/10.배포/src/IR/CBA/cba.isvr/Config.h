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
        bool Init(NPCSTR szProcName, NPCSTR szPid, NPCSTR szCfgFileFullName);
        bool ReadConfig();
        void Display();


    // inlines
    public:
        inline NPCSTR GetProcessName()         { return m_szProcName;         }
        inline NPCSTR GetProcessId()           { return m_szProcId;           }

        inline NPCSTR GetCBListenIp()          { return m_szCBListenIp;       }
        inline uint32 GetCBListenPort()        { return m_nCBListenPort;      }

        inline NPCSTR GetPrefix()              { return m_szLogPrefix;        }
        inline NPCSTR GetLogPath()             { return m_szLogPath;          }
        inline NPCSTR GetLogLinkPath()         { return m_szLogLinkPath;      }
        inline uint32 GetLogLevel()            { return m_nLogLevel;          }

        inline NPCSTR GetDrvRemoteIp()         { return m_szDrvRemoteIp;      }
        inline uint32 GetDrvRemotePort()       { return m_nDrvRemotePort;     }
        inline NPCSTR GetDrvObjectSvcName()    { return m_szDrvSvcObjectName; }
        inline NPCSTR GetDrvSvcAppName()       { return m_szDrvSvcAppName;    }
        inline uint32 GetDrvKeepAliveInterval(){ return m_nKeepAliveInterval; }
        
        inline NPCSTR GetDrvTESystemId()       { return m_szDrvTESystemId;    }
        inline uint32 GetDrvStartChannel()     { return m_nDrvStartChannel;   }
        inline uint32 GetDrvStartPort()        { return m_nDrvStartPort;      }
        inline uint32 GetDrvDNISCount()        { return m_nDrvDNISCount;      }

        inline NPCSTR GetDrvLogPath()          { return m_szDrvLogPath;       }
        inline NPCSTR GetDrvLogLinkPath()      { return m_szDrvLogLinkPath;   }
        inline uint32 GetDrvLogLevel()         { return m_nDrvLogLevel;       }


    // fields..
    protected:
        // process infos
        char        m_szProcName[50];
        char        m_szProcId[25];
        char        m_szCfgFileFullName[128];
        
        // system infos..
        char        m_szSystemType[50]; 
        char        m_szSystemId[25];
        
        // CBA infos
        char        m_szCBListenIp[30]; 
        uint32      m_nCBListenPort;
        uint32      m_nLogLevel;
        char        m_szLogPrefix[50];
        char        m_szLogPath[128];
        char        m_szLogLinkPath[128];

        // Driver Infos
        char        m_szDrvRemoteIp[50];
        uint32      m_nDrvRemotePort;
        char        m_szDrvSvcObjectName[50];
        char        m_szDrvSvcAppName[50];
        uint32      m_nKeepAliveInterval;

        char        m_szDrvTESystemId[25];
        uint32      m_nDrvStartChannel;
        uint32      m_nDrvStartPort;
        uint32      m_nDrvDNISCount;

        uint32      m_nDrvLogLevel;
        char        m_szDrvLogPath[128];
        char        m_szDrvLogLinkPath[128];

        //list<DriverInfo> m_lstDriverInfos;

};

