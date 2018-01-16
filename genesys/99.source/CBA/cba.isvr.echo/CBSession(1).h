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


#include "IVRDriverIF.h"

#include "XmlItem.h"
        

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

        bool HandlePacket(NPCSTR szPacket, int32 nLength);
        bool HandlePacket_LoginResp(CXmlItem xmlItem);
        bool HandlePacket_NewCall(CXmlItem xmlItem);
        bool HandlePacket_RouteRequest(CXmlItem xmlItem);
        bool HandlePacket_OneStepXfer(CXmlItem xmlItem);
        bool HandlePacket_CallInfoResp(CXmlItem xmlItem);
        bool HandlePacket_UDataResp(CXmlItem xmlItem);
        bool HandlePacket_EndCall(CXmlItem xmlItem);
        

        void CombineGLIHeader(NPSTR szSrcPacket, int32 nSrcLength, NPSTR szDestPacket, int32 nDestLength);


    protected:
        virtual NPVOID ThreadRun(NPVOID pArg);
};
