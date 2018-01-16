#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <algorithm>

// tinyxml lib..
#include "tinyxml.h"
#include "xpath_static.h"

#define DOCUMENT_TYPE           ("<!DOCTYPE GctiMsg SYSTEM \"IServer.dtd\">")
#define MAX_XML_SIZE            (4096)
#define MAX_APPEND_DATA_SIZE    (2048)

class CXmlItem 
{
public:
    CXmlItem(const char* pXmlVersion = "1.0", const char* pEncoding = "ISO-8859-1");
    ~CXmlItem(void);

protected:
    void Clear();
    void FreeXmlDocument();



// make xml functions..
public:
    int MakeXml_LoginReq    ( const char* szCallId
                            , const char* szVersion
                            , const char* szClientName
                            , const char* szReportStatus
                            , const char* szServerMonitor );
    
    int MakeXml_LoginResp   ( const char* szCallId
                            , const char* szIServerVersion
                            , const char* szResult
                            , const char* szConfigOptions
                            , const char* szStatus );
    
    int MakeXml_FlowControl ( const char* szCallId
                            , const char* szStatus );
    
    int MakeXml_NewCall     ( const char* szCallId
                            , const char* szCalledNum
                            , const char* szCallControlMode
                            , const char* szVersion
                            , const char* szDNIS
                            , const char* szANI
                            , const char* szUDataEx
                            , const char* szExtnsEx );

    int MakeXml_CallStatus  ( const char* szCallId
                            , const char* szCallStatus );

    int MakeXml_RouteRequest( const char* szCallId
                            , const char* szRouteDN                            
                            , const char* szCED
                            , const char* szUDataEx
                            , const char* szExtnsEx);

    int MakeXml_RouteResponse(const char* szCallId
                            , const char* szRouteType
                            , const char* szDest
                            , const char* szExtnsEx);

    
    int MakeXml_OneStepXfer ( const char* szCallId
                            , const char* szDestDN
                            , const char* szUDataEx
                            , const char* szExtnsEx );

    int MakeXml_CallInfoReq ( const char* szCallId );

    int MakeXml_CallInfoResp( const char* szCallId
                            , const char* szANI
                            , const char* szDNIS
                            , const char* szCalledNum
                            , const char* szConnId
                            , const char* szFirstHomeLocation
                            , const char* szLastEvent
                            , const char* szOtherDN
                            , const char* szOtherQueue
                            , const char* szOtherTrunk
                            , const char* szPortDN
                            , const char* szPortQueue
                            , const char* szPortTrunk
                            , const char* szThirdPartyDN
                            , const char* szTSCallId );

    int MakeXml_EndCall     ( const char* szCallId
                            , const char* szEndCause );
    
    int MakeXml_UDataSet    ( const char* szCallId
                            , const char* szRequestId
                            , const char* szAction
                            , const char* szUDataEx);
        
    int MakeXml_UDataGet    ( const char* szCallId
                            , const char* szRequestId
                            , const char* szKeys
                            , const char *szDelemeter = "%" );

    int MakeXml_UDataResp   ( const char* szCallId
                            , const char* szRequestId
                            , const char* szResult
                            , const char* szUDataEx );


    int MakeXml_PeekStatReq ( const char* szCallId
                            , const char* szRequestId
                            , const char* szStatName);

    int MakeXml_StatResp    ( const char* szCallId
                            , const char* szRequestId
                            , const char* szResultCode
                            , const char* szResult );

    
    

protected:
    void MakeXml_AppendUDataEx  ( TiXmlElement *pNodeParent
                                , const char *szUDataEx
                                , const char *szDelemeter = "%" );

    void MakeXml_AppendExtnsEx ( TiXmlElement *pNodeParent
                                , const char *szExtnsEx
                                , const char *szDelemeter = "%" );



// parsing
public:
    int Parse(const char *szXmlStr);
    int ParseAttachData(const char* szXPath, char* szBuff, int nBufLen, const char *szDelemeter = "%");
    void GetValue(const char* szXPath, char* szBuff, int nBufLen);

public:
    inline const char* GetXmlStr() { return m_szXmlStr;     }
    inline const char* GetXmlCmd() { return m_szXmlCmd;     }
    inline const char* GetCallId() { return m_szCallId;     }
    inline int GetXmlLength()      { return m_nXmlLength;   }

// fields...
private:
    TiXmlDocument m_xmlDoc;
    TiXmlPrinter m_xmlPrinter;
    char m_szCallId[50];
    char m_szXmlCmd[50];

    char m_szXmlVersion[5];
    char m_szEncoding[30];

    char m_szXmlStr[MAX_XML_SIZE];
    int  m_nXmlLength;
};



