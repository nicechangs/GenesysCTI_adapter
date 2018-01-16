/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionBase
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

#include "TcpSessionBase.h"

// constructor && desctructor
CTcpSessionBase::CTcpSessionBase(void) {}
CTcpSessionBase::~CTcpSessionBase(void) {}


/****************************************************************************
 TITLE       : Clear
 DESCRIPTION : member clear..
 PARAMS      : 
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
void CTcpSessionBase::Clear(void)
{
    memset(m_szName, 0x00, sizeof(m_szName));
    memset(m_szIp  , 0x00, sizeof(m_szIp));

    m_nPort         = -1;
    m_socketFd      = -1;
    m_bIsConnected  = false;
}


/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionBase::StartUp(NPVOID pArg)
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
bool CTcpSessionBase::StopAll()
{
    return Stop(true);
}


/****************************************************************************
 TITLE       : Init
 DESCRIPTION : Init session
 PARAMS      : string strName : 세션이름
               string strIp   : 아이피
               int32 nPort    : 포트
               iFd nSocketFd  : 소켓FD

 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionBase::Init(NPSTR szName, NPSTR szIp, int32 nPort, iFd nSocketFd)
{
    strcpy(m_szName, szName);
    strcpy(m_szIp, szIp);

    m_nPort = nPort;
    m_socketFd = nSocketFd;
    if(!m_socket.Attach(nSocketFd))
        return false;

    m_socket.SetNonBlk(true);
    m_bIsConnected = true;

    return true;
}


/****************************************************************************
 TITLE       : Disconnect
 DESCRIPTION : Disconnect session.
 PARAMS      : 
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CTcpSessionBase::Disconnect()
{
    if(m_socket.GetSocket() > 0)
        m_socket.Close();

    m_bIsConnected = false;

    return true;
}


/****************************************************************************
 TITLE       : Send
 DESCRIPTION : Send data
 PARAMS      : szPacket : data
               nSize : data length
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
int32 CTcpSessionBase::Send(NPSTR szPacket, int32 nLength)
{
    int nLen = 0;
    
    if(!m_bIsConnected)
        return -1;

    if(m_socket.GetSocket() < 0)
        return -1;

    nLen = m_socket.Send(szPacket, nLength);
    return nLen;
}







