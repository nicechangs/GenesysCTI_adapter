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


// include
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iCore/iLib.h>

#include "CBADefines.h"
#include "CBAGlobal.h"
#include "TcpSessionBase.h"
#include "CBSession.h"


// constructor && desctructor
CCBSession::CCBSession(void) {}
CCBSession::~CCBSession(void) {}



/****************************************************************************
 TITLE       : ThreadRun - override
 DESCRIPTION : main loop
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
NPVOID CCBSession::ThreadRun(NPVOID pArg)
{
    char szDataBuf[SOCKET_BUFFER_SIZE];
    char szTemp[256];
    int iSelectRes              = 0;
    int nLen                    = 0;
    int nTotalLen               = 0;
    int nPacketLen              = 0;
    
    char szTotalPacket[SOCKET_BUFFER_SIZE];
    char szPartialPacket[SOCKET_BUFFER_SIZE];
    char szPacket[SOCKET_BUFFER_SIZE];
    
    // session loop
    while(IsOn())
    {
        if(m_socket.GetSocket() < 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket GetSocket() error.");
            Disconnect();
            break;
        }

        iSelectRes = m_socket.RSelect(5);
        if(iSelectRes < 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket RSelect error.");
            Disconnect();
            break;
        }

        if(iSelectRes == 0) 
            continue;
    
        memset(szDataBuf, 0x00, sizeof(szDataBuf));
        nLen = m_socket.Recv(szDataBuf, sizeof(szDataBuf));
        if(nLen <= 0)
        {
            LOG_WRITE(ILOG_ERR, "iSocket Recv error.");
            Disconnect();
            break;
        }

        memset(szPartialPacket, 0x00, sizeof(szPartialPacket));        
        memcpy(szPartialPacket, szDataBuf, nLen);
        
        memcpy(szTotalPacket + nTotalLen, szPartialPacket, nLen);
        nTotalLen += nLen;

        pktXCCPHeader *pHeader = NULL;
        pHeader = (pktXCCPHeader*)szTotalPacket;
        if( pHeader->header.stx != PACKET_STX )
        {
            LOG_WRITE(ILOG_ERR, "Not found STX. data=%s", szPartialPacket);
            memset(szTotalPacket  , 0x00, sizeof(szTotalPacket));
            memset(szPartialPacket, 0x00, sizeof(szPartialPacket));
            memset(szPacket       , 0x00, sizeof(szPacket));
            nTotalLen = 0;
            continue;
        }

        memset(szTemp, 0x00, sizeof(szTemp));
        memcpy(szTemp, pHeader->header.sLen, sizeof(pHeader->header.sLen));
        nPacketLen = atoi(szTemp);
        if(nPacketLen < 0)
        {
            LOG_WRITE(ILOG_ERR, "Invalid packet length. data=%s", szPartialPacket);
            memset(szTotalPacket  , 0x00, sizeof(szTotalPacket));
            memset(szPartialPacket, 0x00, sizeof(szPartialPacket));
            memset(szPacket       , 0x00, sizeof(szPacket));
            continue;
        }

        // 전문길이 체크 : 수신계속
        if(nTotalLen < nPacketLen)
            continue;

        // 전문 길이만큼 처리.
        memset(szPacket, 0x00, sizeof(szPacket));
        memcpy(szPacket, szTotalPacket, nTotalLen);
        
        // make queue item..
        QUEUE_ITEM qItem;
        memset(&qItem, 0x00, sizeof(qItem));

        strcpy(qItem.szCBSessionName, GetName());
        strcpy(qItem.szCBSessionIp, GetIp());
        qItem.nCBSessionPort = GetPort();
        qItem.nRequestPacketLen = nTotalLen;
        memcpy(qItem.szRequestPacket, szTotalPacket, nTotalLen);        
        qItem.stXCCPHeader = *pHeader;

        // 요청건 처리..        
        uint32 nLogLv = ILOG_INFO;
        if(qItem.stXCCPHeader.header.cMsgFlag == 'H')
            nLogLv = ILOG_DBG;
        
        LOG_WRITE(nLogLv, "%s >>> %s|Data=(%d)%s", GetName(), PROC_NAME, nTotalLen, szTotalPacket);
        HandlePacket(qItem);

        // clear local value        
        memset(szTotalPacket  , 0x00, sizeof(szTotalPacket));
        memset(szPartialPacket, 0x00, sizeof(szPartialPacket));
        memset(szPacket       , 0x00, sizeof(szPacket));
        
        nLen        = 0;
        nTotalLen   = 0;
        nPacketLen  = 0;
    }

    LOG_WRITE(ILOG_ERR, "CBSession loop exited.");

    return 0;
}

/****************************************************************************
 TITLE       : HandlePacket 
 DESCRIPTION : 요청 패킷 처리
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/
bool CCBSession::HandlePacket(QUEUE_ITEM stQItem)
{
    char szCommand[41];
    memset(szCommand, 0x00, sizeof(szCommand));
 
    if(stQItem.stXCCPHeader.header.cMsgFlag == 'H')
        return HandleHeartBeat(stQItem);

    memcpy(szCommand, stQItem.stXCCPHeader.sCommandType, sizeof(stQItem.stXCCPHeader.sCommandType));

    if( strcmp(szCommand, CB_CMDTYPE_REGISTER_REQ) == 0 )
        return  HandleRegister(stQItem);

    if( strcmp(szCommand, CB_CMDTYPE_DATA_REQ) == 0 )
        return  HandleReqData(stQItem);

    LOG_WRITE(ILOG_WARN, "Not Defined Packet Command. command:%s, packet:%s", szCommand, stQItem.szRequestPacket);

    return false;
}



/****************************************************************************
 TITLE       : HandleHeartBeat 
 DESCRIPTION : Heartbeat processing
 PARAMS      : 1. packet 
               2. packet size
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/
bool CCBSession::HandleHeartBeat(QUEUE_ITEM stQItem)
{
    pktXCCPHeader*  pHeader;
    int             nSentSize = 0;
    char            szTemp[10];
    char            szPacket[2048];
    memset(szPacket, 0x00, sizeof(szPacket));
    
    pHeader = (pktXCCPHeader*)stQItem.szRequestPacket;
    memcpy(szPacket, stQItem.szRequestPacket, stQItem.nRequestPacketLen);

    memset(szTemp, 0x00, sizeof(szTemp));
    memcpy(szTemp, stQItem.stXCCPHeader.header.sFrom, sizeof(stQItem.stXCCPHeader.header.sFrom));

    memcpy(pHeader->header.sFrom, stQItem.stXCCPHeader.header.sTo, sizeof(stQItem.stXCCPHeader.header.sFrom));
    memcpy(pHeader->header.sTo, szTemp, sizeof(stQItem.stXCCPHeader.header.sFrom));

    //int nSize = atoi(pHeader->header.sLen);
    nSentSize = Send(szPacket, stQItem.nRequestPacketLen);
    if(nSentSize < 0)
    {
        LOG_WRITE(ILOG_WARN, "%-12s|HandleHeartBeat fail. Data=%s", "ALIVE", szPacket);
        return false;
    }

    LOG_WRITE(ILOG_DBG, "%s <<< %s|Data=(%d)%s"
                            , GetName()
                            , PROC_NAME
                            , nSentSize
                            , szPacket);

    return true;
}


/****************************************************************************
 TITLE       : HandleRegister 
 DESCRIPTION : register processing
 PARAMS      : 1. packet 
               2. packet size
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/
bool CCBSession::HandleRegister(QUEUE_ITEM stQItem)
{
    pktXCCPHeader*  pHeader;
    int             nSentSize = 0;
    char            szTemp[10];
    char            szPacket[SOCKET_BUFFER_SIZE];
    memset(szPacket, 0x00, sizeof(szPacket));

    pHeader = (pktXCCPHeader*)szPacket;
    memcpy(szPacket, stQItem.szRequestPacket, stQItem.nRequestPacketLen);
    
    memset(szTemp, 0x00, sizeof(szTemp));
    memcpy(szTemp, pHeader->header.sFrom, sizeof(pHeader->header.sFrom));

    memcpy(pHeader->header.sFrom, pHeader->header.sTo, sizeof(pHeader->header.sFrom));
    memcpy(pHeader->header.sTo, szTemp, sizeof(pHeader->header.sFrom));

    memcpy(pHeader->sCommandType, CB_CMDTYPE_REGISTER_SUCCESS, sizeof(pHeader->sCommandType));

    nSentSize = Send(szPacket, stQItem.nRequestPacketLen);
    if(nSentSize < 0)
    {
        LOG_WRITE(ILOG_ERR, "Register Fail. Data=%s", szPacket);
        return false;
    }

    LOG_WRITE(ILOG_INFO, "Registe Success.");

    return true;
}


/****************************************************************************
 TITLE       : HandleReqData 
 DESCRIPTION : Handle command / request Q에 삽입
 PARAMS      : 1. packet 
               2. packet size
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.06.26
****************************************************************************/
bool CCBSession::HandleReqData(QUEUE_ITEM stQItem)
{
    // set body packet
    memcpy(stQItem.szRequestPacketBody, (stQItem.szRequestPacket + sizeof(pktXCCPHeader)), stQItem.nRequestPacketLen - sizeof(pktXCCPHeader));

    int nCnt = CQueueOfRequest::Inst()->Enqueue(stQItem);
    if(nCnt < 0)
    {
        LOG_WRITE(ILOG_ERR, "QueueOfRequest Push Fail. Data=%s", stQItem.szRequestPacket);
        return false;
    }

    return true;
}


/****************************************************************************
 TITLE       : StartUp - override
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::StartUp(NPVOID pArg)
{
    if(pArg == NULL) Start();
    else Start(pArg);

    return true;
}


/****************************************************************************
 TITLE       : StopAll - override
 DESCRIPTION : Stop thread
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::StopAll()
{
    return Stop(true);
}


/****************************************************************************
 TITLE       : LOG_WRITE
 DESCRIPTION : 로그메시지에 추가정보 삽입.
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CCBSession::LOG_WRITE(uint32 nLogLevel, NPCSTR szFmt, ...)
{
    va_list pArg;
    char buf[1024];
    va_start(pArg, szFmt);
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    char szLog[2048];
    memset(szLog, 0x00, sizeof(szLog));
    sprintf(szLog, "%-12s|%s:%d|%s", GetName(), GetIp(), GetPort(), buf);
    g_iLog.Log(nLogLevel, szLog);
}



