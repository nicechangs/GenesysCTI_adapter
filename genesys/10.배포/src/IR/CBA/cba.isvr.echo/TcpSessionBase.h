/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionBase
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

using namespace std;

class CTcpSessionBase : public iThread
{
    public:
        CTcpSessionBase(void);
        virtual ~CTcpSessionBase(void);

    // virtual..
    public:
        virtual void Clear(void);

        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

        virtual bool Init(NPSTR szName, NPSTR szIp, int32 nPort, iFd nSocketFd);
        virtual bool Disconnect();
        virtual int32 Send(NPSTR szPacket, int32 nLength);

    // inlines
    public:
        inline bool IsConnected()          { return m_bIsConnected;    }
        inline NPCSTR GetName()            { return m_szName;          }
        inline NPCSTR GetIp()              { return m_szIp;            }
        inline int32 GetPort()             { return m_nPort;           }

        inline void SetName(NPCSTR szName) { strcpy(m_szName, szName); }
        inline void SetIp(NPCSTR szIp)     { strcpy(m_szIp, szIp);     }
        inline void SetPort(int32 nPort)   { m_nPort = nPort;          }

    // fields..
    protected:
        char        m_szName[30];
        char        m_szIp[30];
        int32       m_nPort;

        iSocket     m_socket;
        iFd         m_socketFd;

        bool        m_bIsConnected;
        iMutex      m_mutexLock;
};
