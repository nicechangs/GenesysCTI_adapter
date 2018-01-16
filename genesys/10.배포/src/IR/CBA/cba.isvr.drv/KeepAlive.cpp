/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge interface library for Genesys IVR Server.

        TITLE   : Keep Alive Check thread
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/
#include "KeepAlive.h"
#include "IVRDriver.h"


// constructor && desctructor
CKeepAlive::CKeepAlive(void)
{
     m_nInterval = 0;
     m_bContinue = false;
}
CKeepAlive::~CKeepAlive(void){}


/****************************************************************************
 TITLE       : StartUp
 DESCRIPTION : Start thread
 PARAMS      : object
 RETURN      : true / false
 WRITER      : KIMCG
 DATE        : 2014.09.17
****************************************************************************/
bool CKeepAlive::StartUp(NPVOID pArg)
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
bool CKeepAlive::StopAll()
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
NPVOID CKeepAlive::ThreadRun(NPVOID pArg)
{
    CIVRDriver *pDriver = (CIVRDriver*)pArg;
    if(pDriver == NULL)
        return NULL;
    
    while(IsOn())
    {
        iSleep(ONESEC * m_nInterval);
        
        if(!m_bContinue)
            continue;

        pDriver->SendKeepAliveMsg();
    }

    return NULL;
}


