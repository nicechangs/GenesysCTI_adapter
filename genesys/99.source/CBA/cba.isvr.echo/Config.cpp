/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : 프로세스 환경설정 클래스
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#include <string.h>
#include <math.h>
#include <iCore/iLib.h>
#include <iCore/iIniFile.h>
#include "Config.h"
#include "LogEx.h"
#include "TcpSessionManagerBase.h"
#include "TcpSessionBase.h"


// constructor && desctructor
CConfig::CConfig(void)
{
    Clear();
}
CConfig::~CConfig(void)
{
    Clear();
}

// singleton
CConfig* CConfig::m_pInstance = NULL;
CConfig* CConfig::Inst()
{
    if(m_pInstance == NULL)
        m_pInstance = new CConfig();
    return m_pInstance;
}

void CConfig::FreeInst()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


/****************************************************************************
 TITLE       : Clear
 DESCRIPTION : Config 클래스 멤버 초기화
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
 void CConfig::Clear()
 {
    memset(m_szAppName          , 0x00, sizeof(m_szAppName));
    memset(m_szProcessId        , 0x00, sizeof(m_szProcessId));

    
    // [CBA_INFO]
    memset(m_szListenIp       , 0x00, sizeof(m_szListenIp));
    m_nListenPort     = 0;


    // [LOG]    
    m_nLogLevel         = 1;
    memset(m_szLogPrefix        , 0x00, sizeof(m_szLogPrefix));
    memset(m_szLogPath          , 0x00, sizeof(m_szLogPath));
    memset(m_szLogLinkPath      , 0x00, sizeof(m_szLogLinkPath));
 }



/****************************************************************************
 TITLE       : Init
 DESCRIPTION : Config 클래스 초기화
 PARAMS      : strProcName : 프로세스명         
               nPid        : 프로세스 아이디
               strCfgPath  : cfg 파일명(경로포함)

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
bool CConfig::Init(NPCSTR szAppName, NPCSTR szPid)
{
    strcpy(m_szAppName, szAppName);
    strcpy(m_szProcessId, szPid);
    
    return true;
}


/****************************************************************************
 TITLE       : ReadConfig
 DESCRIPTION : 환경설정 로드 (cfg 파일)
 PARAMS      : szCFGFile : cfg 파일
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
bool CConfig::ReadConfig(NPCSTR szCFGFile)
{
    iIniFile stIni;
    if(!stIni.OpenIni(szCFGFile))
        return false;

    strcpy(m_szListenIp       , stIni.ReadStr("NETWORK", "LISTEN_IP"));
    m_nListenPort             = stIni.ReadInt("NETWORK", "LISTEN_PORT", 0);

    m_nLogLevel                 = stIni.ReadInt("LOG", "LOG_LEVEL", 2);
    strcpy(m_szLogPrefix        , stIni.ReadStr("LOG", "LOG_PREFIX"));
    strcpy(m_szLogPath          , stIni.ReadStr("LOG", "LOG_PATH"));
    strcpy(m_szLogLinkPath      , stIni.ReadStr("LOG", "LOG_LINK_PATH"));

    return true;
}


/****************************************************************************
 TITLE       : Display
 DESCRIPTION : 환경설정 정보 Display
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.16
****************************************************************************/
void CConfig::Display()
{
    CLogEx::INF("[NETWORK]");
    CLogEx::INF("%-20s : %s", "LISTEN_IP"    , m_szListenIp);
    CLogEx::INF("%-20s : %d", "LISTEN_PORT"  , m_nListenPort);
    

    CLogEx::INF("[LOG]");
    CLogEx::INF("%-20s : %d", "LOG_LEVEL"       , m_nLogLevel);
    CLogEx::INF("%-20s : %s", "LOG_PREFIX"      , m_szLogPrefix);
    CLogEx::INF("%-20s : %s", "LOG_PATH"        , m_szLogPath);
    CLogEx::INF("%-20s : %s", "LOG_LINK_PATH"   , m_szLogLinkPath);
}


