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

#include "CBInterface.h"
#include "TcpSessionBase.h"
#include "CBSession.h"
#include "LogEx.h"

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
    char szDataBuf[SOCK_BUFFER_SIZE];
    int iSelectRes              = 0;
    int nLen                    = 0;
    int nPacketLen              = 0;
    string strPacket            = "";
    
    // session loop
    while(IsOn())
    {
        iSleep(1);

        if(m_socket.GetSocket() < 0)
        {
            CLogEx::ERR("%s|iSocket GetSocket() error!!", GetName());
            Disconnect();
            break;
        }

        iSelectRes = m_socket.RSelect(10);
        if(iSelectRes < 0)
        {
            CLogEx::ERR("%s|iSocket RSelect error !!", GetName());
            Disconnect();
            break;
        }

        if(iSelectRes == 0) 
            continue;
    
        memset(szDataBuf, 0x00, sizeof(szDataBuf));
        nLen = m_socket.Recv(szDataBuf, sizeof(szDataBuf));
        if(nLen <= 0)
        {
            CLogEx::ERR("%s|iSocket Recv error !!", GetName());
            Disconnect();
            break;
        }

        //strPacket = string(szDataBuf);
        CLogEx::LV3("%s|RECEIVE|%06d|%s",GetName(), nLen, szDataBuf);



        HandlePacket(szDataBuf, nLen);

        nLen        = 0;
        nPacketLen  = 0;
        strPacket   = "";

    }

    CLogEx::ERR("%s|TcpSession loop exited !!", GetName());
    
    return 0;
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
	CLogEx::INF("%s|Start !!", GetName());
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
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket(NPCSTR szPacket, int32 nLength)
{    
    char szGLIHeader[7];
    memset(szGLIHeader, 0x00, sizeof(szGLIHeader));
    memcpy(szGLIHeader, szPacket, GLI_HEADER_LENGTH);

    GLI_HEADER *pGLIHeader = (GLI_HEADER*)szGLIHeader;

    CLogEx::LV3("length : %d", ntohs(pGLIHeader->htons_len));

    // HeartBit
    if(pGLIHeader->cMsgType == GLI_MSGTYPE_KEEP_ALIVE_REQ)
    {
        pGLIHeader->cMsgType = GLI_MSGTYPE_KEEP_ALIVE_ACK;
        Send(szGLIHeader, GLI_HEADER_LENGTH);
        return true;
    }


    char szXml[2048];
    memset(szXml, 0x00, sizeof(szXml));
    memcpy(szXml, szPacket+GLI_HEADER_LENGTH, nLength - GLI_HEADER_LENGTH );

    CXmlItem xmlItem;
    int xmlLen = xmlItem.Parse(szXml);
    if(xmlLen <= 0)
    {
        return false;
    }

    // xml command compare.
    bool bRet = false;
    if(strcmp(xmlItem.GetXmlCmd(), "LoginReq") == 0)
    {
        bRet = HandlePacket_LoginResp(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "NewCall") == 0)
    {
        bRet = HandlePacket_NewCall(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "RouteRequest") == 0)
    {
        bRet = HandlePacket_RouteRequest(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "OneStepXfer") == 0)
    {
        bRet = HandlePacket_OneStepXfer(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "CallInfoReq") == 0)
    {
        bRet = HandlePacket_CallInfoResp(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "UDataSet") == 0)
    {
        bRet = HandlePacket_UDataResp(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "UDataGet") == 0)
    {
        bRet = HandlePacket_UDataResp(xmlItem);
    }
    else if(strcmp(xmlItem.GetXmlCmd(), "PeekStatReq") == 0)
    {
        bRet = HandlePacket_StatResp(xmlItem);
    }        
    else if(strcmp(xmlItem.GetXmlCmd(), "EndCall") == 0)
    {
        bRet = HandlePacket_EndCall(xmlItem);
    }
    else
    {
        bRet = false;
    }

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_LoginResp(CXmlItem xmlItem)
{
    char szCallId[30];
    char szIServerVersion[30];
    char szResult[30];
    char szConfigOptions[50];
    char szStatus[30];

    memset(szCallId         , 0x00, sizeof(szCallId));
    memset(szIServerVersion , 0x00, sizeof(szIServerVersion));
    memset(szResult         , 0x00, sizeof(szResult));
    memset(szConfigOptions  , 0x00, sizeof(szConfigOptions));
    memset(szStatus         , 0x00, sizeof(szStatus));

    strcpy(szCallId         , xmlItem.GetCallId());
    strcpy(szIServerVersion , "8.5");
    strcpy(szResult         , "Success");
    strcpy(szConfigOptions  , "");
    strcpy(szStatus         , "OK");

    char szXmlbuf[4069];
    memset(szXmlbuf, 0x00, sizeof(szXmlbuf));
    
    // xml 메시지 생성    
    CXmlItem xItem;
    int nLen =xItem.MakeXml_LoginResp(szCallId
                            , szIServerVersion
                            , szResult
                            , szConfigOptions
                            , szStatus);
    
    strcpy(szXmlbuf, xItem.GetXmlStr());

    // GLI header 결합
    char szNewXmlBuf[2048];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);


    CXmlItem sndXmlItem2;
    
    int nLen2 = sndXmlItem2.MakeXml_FlowControl(szCallId, "off");
    if(nLen2 <= 0)
        return false;

    char szXmlbuf2[4069];
    memset(szXmlbuf2, 0x00, sizeof(szXmlbuf2));
    strcpy(szXmlbuf2, sndXmlItem2.GetXmlStr());

    char szNewXmlBuf2[2048];
    CombineGLIHeader(szXmlbuf2, nLen2, szNewXmlBuf2, sizeof(szNewXmlBuf2));
    Send(szNewXmlBuf2, nLen2 + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_NewCall(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = 0;

      // xml 메시지 생성    
    CXmlItem xItem;
    nLen = xItem.MakeXml_CallStatus(xmlItem.GetCallId(), "Ringing" );    
    strcpy(szXmlbuf, xItem.GetXmlStr());

    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));
    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);






    iSleep(100);

    CXmlItem xItem2;
    nLen = xItem2.MakeXml_CallStatus(xmlItem.GetCallId(), "Established" );
    
    memset(szXmlbuf, 0x00, sizeof(szXmlbuf));
    strcpy(szXmlbuf, xItem2.GetXmlStr());

    // GLI header 결합
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));
    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_RouteRequest(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = sndXmlItem.MakeXml_RouteResponse(xmlItem.GetCallId()
                                                , "Normal"
                                                , "5550700"
                                                , "eee=555|fff=6666|ggg=77777|hhh=88888888");

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

    
    // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_OneStepXfer(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = sndXmlItem.MakeXml_CallStatus(xmlItem.GetCallId(), "XferComplete");
    if(nLen <= 0)
        return false;

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

     // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}



/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_CallInfoResp(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = sndXmlItem.MakeXml_CallInfoResp(xmlItem.GetCallId()
                                                , "1111111111"
                                                , "2222222222"
                                                , "3333333333"
                                                , "4444444444"
                                                , "5555555555"
                                                , "6666666666"
                                                , "7777777777"
                                                , "8888888888"
                                                , "9999999999"
                                                , "aaaaaaaaaa"
                                                , "bbbbbbbbbb"
                                                , "cccccccccc"
                                                , "dddddddddd"
                                                , "eeeeeeeeee"
                                                );
    if(nLen <= 0)
        return false;

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

     // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}




/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_UDataResp(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = sndXmlItem.MakeXml_UDataResp(xmlItem.GetCallId()
                                                , "UEI"
                                                , "Success"
                                                , "eee=555|fff=6666|ggg=77777|hhh=88888888"
                                                );
    if(nLen <= 0)
        return false;

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

     // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket_StatResp
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_StatResp(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[MAX_XML_SIZE];
    int nLen = sndXmlItem.MakeXml_StatResp(xmlItem.GetCallId()
                                                , xmlItem.GetCallId()
                                                , "Success"
                                                , "100"
                                                );
    if(nLen <= 0)
        return false;

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

     // GLI header 결합
    char szNewXmlBuf[MAX_XML_SIZE];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}


/****************************************************************************
 TITLE       : HandlePacket
 DESCRIPTION : 수신패킷 처리
 PARAMS      : 수신패킷, 수신패킷길이
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CCBSession::HandlePacket_EndCall(CXmlItem xmlItem)
{
    CXmlItem sndXmlItem;
    char szXmlbuf[4096];
    int nLen = sndXmlItem.MakeXml_EndCall(xmlItem.GetCallId(), "Normal");
    if(nLen <= 0)
        return false;

    strcpy(szXmlbuf, sndXmlItem.GetXmlStr());

     // GLI header 결합
    char szNewXmlBuf[2048];
    CombineGLIHeader(szXmlbuf, nLen, szNewXmlBuf, sizeof(szNewXmlBuf));

    Send(szNewXmlBuf, nLen + GLI_HEADER_LENGTH);

    CLogEx::LV3("%s|SEND   |%06d|%s",GetName(), nLen, szXmlbuf);

    return true;
}


/****************************************************************************
 TITLE       : CombineGLIHeader
 DESCRIPTION : XML데이터에 GLI헤더 결합.
 PARAMS      : NPSTR szSrcPacket
               int32 nSrcLength
               NPSTR szDestPacket
               int32 nDestLength
 RETURN      : 전체 byte 길이
 WRITER      : KIMCG
 DATE        : 2014.11.26
****************************************************************************/
void CCBSession::CombineGLIHeader(NPSTR szSrcPacket, int32 nSrcLength, NPSTR szDestPacket, int32 nDestLength)
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

    CLogEx::LV3("pktLen:%d, htons_len:%d", htonsLen, pGLIHeader->htons_len);

    // body
    memcpy(szDestPacket + sizeof(GLI_HEADER), szSrcPacket, nSrcLength);
}
