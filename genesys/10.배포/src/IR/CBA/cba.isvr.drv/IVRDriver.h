/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge interface library for Genesys IVR Server.

        TITLE   : Main entry
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
#include <iCore/iLog.h>
#include <iCore/iLib.h>
#include <iCore/iThread.h>
#include <iCore/iSocket.h>

#include "XmlItem.h"
#include "KeepAlive.h"
#include "IVRDriverIF.h"


using namespace std;

// define callback function
typedef void (*CALLBACK_PacketSent)         (NPCSTR szDriverName, NPCSTR szData);

typedef void (*CALLBACK_PacketReceived)     (NPCSTR szDriverName, NPCSTR szData);

typedef void (*CALLBACK_LoginReq)           (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szVersion
                                            , NPCSTR szClientName
                                            , NPCSTR szReportStatus
                                            , NPCSTR szServerMonitor
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_LoginResp)          (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szIServerVersion
                                            , NPCSTR szResult
                                            , NPCSTR szConfigOptions
                                            , NPCSTR szStatus
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_NewCall)            (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szCalledNum        
                                            , NPCSTR szCallControlMode
                                            , NPCSTR szVersion
                                            , NPCSTR szDNIS
                                            , NPCSTR szANI
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_CallStatus)         (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEvent
                                            , ilCallStatus eCallStatus
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_RouteRequest)       (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRouteDN
                                            , NPCSTR szCED
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_RouteResponse)      (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRouteType
                                            , NPCSTR szDest
                                            , NPCSTR szUEI
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_OneStepXfer)        (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szDestDN
                                            , NPCSTR szUDataEx
                                            , NPCSTR szExtnsEx
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_CallInfoReq)        (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_CallInfoResp)       (NPCSTR szDriverName
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

typedef void (*CALLBACK_EndCallReq)         (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_EndCallResp)        (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData);


typedef void (*CALLBACK_UDataSet)           (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRequestId
                                            , NPCSTR szAction
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_UDataGet)           (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szKey
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_UDataResp)          (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRequestId
                                            , NPCSTR szResult
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_PeekStatReq)        (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szStatName
                                            , NPCSTR szXmlData);

typedef void (*CALLBACK_StatResp)           (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szResult
                                            , NPCSTR szResultCode
                                            , NPCSTR szXmlData);


class CIVRDriver : public iSocket
{
    // constructor && destructor
    public:
        CIVRDriver(NPCSTR szOwnerName, NPCSTR szOwnerId, NPCSTR szSvcAppName, NPCSTR szSvcObjectName);
        virtual ~CIVRDriver(void);

    public:
        ilRET SetVersionNumber      (NPCSTR szVersion);

        void  CreateRequestKey      (NPSTR szBuf, int nSize);
    
    public:
        ilRET Deinitiate(void);

        ilRET Initiate              ( NPCSTR szRemoteIp
                                    , uint32 nRemotePort );

        ilRET InitLog               ( NPCSTR szLogPath
                                    , NPCSTR szLogLinkPath
                                    , NPCSTR szLogPrefix
                                    , uint32 nLogLevel );

    protected:
        ilRET Reconnect();

        void  Disconnect();

        ilRET ConnectionOpen        ( NPCSTR szRemote
                                    , uint32 nRemotePort
                                    , uint32 nTimeout = 5000 );

        int32 SendData              ( NPSTR szPacket, int32 nLength );

        void  CombineGLIHeader4Xml  ( NPSTR szSrcPacket
                                    , int32 nSrcLength
                                    , NPSTR szDestPacket
                                    , int32 nDestLength );

    
    public:
        ilRET NoteCallStart         ( NPCSTR szReqKey
                                    , NPCSTR szCalledNum        // ivr port number
                                    , NPCSTR szCallControlMode
                                    , NPCSTR szVersion
                                    , NPCSTR szDNIS
                                    , NPCSTR szANI
                                    , NPCSTR szUDataEx
                                    , NPCSTR szExtnsEx);

        ilRET RouteRequest          ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szRouteDN
                                    , NPCSTR szCED
                                    , NPCSTR szUDataEx
                                    , NPCSTR szExtnsEx);

        ilRET CallTransferKVList    ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szDestDN
                                    , NPCSTR szUDataEx
                                    , NPCSTR szExtnsEx);

        ilRET NoteCallEnd           ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szEndCause );

        ilRET GetCallInfo           ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort);

        ilRET UDataSetKV            ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szRequestId
                                    , NPCSTR szAction
                                    , NPCSTR szUDataEx );

        ilRET UDataGetKV            ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szRequestId
                                    , NPCSTR szKeys );

        ilRET PeekStatReq           ( NPCSTR szReqKey
                                    , NPCSTR szGCTIPort
                                    , NPCSTR szRequestId
                                    , NPCSTR szStatName );


        ilRET KeepAlive             ( uint32 nInterval );

        void  ContinueKeepAlive     ( bool bIsContinue );

    public:
        void SendKeepAliveMsg();

    // inlines..
    public:
        inline bool IsInitDriver()      { return m_bIsInit;         }
        inline bool IsConnected()       { return m_bIsConnected;    }
        inline NPCSTR GetObjectName()   { return m_szSvcObjectName; }
        inline NPCSTR GetAppName()      { return m_szSvcAppName;    }
        inline NPCSTR GetRemoteIp()     { return m_szRemoteIp;      }
        inline int32 GetRemotePort()    { return m_nRemotePort;     }
        

        // callback..
        inline void Set_CALLBACK_PacketSent(CALLBACK_PacketSent callback)
        {
            m_pCALLBACK_PacketSent = callback;
        }
        inline void Set_CALLBACK_PacketReceived(CALLBACK_PacketReceived callback)
        {
            m_pCALLBACK_PacketReceived = callback;
        }
        inline void Set_CALLBACK_LoginReq(CALLBACK_LoginReq callback)
        {
            m_pCALLBACK_LoginReq = callback;
        }
        inline void Set_CALLBACK_LoginResp(CALLBACK_LoginResp callback)
        {
            m_pCALLBACK_LoginResp = callback;
        }
        inline void Set_CALLBACK_NewCall(CALLBACK_NewCall callback)
        {
            m_pCALLBACK_NewCall = callback;
        }
        inline void Set_CALLBACK_CallStatus(CALLBACK_CallStatus callback)
        {
            m_pCALLBACK_CallStatus = callback;
        }
        inline void Set_CALLBACK_RouteRequest(CALLBACK_RouteRequest callback)
        {
            m_pCALLBACK_RouteRequest = callback;
        }
        inline void Set_CALLBACK_RouteResponse(CALLBACK_RouteResponse callback)
        {
            m_pCALLBACK_RouteResponse = callback;
        }
        inline void Set_CALLBACK_OneStepXfer(CALLBACK_OneStepXfer callback)
        {
            m_pCALLBACK_OneStepXfer = callback;
        }
        inline void Set_CALLBACK_CallInfoReq(CALLBACK_CallInfoReq callback)
        {
            m_pCALLBACK_CallInfoReq = callback;
        }
        inline void Set_CALLBACK_CallInfoResp(CALLBACK_CallInfoResp callback)
        {
            m_pCALLBACK_CallInfoResp = callback;
        }
        inline void Set_CALLBACK_EndCallReq(CALLBACK_EndCallReq callback)
        {
            m_pCALLBACK_EndCallReq = callback;
        }
        inline void Set_CALLBACK_EndCallResp(CALLBACK_EndCallResp callback)
        {
            m_pCALLBACK_EndCallResp = callback;
        }
        inline void Set_CALLBACK_UDataSet(CALLBACK_UDataSet callback)
        {
            m_pCALLBACK_UDataSet = callback;
        }
        inline void Set_CALLBACK_UDataGet(CALLBACK_UDataGet callback)
        {
            m_pCALLBACK_UDataGet = callback;
        }
        inline void Set_CALLBACK_UDataResp(CALLBACK_UDataResp callback)
        {
            m_pCALLBACK_UDataResp = callback;
        }
        inline void Set_CALLBACK_PeekStatReq(CALLBACK_PeekStatReq callback)
        {
            m_pCALLBACK_PeekStatReq = callback;
        }

        inline void Set_CALLBACK_StatResp(CALLBACK_StatResp callback)
        {
            m_pCALLBACK_StatResp = callback;
        }


    // internal : handle receive packets..
    protected:
        bool HandlePacket(NPCSTR szPacket, int32 nLength);
        bool HandlePacket_LoginResp(CXmlItem XmlItem);
        bool HandlePacket_CallStatus(CXmlItem XmlItem);
        bool HandlePacket_RouteResponse(CXmlItem XmlItem);
        bool HandlePacket_CallInfoResp(CXmlItem XmlItem);
        bool HandlePacket_UDataResp(CXmlItem XmlItem);
        bool HandlePacket_StatResp(CXmlItem XmlItem);
        bool HandlePacket_FlowControl(CXmlItem XmlItem);
        bool HandlePacket_CallError(CXmlItem XmlItem);
        bool HandlePacket_EndCall(CXmlItem XmlItem);

        ilCallStatus GetCallStatusByStr(NPCSTR szEvent);

    // thread control..
    public:
        virtual bool StartUp(NPVOID pArg);
        virtual bool StopAll();

    public:
        virtual NPVOID ThreadRun(NPVOID pArg);
        void Clear(void);

    // log
    private:
        void LOG_WRITE(uint32 nLogLvl, NPCSTR szFmt, ...);


    // fields..
    protected:
        bool m_bIsInit;
        bool m_bIsConnected;
        char m_szVersion[8];

        char m_szOwnerName[50];
        char m_szOwnerId[50];
        char m_szSvcAppName[50];
        char m_szSvcObjectName[50];
        char m_szRemoteIp[50];
        uint32 m_nRemotePort;

        // log infos..
        iLog m_log;
        char m_szLogPath[128];
        char m_szLogLinkPath[128];
        char m_szLogPrefix[50];
        uint32 m_nLogLevel;

        uint32 m_nReqKeySeq;
        iMutex m_mutexLock;

        CKeepAlive *m_pKeepAlive;

        // callback pointers...
        CALLBACK_PacketSent             m_pCALLBACK_PacketSent;
        CALLBACK_PacketReceived         m_pCALLBACK_PacketReceived;
        CALLBACK_LoginReq               m_pCALLBACK_LoginReq;
        CALLBACK_LoginResp              m_pCALLBACK_LoginResp;
        CALLBACK_NewCall                m_pCALLBACK_NewCall;
        CALLBACK_CallStatus             m_pCALLBACK_CallStatus;
        CALLBACK_RouteRequest           m_pCALLBACK_RouteRequest;
        CALLBACK_RouteResponse          m_pCALLBACK_RouteResponse;
        CALLBACK_OneStepXfer            m_pCALLBACK_OneStepXfer;
        CALLBACK_CallInfoReq            m_pCALLBACK_CallInfoReq;
        CALLBACK_CallInfoResp           m_pCALLBACK_CallInfoResp;
        CALLBACK_EndCallReq             m_pCALLBACK_EndCallReq;
        CALLBACK_EndCallResp            m_pCALLBACK_EndCallResp;
        CALLBACK_UDataSet               m_pCALLBACK_UDataSet;
        CALLBACK_UDataGet               m_pCALLBACK_UDataGet;
        CALLBACK_UDataResp              m_pCALLBACK_UDataResp;
        CALLBACK_PeekStatReq            m_pCALLBACK_PeekStatReq;
        CALLBACK_StatResp               m_pCALLBACK_StatResp;

};


