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


// include
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "LogEx.h"
#include "Config.h"
#include "IVRDriverController.h"

// constructor && desctructor
CIVRDriverController::CIVRDriverController(void) {}
CIVRDriverController::~CIVRDriverController(void) {}

// singleton
CIVRDriverController* CIVRDriverController::m_pInstance = NULL;
CIVRDriverController* CIVRDriverController::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CIVRDriverController();

    return m_pInstance;
}

void CIVRDriverController::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


/****************************************************************************
 TITLE       : Clear
 DESCRIPTION : member clear..
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriverController::Clear(void)
{
    memset(m_szName       , 0x00, sizeof(m_szName));
    
    if(m_pDriver != NULL)
    {
        delete m_pDriver;
        m_pDriver = NULL;
    }
}


/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::StartUp(NPVOID pArg)
{
    SetName("IDRIVER_CTL");
    
    // 드라이버 멤버 초기화
    InitIVRDriver();

    // 포트정보 초기화
    InitPortInfos();

    if(pArg == NULL)
        Start();
    else
        Start(pArg);


    return true;
}


/****************************************************************************
 TITLE       : StopAll
 DESCRIPTION : Stop thread
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::StopAll()
{
    return Stop(true);
}


/****************************************************************************
 TITLE       : ThreadRun - override
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
NPVOID CIVRDriverController::ThreadRun(NPVOID pArg)
{
    // init sleep..
    iSleep(ONESEC);

    while(IsOn())
    {
        // queue polling..
        iSleep(50);

        if(CQueueOfRequest::Inst()->GetCount() <= 0)
        {
            m_pDriver->ContinueKeepAlive(true);
            continue;
        }

        // 요청건이 존재하면 Alive check 중지.
        m_pDriver->ContinueKeepAlive(false);

        QueueItem qItem = CQueueOfRequest::Inst()->Dequeue();
        //LOG_WRITE(ILOG_INFO, "QueueOfRequest Pop Success. Data=%s", qItem.szRequestPacket);
        
        // 요청건 처리
        HandleRequest(qItem);
    }
    
    return 0;
}


/****************************************************************************
 TITLE       : InitPortInfos
 DESCRIPTION : 포트정보 초기화
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::InitPortInfos()
{
    int32 nStartChannel = CConfig::Inst()->GetDrvStartChannel();
    int32 nStartPort    = CConfig::Inst()->GetDrvStartPort();
    int32 nChannelCnt   = CConfig::Inst()->GetDrvDNISCount();
    int32 nTESystemId   = atoi(CConfig::Inst()->GetDrvTESystemId());

    for(int nIndex = 0; nIndex < nChannelCnt; nIndex++)
    {
        CPortInfo*  pPortInfo = new CPortInfo();
        
        char szPortKey[10];
        memset(szPortKey, 0x00, sizeof(szPortKey));

        int nChannelNo = nStartChannel + nIndex;
        int nPortNo    = nStartPort + nIndex;
        
        sprintf(szPortKey, "%04d%04d", nTESystemId, nChannelNo);

        strcpy(pPortInfo->szPortKey, szPortKey);
        pPortInfo->nTESystemID = nTESystemId;
        pPortInfo->nChannelNo  = nChannelNo;
        pPortInfo->nIndex      = nIndex;
        pPortInfo->nPort       = nPortNo;

        AddPortInfo(pPortInfo);
    }

    return true;

}

/****************************************************************************
 TITLE       : AddPortInfo
 DESCRIPTION : 포트정보 추가
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::AddPortInfo(CPortInfo* pPortInfo)
{
    m_mutexLock.Lock();
    m_mapPortInfo.insert(make_pair(pPortInfo->szPortKey, pPortInfo));
    m_mutexLock.Unlock();

    LOG_WRITE(ILOG_INFO, "Add port info. TotalCnt:%d, PortKey:%s, TESystemID:%d, ChannelNo:%d, GCTI-Port:%d"
                            , m_mapPortInfo.size()
                            , pPortInfo->szPortKey
                            , pPortInfo->nTESystemID
                            , pPortInfo->nChannelNo
                            , pPortInfo->nPort
                            );

    return true;
}

/****************************************************************************
 TITLE       : AddCtiReqInfo
 DESCRIPTION : CTI로 요청 정보 보관
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::AddCtiReqInfo(NPCSTR szReqKey, CCtiReqItem* pCtiReqItem)
{
    m_mutexLock.Lock();
    m_mapCtiReqInfo.insert(make_pair(string(szReqKey), pCtiReqItem));
    m_mutexLock.Unlock();

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Add GCTI Request Info. Count:%d"
                        , pCtiReqItem->m_portInfo.nPort
                        , szReqKey
                        , m_mapCtiReqInfo.size());

    return true;
}


/****************************************************************************
 TITLE       : RemoveCtiReqInfo
 DESCRIPTION : CTI로 요청 정보 제거
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::RemoveCtiReqInfo(NPCSTR szReqKey)
{
    char szGCTIPort[10];    
    memset(szGCTIPort, 0x00, sizeof(szGCTIPort));

    CCtiReqItem *pCtiItem = GetCtiReqInfo(szReqKey);    
    if(pCtiItem != NULL)
    {
        sprintf(szGCTIPort, "%d", pCtiItem->m_portInfo.nPort);
        delete pCtiItem;
    }

    m_mutexLock.Lock();
    m_mapCtiReqInfo.erase (string(szReqKey));
    m_mutexLock.Unlock();

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Remove GCTI Request Info. Map Count:%d"
                        , szGCTIPort
                        , szReqKey
                        , m_mapCtiReqInfo.size());

    return true;
}

/****************************************************************************
 TITLE       : GetPortInfo
 DESCRIPTION : 포트정보 얻음.
 PARAMS      : TE시스템아이디
               채널번호
               포트정보 포인터

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
CPortInfo* CIVRDriverController::GetPortInfo(int32 nTESystemId, int32 nChannelNo)
{
    CPortInfo *pPortInfo = NULL;
    
    char szPortKey[10];
    memset(szPortKey, 0x00, sizeof(szPortKey));
    sprintf(szPortKey, "%04d%04d", nTESystemId, nChannelNo);

    MAP_PORT_INFO::iterator itor;

    m_mutexLock.Lock();
    
    itor = m_mapPortInfo.find( string(szPortKey) );
    if(itor != m_mapPortInfo.end())
        pPortInfo = itor->second;
    else
        pPortInfo = NULL;

    m_mutexLock.Unlock();

    return pPortInfo;
}


/****************************************************************************
 TITLE       : GetCtiReqInfo
 DESCRIPTION : 요청정보 얻음
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
CCtiReqItem* CIVRDriverController::GetCtiReqInfo(NPCSTR szReqKey)
{
    CCtiReqItem *pCtiItem = NULL;
    
    MAP_CTI_REQ_INFO::iterator itor;

    m_mutexLock.Lock();
    
    itor = m_mapCtiReqInfo.find( string(szReqKey) );
    if(itor != m_mapCtiReqInfo.end())
        pCtiItem = itor->second;
    else
        pCtiItem = NULL;

    m_mutexLock.Unlock();

    return pCtiItem;
}


/****************************************************************************
 TITLE       : NeedNewCall
 DESCRIPTION : NewCall 이 실행되었는지 여부
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::NeedNewCall(NPCSTR szReqKey)
{
    CCtiReqItem* pItem = GetCtiReqInfo(szReqKey);
    if(pItem == NULL)
        return true;

    if(!pItem->m_bIsNewCall)
        return true;

    return false;
}


/****************************************************************************
 TITLE       : InitIVRDriver
 DESCRIPTION : 드라이버 멤버변수 초기화
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::InitIVRDriver()
{
    ilRET ret = ilRET_NONE;
    
    m_pDriver = new CIVRDriver( CConfig::Inst()->GetProcessName()       // 부모프로세스 이름
                                , CConfig::Inst()->GetProcessId()       // 부모프로세스 아이디
                                , CConfig::Inst()->GetDrvSvcAppName()   // 드라이버 서비스 APP이름
                                , CConfig::Inst()->GetDrvObjectSvcName()// 드라이버 서비스 Object이름
                                );

    // init callback functions..
    //m_pDriver->Set_CALLBACK_PacketSent(CALLBACK_PacketSent);
    //m_pDriver->Set_CALLBACK_PacketReceived(CALLBACK_PacketReceived);
    m_pDriver->Set_CALLBACK_PacketSent(NULL);
    m_pDriver->Set_CALLBACK_PacketReceived(NULL);
    m_pDriver->Set_CALLBACK_LoginReq(CALLBACK_LoginReq);
    m_pDriver->Set_CALLBACK_LoginResp(CALLBACK_LoginResp);
    m_pDriver->Set_CALLBACK_NewCall(CALLBACK_NewCall);
    m_pDriver->Set_CALLBACK_CallStatus(CALLBACK_CallStatus);
    m_pDriver->Set_CALLBACK_RouteRequest(CALLBACK_RouteRequest);
    m_pDriver->Set_CALLBACK_RouteResponse(CALLBACK_RouteResponse);
    m_pDriver->Set_CALLBACK_OneStepXfer(CALLBACK_OneStepXfer);
    m_pDriver->Set_CALLBACK_CallInfoReq(CALLBACK_CallInfoReq);
    m_pDriver->Set_CALLBACK_CallInfoResp(CALLBACK_CallInfoResp);
    m_pDriver->Set_CALLBACK_EndCallReq(CALLBACK_EndCallReq);
    m_pDriver->Set_CALLBACK_EndCallResp(CALLBACK_EndCallResp);
    m_pDriver->Set_CALLBACK_UDataSet(CALLBACK_UDataSet);
    m_pDriver->Set_CALLBACK_UDataGet(CALLBACK_UDataGet);
    m_pDriver->Set_CALLBACK_UDataResp(CALLBACK_UDataResp);

    // set driver version..
    m_pDriver->SetVersionNumber("8.5");

    // init log..
    char szLogPrefix[100];
    memset(szLogPrefix, 0x00, sizeof(szLogPrefix));
    sprintf(szLogPrefix, "cba.idrv.%s", CConfig::Inst()->GetProcessId());
    
    ret = m_pDriver->InitLog(CConfig::Inst()->GetDrvLogPath()
                                , CConfig::Inst()->GetDrvLogLinkPath()
                                , szLogPrefix
                                , CConfig::Inst()->GetDrvLogLevel()
                                );
    if(ret != ilRET_SUCCESS)
    {
        delete m_pDriver;
        return false;
    }

    // init driver infos..
    ret = m_pDriver->Initiate( CConfig::Inst()->GetDrvRemoteIp()
                                , CConfig::Inst()->GetDrvRemotePort()
                                );
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "Initiate IDriver fail !! err:%d", ret);
        delete m_pDriver;
        return false;
    }
    
    LOG_WRITE(ILOG_INFO, "Initiate IDriver Success");
    
    // driver start !!
    m_pDriver->StartUp(NULL);

    if(CConfig::Inst()->GetDrvKeepAliveInterval() > 0)
    {
        m_pDriver->KeepAlive(CConfig::Inst()->GetDrvKeepAliveInterval());
        LOG_WRITE(ILOG_INFO, "Start IDriver Keep-Alive check. Interval:%d", CConfig::Inst()->GetDrvKeepAliveInterval());
    }
    
    return true;
}


/****************************************************************************
 TITLE       : HandleRequest
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest(QueueItem stItem)
{
    CLineCommand lineCmd;
    char szCmd[30];
    memset(szCmd, 0x00, sizeof(szCmd));
    
    lineCmd = stItem.szRequestPacketBody;
    string strCmd = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strCmd.c_str());

    // 드라이버 연결상태 확인
    if(!m_pDriver->IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "IVRDriver Connection Fail.");
        char szBody[1024];
        memset(szBody, 0x00, sizeof(szBody));
    
        lineCmd.setKeyValue("sRequest", szCmd, szBody);    
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "GCTIConnectionFail", szBody);
    
        CIVRDriverController::Inst()->PushResponse(stItem, szBody);
        return false;
    }

    if( strcmp(szCmd, "icMakeCall") == 0 )
        return HandleRequest_icMakeCall(stItem);

    if( strcmp(szCmd, "icNewCall") == 0 )
        return HandleRequest_icNewCall(stItem);

    if( strcmp(szCmd, "icWaitInfo") == 0 )
        return HandleRequest_icWaitInfo(stItem);

    if( strcmp(szCmd, "icRouteReq") == 0 )
        return HandleRequest_icRouteRequest(stItem);

    if( strcmp(szCmd, "icDeflectTR") == 0 )
        return HandleRequest_icDeflectTR(stItem);

    if( strcmp(szCmd, "icCallInfo") == 0 )
        return HandleRequest_icCallInfo(stItem);

    if( strcmp(szCmd, "icClearCall") == 0 )
        return HandleRequest_icClearCall(stItem);

    if( strcmp(szCmd, "icSendUData") == 0 )
        return HandleRequest_icSendUData(stItem);

    if( strcmp(szCmd, "icGetUData") == 0 )
        return HandleRequest_icGetUData(stItem);

    LOG_WRITE(ILOG_ERR, "Not Defined Command. Command=%s, Data=", szCmd, stItem.szRequestPacket);

    return false;
}


/****************************************************************************
 TITLE       : Handle_icMakeCall
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icMakeCall(QueueItem stItem)
{
    // to - be
    return true;
}


/****************************************************************************
 TITLE       : Handle_icNewCall
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icNewCall(QueueItem stItem)
{
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[8];
    char szChanNo[8];
    char szDNIS[20];
    char szANI[20];
    char szUDataEx[2048];
    char szExtnsEx[2048];

    char szCmd[30];
    char szPacketLen[10];
    char szBody[4196];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));
    memset(szReqKey   , 0x00, sizeof(szReqKey));
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szDNIS     , 0x00, sizeof(szDNIS));
    memset(szANI      , 0x00, sizeof(szANI));
    memset(szUDataEx  , 0x00, sizeof(szUDataEx));
    memset(szExtnsEx  , 0x00, sizeof(szExtnsEx));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));
    
    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    // reference call id 생성
    m_pDriver->CreateRequestKey(szReqKey, sizeof(szReqKey));

    CLineCommand lineCmd;    
    lineCmd = stItem.szRequestPacketBody;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sDNIS"].getValue();
    strcpy(szDNIS, strTemp.c_str());

    strTemp = lineCmd[(char*)"sANI"].getValue();
    strcpy(szANI, strTemp.c_str());

    strTemp = lineCmd[(char*)"sUDataEx"].getValue();
    strcpy(szUDataEx, strTemp.c_str());

    strTemp = lineCmd[(char*)"sExtnsEx"].getValue();
    strcpy(szExtnsEx, strTemp.c_str());

    int nTeNo = atoi(szTENo);
    int nChannelNo = atoi(szChanNo);
    
    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }
    
    pPortInfo->ClearQItem();
    strcpy(stItem.szReqKey, szReqKey);
    pPortInfo->m_stQItem = stItem;    
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s,TENo:%s, ChanNo:%s, DNIS:%s, ANI:%s, UDataEx:%s, ExtnsEx:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo
                            , szDNIS
                            , szANI
                            , szUDataEx
                            , szExtnsEx);

    // NewCall은 항상 새로운 콜이여야만 함.
    CCtiReqItem *pCtiItem = GetCtiReqInfo(szReqKey);
    if(pCtiItem != NULL)
        delete pCtiItem;

    // cti item 추가 : 콜백처리 위함.
    pCtiItem = new CCtiReqItem();
    pCtiItem->m_bIsNewCall          = false;
    pCtiItem->m_eCallStatus         = eCallStatusUnknown;
    pCtiItem->m_portInfo            = *pPortInfo;
    pCtiItem->m_stQueueItem         = stItem;
    strcpy(pCtiItem->m_szCallId     , szReqKey);
    strcpy(pCtiItem->m_szReqCommand , szCmd);
    
    AddCtiReqInfo(szReqKey, pCtiItem);

    // new call
    ilRET ret = m_pDriver->NoteCallStart(szReqKey
                                , szGCTIPort
                                , "Network"
                                , "4.0"
                                , szDNIS
                                , szANI
                                , szUDataEx
                                , szExtnsEx);
    if(ret != ilRET_SUCCESS)
    {
         LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s,TENo:%s, ChanNo:%s, DNIS:%s, ANI:%s, UDataEx:%s, ExtnsEx:%s"
                                , szGCTIPort
                                , szReqKey
                                , szCmd
                                , szTENo
                                , szChanNo
                                , szDNIS
                                , szANI
                                , szUDataEx
                                , szExtnsEx);

        lineCmd.setKeyValue("sRequest", szCmd              , szBody);
        lineCmd.setKeyValue("sResult" , "0"                , szBody);
        lineCmd.setKeyValue("sDesc"   , "NoteCallStartFail", szBody);
        PushResponse(stItem, szBody);
        RemoveCtiReqInfo(szReqKey);
        return false;
    }

    pCtiItem->m_bIsNewCall = true;

    return true;
}


/****************************************************************************
 TITLE       : Handle_icWaitInfo
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icWaitInfo(QueueItem stItem)
{

    return true;
}


bool CIVRDriverController::HandleRequest_icRouteRequest(QueueItem stItem)
{
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[8];
    char szChanNo[8];
    char szRouteDN[20];
    char szCED[1024];
    char szUDataEx[2048];
    char szExtnsEx[2048];
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));
    memset(szReqKey   , 0x00, sizeof(szReqKey));
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szRouteDN  , 0x00, sizeof(szRouteDN));
    memset(szCED      , 0x00, sizeof(szCED));
    memset(szUDataEx  , 0x00, sizeof(szUDataEx));
    memset(szExtnsEx  , 0x00, sizeof(szExtnsEx));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));
    
    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sRouteDN"].getValue();
    strcpy(szRouteDN, strTemp.c_str());

    strTemp = lineCmd[(char*)"sCED"].getValue();
    strcpy(szCED, strTemp.c_str());

    strTemp = lineCmd[(char*)"sUDataEx"].getValue();
    strcpy(szUDataEx, strTemp.c_str());

    strTemp = lineCmd[(char*)"sExtnsEx"].getValue();
    strcpy(szExtnsEx, strTemp.c_str());

    int nTeNo = atoi(szTENo);
    int nChannelNo = atoi(szChanNo);
    
    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"  , "icRouteReq"      , szBody);
        lineCmd.setKeyValue("sResult"   , "0"               , szBody);
        lineCmd.setKeyValue("sDesc"     , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sRouteType", ""                , szBody);
        lineCmd.setKeyValue("sDest"     , ""                , szBody);
        lineCmd.setKeyValue("sExtnsEx"  , ""                , szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);    
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);
    
    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s, RouteDN:%s, CED:%s, UDataEx:%s, ExtnsEx:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo                            
                            , szRouteDN
                            , szCED
                            , szUDataEx
                            , szExtnsEx);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"  , szCmd        , szBody);
        lineCmd.setKeyValue("sResult"   , "0"          , szBody);
        lineCmd.setKeyValue("sDesc"     , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sRouteType", ""           , szBody);
        lineCmd.setKeyValue("sDest"     , ""           , szBody);
        lineCmd.setKeyValue("sExtnsEx"  , ""           , szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // 요청건 정보 업데이트.
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    strcpy(pCtiItem->m_szReqCommand               , "icRouteReq");
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;

    // 라우트 요청
    ilRET ret = m_pDriver->RouteRequest(szReqKey, szGCTIPort, szRouteDN, szCED, szUDataEx, szExtnsEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|RouteRequest Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"  , szCmd, szBody);
        lineCmd.setKeyValue("sResult"   , "0", szBody);
        lineCmd.setKeyValue("sDesc"     , "RouteRequestFail", szBody);
        lineCmd.setKeyValue("sRouteType", "", szBody);
        lineCmd.setKeyValue("sDest"     , "", szBody);
        lineCmd.setKeyValue("sExtnsEx"  , "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    return true;
}

/****************************************************************************
 TITLE       : Handle_icDeflectTR
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icDeflectTR(QueueItem stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;

    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[10];
    char szChanNo[10];
    char szDestDN[10];
    char szUDataEx[2048];
    char szExtnsEx[2048];
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));
    memset(szReqKey   , 0x00, sizeof(szReqKey));
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szDestDN   , 0x00, sizeof(szDestDN));
    memset(szUDataEx  , 0x00, sizeof(szUDataEx));
    memset(szExtnsEx  , 0x00, sizeof(szExtnsEx));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));
    
    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sDestDN"].getValue();
    strcpy(szDestDN, strTemp.c_str());

    strTemp = lineCmd[(char*)"sUDataEx"].getValue();
    strcpy(szUDataEx, strTemp.c_str());

    strTemp = lineCmd[(char*)"sExtnsEx"].getValue();
    strcpy(szExtnsEx, strTemp.c_str());

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);

    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s, DestDN:%s, szUDataEx:%s, szExtnsEx:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo
                            , szDestDN
                            , szUDataEx
                            , szExtnsEx);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd        , szBody);
        lineCmd.setKeyValue("sResult" , "0"          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // 요청건 정보 업데이트.
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    strcpy(pCtiItem->m_szReqCommand , szCmd);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;
    
    ilRET ret = m_pDriver->CallTransferKVList(szReqKey, szGCTIPort, szDestDN, szUDataEx, szExtnsEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|CallTransferKVList Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd                   , szBody);
        lineCmd.setKeyValue("sResult" , "0"                     , szBody);
        lineCmd.setKeyValue("sDesc"   , "CallTransferKVListFail", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    return true;
}

/****************************************************************************
 TITLE       : Handle_icCallInfo
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icCallInfo(QueueItem stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[10];
    char szChanNo[10];
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));    
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));

    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);
    
    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"          , szCmd             , szBody);
        lineCmd.setKeyValue("sResult"           , "0"               , szBody);
        lineCmd.setKeyValue("sDesc"             , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sANI"              , ""                , szBody);
        lineCmd.setKeyValue("sCalledNum"        , ""                , szBody);
        lineCmd.setKeyValue("sConnId"           , ""                , szBody);
        lineCmd.setKeyValue("sDNIS"             , ""                , szBody);
        lineCmd.setKeyValue("sFirstHomeLocation", ""                , szBody);
        lineCmd.setKeyValue("sLastEvent"        , ""                , szBody);
        lineCmd.setKeyValue("sOtherDN"          , ""                , szBody);
        lineCmd.setKeyValue("sOtherQueue"       , ""                , szBody);
        lineCmd.setKeyValue("sOtherTrunk"       , ""                , szBody);
        lineCmd.setKeyValue("sPortDN"           , ""                , szBody);
        lineCmd.setKeyValue("sPortQueue"        , ""                , szBody);
        lineCmd.setKeyValue("sPortTrunk"        , ""                , szBody);
        lineCmd.setKeyValue("sTSCallId"         , ""                , szBody);
        PushResponse(stItem, szBody);
        
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);

    
    
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);
    
    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(pPortInfo->m_stQItem.szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"          , szCmd        , szBody);
        lineCmd.setKeyValue("sResult"           , "0"          , szBody);
        lineCmd.setKeyValue("sDesc"             , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sANI"              , ""           , szBody);
        lineCmd.setKeyValue("sCalledNum"        , ""           , szBody);
        lineCmd.setKeyValue("sConnId"           , ""           , szBody);
        lineCmd.setKeyValue("sDNIS"             , ""           , szBody);
        lineCmd.setKeyValue("sFirstHomeLocation", ""           , szBody);
        lineCmd.setKeyValue("sLastEvent"        , ""           , szBody);
        lineCmd.setKeyValue("sOtherDN"          , ""           , szBody);
        lineCmd.setKeyValue("sOtherQueue"       , ""           , szBody);
        lineCmd.setKeyValue("sOtherTrunk"       , ""           , szBody);
        lineCmd.setKeyValue("sPortDN"           , ""           , szBody);
        lineCmd.setKeyValue("sPortQueue"        , ""           , szBody);
        lineCmd.setKeyValue("sPortTrunk"        , ""           , szBody);
        lineCmd.setKeyValue("sTSCallId"         , ""           , szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    
    // 콜 상태 체크    
    if(pCtiItem->m_eCallStatus != eCallStatusRinging &&
        pCtiItem->m_eCallStatus != eCallStatusEstablished)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Invalid CallStatus. Command:%s, CallStatus:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , pCtiItem->m_szEvent
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"          , szCmd              , szBody);
        lineCmd.setKeyValue("sResult"           , "0"                , szBody);
        lineCmd.setKeyValue("sDesc"             , "InvalidCallStatus", szBody);
        lineCmd.setKeyValue("sANI"              , ""                 , szBody);
        lineCmd.setKeyValue("sCalledNum"        , ""                 , szBody);
        lineCmd.setKeyValue("sConnId"           , ""                 , szBody);
        lineCmd.setKeyValue("sDNIS"             , ""                 , szBody);
        lineCmd.setKeyValue("sFirstHomeLocation", ""                 , szBody);
        lineCmd.setKeyValue("sLastEvent"        , ""                 , szBody);
        lineCmd.setKeyValue("sOtherDN"          , ""                 , szBody);
        lineCmd.setKeyValue("sOtherQueue"       , ""                 , szBody);
        lineCmd.setKeyValue("sOtherTrunk"       , ""                 , szBody);
        lineCmd.setKeyValue("sPortDN"           , ""                 , szBody);
        lineCmd.setKeyValue("sPortQueue"        , ""                 , szBody);
        lineCmd.setKeyValue("sPortTrunk"        , ""                 , szBody);
        lineCmd.setKeyValue("sTSCallId"         , ""                 , szBody);

        PushResponse(stItem, szBody);
        return false;
    }
        

    // 요청건 정보 업데이트.
    strcpy(pCtiItem->m_szReqCommand               , szCmd);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;

    ilRET ret = m_pDriver->GetCallInfo(szReqKey, szGCTIPort);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|GetCallInfo Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"          , szCmd, szBody);
        lineCmd.setKeyValue("sResult"           , "0"              , szBody);
        lineCmd.setKeyValue("sDesc"             , "GetCallInfoFail", szBody);
        lineCmd.setKeyValue("sANI"              , "", szBody);
        lineCmd.setKeyValue("sCalledNum"        , "", szBody);
        lineCmd.setKeyValue("sConnId"           , "", szBody);
        lineCmd.setKeyValue("sDNIS"             , "", szBody);
        lineCmd.setKeyValue("sFirstHomeLocation", "", szBody);
        lineCmd.setKeyValue("sLastEvent"        , "", szBody);
        lineCmd.setKeyValue("sOtherDN"          , "", szBody);
        lineCmd.setKeyValue("sOtherQueue"       , "", szBody);
        lineCmd.setKeyValue("sOtherTrunk"       , "", szBody);
        lineCmd.setKeyValue("sPortDN"           , "", szBody);
        lineCmd.setKeyValue("sPortQueue"        , "", szBody);
        lineCmd.setKeyValue("sPortTrunk"        , "", szBody);
        lineCmd.setKeyValue("sTSCallId"         , "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    return true;
}

/****************************************************************************
 TITLE       : Handle_icClearCall
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icClearCall(QueueItem stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[10];
    char szChanNo[10];
    char szEndCause[50];
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));    
    memset(szReqKey   , 0x00, sizeof(szReqKey));    
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szEndCause , 0x00, sizeof(szEndCause));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));

    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sEndCause"].getValue();
    strcpy(szEndCause, strTemp.c_str());
    if(strlen(szEndCause) <= 0)
        strcpy(szEndCause, "Normal");

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);
    
    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);
        
        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);
 
    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s, EndCause:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo
                            , szEndCause);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        PushResponse(stItem, szBody);
        pPortInfo->ClearQItem();
        return false;
    }

    // 요청건 정보 업데이트.
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    strcpy(pCtiItem->m_szReqCommand               , szCmd);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;

    ilRET ret = m_pDriver->NoteCallEnd(szReqKey, szGCTIPort, szEndCause);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|NoteCallEnd Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NoteCallEndFail", szBody);
        PushResponse(stItem, szBody);
        pPortInfo->ClearQItem();
        return false;
    }

    pPortInfo->ClearQItem();
    return true;
}

/****************************************************************************
 TITLE       : Handle_icSendUData
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icSendUData(QueueItem stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[10];
    char szChanNo[10];
    char szRequestId[50];
    char szAction[20];
    char szUDataEx[2048];
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId   , 0x00, sizeof(szCallId));
    memset(szReqKey   , 0x00, sizeof(szReqKey));
    memset(szTENo     , 0x00, sizeof(szTENo));
    memset(szChanNo   , 0x00, sizeof(szChanNo));
    memset(szRequestId, 0x00, sizeof(szRequestId));
    memset(szAction   , 0x00, sizeof(szAction));
    memset(szUDataEx  , 0x00, sizeof(szUDataEx));
    memset(szBody     , 0x00, sizeof(szBody));
    memset(szCmd      , 0x00, sizeof(szCmd));
    memset(szPacketLen, 0x00, sizeof(szPacketLen));
    memset(szGCTIPort , 0x00, sizeof(szGCTIPort));

    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);

    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;
    
    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sRequestId"].getValue();
    strcpy(szRequestId, strTemp.c_str());

    strTemp = lineCmd[(char*)"sAction"].getValue();
    strcpy(szAction, strTemp.c_str());
    
    strTemp = lineCmd[(char*)"sUDataEx"].getValue();
    strcpy(szUDataEx, strTemp.c_str());

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);

    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);
    sprintf(szGCTIPort, "%d", pPortInfo->nPort);

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s, Action:%s, UDataEx:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo
                            , szAction
                            , szUDataEx);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // 요청건 정보 업데이트.
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    strcpy(pCtiItem->m_szReqCommand               , "icSendUData");
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;

    ilRET ret = m_pDriver->UDataSetKV(szReqKey, szGCTIPort, szRequestId, szAction, szUDataEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|UDataAddKV Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "UDataAddKVFail", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    return true;
}


/****************************************************************************
 TITLE       : HandleRequest_icGetUData
 DESCRIPTION : CB로부터의 요청건 처리
 PARAMS      : Queue item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::HandleRequest_icGetUData(QueueItem stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[64];
    char szTENo[10];
    char szChanNo[10];
    char szUDataReqId[50];    
    char szKeys[128];    
    char szBody[2048];
    char szCmd[30];
    char szPacketLen[10];
    char szGCTIPort[10];

    memset(szCallId    , 0x00, sizeof(szCallId));
    memset(szReqKey    , 0x00, sizeof(szReqKey));
    memset(szTENo      , 0x00, sizeof(szTENo));
    memset(szChanNo    , 0x00, sizeof(szChanNo));
    memset(szUDataReqId, 0x00, sizeof(szUDataReqId));
    memset(szKeys      , 0x00, sizeof(szKeys));
    memset(szBody      , 0x00, sizeof(szBody));
    memset(szCmd       , 0x00, sizeof(szCmd));
    memset(szPacketLen , 0x00, sizeof(szPacketLen));
    memset(szGCTIPort  , 0x00, sizeof(szGCTIPort));

    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1);
    
    CLineCommand lineCmd;
    lineCmd = stItem.szRequestPacketBody;;

    string strTemp = "";
    strTemp = lineCmd[(char*)"sRequest"].getValue();
    strcpy(szCmd, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sRequestId"].getValue();
    strcpy(szUDataReqId, strTemp.c_str());

    strTemp = lineCmd[(char*)"sKeys"].getValue();
    strcpy(szKeys, strTemp.c_str());
    if(strlen(szKeys) <= 0)
        strcpy(szKeys, "UEI");
    
    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);

    // get port info
    CPortInfo* pPortInfo = GetPortInfo(nTeNo, nChannelNo);
    if(pPortInfo == NULL)
    {
        LOG_WRITE(ILOG_ERR, "Not Found PortInfo. Command:%s, SystemId:%s, ChannelNo:%s, Data:%s"
                            , szCmd
                            , szTENo
                            , nChannelNo
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

    // Get RequestKey
    strcpy(szReqKey, pPortInfo->m_stQItem.szReqKey);

    sprintf(szGCTIPort, "%d", pPortInfo->nPort);

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4s|REQ_KEY:%s|Command:%s, TENo:%s, ChanNo:%s, Keys:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , szTENo
                            , szChanNo
                            , szKeys);

    // NewCall 이 필요한지 확인.
    if(NeedNewCall(szReqKey))
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Need icNewCall. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // 요청건 정보 업데이트.
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    strcpy(pCtiItem->m_szReqCommand               , szCmd);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionName, stItem.szCBSessionName);
    strcpy(pCtiItem->m_stQueueItem.szCBSessionIp  , stItem.szCBSessionIp);
    memcpy(pCtiItem->m_stQueueItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    pCtiItem->m_stQueueItem.nCBSessionPort        = stItem.nCBSessionPort;

    ilRET ret = m_pDriver->UDataGetKV(szReqKey, szGCTIPort, szUDataReqId, szKeys);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|UDataGetKD Fail. Command:%s, Data:%s"
                            , szGCTIPort
                            , szReqKey
                            , szCmd
                            , stItem.szRequestPacket);
        
        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sDesc"   , "UDataGetKVFail", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    return true;
}


void CIVRDriverController::CALLBACK_PacketSent(NPCSTR szDriverName, NPCSTR szXmlData)
{
    //CLogEx::LV3("%s|CALLBACK_PacketSent xml=[%s] ", szDriverName, szXmlData);
}


void CIVRDriverController::CALLBACK_PacketReceived(NPCSTR szDriverName, NPCSTR szXmlData)
{
    //CLogEx::LV3("%s|CALLBACK_PacketReceived xml=[%s] ", szDriverName, szXmlData);
}


void CIVRDriverController::CALLBACK_LoginReq(NPCSTR szDriverName
                                        , NPCSTR szCommand
                                        , NPCSTR szReqKey
                                        , NPCSTR szVersion
                                        , NPCSTR szClientName
                                        , NPCSTR szReportStatus
                                        , NPCSTR szServerMonitor
                                        , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    ctrl->LOG_WRITE(ILOG_INFO, "REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                                        , szReqKey
                                        , szCommand
                                        , szXmlData);
}


void CIVRDriverController::CALLBACK_LoginResp (NPCSTR szDriverName
                                        , NPCSTR szCommand
                                        , NPCSTR szReqKey
                                        , NPCSTR szIServerVersion
                                        , NPCSTR szResult
                                        , NPCSTR szConfigOptions
                                        , NPCSTR szStatus
                                        , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    ctrl->LOG_WRITE(ILOG_INFO, "REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                                        , szReqKey
                                        , szCommand
                                        , szXmlData);
}


void CIVRDriverController::CALLBACK_NewCall (NPCSTR szDriverName
                                        , NPCSTR szCommand
                                        , NPCSTR szReqKey
                                        , NPCSTR szCalledNum        // ivr port number
                                        , NPCSTR szCallControlMode
                                        , NPCSTR szVersion
                                        , NPCSTR szDNIS
                                        , NPCSTR szANI
                                        , NPCSTR szUDataEx
                                        , NPCSTR szExtnsEx
                                        , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[2048];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = ctrl->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

        return;
    }



    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    QueueItem qItem = pCtiItem->m_stQueueItem;    
    lineCmd.setKeyValue("sRequest", pCtiItem->m_szReqCommand, szBody);
    lineCmd.setKeyValue("sResult" , "1"                     , szBody);
    lineCmd.setKeyValue("sDesc"   , "Success"               , szBody);

    CIVRDriverController::Inst()->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_CallStatus (NPCSTR szDriverName
                                      , NPCSTR szCommand
                                      , NPCSTR szReqKey
                                      , NPCSTR szEvent
                                      , ilCallStatus eCallStatus
                                      , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[2048];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    pCtiItem->m_eCallStatus = eCallStatus;
    strcpy(pCtiItem->m_szEvent, szEvent);
    strcpy(pCtiItem->m_szResCommand, szCommand);

    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    if( strcmp(strReqCmd.c_str(), "icDeflectTR") == 0 )
    {
        lineCmd.setKeyValue("sRequest", pCtiItem->m_szReqCommand, szBody);
        if(strcmp(szEvent, "XferComplete") == 0 )
        {
            lineCmd.setKeyValue("sResult", "1", szBody);
            lineCmd.setKeyValue("sDesc", "Success", szBody);
        }
        else
        {
            lineCmd.setKeyValue("sResult", "0", szBody);
            lineCmd.setKeyValue("sDesc", szEvent, szBody);
        }
        
        ctrl->PushResponse(qItem, szBody);
        return;
    }
}


void CIVRDriverController::CALLBACK_RouteRequest   (NPCSTR szDriverName
                                      , NPCSTR szCommand
                                      , NPCSTR szReqKey
                                      , NPCSTR szRouteDN
                                      , NPCSTR szCED
                                      , NPCSTR szUDataEx
                                      , NPCSTR szExtnsEx
                                      , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
}


void CIVRDriverController::CALLBACK_RouteResponse  (NPCSTR szDriverName
                                      , NPCSTR szCommand
                                      , NPCSTR szReqKey
                                      , NPCSTR szRouteType
                                      , NPCSTR szDest
                                      , NPCSTR szExtnsEx
                                      , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[1024];
    memset(szBody, 0x00, sizeof(szBody));
    
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    strcpy(pCtiItem->m_szResCommand, szCommand);    
    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    lineCmd.setKeyValue("sRequest"  , pCtiItem->m_szReqCommand, szBody);
    lineCmd.setKeyValue("sResult"   , "1"         , szBody);
    lineCmd.setKeyValue("sDesc"     , "Success"   , szBody);
    lineCmd.setKeyValue("sRouteType", szRouteType , szBody);
    lineCmd.setKeyValue("sDest"     , szDest      , szBody);
    lineCmd.setKeyValue("sExtnsEx"  , szExtnsEx   , szBody);

    CIVRDriverController::Inst()->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_OneStepXfer  (NPCSTR szDriverName
                                          , NPCSTR szCommand
                                          , NPCSTR szReqKey
                                          , NPCSTR szDestDN
                                          , NPCSTR szUDataEx
                                          , NPCSTR szExtnsEx
                                          , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
}

void CIVRDriverController::CALLBACK_CallInfoReq  (NPCSTR szDriverName
                                          , NPCSTR szCommand
                                          , NPCSTR szReqKey
                                          , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
}



void CIVRDriverController::CALLBACK_CallInfoResp  (NPCSTR szDriverName
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
                                        , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[1024];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    strcpy(pCtiItem->m_szResCommand, szCommand);    
    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    lineCmd.setKeyValue("sRequest"         , pCtiItem->m_szReqCommand       , szBody);
    lineCmd.setKeyValue("sResult"          , "1"                , szBody);
    lineCmd.setKeyValue("ANI"              , szANI              , szBody);
    lineCmd.setKeyValue("DNIS"             , szDNIS             , szBody);
    lineCmd.setKeyValue("CalledNum"        , szCalledNum        , szBody);
    lineCmd.setKeyValue("ConnId"           , szConnId           , szBody);
    lineCmd.setKeyValue("FirstHomeLocation", szFirstHomeLocation, szBody);
    lineCmd.setKeyValue("LastEvent"        , szLastEvent        , szBody);
    lineCmd.setKeyValue("OtherDN"          , szOtherDN          , szBody);
    lineCmd.setKeyValue("OtherQueue"       , szOtherQueue       , szBody);
    lineCmd.setKeyValue("OtherTrunk"       , szOtherTrunk       , szBody);
    lineCmd.setKeyValue("PortDN"           , szPortDN           , szBody);
    lineCmd.setKeyValue("PortQueue"        , szPortQueue        , szBody);
    lineCmd.setKeyValue("PortTrunk"        , szPortTrunk        , szBody);
    lineCmd.setKeyValue("ThirdPartyDN"     , szThirdPartyDN     , szBody);
    lineCmd.setKeyValue("TSCallId"         , szTSCallId         , szBody);
    
    CIVRDriverController::Inst()->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_EndCallReq (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[1024];
    memset(szBody, 0x00, sizeof(szBody));    

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%sReq, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%sReq, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%sReq, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    // EndCall 처리여부 flag set.
    pCtiItem->m_bIsEndCall = true;

    strcpy(pCtiItem->m_szResCommand, szCommand);    
    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    lineCmd.setKeyValue("sRequest", pCtiItem->m_szReqCommand, szBody);
    lineCmd.setKeyValue("sResult" , "1"          , szBody);
    lineCmd.setKeyValue("sDesc"   , "Success"    , szBody);
    
    ctrl->PushResponse(qItem, szBody);
    ctrl->RemoveCtiReqInfo(szReqKey);
}



void CIVRDriverController::CALLBACK_EndCallResp (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[1024];
    memset(szBody, 0x00, sizeof(szBody));    

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_LV3, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. May be already removed. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    strcpy(pCtiItem->m_szResCommand, szCommand);    
    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    lineCmd.setKeyValue("sRequest", pCtiItem->m_szReqCommand, szBody);
    lineCmd.setKeyValue("sResult" , "1"          , szBody);
    lineCmd.setKeyValue("sDesc"   , "Success"    , szBody);
    
    ctrl->PushResponse(qItem, szBody);
    ctrl->RemoveCtiReqInfo(szReqKey);
}


void CIVRDriverController::CALLBACK_UDataSet (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szUDataReqId
                                            , NPCSTR szAction
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
}

void CIVRDriverController::CALLBACK_UDataGet (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szKeys
                                            , NPCSTR szXmlData)
{
    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
}



void CIVRDriverController::CALLBACK_UDataResp (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szRequestId
                                            , NPCSTR szResult
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[1024];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();
    CCtiReqItem* pCtiItem = CIVRDriverController::Inst()->GetCtiReqInfo(szReqKey);
    if(pCtiItem == NULL)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:NULL|REQ_KEY:%s|Func:CALLBACK_%s, Not Found GCTI Info. XmlData:%s"
                            , szReqKey
                            , szCommand
                            , szXmlData );
        return;
    }

    string strReqCmd = string(pCtiItem->m_szReqCommand);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, Invalid Command. XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-4d|REQ_KEY:%s|Func:CALLBACK_%s, XmlData:%s"
                            , pCtiItem->m_portInfo.nPort
                            , szReqKey
                            , szCommand
                            , szXmlData);

    strcpy(pCtiItem->m_szResCommand, szCommand);    
    QueueItem qItem = pCtiItem->m_stQueueItem;
    
    lineCmd.setKeyValue("sRequest", pCtiItem->m_szReqCommand, szBody);
    lineCmd.setKeyValue("sResult" , "1"      , szBody);
    lineCmd.setKeyValue("sDesc"   , szResult , szBody);
    lineCmd.setKeyValue("sUDataEx", szUDataEx, szBody);

    ctrl->PushResponse(qItem, szBody);
}



bool CIVRDriverController::PushResponse(QueueItem qItem, NPCSTR szUserData)
{
    char            szPacket[SOCKET_BUFFER_SIZE];
    char            szTemp[2048];
    pktXCCPHeader*  pHeader;
    int             nPacketSize;

    memset(szPacket, 0x00, sizeof(szPacket));
    pHeader = (pktXCCPHeader*)szPacket; 

    memcpy(szPacket, qItem.szRequestPacket, sizeof(pktXCCPHeader));

    memset(szTemp, 0x00, sizeof(szTemp));
    memcpy(szTemp, pHeader->header.sFrom, sizeof(pHeader->header.sFrom));

    memcpy(pHeader->header.sFrom, pHeader->header.sTo, sizeof(pHeader->header.sFrom));
    memcpy(pHeader->header.sTo, szTemp, sizeof(pHeader->header.sFrom));
    memcpy(pHeader->sCommandType, CB_CMDTYPE_DATA_SUCCESS, sizeof(pHeader->sCommandType));
    memcpy(szPacket + sizeof(pktXCCPHeader), szUserData, strlen(szUserData));
    
    nPacketSize = sizeof(pktXCCPHeader) + strlen(szUserData);
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%05d", nPacketSize + 1);
    memcpy(pHeader->header.sLen, szTemp, sizeof(pHeader->header.sLen));

    szPacket[nPacketSize] = PACKET_ETX;
    qItem.nResponsePacketLen = nPacketSize + 1;
    memcpy(qItem.szResponsePacket, szPacket, nPacketSize + 1);

    CQueueOfResponse::Inst()->Enqueue(qItem);
    
    return true;
}


/****************************************************************************
 TITLE       : LOG_WRITE
 DESCRIPTION : 로그메시지에 추가정보 삽입.
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriverController::LOG_WRITE(uint32 nLogLvl, NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    char szLog[2048];
    memset(szLog, 0x00, sizeof(szLog));
    sprintf(szLog, "%-12s|%s:%d|%s", GetName(), m_pDriver->GetRemoteIp(), m_pDriver->GetRemotePort(), buf);

    CLogEx::WRITE(nLogLvl, szLog);
}


