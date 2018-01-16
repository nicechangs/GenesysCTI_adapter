/*
***************************************************************************
        Bridgetec.cop 
        IVR driver for Genesys IVR Server.

        TITLE   : CIVRDriver
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

// include ..
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

// icore..
#include <iCore/iLog.h>

#include "IVRDriver.h"

// constructor && desctructor
CIVRDriver::CIVRDriver(NPCSTR szOwnerName
                    , NPCSTR szOwnerId
                    , NPCSTR szSvcAppName
                    , NPCSTR szSvcObjectName)
{
    Clear();

    strcpy(m_szOwnerName    , szOwnerName);
    strcpy(m_szOwnerId      , szOwnerId);
    strcpy(m_szSvcAppName   , szSvcAppName);
    strcpy(m_szSvcObjectName, szSvcObjectName);
}

CIVRDriver::~CIVRDriver(void) 
{
}



/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::StartUp(NPVOID pArg)
{
    if(pArg == NULL)
        return Start();
    else
        return Start(pArg);
}


/****************************************************************************
 TITLE       : StopAll
 DESCRIPTION : Stop thread
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::StopAll()
{
    if(m_pKeepAlive != NULL)
    {
        m_pKeepAlive->StopAll();
        delete m_pKeepAlive;
    }

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
NPVOID CIVRDriver::ThreadRun(NPVOID pArg)
{
    char szDataBuf[SOCK_BUFFER_SIZE];
    int iSelectRes         = 0;
    int nLen               = 0;
    
    char szPartialPacket[SOCK_BUFFER_SIZE];
    char szGLIHeader[GLI_HEADER_LENGTH + 1];
    int  ntohs_len         = 0;    
    int  nPartialPacketLen = 0;
    int  nDataOffset       = 0;
    uint32 nLogLv;
    
    char szXml[MAX_XML_SIZE];
    int nXmlLen = 0;
    
    ilRET ret = ilRET_NONE;

    while(IsOn())
    { 
        ret = Reconnect();
        if(ret != ilRET_SUCCESS)
        {
            iSleep(ONESEC * 5);
            continue;
        }

        if(GetSocket() < 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket GetSocket() error.");
            Disconnect();
            continue;
        }

        iSelectRes = RSelect(5);

        if(iSelectRes < 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket RSelect error.");
            Disconnect();
            iSleep(ONESEC * 5);
            continue;
        }
 
        if(iSelectRes == 0) 
            continue;
    
        memset(szDataBuf, 0x00, sizeof(szDataBuf));
        nLen = Recv(szDataBuf, sizeof(szDataBuf));
        if(nLen <= 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket Recv error.");
            Disconnect();
            iSleep(ONESEC * 5);
            continue;
        }

        // callback..
        if(m_pCALLBACK_PacketReceived != NULL)
            m_pCALLBACK_PacketReceived(GetObjectName(), szDataBuf);

        // 여러개의 xml데이터 처리.
        nDataOffset = 0;
        nLogLv      = ILOG_LV1;
        while(nDataOffset < nLen)
        {
            memset(szPartialPacket, 0x00, sizeof(szPartialPacket));
            memset(szXml, 0x00, sizeof(szXml));
            memset(szGLIHeader, 0x00, sizeof(szGLIHeader));
            memcpy(szGLIHeader, szDataBuf + nDataOffset, GLI_HEADER_LENGTH);

            GLI_HEADER *pGLIHeader = (GLI_HEADER*)szGLIHeader;
            ntohs_len = (int)ntohs(pGLIHeader->htons_len);
            nPartialPacketLen = ntohs_len + 4;

            if(pGLIHeader->cMsgType == GLI_MSGTYPE_KEEP_ALIVE_ACK)
                nLogLv = ILOG_LV3;
            
            LOG_WRITE(nLogLv, "%-15s|GLIMsgType:%x, htons_len:%06x, GLI_VERSION:%x, Data:(%06d)[%s]"
                                , "RECEIVE"
                                , pGLIHeader->cMsgType
                                , pGLIHeader->htons_len
                                , pGLIHeader->cGLI_VERSION
                                , nLen
                                , szDataBuf);

            memcpy(szPartialPacket, szDataBuf + nDataOffset, nPartialPacketLen);
            nDataOffset += nPartialPacketLen;

            nXmlLen = nPartialPacketLen - GLI_HEADER_LENGTH;
            memcpy(szXml, szPartialPacket + GLI_HEADER_LENGTH, nXmlLen);

            // HeartBit
            if(pGLIHeader->cMsgType == GLI_MSGTYPE_KEEP_ALIVE_ACK)
                continue;
               
            // 수신 패킷 처리
            HandlePacket(szXml, nXmlLen);
        }
    }
    
    return 0;
}


/****************************************************************************
 TITLE       : CreateRequestKey 
 DESCRIPTION : GCTI로 요청할 Key생성 (NewCall 에서 생성됨.)
 PARAMS      : buffer, buffer size
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.12.01
****************************************************************************/
void CIVRDriver::CreateRequestKey(NPSTR szBuf, int nSize)
{
    if(m_nReqKeySeq >= 9999)
        m_nReqKeySeq = 1;

    // increase key sequence..
    m_nReqKeySeq++;

    memset(szBuf, 0x00, nSize);

    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime );
    timeinfo = localtime ( &rawtime );
    
    sprintf(szBuf, "%s_%s_%02d%02d%02d%02d%02d%06d%04d"
                    , GetObjectName()
                    , GetAppName()
                    , (timeinfo->tm_mon+1)
                    , timeinfo->tm_mday
                    , timeinfo->tm_hour
                    , timeinfo->tm_min
                    , timeinfo->tm_sec
                    , milli
                    , m_nReqKeySeq
                    );
}


/****************************************************************************
 TITLE       : Initiate 
 DESCRIPTION : IVRServer Driver initialize
 PARAMS      : szRemoteIp  - GCTI IP
               nRemotePort - GCTI Port
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.12.01
****************************************************************************/
ilRET CIVRDriver::Initiate( NPCSTR szRemoteIp
                            , uint32 nRemotePort)
{
    strcpy(m_szRemoteIp, szRemoteIp);
    if(strlen(m_szRemoteIp) <= 0)
        return ilRET_INVALID_REMOTE_IP;

    m_nRemotePort = nRemotePort;
    if(m_nRemotePort <= 0)
        return ilRET_INVALID_REMOTE_PORT;

    // set version..
    ilRET ret = SetVersionNumber("8.1");
    if(ret != ilRET_SUCCESS)
        return ret;

    LOG_WRITE(ILOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    LOG_WRITE(ILOG_INFO, "IVRDriver Initialize success.");
    LOG_WRITE(ILOG_INFO, "OwnerName     : %s", m_szOwnerName);
    LOG_WRITE(ILOG_INFO, "OwnerId       : %s", m_szOwnerId);
    LOG_WRITE(ILOG_INFO, "SvcAppName    : %s", m_szSvcAppName);
    LOG_WRITE(ILOG_INFO, "SvcObjectName : %s", m_szSvcObjectName);
    LOG_WRITE(ILOG_INFO, "LogPath       : %s", m_szLogPath);
    LOG_WRITE(ILOG_INFO, "LogLinkPath   : %s", m_szLogLinkPath);
    LOG_WRITE(ILOG_INFO, "LogLevel      : %d", m_nLogLevel);
    LOG_WRITE(ILOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    m_bIsInit = true;

    return ilRET_SUCCESS;
}



/****************************************************************************
 TITLE       : Deinitiate 
 DESCRIPTION : IVRServer Driver Deinitialize..
 PARAMS      : 
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::Deinitiate(void)
{
    Clear();
    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : InitLog 
 DESCRIPTION : IVRServer Driver log initialize..
 PARAMS      : strLogPath    - 로그생성 위치
               strLogLinkPath- 로그링크 생성위치
               strLogPrefix  - 로그파일 접두어
               nLogLevel     - 로그레벨
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::InitLog(NPCSTR szLogPath
                            , NPCSTR szLogLinkPath
                            , NPCSTR szLogPrefix
                            , uint32 nLogLevel)
{
    memset(m_szLogPath      , 0x00, sizeof(m_szLogPath));
    memset(m_szLogLinkPath  , 0x00, sizeof(m_szLogLinkPath));
    memset(m_szLogPrefix    , 0x00, sizeof(m_szLogPrefix));
    m_nLogLevel = 0;

    strcpy(m_szLogPath, szLogPath);
    strcpy(m_szLogLinkPath, szLogLinkPath);
    strcpy(m_szLogPrefix, szLogPrefix);
    m_nLogLevel = nLogLevel;
    
    m_log.Init(m_szLogPrefix, m_szLogPath, m_nLogLevel);
    m_log.MakeLink(m_szLogPrefix, m_szLogLinkPath);

    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : Reconnect
 DESCRIPTION : IVRServer Driver Reconnect to genesys IVR SErver
 PARAMS      : 
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::Reconnect()
{
    if(!IsInitDriver())
        return ilRET_NOT_INITIALIZED;

    if(IsConnected())
        return ilRET_SUCCESS;
    
    return ConnectionOpen(m_szRemoteIp, m_nRemotePort);
}


/****************************************************************************
 TITLE       : Disconnect
 DESCRIPTION : IVRServer Driver Reconnect to genesys IVR SErver
 PARAMS      : 
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriver::Disconnect()
{
    if(GetSocket() >= 0)
        Close();

    m_bIsConnected = false;
    LOG_WRITE(ILOG_ERR, "Disconnect success.");
}


/****************************************************************************
 TITLE       : ConnectionOpen 
 DESCRIPTION : IVRServer Driver connect to genesys IVR SErver
 PARAMS      : szRemote    - 제네시스 서버 접속 ip
               nRemotePort - 제네시스 서버 접속 port
               nTimeout    - 서버접속 타임아웃
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::ConnectionOpen(NPCSTR szRemote
                                , uint32 nRemotePort
                                , uint32 nTimeout)
{
    if(!Open(PF_INET, SOCK_STREAM))
        return ilRET_FAIL_SOCKET_OPEN;

    if(!Connect(szRemote, nRemotePort, nTimeout))
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        m_bIsConnected = false;
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    LOG_WRITE(ILOG_INFO, "Connection success.");

    // set connection flag
    m_bIsConnected = true;

    char szCallId[50];
    char szVersion[8];
    char szClientName[50];
    char szReportStatus[10];
    char szServerMonitor[10];
    char szXmlbuf[MAX_XML_SIZE];

    memset(szCallId        , 0x00, sizeof(szCallId));
    memset(szVersion       , 0x00, sizeof(szVersion));
    memset(szClientName    , 0x00, sizeof(szClientName));
    memset(szReportStatus  , 0x00, sizeof(szReportStatus));
    memset(szServerMonitor , 0x00, sizeof(szServerMonitor));
    memset(szXmlbuf        , 0x00, sizeof(szXmlbuf));

    // call id 생성
    CreateRequestKey(szCallId, sizeof(szCallId));

    strcpy(szVersion       , "4.0");
    strcpy(szClientName    , GetObjectName());
    strcpy(szReportStatus  , "true");    
    strcpy(szServerMonitor , "clear");
    
    // xml 메시지 생성
    CXmlItem xmlItem;
    int nXmlLen =xmlItem.MakeXml_LoginReq(szCallId
                            , szVersion
                            , szClientName
                            , szReportStatus
                            , szServerMonitor);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "LoginReq xml message make failed.");
        return ilRET_FAIL_MAKE_XML_LOGINREQ;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , xmlItem.GetXmlCmd()
                            , szCallId
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[2048];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , xmlItem.GetXmlCmd()
                            , szCallId
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback function
    if(m_pCALLBACK_LoginReq != NULL)
    {
        m_pCALLBACK_LoginReq(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szCallId
                            , szVersion
                            , szClientName
                            , szReportStatus
                            , szServerMonitor
                            , szXmlbuf );
    }

    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : SetVersionNumber
 DESCRIPTION : IVRServer Driver set version. 제네시스 서버와 버젼을 맞춘다.
 PARAMS      : szVersion    - 드라이버 버젼
 RETURN      : ilRET 메시지
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::SetVersionNumber(NPCSTR szVersion)
{
    strcpy(m_szVersion, (char*)szVersion);
    if(strlen(m_szVersion) <= 0)
        return ilRET_INVALID_VERSION;

    return ilRET_SUCCESS;
}



/****************************************************************************
 TITLE       : Clear
 DESCRIPTION : 멤버변수 클리어
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriver::Clear()
{
    memset(m_szVersion      , 0x00, sizeof(m_szVersion));
    memset(m_szOwnerName    , 0x00, sizeof(m_szOwnerName));
    memset(m_szOwnerId      , 0x00, sizeof(m_szOwnerId));
    memset(m_szSvcAppName   , 0x00, sizeof(m_szSvcAppName));
    memset(m_szSvcObjectName, 0x00, sizeof(m_szSvcObjectName));
    memset(m_szRemoteIp     , 0x00, sizeof(m_szRemoteIp));

    m_nRemotePort               = 0;
    m_bIsConnected              = false;
    m_bIsInit                   = false;
    m_nReqKeySeq                = 1;
    m_pKeepAlive                = NULL;

    // callback clear.
    m_pCALLBACK_PacketSent      = NULL;
    m_pCALLBACK_PacketReceived  = NULL;
    m_pCALLBACK_LoginReq        = NULL;
    m_pCALLBACK_LoginResp       = NULL;
    m_pCALLBACK_NewCall         = NULL;
    m_pCALLBACK_CallStatus      = NULL;
    m_pCALLBACK_RouteRequest    = NULL;
    m_pCALLBACK_RouteResponse   = NULL;
    m_pCALLBACK_OneStepXfer     = NULL;
    m_pCALLBACK_CallInfoReq     = NULL;
    m_pCALLBACK_CallInfoResp    = NULL;
    m_pCALLBACK_EndCallReq      = NULL;
    m_pCALLBACK_EndCallResp     = NULL;
    m_pCALLBACK_UDataSet        = NULL;
    m_pCALLBACK_UDataGet        = NULL;
    m_pCALLBACK_UDataResp       = NULL;
    m_pCALLBACK_PeekStatReq     = NULL;
    m_pCALLBACK_StatResp        = NULL;
}

/****************************************************************************
 TITLE       : CombineGLIHeader4Xml
 DESCRIPTION : XML데이터에 GLI헤더 결합.
 PARAMS      : NPSTR szSrcPacket
               int32 nSrcLength
               NPSTR szDestPacket
               int32 nDestLength
 RETURN      : 전체 byte 길이
 WRITER      : KIMCG
 DATE        : 2014.11.26
****************************************************************************/
void CIVRDriver::CombineGLIHeader4Xml(NPSTR szSrcPacket, int32 nSrcLength, NPSTR szDestPacket, int32 nDestLength)
{
    memset(szDestPacket, 0x00, sizeof(nDestLength));

    unsigned short htonsLen = (nSrcLength + GLI_HEADER_LENGTH) - 4;

    // header 
    GLI_HEADER *pGLIHeader;
    pGLIHeader                  = (GLI_HEADER *)szDestPacket;
    pGLIHeader->cNull           = 0x0;
    pGLIHeader->cMsgType        = GLI_MSGTYPE_XML_DATA;
    pGLIHeader->htons_len       = htons(htonsLen);
    pGLIHeader->cGLI_VERSION    = GLI_VERSION;
    pGLIHeader->cGLI_DEFAULT_APP= GLI_DEFAULT_APP;

    // body
    memcpy(szDestPacket + sizeof(GLI_HEADER), szSrcPacket, nSrcLength);
}




/****************************************************************************
 TITLE       : SendData
 DESCRIPTION : 패킷 데이터 전송
 PARAMS      : 보낼패킷, 보낼패킷길이
 RETURN      : 보낸 패킷 길이
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
int32 CIVRDriver::SendData(NPSTR szPacket, int32 nLength)
{
    int nLen = 0;
    int nTotalLen = 0;
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "%-15s|iSocket is not connected", "SendData");
        Disconnect();
        return -1;
    }

    if(GetSocket() < 0)
    {
        LOG_WRITE(ILOG_ERR, "%-15s|iSocket GetSocket() ", "SendData");
        Disconnect();
        return -1;
    }

    char szGLIHeader[GLI_HEADER_LENGTH];
    memset(szGLIHeader, 0x00, sizeof(szGLIHeader));
    memcpy(szGLIHeader, szPacket, GLI_HEADER_LENGTH);
    GLI_HEADER *pGLIHeader = (GLI_HEADER*)szGLIHeader;

    m_mutexLock.Lock();
    nLen = Send(szPacket, nLength);
    m_mutexLock.Unlock();
    nTotalLen = nLen;

    uint32 nLogLv = ILOG_LV1;
    if(pGLIHeader->cMsgType == GLI_MSGTYPE_KEEP_ALIVE_REQ)
        nLogLv = ILOG_LV3;

    LOG_WRITE(nLogLv, "%-15s|GLIMsgType:%x, htons_len:%06x, GLI_VERSION:%x, Data:(%06d)[%s]"
                                , "SEND"
                                , pGLIHeader->cMsgType
                                , pGLIHeader->htons_len
                                , pGLIHeader->cGLI_VERSION
                                , nLen
                                , szPacket);

    // callback..
    if(m_pCALLBACK_PacketSent != NULL)
        m_pCALLBACK_PacketSent(GetObjectName(), szPacket);

    return nTotalLen;
}



/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket(NPCSTR szPacket, int32 nLength)
{
    CXmlItem xmlItem;
    int xmlLen = xmlItem.Parse(szPacket);
    if(xmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "XML data parsing fails. xml=[%s]", szPacket);        
        return false;
    }


    bool bRet = false;

    // xml command compare.
    if(strcmp(xmlItem.GetXmlCmd(), "LoginResp") == 0)
        bRet = HandlePacket_LoginResp(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "CallStatus") == 0)
        bRet = HandlePacket_CallStatus(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "RouteResponse") == 0)
        bRet = HandlePacket_RouteResponse(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "CallInfoResp") == 0)
        bRet = HandlePacket_CallInfoResp(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "UDataResp") == 0)
        bRet = HandlePacket_UDataResp(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "StatResp") == 0)
        bRet = HandlePacket_StatResp(xmlItem);

    else if(strcmp(xmlItem.GetXmlCmd(), "FlowControl") == 0)
        bRet = HandlePacket_FlowControl(xmlItem);    

    else if(strcmp(xmlItem.GetXmlCmd(), "CallError") == 0)
        bRet = HandlePacket_CallError(xmlItem);    

    else if(strcmp(xmlItem.GetXmlCmd(), "EndCall") == 0)
        bRet = HandlePacket_EndCall(xmlItem);

    else
        LOG_WRITE(ILOG_ERR, "Not defined xml packet command. command:%s", xmlItem.GetXmlCmd());

    return bRet;
}


/****************************************************************************
 TITLE       : HandlePacket_LoginResp
 DESCRIPTION : LoginResp => LoginReq에 대한 응답 메시지 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.12.01
****************************************************************************/
bool CIVRDriver::HandlePacket_LoginResp(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szIServerVersion[50];
    char szResult[50];
    char szConfigOptions[50];
    char szStatus[50];

    memset(szCommand        , 0x00, sizeof(szCommand));
    memset(szReqKey         , 0x00, sizeof(szReqKey));
    memset(szIServerVersion , 0x00, sizeof(szIServerVersion));
    memset(szResult         , 0x00, sizeof(szResult));
    memset(szConfigOptions  , 0x00, sizeof(szConfigOptions));
    memset(szStatus         , 0x00, sizeof(szStatus));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());

    xmlItem.GetValue("//GctiMsg/LoginResp/@IServerVersion", szIServerVersion, sizeof(szIServerVersion));
    xmlItem.GetValue("//GctiMsg/LoginResp/@Result"        , szResult        , sizeof(szResult));
    xmlItem.GetValue("//GctiMsg/LoginResp/@ConfigOptions" , szConfigOptions , sizeof(szConfigOptions));
    xmlItem.GetValue("//GctiMsg/LoginResp/@Status"        , szStatus        , sizeof(szStatus));

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , xmlItem.GetXmlLength()
                            , xmlItem.GetXmlStr());

    // callback call
    if(m_pCALLBACK_LoginResp != NULL)
    {
        m_pCALLBACK_LoginResp(GetObjectName()
                            , szCommand
                            , szReqKey
                            , szIServerVersion
                            , szResult
                            , szConfigOptions
                            , szStatus
                            , xmlItem.GetXmlStr());
    }

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket_CallStatus
 DESCRIPTION : 수신패킷 - CallStatus 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_CallStatus(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szEvent[30];
    
    memset(szCommand        , 0x00, sizeof(szCommand));
    memset(szReqKey         , 0x00, sizeof(szReqKey));
    memset(szEvent          , 0x00, sizeof(szEvent));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());

    xmlItem.GetValue("//GctiMsg/CallStatus/@Event", szEvent, sizeof(szEvent));

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, Event:%s, XML_DATA=(%d)%s"
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szEvent
                            , xmlItem.GetXmlLength()
                            , xmlItem.GetXmlStr());
    
    // callback call
    ilCallStatus eCallStatus = GetCallStatusByStr(szEvent);
    if(m_pCALLBACK_CallStatus != NULL)
    {
        m_pCALLBACK_CallStatus(GetObjectName()
                            , szCommand
                            , szReqKey
                            , szEvent
                            , eCallStatus
                            , xmlItem.GetXmlStr());
    }

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket_CallStatus
 DESCRIPTION : 수신패킷 - CallStatus 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_RouteResponse(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szRouteType[30];
    char szDest[30];
    char szExtnsEx[1024];
    
    memset(szCommand   , 0x00, sizeof(szCommand));
    memset(szReqKey    , 0x00, sizeof(szReqKey));
    memset(szRouteType , 0x00, sizeof(szRouteType));
    memset(szDest      , 0x00, sizeof(szDest));
    memset(szExtnsEx   , 0x00, sizeof(szExtnsEx));
    
    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());
    
    xmlItem.GetValue("//GctiMsg/RouteResponse/@RouteType", szRouteType, sizeof(szRouteType));
    xmlItem.GetValue("//GctiMsg/RouteResponse/Dest/text()", szDest, sizeof(szDest));
    
    char szXPath[1024];
    memset(szXPath, 0x00, sizeof(szXPath));
    sprintf(szXPath, "//GctiMsg/%s/ExtnsEx", xmlItem.GetXmlCmd());
    xmlItem.ParseAttachData(szXPath, szExtnsEx, sizeof(szExtnsEx));

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());

    // callback call
    if(m_pCALLBACK_RouteResponse != NULL)
    {
        m_pCALLBACK_RouteResponse(GetObjectName()
                                , szCommand
                                , szReqKey
                                , szRouteType
                                , szDest
                                , szExtnsEx
                                , xmlItem.GetXmlStr());
    }

    return true;
}

/****************************************************************************
 TITLE       : HandlePacket_CallInfoResp
 DESCRIPTION : CallInfoResp => CallInfoReq에 대한 응답 메시지 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.12.01
****************************************************************************/
bool CIVRDriver::HandlePacket_CallInfoResp(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szANI[50];
    char szDNIS[50];
    char szCalledNum[50];
    char szConnId[50];
    char szFirstHomeLocation[50];
    char szLastEvent[50];
    char szOtherDN[50];
    char szOtherQueue[50];
    char szOtherTrunk[50];
    char szPortDN[50];
    char szPortQueue[50];
    char szPortTrunk[50];
    char szThirdPartyDN[50];
    char szTSCallId[50];
    char szUUID[50];

    memset(szCommand           , 0x00, sizeof(szCommand));
    memset(szReqKey            , 0x00, sizeof(szReqKey));
    memset(szANI               , 0x00, sizeof(szANI));
    memset(szDNIS              , 0x00, sizeof(szDNIS));
    memset(szCalledNum         , 0x00, sizeof(szCalledNum));
    memset(szConnId            , 0x00, sizeof(szConnId));
    memset(szFirstHomeLocation , 0x00, sizeof(szFirstHomeLocation));
    memset(szLastEvent         , 0x00, sizeof(szLastEvent));
    memset(szOtherDN           , 0x00, sizeof(szOtherDN));
    memset(szOtherQueue        , 0x00, sizeof(szOtherQueue));
    memset(szOtherTrunk        , 0x00, sizeof(szOtherTrunk));
    memset(szPortDN            , 0x00, sizeof(szPortDN));
    memset(szPortQueue         , 0x00, sizeof(szPortQueue));
    memset(szPortTrunk         , 0x00, sizeof(szPortTrunk));
    memset(szThirdPartyDN      , 0x00, sizeof(szThirdPartyDN));
    memset(szTSCallId          , 0x00, sizeof(szTSCallId));
    memset(szUUID              , 0x00, sizeof(szUUID));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());
    
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@ANI"              , szANI              , sizeof(szANI));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@DNIS"             , szDNIS             , sizeof(szDNIS));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@CalledNum"        , szCalledNum        , sizeof(szCalledNum));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@ConnId"           , szConnId           , sizeof(szConnId));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@FirstHomeLocation", szFirstHomeLocation, sizeof(szFirstHomeLocation));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@LastEvent"        , szLastEvent        , sizeof(szLastEvent));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@OtherDN"          , szOtherDN          , sizeof(szOtherDN));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@OtherQueue"       , szOtherQueue       , sizeof(szOtherQueue));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@OtherTrunk"       , szOtherTrunk       , sizeof(szOtherTrunk));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@PortDN"           , szPortDN           , sizeof(szPortDN));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@PortQueue"        , szPortQueue        , sizeof(szPortQueue));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@PortTrunk"        , szPortTrunk        , sizeof(szPortTrunk));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@ThirdPartyDN"     , szThirdPartyDN     , sizeof(szThirdPartyDN));
    xmlItem.GetValue("//GctiMsg/CallInfoResp/@TSCallId"         , szTSCallId         , sizeof(szTSCallId));

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());
    // callback call
    if(m_pCALLBACK_CallInfoResp != NULL)
    {
        m_pCALLBACK_CallInfoResp(GetObjectName()
                                , szCommand
                                , szReqKey
                                , szANI
                                , szDNIS
                                , szCalledNum
                                , szConnId
                                , szFirstHomeLocation
                                , szLastEvent
                                , szOtherDN
                                , szOtherQueue
                                , szOtherTrunk
                                , szPortDN
                                , szPortQueue
                                , szPortTrunk
                                , szThirdPartyDN
                                , szTSCallId
                                , xmlItem.GetXmlStr());
    }

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket_UDataResp
 DESCRIPTION : 수신패킷 - CallStatus 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_UDataResp(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szRequestId[30];
    char szResult[50];
    char szUDataEx[2048];
    
    memset(szCommand           , 0x00, sizeof(szCommand));
    memset(szReqKey            , 0x00, sizeof(szReqKey));
    memset(szRequestId         , 0x00, sizeof(szRequestId));
    memset(szResult            , 0x00, sizeof(szResult));
    memset(szUDataEx           , 0x00, sizeof(szUDataEx));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());
    
    xmlItem.GetValue("//GctiMsg/UDataResp/@Result"         , szResult   , sizeof(szResult));
    xmlItem.GetValue("//GctiMsg/UDataResp/RequestId/text()", szRequestId, sizeof(szRequestId));

    xmlItem.ParseAttachData("//GctiMsg/UDataResp/UDataEx", szUDataEx, sizeof(szUDataEx));
    
    // callback call
    if(m_pCALLBACK_UDataResp != NULL)
    {
        m_pCALLBACK_UDataResp(GetObjectName()
                                , szCommand
                                , szReqKey
                                , szRequestId
                                , szResult
                                , szUDataEx
                                , xmlItem.GetXmlStr());
    }

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket_StatResp
 DESCRIPTION : 수신패킷 - StatResp 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_StatResp(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szRequestId[30];
    char szResult[50];
    char szResultCode[50];
    
    memset(szCommand           , 0x00, sizeof(szCommand));
    memset(szReqKey            , 0x00, sizeof(szReqKey));
    memset(szRequestId         , 0x00, sizeof(szRequestId));
    memset(szResultCode        , 0x00, sizeof(szResultCode));
    memset(szResult            , 0x00, sizeof(szResult));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());
    
    xmlItem.GetValue("//GctiMsg/StatResp/@ResultCode"     , szResultCode, sizeof(szResultCode));
    xmlItem.GetValue("//GctiMsg/StatResp/RequestId/text()", szRequestId , sizeof(szRequestId));
    xmlItem.GetValue("//GctiMsg/StatResp/Result/text()"   , szResult    , sizeof(szResult));
    
    // callback call
    if(m_pCALLBACK_StatResp != NULL)
    {
        m_pCALLBACK_StatResp(GetObjectName()
                                , szCommand
                                , szReqKey
                                , szResult
                                , szResultCode
                                , xmlItem.GetXmlStr());
    }

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket_FlowControl
 DESCRIPTION : 수신패킷 - CallStatus 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_FlowControl(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szStatus[50];
    
    memset(szCommand , 0x00, sizeof(szCommand));
    memset(szReqKey  , 0x00, sizeof(szReqKey));
    memset(szStatus  , 0x00, sizeof(szStatus));

    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());
    
    xmlItem.GetValue("//GctiMsg/FlowControl/@Status"   , szStatus   , sizeof(szStatus));
    
    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());
    
    return true;
}


/****************************************************************************
 TITLE       : HandlePacket_CallError
 DESCRIPTION : 수신패킷 - CallError 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_CallError(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szFailedReq[50];
    char szTlibErrCode[50];
    
    memset(szCommand    , 0x00, sizeof(szCommand));
    memset(szReqKey     , 0x00, sizeof(szReqKey));
    memset(szFailedReq  , 0x00, sizeof(szFailedReq));
    memset(szTlibErrCode, 0x00, sizeof(szTlibErrCode));
    
    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());
    
    xmlItem.GetValue("//GctiMsg/CallError/@FailedReq"  , szFailedReq   , sizeof(szFailedReq));
    xmlItem.GetValue("//GctiMsg/CallError/@TlibErrCode", szTlibErrCode   , sizeof(szTlibErrCode));
    
    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());
    
    return true;
}


/****************************************************************************
 TITLE       : HandlePacket_EndCall
 DESCRIPTION : 수신패킷 - EndCall 처리
 PARAMS      : xml item
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CIVRDriver::HandlePacket_EndCall(CXmlItem xmlItem)
{
    char szCommand[50];
    char szReqKey[50];
    char szEndCause[50];
    
    memset(szCommand , 0x00, sizeof(szCommand));
    memset(szReqKey  , 0x00, sizeof(szReqKey));
    memset(szEndCause, 0x00, sizeof(szEndCause));
    
    strcpy(szCommand, xmlItem.GetXmlCmd());
    strcpy(szReqKey, xmlItem.GetCallId());

    xmlItem.GetValue("//GctiMsg/EndCall/@EndCause"  , szEndCause   , sizeof(szEndCause));

    LOG_WRITE(ILOG_INFO, "%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                                , xmlItem.GetXmlCmd()
                                , szReqKey
                                , xmlItem.GetXmlLength()
                                , xmlItem.GetXmlStr());    

    // call callback function
    if(m_pCALLBACK_EndCallResp != NULL)
        m_pCALLBACK_EndCallResp(GetObjectName(), xmlItem.GetXmlCmd(), szReqKey, szEndCause, xmlItem.GetXmlStr());

    return true;
}



/****************************************************************************
 TITLE       : NoteCallStart
 DESCRIPTION : Newcall
 PARAMS      : CalledNum        // ivr port number
               CallControlMode
               Version
               ANIRestriction
               DNIS
               ANI
               UDataEx
               ExtnsEx
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::NoteCallStart( NPCSTR szReqKey
                                , NPCSTR szCalledNum        // ivr port number
                                , NPCSTR szCallControlMode
                                , NPCSTR szVersion
                                , NPCSTR szDNIS
                                , NPCSTR szANI
                                , NPCSTR szUDataEx
                                , NPCSTR szExtnsEx)
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    // xml 메시지 생성
    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_NewCall(szReqKey
                            , szCalledNum
                            , szCallControlMode
                            , szVersion
                            , szDNIS
                            , szANI
                            , szUDataEx
                            , szExtnsEx);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "NewCall xml message make failed.");
        return ilRET_FAIL_MAKE_XML_NEWCALL;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szCalledNum
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);
    
    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szCalledNum
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback
    if(m_pCALLBACK_NewCall != NULL)
    {
        m_pCALLBACK_NewCall(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szCalledNum
                            , szCallControlMode
                            , szVersion
                            , szDNIS
                            , szANI
                            , szUDataEx
                            , szExtnsEx
                            , szXmlbuf);
    }
    
    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : RouteRequest
 DESCRIPTION : RouteRequest
 PARAMS      : ReqId(CallId)        
               szRouteDN
               szCED
               szUDataEx
               szExtnsEx
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::RouteRequest( NPCSTR szReqKey
                            , NPCSTR szGCTIPort                         
                            , NPCSTR szRouteDN
                            , NPCSTR szCED
                            , NPCSTR szUDataEx
                            , NPCSTR szExtnsEx)
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));


    // xml 메시지 생성
    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_RouteRequest(szReqKey
                            , szRouteDN
                            , szCED
                            , szUDataEx
                            , szExtnsEx);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "RouteRequest xml message make failed.");
        return ilRET_FAIL_MAKE_XML_ROUTEREQUEST;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);
    
    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }
        

      // call callback
    if(m_pCALLBACK_RouteRequest != NULL)
    {
        m_pCALLBACK_RouteRequest(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szRouteDN
                            , szCED
                            , szUDataEx
                            , szExtnsEx
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}



/****************************************************************************
 TITLE       : CallTransferKVList
 DESCRIPTION : One step transfer
 PARAMS      : ReqId(CallId)        
               szDestDN
               szUDataEx
               szExtnsEx
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::CallTransferKVList( NPCSTR szReqKey
                            , NPCSTR szGCTIPort
                            , NPCSTR szDestDN
                            , NPCSTR szUDataEx
                            , NPCSTR szExtnsEx )
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_OneStepXfer(szReqKey
                            , szDestDN                            
                            , szUDataEx
                            , szExtnsEx);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "OneStepXfer xml message make failed.");
        return ilRET_FAIL_MAKE_XML_ONESTEPXFER;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_MAKE_XML_NEWCALL;
    }
        

    // call callback
    if(m_pCALLBACK_OneStepXfer != NULL)
    {
        m_pCALLBACK_OneStepXfer(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szDestDN
                            , szUDataEx
                            , szExtnsEx
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}




/****************************************************************************
 TITLE       : GetCallInfo
 DESCRIPTION : One step transfer
 PARAMS      : ReqId(CallId)        
               szReportUUID  (true / false)
               szReportThirdPartyDN (true / false)
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::GetCallInfo( NPCSTR szReqKey, NPCSTR szGCTIPort)
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_CallInfoReq(szReqKey);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "CallInfoReq xml message make failed.");
        return ilRET_FAIL_MAKE_XML_CALLINFOREQ;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {        
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_MAKE_XML_CALLINFOREQ;
    }

    // call callback
    if(m_pCALLBACK_CallInfoReq != NULL)
    {
        m_pCALLBACK_CallInfoReq(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : NoteCallEnd
 DESCRIPTION : End call
 PARAMS      : ReqId(CallId)
               EndCause
                + Normal (default)
                + Abandoned
                + Resources
                + FeatureNotSupported
                + InvalidVersion
                + InvalidStateTransition
                + ServerPaused
                + Timeout

 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::NoteCallEnd( NPCSTR szReqKey, NPCSTR szGCTIPort, NPCSTR szEndCause)
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_EndCall(szReqKey, szEndCause);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "EndCall xml message make failed.");
        return ilRET_FAIL_MAKE_XML_ENDCALL;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {        
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback
    if(m_pCALLBACK_EndCallReq != NULL)
    {
        m_pCALLBACK_EndCallReq(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szEndCause
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}

/****************************************************************************
 TITLE       : UDataSetKV
 DESCRIPTION : UData add key=data|key=data...
 PARAMS      : ReqId(CallId)
               RequestId
               
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::UDataSetKV( NPCSTR szReqKey, NPCSTR szGCTIPort, NPCSTR szRequestId, NPCSTR szAction, NPCSTR szUDataEx )
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    char szNewRequestId[50];
    memset(szNewRequestId, 0x00, sizeof(szNewRequestId));

    if(strlen(szRequestId) <= 0)
        strcpy(szNewRequestId, szReqKey);
    else
        strcpy(szNewRequestId, szRequestId);


    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_UDataSet(szReqKey
                            , szNewRequestId
                            , szAction
                            , szUDataEx );
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "UDataSet xml message make failed.");
        return ilRET_FAIL_MAKE_XML_UDATASET;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {        
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback function    
    if(m_pCALLBACK_UDataSet != NULL)
    {
        m_pCALLBACK_UDataSet(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szNewRequestId
                            , szAction
                            , szUDataEx
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : UDataGetKV
 DESCRIPTION : UData add key:data
 PARAMS      : ReqId(CallId)
               RequestId
               
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::UDataGetKV( NPCSTR szReqKey, NPCSTR szGCTIPort, NPCSTR szRequestId, NPCSTR szKeys )
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    char szNewRequestId[50];
    memset(szNewRequestId, 0x00, sizeof(szNewRequestId));

    if(strlen(szRequestId) <= 0)
        strcpy(szNewRequestId, szReqKey);
    else
        strcpy(szNewRequestId, szRequestId);

    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_UDataGet(szReqKey, szNewRequestId, szKeys);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "UDataGet xml message make failed.");
        return ilRET_FAIL_MAKE_XML_UDATAGET;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());    

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback function    
    if(m_pCALLBACK_UDataGet != NULL)
    {
        m_pCALLBACK_UDataGet(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szKeys
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : PeekStatReq
 DESCRIPTION : Peek Stat Request
 PARAMS      : ReqId(CallId)
               RequestId
               
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::PeekStatReq( NPCSTR szReqKey, NPCSTR szGCTIPort, NPCSTR szRequestId, NPCSTR szStatName )
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "Connection failed.");
        return ilRET_NOT_CONNECTED_REMOTE;
    }

    char szXmlbuf[MAX_XML_SIZE];
    memset(szXmlbuf , 0x00, sizeof(szXmlbuf));

    char szNewRequestId[50];
    memset(szNewRequestId, 0x00, sizeof(szNewRequestId));

    if(strlen(szRequestId) <= 0)
        strcpy(szNewRequestId, szReqKey);
    else
        strcpy(szNewRequestId, szRequestId);

    CXmlItem xmlItem;
    int nXmlLen = xmlItem.MakeXml_PeekStatReq(szReqKey, szNewRequestId, szStatName);
    if(nXmlLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "PeekStatReq xml message make failed.");
        return ilRET_FAIL_MAKE_XML_PEEKSTATREQ;
    }

    strcpy(szXmlbuf, xmlItem.GetXmlStr());    

    LOG_WRITE(ILOG_INFO, "GCTI_PORT:%-5s|%-15s|REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader4Xml(szXmlbuf, nXmlLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    // send data..
    int nSndLen = SendData(szNewXmlBuf, nXmlLen + GLI_HEADER_LENGTH);
    if(nSndLen <= 0)
    {
        LOG_WRITE(ILOG_ERR, "GCTI_PORT:%-5s|%-15s|xml data send failed. REQ_KEY=%s, XML_DATA=(%d)%s"
                            , szGCTIPort
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , nXmlLen
                            , szXmlbuf);

        return ilRET_FAIL_SOCKET_SEND_DATA;
    }

    // call callback function    
    if(m_pCALLBACK_PeekStatReq != NULL)
    {
        m_pCALLBACK_PeekStatReq(GetObjectName()
                            , xmlItem.GetXmlCmd()
                            , szReqKey
                            , szStatName
                            , szXmlbuf);
    }

    return ilRET_SUCCESS;
}



/****************************************************************************
 TITLE       : KeepAlive
 DESCRIPTION : Keep Alive GCTI check
 PARAMS      : bIsUse
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilRET CIVRDriver::KeepAlive( uint32 nInterval)
{
    if(nInterval < 0)
        return ilRET_SUCCESS;

    m_pKeepAlive = new CKeepAlive();
    m_pKeepAlive->m_nInterval = nInterval;
    m_pKeepAlive->Start(this);

    LOG_WRITE(ILOG_INFO, "%-15s|Start Keep-Alive Check. Interval:%dsec", "KEEP-ALIVE", nInterval);
    
    return ilRET_SUCCESS;
}


/****************************************************************************
 TITLE       : ContinueKeepAlive
 DESCRIPTION : Keep Alive GCTI check
 PARAMS      : bIsUse
 RETURN      : result message
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriver::ContinueKeepAlive(bool bIsContinue)
{
    if(m_pKeepAlive == NULL)
        return;

    m_pKeepAlive->m_bContinue = bIsContinue;
}

/****************************************************************************
 TITLE       : SendKeepAliveMsg
 DESCRIPTION : Keep Alive 메시지 송신
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.12.02
****************************************************************************/
void CIVRDriver::SendKeepAliveMsg()
{
    if(!IsConnected())
    {
        LOG_WRITE(ILOG_ERR, "%-15s|Connection failed.", "KEEP-ALIVE");
        return;
    }

    char szMsg[16];
    memset(szMsg, 0x00, sizeof(szMsg));

    // header 
    GLI_HEADER *pGLIHeader;
    pGLIHeader                  = (GLI_HEADER *)szMsg;
    pGLIHeader->cNull           = 0x0;
    pGLIHeader->cMsgType        = GLI_MSGTYPE_KEEP_ALIVE_REQ;
    pGLIHeader->htons_len       = htons(GLI_HEADER_LENGTH - 4);
    pGLIHeader->cGLI_VERSION    = GLI_VERSION;
    pGLIHeader->cGLI_DEFAULT_APP= GLI_DEFAULT_APP;
    
    int nLen = SendData(szMsg, GLI_HEADER_LENGTH);
    if(nLen <= 0)
        LOG_WRITE(ILOG_ERR, "%-15s|Send Data Fail. length:%d", "KEEP-ALIVE", nLen);
}


/****************************************************************************
 TITLE       : GetCallStatusByStr
 DESCRIPTION : CallStatus 이벤트 메시지 코드 변환.
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
ilCallStatus CIVRDriver::GetCallStatusByStr(NPCSTR szEvent)
{
    string strEvent = string(szEvent);
    if(strcmp(strEvent.c_str(), "Dialing")      == 0) return eCallStatusDialing;
    if(strcmp(strEvent.c_str(), "Ringing")      == 0) return eCallStatusRinging;
    if(strcmp(strEvent.c_str(), "Established")  == 0) return eCallStatusEstablished;
    if(strcmp(strEvent.c_str(), "Retrieved")    == 0) return eCallStatusRetrieved;
    if(strcmp(strEvent.c_str(), "Busy")         == 0) return eCallStatusBusy;
    if(strcmp(strEvent.c_str(), "Held")         == 0) return eCallStatusHeld;
    if(strcmp(strEvent.c_str(), "ConfPartyAdd") == 0) return eCallStatusConfPartyAdd;
    if(strcmp(strEvent.c_str(), "ConfPartyDel") == 0) return eCallStatusConfPartyDel;
    if(strcmp(strEvent.c_str(), "XferComplete") == 0) return eCallStatusXferComplete;
    if(strcmp(strEvent.c_str(), "Released")     == 0) return eCallStatusReleased;

    return eCallStatusUnknown;
}


/****************************************************************************
 TITLE       : LOG_WRITE
 DESCRIPTION : 로그메시지에 추가정보 삽입.
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CIVRDriver::LOG_WRITE(uint32 nLogLvl, NPCSTR szFmt, ...)
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
    sprintf(szLog, "%s|%s:%d|%s", GetAppName(), GetRemoteIp(), GetRemotePort(), buf);

    m_log.Log(nLogLvl, szLog);
}

