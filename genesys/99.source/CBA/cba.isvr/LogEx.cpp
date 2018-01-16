/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : iLog 확장 클래스
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#include "LogEx.h"
#include "IVRDriverIF.h"
#include "IVRDriver.h"

CLogEx::CLogEx(void) {}
CLogEx::~CLogEx(void) {}


/****************************************************************************
 TITLE       : WRITE
 DESCRIPTION : LogWrite
 PARAMS      : szEvent
 RETURN      : ilCallStatus
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CLogEx::WRITE(uint32 nLogLvl, NPCSTR szLog)
{
    m_log.Log(nLogLvl, szLog);
}

// implementation
/****************************************************************************
 TITLE       : Init
 DESCRIPTION : LogEx 클래스 초기화
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
bool CLogEx::Init(NPCSTR szPrefix, NPCSTR szLogPath, NPCSTR szLogLinkPath, uint32 nLogLevel)
{
    m_log.Init(szPrefix, szLogPath, nLogLevel);
    m_log.MakeLink(szPrefix, szLogLinkPath);

    return true;
}


/****************************************************************************
 TITLE       : INF
 DESCRIPTION : Information log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CLogEx::INF(NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    m_log.Log(ILOG_INFO, buf);
}

/****************************************************************************
 TITLE       : WRN
 DESCRIPTION : Warring log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CLogEx::WRN(NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    m_log.Log(ILOG_WARN, buf);

}

/****************************************************************************
 TITLE       : ERR
 DESCRIPTION : Error log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
// void CLogEx::ERR(NPCSTR szFmt, ...)
// {
//     va_list pArg;
//     va_start(pArg, szFmt);
//     char buf[1024];
//     int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
//     va_end(pArg);

//     if (len <= 0)
//         return;

//     m_log.Log(ILOG_ERR, buf);
// }

/****************************************************************************
 TITLE       : LV1
 DESCRIPTION : Level1 log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CLogEx::LV1(NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    m_log.Log(ILOG_LV1, buf);

}

/****************************************************************************
 TITLE       : LV2
 DESCRIPTION : Level2 log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CLogEx::LV2(NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    m_log.Log(ILOG_LV2, buf);
}

/****************************************************************************
 TITLE       : LV3
 DESCRIPTION : Level3 log
 PARAMS      : strMsg : log message
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CLogEx::LV3(NPCSTR szFmt, ...)
{
    va_list pArg;
    va_start(pArg, szFmt);
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), szFmt, pArg);
    va_end(pArg);

    if (len <= 0)
        return;

    m_log.Log(ILOG_LV3, buf);
}


// static members
iLog     CLogEx::m_log;



