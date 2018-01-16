/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionManagerBase
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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

//#include "QueueBase.h"


#include "Config.h"
#include "QueueOfRequest.h"
#include "QueueOfResponse.h"
#include "CBInterface.h"
#include "IVRDriver.h"
#include "IVRDriverIF.h"
#include "LineCommand.h"

using namespace std;

// port info item..
class CPortInfo
{
    public:
        CPortInfo(void)
        {
            nTESystemID = 0;
            nChannelNo  = 0;
            nIndex      = 0;
            nPort       = 0;
            
            memset(szPortKey, 0x00, sizeof(szPortKey));
            memset(szStimeData, 0x00, sizeof(szStimeData));
            
            ClearQItem();
        }
        virtual ~CPortInfo(void){}


    public:
        void ClearQItem()
        {
            memset(&m_stQItem, 0x00, sizeof(m_stQItem));
            
        }
    // fields..
    public:
        int       nTESystemID;
        int       nChannelNo;
        int       nIndex;
        int       nPort;
        
        char      szPortKey[10];
        char      szStimeData[30];

        QueueItem m_stQItem;
};


// request cti item..
class CCtiReqItem
{
    public:
        CCtiReqItem(void){}
        virtual ~CCtiReqItem(void){}

    // fields..
    public:
        char         m_szCallId[50];
        bool         m_bIsNewCall;
        bool         m_bIsEndCall;
        char         m_szReqCommand[30];
        char         m_szResCommand[30];
        char         m_szEvent[30];
        ilCallStatus m_eCallStatus;

        CPortInfo m_portInfo;
        QueueItem m_stQueueItem;
};


typedef map<string, CPortInfo*>    MAP_PORT_INFO;
typedef map<string, CCtiReqItem*>  MAP_CTI_REQ_INFO;

class CIVRDriverController : public iThread
{
    public:
        CIVRDriverController(void);
        virtual ~CIVRDriverController(void);

    // for sigleton ..
    public:
        static CIVRDriverController* Inst(); 
        static void FreeInst();
    protected:
        static CIVRDriverController *m_pInstance;


    protected:
        virtual NPVOID ThreadRun(NPVOID pArg);
        virtual void Clear(void);

        bool    InitPortInfos();
        bool    InitIVRDriver();

        bool    AddPortInfo(CPortInfo* pPortInfo);
        bool    AddCtiReqInfo(NPCSTR szReqKey, CCtiReqItem* pCtiReqItem);
        bool    RemoveCtiReqInfo(NPCSTR szReqKey);

        CPortInfo* GetPortInfo(int32 nTESystemId, int32 nChannelNo);

    public:
        CCtiReqItem* GetCtiReqInfo(NPCSTR szReqKey);
        bool    NeedNewCall(NPCSTR szReqKey);
        
        // request from CB
        bool    HandleRequest(QueueItem stItem);
        bool    HandleRequest_icNewCall(QueueItem stItem);
        bool    HandleRequest_icMakeCall(QueueItem stItem);
        bool    HandleRequest_icWaitInfo(QueueItem stItem);
        bool    HandleRequest_icRouteRequest(QueueItem stItem);
        bool    HandleRequest_icDeflectTR(QueueItem stItem);
        bool    HandleRequest_icCallInfo(QueueItem stItem);
        bool    HandleRequest_icClearCall(QueueItem stItem);
        bool    HandleRequest_icSendUData(QueueItem stItem);
        bool    HandleRequest_icGetUData(QueueItem stItem);
        bool    HandleRequest_icStatReq(QueueItem stItem);

        // insert response queue
        bool    PushResponse(QueueItem qItem, NPCSTR szUserData);


    // callback functions
    protected:
        static void CALLBACK_PacketSent     (NPCSTR szDriverName, NPCSTR szXmlData);

        static void CALLBACK_PacketReceived (NPCSTR szDriverName, NPCSTR szXmlData);

        static void CALLBACK_LoginReq       (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szVersion
                                            , NPCSTR szClientName
                                            , NPCSTR szReportStatus
                                            , NPCSTR szServerMonitor
                                            , NPCSTR szXmlData);

        static void CALLBACK_LoginResp      (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szIServerVersion
                                            , NPCSTR szResult
                                            , NPCSTR szConfigOptions
                                            , NPCSTR szStatus
                                            , NPCSTR szXmlData);

        static void CALLBACK_NewCall        (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szCalledNum        // ivr port number
                                            , NPCSTR szCallControlMode
                                            , NPCSTR szVersion
                                            , NPCSTR szDNIS
                                            , NPCSTR szANI
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

        static void CALLBACK_CallStatus     (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEvent
                                            , ilCallStatus eCallStatus
                                            , NPCSTR szXmlData);

        static void CALLBACK_RouteRequest   (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRouteDN
                                            , NPCSTR szCED
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

        static void CALLBACK_RouteResponse  (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRouteType
                                            , NPCSTR szDest
                                            , NPCSTR szUEI
                                            , NPCSTR szXmlData);

        static void CALLBACK_OneStepXfer    (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szDestDN
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

        static void CALLBACK_CallInfoReq    (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey                                            
                                            , NPCSTR szXmlData);

        static void CALLBACK_CallInfoResp   (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szANI
                                            , NPCSTR szDNIS
                                            , NPCSTR szCalledNum
                                            , NPCSTR szConnId
                                            , NPCSTR szFirstHomeLocation
                                            , NPCSTR szLastEvent
                                            , NPCSTR szOtherDN
                                            , NPCSTR szOtherQueue
                                            , NPCSTR szOtherTrunk
                                            , NPCSTR szPortDN
                                            , NPCSTR szPortQueue
                                            , NPCSTR szPortTrunk
                                            , NPCSTR szThirdPartyDN
                                            , NPCSTR szTSCallId
                                            , NPCSTR szXmlData);

        static void CALLBACK_EndCallReq     (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData);

        static void CALLBACK_EndCallResp   (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData);
        
        static void CALLBACK_UDataSet      (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szUDataReqId
                                            , NPCSTR szAction
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData);        

        static void CALLBACK_UDataGet      (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szKeys
                                            , NPCSTR szXmlData);

        static void CALLBACK_UDataResp     (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRequestId
                                            , NPCSTR szResult
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData);

        static void CALLBACK_PeekStatReq   (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szStatName
                                            , NPCSTR szXmlData);

        static void CALLBACK_StatResp      (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szResult
                                            , NPCSTR szResultCode
                                            , NPCSTR szXmlData);



    // virtual..
    public:
        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    // inlines
    public:
        inline void SetName(NPCSTR szName) { strcpy(m_szName, szName); }
        inline NPCSTR GetName()            { return m_szName; }

    public:
        void LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...);
        
        
    // fields..        
    protected:
        char        m_szName[30];
        CIVRDriver* m_pDriver;
        iMutex m_mutexLock;

        // 포트 매핑 정보 보관
        MAP_PORT_INFO m_mapPortInfo;

        // CTI 로 요청한 데이터 보관 : 콜백 처리를 위함.
        MAP_CTI_REQ_INFO m_mapCtiReqInfo;
        
};


