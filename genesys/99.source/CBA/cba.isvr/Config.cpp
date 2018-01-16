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
    // process infos
    memset(m_szProcName        , 0x00, sizeof(m_szProcName));
    memset(m_szProcId          , 0x00, sizeof(m_szProcId));
    memset(m_szCfgFileFullName , 0x00, sizeof(m_szCfgFileFullName));

    // [SYSTEM]
    memset(m_szSystemType      , 0x00, sizeof(m_szSystemType));
    memset(m_szSystemId        , 0x00, sizeof(m_szSystemId));
    
    // [CBA_INFO]
    memset(m_szCBListenIp      , 0x00, sizeof(m_szCBListenIp));
    m_nCBListenPort            = 0;
    m_nLogLevel                = 1;
    memset(m_szLogPrefix       , 0x00, sizeof(m_szLogPrefix));
    memset(m_szLogPath         , 0x00, sizeof(m_szLogPath));
    memset(m_szLogLinkPath     , 0x00, sizeof(m_szLogLinkPath));

    // [IVR_DRIVER]
    memset(m_szDrvRemoteIp     , 0x00, sizeof(m_szDrvRemoteIp));
    m_nDrvRemotePort           = 0;
    memset(m_szDrvSvcObjectName, 0x00, sizeof(m_szDrvSvcObjectName));
    memset(m_szDrvSvcAppName   , 0x00, sizeof(m_szDrvSvcAppName));
    m_nKeepAliveInterval       = 0;

    memset(m_szDrvTESystemId   , 0x00, sizeof(m_szDrvTESystemId));
    m_nDrvStartChannel         = 0;
    m_nDrvStartPort            = 0;
    m_nDrvDNISCount            = 0;
    m_nDrvLogLevel           = 0;

    memset(m_szDrvLogPath      , 0x00, sizeof(m_szDrvLogPath));
    memset(m_szDrvLogLinkPath  , 0x00, sizeof(m_szDrvLogLinkPath));
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
bool CConfig::Init(NPCSTR szProcName, NPCSTR szPid, NPCSTR szCfgFileFullName)
{
    strcpy(m_szProcName, szProcName);
    strcpy(m_szProcId, szPid);
    strcpy(m_szCfgFileFullName, szCfgFileFullName);
    
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
bool CConfig::ReadConfig()
{
    iIniFile stIni;
    if(!stIni.OpenIni(m_szCfgFileFullName))
        return false;

    // cba infos
    strcpy(m_szSystemType       , stIni.ReadStr("SYSTEM", "SYSTEM_TYPE"));
    strcpy(m_szSystemId         , stIni.ReadStr("SYSTEM", "SYSTEM_ID"));

    strcpy(m_szCBListenIp       , stIni.ReadStr("CBA_INFO", "CB_LISTEN_IP"));
    m_nCBListenPort             = stIni.ReadInt("CBA_INFO", "CB_LISTEN_PORT", 0);
    m_nLogLevel                 = stIni.ReadInt("CBA_INFO", "LOG_LEVEL", 2);

    char szTempLogPrefix[100];
    memset(szTempLogPrefix      , 0x00, sizeof(szTempLogPrefix));
    strcpy(szTempLogPrefix      , stIni.ReadStr("CBA_INFO", "LOG_PREFIX"));
    sprintf(m_szLogPrefix, "%s.%s", szTempLogPrefix, m_szProcId);

    strcpy(m_szLogPath          , stIni.ReadStr("CBA_INFO", "LOG_PATH"));
    strcpy(m_szLogLinkPath      , stIni.ReadStr("CBA_INFO", "LOG_LINK_PATH"));

    // driver info
    strcpy(m_szDrvRemoteIp      , stIni.ReadStr("IVR_DRIVER", "REMOTE_IP"));
    m_nDrvRemotePort            = stIni.ReadInt("IVR_DRIVER", "REMOTE_PORT", 0);
    strcpy(m_szDrvSvcObjectName , stIni.ReadStr("IVR_DRIVER", "SVC_OBJECT_NAME"));
    strcpy(m_szDrvSvcAppName    , stIni.ReadStr("IVR_DRIVER", "SVC_APP_NAME"));
    m_nKeepAliveInterval        = stIni.ReadInt("IVR_DRIVER", "KEEP_ALIVE_INTERVAL", 0);

    strcpy(m_szDrvTESystemId    , stIni.ReadStr("IVR_DRIVER", "TE_SYSTEM_ID"));
    m_nDrvStartChannel          = stIni.ReadInt("IVR_DRIVER", "START_CHANNEL", 0);
    m_nDrvStartPort             = stIni.ReadInt("IVR_DRIVER", "START_PORT", 0);
    m_nDrvDNISCount             = stIni.ReadInt("IVR_DRIVER", "DNIS_COUNT", 0);

    m_nDrvLogLevel              = stIni.ReadInt("IVR_DRIVER", "LOG_LEVEL", 0);
    strcpy(m_szDrvLogPath       , stIni.ReadStr("IVR_DRIVER", "LOG_PATH"));
    strcpy(m_szDrvLogLinkPath   , stIni.ReadStr("IVR_DRIVER", "LOG_LINK_PATH"));
    
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
    g_iLog.Log(ILOG_INFO, "[PROCESS]");
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "PROCESS_NAME"        , m_szProcName);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "PROCESS_ID"          , m_szProcId);
    g_iLog.Log(ILOG_INFO, "\r\n");

    g_iLog.Log(ILOG_INFO, "[SYSTEM]");
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "SYSTEM_TYPE"         , m_szSystemType);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "SYSTEM_ID"           , m_szSystemId);
    g_iLog.Log(ILOG_INFO, "\r\n");

    g_iLog.Log(ILOG_INFO, "[CBA_INFO]");
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "CB_LISTEN_IP"        , m_szCBListenIp);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "CB_LISTEN_PORT"      , m_nCBListenPort);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "LOG_LEVEL"           , m_nLogLevel);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "LOG_PREFIX"          , m_szLogPrefix);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "LOG_PATH"            , m_szLogPath);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "LOG_LINK_PATH"       , m_szLogLinkPath);
    g_iLog.Log(ILOG_INFO, "\r\n");

    // driver info    
    g_iLog.Log(ILOG_INFO, "[IVR_DRIVER]");
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "REMOTE_IP"           , m_szDrvRemoteIp);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "REMOTE_PORT"         , m_nDrvRemotePort);    
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "SVC_OBJECT_NAME"     , m_szDrvSvcObjectName);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "SVC_APP_NAME"        , m_szDrvSvcAppName);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "KEEP_ALIVE_INTERVAL" , m_nKeepAliveInterval);

    g_iLog.Log(ILOG_INFO, "%-20s : %s", "TE_SYSTEM_ID"        , m_szDrvTESystemId);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "START_CHANNEL"       , m_nDrvStartChannel);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "START_PORT"          , m_nDrvStartPort);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "DNIS_COUNT"          , m_nDrvDNISCount);
    g_iLog.Log(ILOG_INFO, "%-20s : %d", "LOG_LEVEL"           , m_nDrvLogLevel);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "LOG_PATH"            , m_szDrvLogPath);
    g_iLog.Log(ILOG_INFO, "%-20s : %s", "LOG_LINK_PATH"       , m_szDrvLogLinkPath);    
    g_iLog.Log(ILOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

