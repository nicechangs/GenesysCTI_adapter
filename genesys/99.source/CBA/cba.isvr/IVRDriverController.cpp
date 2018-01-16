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

#include "CBAGlobal.h"
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
    
    // map clear..
    m_mapGCTIPort.clear();
    m_mapGCTIItem.clear();

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
        iSleep(10);

        if(CQueueOfRequest::Inst()->GetCount() <= 0)
        {
            m_pDriver->ContinueKeepAlive(true);
            continue;
        }

        // 요청건이 존재하면 Alive check 중지.
        m_pDriver->ContinueKeepAlive(false);

        QUEUE_ITEM qItem = CQueueOfRequest::Inst()->Dequeue();
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
        int nChnlId     = nStartChannel + nIndex;
        int nPortNo     = nStartPort + nIndex;
        
        GCTI_PORT stGCtiPort;
        memset(&stGCtiPort, 0x00, sizeof(stGCtiPort));

        sprintf(stGCtiPort.szPortKey, "%04d%04d", nTESystemId, nChnlId);
        stGCtiPort.nTeSysID     = nTESystemId;
        stGCtiPort.nIdx         = nIndex;
        stGCtiPort.nChnlId      = nChnlId;
        stGCtiPort.nGCtiPort    = nPortNo;

        AddGCtiPort(stGCtiPort);
    }

    return true;
}


/****************************************************************************
 TITLE       : AddGCtiPort
 DESCRIPTION : GCTI포트 추가
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::AddGCtiPort(GCTI_PORT stCtiPort)
{
    m_mutexLock.Lock();
    m_mapGCTIPort.insert(make_pair(stCtiPort.szPortKey, stCtiPort));
    m_mutexLock.Unlock();

    LOG_WRITE(ILOG_INFO, stCtiPort.nGCtiPort, "EMPTY", "TE:%d|CH:%d|PORT_KEY:%s|TOT_CNT:%d|MSG:Add port info."
                            , stCtiPort.nTeSysID
                            , stCtiPort.nChnlId
                            , stCtiPort.szPortKey
                            , m_mapGCTIPort.size()
                            );
    return true;
}


/****************************************************************************
 TITLE       : GetGCtiPort
 DESCRIPTION : 포트정보 얻음.
 PARAMS      : TE시스템아이디
               채널번호
               포트정보 포인터

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::GetGCtiPort(int32 nTESystemId, int32 nChannelNo, GCTI_PORT& stPort)
{
    char szPortKey[GCTI_PORT_KEY_SIZE];
    memset(szPortKey, 0x00, sizeof(szPortKey));
    sprintf(szPortKey, "%04d%04d", nTESystemId, nChannelNo);

    MAP_GCTI_PORT::iterator itor;
    bool bRet = false;
    m_mutexLock.Lock();    
    itor = m_mapGCTIPort.find( string(szPortKey) );
    if(itor != m_mapGCTIPort.end())
    {
        stPort = itor->second;
        bRet = true;
    }
    else
    {
        bRet = false;
    }
    m_mutexLock.Unlock();

    return bRet;
}


/****************************************************************************
 TITLE       : UpdateGCtiPort
 DESCRIPTION : 포트정보 Update.
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::UpdateGCtiPort(GCTI_PORT stCtiPort)
{
    MAP_GCTI_PORT::iterator itor;
    bool bRet = false;
    m_mutexLock.Lock();    
    itor = m_mapGCTIPort.find( string(stCtiPort.szPortKey) );
    if(itor != m_mapGCTIPort.end())
    {
        itor->second = stCtiPort;
        bRet = true;
    }
    else
    {
        bRet = false;
    }
    m_mutexLock.Unlock();

    return bRet;
}


/****************************************************************************
 TITLE       : AddCtiReqInfo
 DESCRIPTION : CTI로 요청 정보 보관
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::AddGCtiItem(NPCSTR szReqKey, GCTI_ITEM stGctiItem)
{
    m_mutexLock.Lock();
    m_mapGCTIItem.insert(make_pair(string(szReqKey), stGctiItem));
    m_mutexLock.Unlock();

    LOG_WRITE(ILOG_LV1, stGctiItem.nGctiPort, stGctiItem.szReqCmd, "REQKEY:%s|TOT_CNT:%d|MSG:Add GCTI Request Info."
                            , szReqKey, m_mapGCTIItem.size());
    return true;
}



/****************************************************************************
 TITLE       : GetGCtiItem
 DESCRIPTION : CTI 요청 아이템 얻음.
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::GetGCtiItem(NPCSTR szReqKey, GCTI_ITEM& stGctiItem)
{
    MAP_GCTI_ITEM::iterator itor;

    bool bRet = false;
    m_mutexLock.Lock();    
    itor = m_mapGCTIItem.find( string(szReqKey) );
    if(itor != m_mapGCTIItem.end())
    {
        stGctiItem = itor->second;
        bRet = true;
    }
    else
    {
        bRet =  false;
    }

    m_mutexLock.Unlock();
    return bRet;
}


bool CIVRDriverController::RemoveGCtiItem(NPCSTR szReqKey)
{
    GCTI_ITEM stItem;
    GetGCtiItem(szReqKey, stItem);

    m_mapGCTIItem.erase(string(szReqKey));
    
    LOG_WRITE(ILOG_LV1, stItem.nGctiPort, stItem.szReqCmd, "REQKEY:%s|TOT_CNT:%d|MSG:Remove GCTI Request Info."
                            , szReqKey, m_mapGCTIItem.size());
    return true;
}


/****************************************************************************
 TITLE       : UpdateGCtiItem
 DESCRIPTION : CTI 요청 아이템 얻음.
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriverController::UpdateGCtiItem(GCTI_ITEM stGctiItem)
{
    MAP_GCTI_ITEM::iterator itor;

    bool bRet = false;
    m_mutexLock.Lock();    
    itor = m_mapGCTIItem.find( string(stGctiItem.szReqKey) );
    if(itor != m_mapGCTIItem.end())
    {
        itor->second = stGctiItem;
        bRet = true;
    }
    else
    {
        bRet =  false;
    }

    m_mutexLock.Unlock();
    return bRet;
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
    m_pDriver->Set_CALLBACK_PeekStatReq(CALLBACK_PeekStatReq);
    m_pDriver->Set_CALLBACK_StatResp(CALLBACK_StatResp);

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
bool CIVRDriverController::HandleRequest(QUEUE_ITEM stItem)
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

    if( strcmp(szCmd, "icStatReq") == 0 )
        return HandleRequest_icStatReq(stItem);

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
bool CIVRDriverController::HandleRequest_icMakeCall(QUEUE_ITEM stItem)
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
bool CIVRDriverController::HandleRequest_icNewCall(QUEUE_ITEM stItem)
{
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szDNIS[MAX_DNIS_SIZE];
    char szANI[MAX_ANI_SIZE];
    char szUDataEx[GCTI_UDATA_SIZE];
    char szExtnsEx[GCTI_EXDATA_SIZE];

    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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
    
    memcpy(szCallId, stItem.stXCCPHeader.sCallID, sizeof(stItem.stXCCPHeader.sCallID)-1 );

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , ""                , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    // reference call id 생성 : 존재시 이전 키로 진행.
    if(strlen(szReqKey) <= 0)
        m_pDriver->CreateRequestKey(szReqKey, sizeof(szReqKey));
    
    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);


    // NewCall은 항상 새로운 콜이여야만 함.
    GCTI_ITEM stCtiItem;
    if(GetGCtiItem(szReqKey, stCtiItem))
        RemoveGCtiItem(szReqKey);

    memset(&stCtiItem, 0x00, sizeof(stCtiItem));
    strcpy(stCtiItem.szReqKey, szReqKey);
    stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
    strcpy(stCtiItem.szReqCmd, szCmd);
    stCtiItem.eCallStatus = eCallStatusUnknown;

    // Queue Item SET;
    stCtiItem.stQItem = stItem;
    

    // I-Server로 요청할 아이템 추가.
    AddGCtiItem(szReqKey, stCtiItem);
    
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
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NoteCallStartFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd              , szBody);
        lineCmd.setKeyValue("sResult" , "0"                , szBody);
        lineCmd.setKeyValue("sReqKey" , ""                 , szBody);
        lineCmd.setKeyValue("sDesc"   , "NoteCallStartFail", szBody);
        PushResponse(stItem, szBody);
        RemoveGCtiItem(szReqKey);
        return false;
    }

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
bool CIVRDriverController::HandleRequest_icWaitInfo(QUEUE_ITEM stItem)
{

    return true;
}


bool CIVRDriverController::HandleRequest_icRouteRequest(QUEUE_ITEM stItem)
{
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szRouteDN[MAX_DN_SIZE];
    char szCED[GCTI_CED_SIZE];
    char szUDataEx[GCTI_UDATA_SIZE];
    char szExtnsEx[GCTI_EXDATA_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest"  , "icRouteReq"      , szBody);
        lineCmd.setKeyValue("sResult"   , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey"   , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"     , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sRouteType", ""                , szBody);
        lineCmd.setKeyValue("sDest"     , ""                , szBody);
        lineCmd.setKeyValue("sExtnsEx"  , ""                , szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);
    
    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
    
    // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest"  , szCmd        , szBody);
        lineCmd.setKeyValue("sResult"   , "0"          , szBody);
        lineCmd.setKeyValue("sReqKey"   , szReqKey     , szBody);
        lineCmd.setKeyValue("sDesc"     , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sRouteType", ""           , szBody);
        lineCmd.setKeyValue("sDest"     , ""           , szBody);
        lineCmd.setKeyValue("sExtnsEx"  , ""           , szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);        
    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

    // 라우트 요청
    ilRET ret = m_pDriver->RouteRequest(szReqKey, szGCTIPort, szRouteDN, szCED, szUDataEx, szExtnsEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NoteCallStartFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

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
bool CIVRDriverController::HandleRequest_icDeflectTR(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;

    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szDestDN[MAX_DN_SIZE];
    char szUDataEx[MAX_PACKET_BODY_SIZE];
    char szExtnsEx[MAX_PACKET_LEN_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
    
    // Need NewCall
    if(strlen(szReqKey)<=0)
    {        
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                                    , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);        
    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);
    
    ilRET ret = m_pDriver->CallTransferKVList(szReqKey, szGCTIPort, szDestDN, szUDataEx, szExtnsEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:CallTransferKVListFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd                   , szBody);
        lineCmd.setKeyValue("sResult" , "0"                     , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey                , szBody);
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
bool CIVRDriverController::HandleRequest_icCallInfo(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

    memset(szCallId   , 0x00, sizeof(szCallId));
    memset(szReqKey   , 0x00, sizeof(szReqKey));
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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);


    // get port info
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest"          , szCmd             , szBody);        
        lineCmd.setKeyValue("sResult"           , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey"           , szReqKey          , szBody);
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

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);
    
    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

    // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest"          , szCmd        , szBody);        
        lineCmd.setKeyValue("sResult"           , "0"          , szBody);
        lineCmd.setKeyValue("sReqKey"           , szReqKey     , szBody);
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

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
    }

    // 콜 상태 체크    
    if(stCtiItem.eCallStatus != eCallStatusRinging &&
        stCtiItem.eCallStatus != eCallStatusEstablished)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-4s|REQ_KEY:%s|Invalid CallStatus. Command:%s, CallStatus:%s, Data:%s"
                            , szGCTIPort, szReqKey, szCmd, stCtiItem.szEvent, stItem.szRequestPacket);

        lineCmd.setKeyValue("sRequest"          , szCmd              , szBody);
        lineCmd.setKeyValue("sResult"           , "0"                , szBody);
        lineCmd.setKeyValue("sReqKey"           , szReqKey           , szBody);
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
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

    ilRET ret = m_pDriver->GetCallInfo(szReqKey, szGCTIPort);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NoteCallStartFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest"          , szCmd, szBody);
        lineCmd.setKeyValue("sResult"           , "0"              , szBody);
        lineCmd.setKeyValue("sReqKey"           , szReqKey         , szBody);
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
bool CIVRDriverController::HandleRequest_icClearCall(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szEndCause[GCTI_EVENT_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

     // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);        

    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

     ilRET ret = m_pDriver->NoteCallEnd(szReqKey, szGCTIPort, szEndCause);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NoteCallEndFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NoteCallEndFail", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

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
bool CIVRDriverController::HandleRequest_icSendUData(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szRequestId[GCTI_REQUESTID_SIZE];
    char szAction[GCTI_ACTION_SIZE];
    char szUDataEx[GCTI_UDATA_SIZE];
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

    // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

    ilRET ret = m_pDriver->UDataSetKV(szReqKey, szGCTIPort, szRequestId, szAction, szUDataEx);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:UDataAddKVFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
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
bool CIVRDriverController::HandleRequest_icGetUData(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szUDataReqId[GCTI_REQUESTID_SIZE];    
    char szKeys[GCTI_UDATA_KEY_SIZE];    
    char szBody[GCTI_UDATA_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

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
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

    // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        lineCmd.setKeyValue("sUDataEx", "", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);        
    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

    ilRET ret = m_pDriver->UDataGetKV(szReqKey, szGCTIPort, szUDataReqId, szKeys);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:UDataGetKVFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
        
        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "UDataGetKVFail", szBody);
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
bool CIVRDriverController::HandleRequest_icStatReq(QUEUE_ITEM stItem)
{
    int  nTeNo      = 0;
    int  nChannelNo = 0;
    char szCallId[MAX_CALLID_SIZE];
    char szReqKey[GCTI_REQKEY_SIZE];
    char szTENo[MAX_TENO_SIZE];
    char szChanNo[MAX_CHNO_SIZE];
    char szRequestId[GCTI_REQUESTID_SIZE];
    char szStatName[GCTI_STAT_NAME_SIZE]; 
    char szBody[MAX_PACKET_BODY_SIZE];
    char szCmd[GCTI_COMMAND_SIZE];
    char szPacketLen[MAX_PACKET_LEN_SIZE];
    char szGCTIPort[GCTI_PORT_SIZE];

    memset(szCallId    , 0x00, sizeof(szCallId));
    memset(szReqKey    , 0x00, sizeof(szReqKey));
    memset(szTENo      , 0x00, sizeof(szTENo));
    memset(szChanNo    , 0x00, sizeof(szChanNo));
    memset(szRequestId , 0x00, sizeof(szRequestId));
    memset(szStatName  , 0x00, sizeof(szStatName));
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

    strTemp = lineCmd[(char*)"sReqKey"].getValue();
    strcpy(szReqKey, strTemp.c_str());

    strTemp = lineCmd[(char*)"sTENo"].getValue();
    strcpy(szTENo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sChanNo"].getValue();
    strcpy(szChanNo, strTemp.c_str());

    strTemp = lineCmd[(char*)"sStatName"].getValue();
    strcpy(szStatName, strTemp.c_str());

    strTemp = lineCmd[(char*)"sRequestId"].getValue();
    strcpy(szRequestId, strTemp.c_str());

    nTeNo      = atoi(szTENo);
    nChannelNo = atoi(szChanNo);

     // get port info
    GCTI_PORT stCtiPort;
    memset(&stCtiPort, 0x00, sizeof(stCtiPort));    
    if(!GetGCtiPort(nTeNo, nChannelNo, stCtiPort))
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Not Found PortInfo."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "NotFoundPortInfo", szBody);
        lineCmd.setKeyValue("sValue", "", szBody);
        PushResponse(stItem, szBody);        
        return false;
    }

    sprintf(szGCTIPort, "%d", stCtiPort.nGCtiPort);

    LOG_WRITE(ILOG_LV1, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Parameter Analysis Success."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

      // Need NewCall
    if(strlen(szReqKey)<=0)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:Need icNewCall."
                            , szReqKey, szCallId, nTeNo, nChannelNo);

        lineCmd.setKeyValue("sRequest", szCmd             , szBody);
        lineCmd.setKeyValue("sResult" , "0"               , szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey          , szBody);
        lineCmd.setKeyValue("sDesc"   , "NeedNewCall", szBody);
        PushResponse(stItem, szBody);
        return false;
    }

    GCTI_ITEM stCtiItem;
    if(!GetGCtiItem(szReqKey, stCtiItem))
    {
        memset(&stCtiItem, 0x00, sizeof(stCtiItem));
        strcpy(stCtiItem.szReqKey, szReqKey);
        stCtiItem.nGctiPort = stCtiPort.nGCtiPort;
        strcpy(stCtiItem.szReqCmd, szCmd);        
        stCtiItem.stQItem = stItem;
        
        AddGCtiItem(szReqKey, stCtiItem);
        LOG_WRITE(ILOG_WARN, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NotFound RequestKey => Created GCTI Item."
                            , szReqKey, szCallId, nTeNo, nChannelNo);        
    }

    // 요청건 정보 업데이트.
    strcpy(stCtiItem.szReqCmd, szCmd);
    memcpy(stCtiItem.stQItem.szRequestPacket, stItem.szRequestPacket, stItem.nRequestPacketLen);
    UpdateGCtiItem(stCtiItem);

    ilRET ret = m_pDriver->PeekStatReq(szReqKey, szGCTIPort, szRequestId, szStatName);
    if(ret != ilRET_SUCCESS)
    {
        LOG_WRITE(ILOG_ERR, stCtiPort.nGCtiPort, szCmd, "REQKEY:%s|CALLID:%s|TE:%d|CH:%04d|MSG:NoteCallStartFail."
                            , szReqKey, szCallId, nTeNo, nChannelNo);
        
        lineCmd.setKeyValue("sRequest", szCmd, szBody);
        lineCmd.setKeyValue("sResult" , "0", szBody);
        lineCmd.setKeyValue("sReqKey" , szReqKey, szBody);
        lineCmd.setKeyValue("sDesc"   , "StatReqFail", szBody);
        lineCmd.setKeyValue("sValue"  , "", szBody);
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
    ctrl->LOG_WRITE(ILOG_LV1, 0, szCommand, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
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
    ctrl->LOG_WRITE(ILOG_LV1, 0, szCommand, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
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
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, atoi(szCalledNum), szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    lineCmd.setKeyValue("sRequest", stCtiItem.szReqCmd      , szBody);
    lineCmd.setKeyValue("sResult" , "1"                     , szBody);
    lineCmd.setKeyValue("sReqKey" , szReqKey                , szBody);
    lineCmd.setKeyValue("sDesc"   , "Success"               , szBody);

    ctrl->PushResponse(qItem, szBody);
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


    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    stCtiItem.eCallStatus = eCallStatus;
    strcpy(stCtiItem.szEvent, szEvent);
    strcpy(stCtiItem.szResCmd, szCommand);

    ctrl->UpdateGCtiItem(stCtiItem);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    
    if( strcmp(strReqCmd.c_str(), "icDeflectTR") == 0 )
    {
        lineCmd.setKeyValue("sRequest", stCtiItem.szReqCmd, szBody);
        lineCmd.setKeyValue("sReqKey", szReqKey, szBody);
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
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
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
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    lineCmd.setKeyValue("sRequest"  , stCtiItem.szReqCmd, szBody);
    lineCmd.setKeyValue("sResult"   , "1"         , szBody);
    lineCmd.setKeyValue("sReqKey"   , szReqKey    , szBody);
    lineCmd.setKeyValue("sDesc"     , "Success"   , szBody);
    lineCmd.setKeyValue("sRouteType", szRouteType , szBody);
    lineCmd.setKeyValue("sDest"     , szDest      , szBody);
    lineCmd.setKeyValue("sExtnsEx"  , szExtnsEx   , szBody);

    ctrl->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_OneStepXfer  (NPCSTR szDriverName
                                          , NPCSTR szCommand
                                          , NPCSTR szReqKey
                                          , NPCSTR szDestDN
                                          , NPCSTR szUDataEx
                                          , NPCSTR szExtnsEx
                                          , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

   
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
}

void CIVRDriverController::CALLBACK_CallInfoReq  (NPCSTR szDriverName
                                          , NPCSTR szCommand
                                          , NPCSTR szReqKey
                                          , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
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
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, atoi(szCalledNum), szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    
    lineCmd.setKeyValue("sRequest"          , stCtiItem.szReqCmd , szBody);
    lineCmd.setKeyValue("sResult"           , "1"                , szBody);
    lineCmd.setKeyValue("sReqKey"           , szReqKey           , szBody);
    lineCmd.setKeyValue("sDesc"             , "Success"          , szBody);
    lineCmd.setKeyValue("sANI"              , szANI              , szBody);
    lineCmd.setKeyValue("sDNIS"             , szDNIS             , szBody);
    lineCmd.setKeyValue("sCalledNum"        , szCalledNum        , szBody);
    lineCmd.setKeyValue("sConnId"           , szConnId           , szBody);
    lineCmd.setKeyValue("sFirstHomeLocation", szFirstHomeLocation, szBody);
    lineCmd.setKeyValue("sLastEvent"        , szLastEvent        , szBody);
    lineCmd.setKeyValue("sOtherDN"          , szOtherDN          , szBody);
    lineCmd.setKeyValue("sOtherQueue"       , szOtherQueue       , szBody);
    lineCmd.setKeyValue("sOtherTrunk"       , szOtherTrunk       , szBody);
    lineCmd.setKeyValue("sPortDN"           , szPortDN           , szBody);
    lineCmd.setKeyValue("sPortQueue"        , szPortQueue        , szBody);
    lineCmd.setKeyValue("sPortTrunk"        , szPortTrunk        , szBody);
    lineCmd.setKeyValue("sThirdPartyDN"     , szThirdPartyDN     , szBody);
    lineCmd.setKeyValue("sTSCallId"         , szTSCallId         , szBody);
    
    ctrl->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_EndCallReq (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData)
{   
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

   
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
    QUEUE_ITEM qItem = stCtiItem.stQItem;
    lineCmd.setKeyValue("sRequest"  , stCtiItem.szReqCmd, szBody);
    lineCmd.setKeyValue("sResult"   , "1"         , szBody);
    lineCmd.setKeyValue("sReqKey"   , szReqKey    , szBody);
    lineCmd.setKeyValue("sDesc"     , "Success"   , szBody);

    ctrl->PushResponse(qItem, szBody);
    ctrl->RemoveGCtiItem(szReqKey);
}



void CIVRDriverController::CALLBACK_EndCallResp (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szEndCause
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

 
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_WARN, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info. May be already removed."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    lineCmd.setKeyValue("sRequest"  , stCtiItem.szReqCmd, szBody);
    lineCmd.setKeyValue("sResult"   , "1"         , szBody);
    lineCmd.setKeyValue("sReqKey"   , szReqKey    , szBody);
    lineCmd.setKeyValue("sDesc"     , "Success"   , szBody);

    ctrl->PushResponse(qItem, szBody);
    ctrl->RemoveGCtiItem(szReqKey);
}


void CIVRDriverController::CALLBACK_UDataSet (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szUDataReqId
                                            , NPCSTR szAction
                                            , NPCSTR szUDataEx
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
}

void CIVRDriverController::CALLBACK_UDataGet (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szKeys
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
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
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;
    
    lineCmd.setKeyValue("sRequest", stCtiItem.szReqCmd, szBody);
    lineCmd.setKeyValue("sResult" , "1"      , szBody);
    lineCmd.setKeyValue("sReqKey" , szReqKey , szBody);
    lineCmd.setKeyValue("sDesc"   , szResult , szBody);
    lineCmd.setKeyValue("sUDataEx", szUDataEx, szBody);

    ctrl->PushResponse(qItem, szBody);
}


void CIVRDriverController::CALLBACK_PeekStatReq (NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szStatName
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

    
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);
}

void CIVRDriverController::CALLBACK_StatResp(NPCSTR szDriverName
                                            , NPCSTR szCommand
                                            , NPCSTR szReqKey
                                            , NPCSTR szResult
                                            , NPCSTR szResultCode
                                            , NPCSTR szXmlData)
{
    CLineCommand lineCmd;
    char szBody[SOCKET_BUFFER_SIZE];
    memset(szBody, 0x00, sizeof(szBody));

    CIVRDriverController *ctrl = CIVRDriverController::Inst();

  
    GCTI_ITEM stCtiItem;
    if(!ctrl->GetGCtiItem(szReqKey, stCtiItem))
    {
        ctrl->LOG_WRITE(ILOG_ERR, 0, szCommand, "REQKEY:%s|%s|XML:%s|MSG:Not Found GCTI Info."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    string strReqCmd = string(stCtiItem.szReqCmd);
    if(strReqCmd.size() <= 0)
    {
        ctrl->LOG_WRITE(ILOG_ERR, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s|MSG:Invalid Command."
                        , szReqKey, __FUNCTION__, szXmlData);
        return;
    }

    ctrl->LOG_WRITE(ILOG_LV1, stCtiItem.nGctiPort, stCtiItem.szReqCmd, "REQKEY:%s|%s|XML:%s", szReqKey, __FUNCTION__, szXmlData);

    QUEUE_ITEM qItem = stCtiItem.stQItem;

    lineCmd.setKeyValue("sRequest", stCtiItem.szReqCmd, szBody);
    if( strcmp(szResultCode, "Success") == 0 )
        lineCmd.setKeyValue("sResult" , "1", szBody);
    else
        lineCmd.setKeyValue("sResult" , "0", szBody);
    
    lineCmd.setKeyValue("sReqKey" , szReqKey , szBody);
    lineCmd.setKeyValue("sDesc"   , szResultCode , szBody);
    lineCmd.setKeyValue("sValue"  , szResult, szBody);

    ctrl->PushResponse(qItem, szBody);
}



bool CIVRDriverController::PushResponse(QUEUE_ITEM qItem, NPCSTR szUserData)
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

    g_iLog.Log(nLogLvl, szLog);
}

void CIVRDriverController::LOG_WRITE(uint32 nLogLevel, int32 nCtiPort, NPCSTR szCmd, NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[2048];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    char szLog[2048];
    memset(szLog, 0x00, sizeof(szLog));
    sprintf(szLog, "%-12s|PORT:%05d|%-12s|%s", GetName(), nCtiPort, szCmd, buf);

    g_iLog.Log(nLogLevel, szLog);
}
