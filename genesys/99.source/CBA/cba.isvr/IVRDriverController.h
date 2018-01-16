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

#define GCTI_REQKEY_SIZE        (64)
#define GCTI_COMMAND_SIZE       (32)
#define GCTI_EVENT_SIZE         (32)
#define GCTI_PORT_KEY_SIZE      (16)
#define GCTI_PORT_SIZE          (8)
#define GCTI_UDATA_KEY_SIZE     (128)
#define GCTI_UDATA_SIZE         (2048)
#define GCTI_EXDATA_SIZE        (2048)
#define GCTI_CED_SIZE           (1024)
#define GCTI_REQUESTID_SIZE     (64)
#define GCTI_ACTION_SIZE        (32)
#define GCTI_STAT_NAME_SIZE     (64)

#define MAX_TENO_SIZE           (8)
#define MAX_CHNO_SIZE           (8)
#define MAX_DNIS_SIZE           (20)
#define MAX_ANI_SIZE            (20)
#define MAX_PACKET_BODY_SIZE    (4096)
#define MAX_PACKET_LEN_SIZE     (10)
#define MAX_DN_SIZE             (20)



typedef struct _GCTI_PORT
{
    char szPortKey[GCTI_PORT_KEY_SIZE];
    int  nTeSysID;
    int  nChnlId;
    int  nIdx;
    int  nGCtiPort;
}GCTI_PORT;

typedef struct _GCTI_ITEM
{
    char szReqKey[GCTI_REQKEY_SIZE];
    int nGctiPort;
    char szReqCmd[GCTI_COMMAND_SIZE];
    char szResCmd[GCTI_COMMAND_SIZE];
    char szEvent[GCTI_EVENT_SIZE];
    ilCallStatus eCallStatus;
    QUEUE_ITEM stQItem;
}GCTI_ITEM;


typedef map<string, GCTI_PORT>  MAP_GCTI_PORT;
typedef map<string, GCTI_ITEM>  MAP_GCTI_ITEM;


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

        bool AddGCtiPort(GCTI_PORT stCtiPort);        
        bool GetGCtiPort(int32 nTESystemId, int32 nChannelNo, GCTI_PORT& stPort);
        bool UpdateGCtiPort(GCTI_PORT stCtiPort);

    public:
        bool AddGCtiItem(NPCSTR szReqKey, GCTI_ITEM stGctiItem);
        bool GetGCtiItem(NPCSTR szReqKey, GCTI_ITEM& stGctiItem);
        bool RemoveGCtiItem(NPCSTR szReqKey);
        bool UpdateGCtiItem(GCTI_ITEM stGctiItem);




        // request from CB
        bool    HandleRequest(QUEUE_ITEM stItem);
        bool    HandleRequest_icNewCall(QUEUE_ITEM stItem);
        bool    HandleRequest_icMakeCall(QUEUE_ITEM stItem);
        bool    HandleRequest_icWaitInfo(QUEUE_ITEM stItem);
        bool    HandleRequest_icRouteRequest(QUEUE_ITEM stItem);
        bool    HandleRequest_icDeflectTR(QUEUE_ITEM stItem);
        bool    HandleRequest_icCallInfo(QUEUE_ITEM stItem);
        bool    HandleRequest_icClearCall(QUEUE_ITEM stItem);
        bool    HandleRequest_icSendUData(QUEUE_ITEM stItem);
        bool    HandleRequest_icGetUData(QUEUE_ITEM stItem);
        bool    HandleRequest_icStatReq(QUEUE_ITEM stItem);

        // insert response queue
        bool    PushResponse(QUEUE_ITEM qItem, NPCSTR szUserData);


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
        void LOG_WRITE(uint32 nLogLevel, int32 nCtiPort, NPCSTR szCmd, NPCSTR szFmt, ...);
        
        
    // fields..        
    protected:
        char        m_szName[30];
        CIVRDriver* m_pDriver;
        iMutex m_mutexLock;


        // 포트 매핑 정보 보관
        MAP_GCTI_PORT m_mapGCTIPort;
        
        // CTI 로 요청한 데이터 보관 : 콜백 처리를 위함.
        MAP_GCTI_ITEM m_mapGCTIItem;
};


