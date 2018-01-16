#include "XmlItem.h"


CXmlItem::CXmlItem(const char* pXmlVersion, const char* pEncoding)
{   
    Clear();

    strcpy(m_szXmlVersion, pXmlVersion);
    strcpy(m_szEncoding  , pEncoding);

    TiXmlDeclaration* decl  = new TiXmlDeclaration( pXmlVersion, pEncoding, "" );  
    m_xmlDoc.LinkEndChild( decl );    
    
	m_xmlDoc.Parse( DOCUMENT_TYPE );
		
	TiXmlHandle docH( &m_xmlDoc );
	//TiXmlUnknown* unknown = docH.Child( 1 ).Unknown();

    m_xmlPrinter.SetStreamPrinting();
}


CXmlItem::~CXmlItem(void)
{
    Clear();
    FreeXmlDocument();
}

void CXmlItem::Clear()
{
    memset(m_szCallId    , 0x00, sizeof(m_szCallId));
    memset(m_szXmlCmd    , 0x00, sizeof(m_szXmlCmd));
    memset(m_szXmlVersion, 0x00, sizeof(m_szXmlVersion));
    memset(m_szEncoding  , 0x00, sizeof(m_szEncoding));
    memset(m_szXmlStr    , 0x00, sizeof(m_szXmlStr));
    m_nXmlLength = 0;
}

void CXmlItem::FreeXmlDocument()
{
    m_xmlDoc.Clear();
}



/****************************************************************************
 TITLE       : MakeXml_LoginReq
 DESCRIPTION : Make LoginReq Xml message
 PARAMS      : CallId
               Version
               ClientName
               ReportStatus
               ServerMonitor
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
****************************************************************************/
int CXmlItem::MakeXml_LoginReq(const char* szCallId
                                , const char* szVersion
                                , const char* szClientName
                                , const char* szReportStatus
                                , const char* szServerMonitor)
{   
    strcpy(m_szXmlCmd, "LoginReq");
    strcpy(m_szCallId, szCallId);
    
    TiXmlElement *pRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pRoot );

    TiXmlElement *pCallId = new TiXmlElement( "CallId" );
    pCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pRoot->LinkEndChild(pCallId);
    
    TiXmlElement *pLoginReq = new TiXmlElement( m_szXmlCmd );
    pLoginReq->LinkEndChild(new TiXmlText(""));
    pRoot->LinkEndChild(pLoginReq);
    
    pLoginReq->SetAttribute("Version"       , szVersion);
    pLoginReq->SetAttribute("ClientName"    , szClientName);
    pLoginReq->SetAttribute("ReportStatus"  , szReportStatus);
    pLoginReq->SetAttribute("ServerMonitor" , szServerMonitor);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_LoginResp
 DESCRIPTION : Make LoginResp Xml message
 PARAMS      : szCallId
               szIServerVersion
               szResult
               szConfigOptions
               szStatus               
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_LoginResp(const char* szCallId
                                , const char* szIServerVersion
                                , const char* szResult
                                , const char* szConfigOptions
                                , const char* szStatus)
{
    strcpy(m_szXmlCmd, "LoginResp");
    strcpy(m_szCallId, szCallId);
    
    // node Root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node CallId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node LoginResp
    TiXmlElement *pNodeLoginResp = new TiXmlElement( m_szXmlCmd );
    pNodeLoginResp->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeLoginResp);

    pNodeLoginResp->SetAttribute("IServerVersion", szIServerVersion);
    pNodeLoginResp->SetAttribute("Result"        , szResult);
    pNodeLoginResp->SetAttribute("ConfigOptions" , szConfigOptions);
    pNodeLoginResp->SetAttribute("Status"        , szStatus);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_FlowControl
 DESCRIPTION : Make FlowControl Xml message
 PARAMS      : szCallId
               szStatus               
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_FlowControl(const char* szCallId, const char* szStatus)
{
    strcpy(m_szXmlCmd, "FlowControl");
    strcpy(m_szCallId, szCallId);
    
    // node Root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node NodeCallId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node FlowControl
    TiXmlElement *pFlowControl = new TiXmlElement( m_szXmlCmd );
    pFlowControl->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pFlowControl);

    pFlowControl->SetAttribute("Status", szStatus);
    
    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_FlowControl
 DESCRIPTION : Make FlowControl Xml message
 PARAMS      : szCallId
               szCalledNum          // GCTI port number
               szCallControlMode
               szVersion
               szDNIS
               szANI
               szUDataEx
               szExtnsEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_NewCall( const char* szCallId
                                , const char* szCalledNum
                                , const char* szCallControlMode
                                , const char* szVersion
                                , const char* szDNIS
                                , const char* szANI
                                , const char* szUDataEx
                                , const char* szExtnsEx )
{
    strcpy(m_szXmlCmd, "NewCall");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node NewCall
    TiXmlElement *pNodeNewCall = new TiXmlElement( m_szXmlCmd );
    pNodeNewCall->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeNewCall);

    pNodeNewCall->SetAttribute("CallControlMode", szCallControlMode);
    pNodeNewCall->SetAttribute("Version"        , szVersion);

    // node CalledNum
    TiXmlElement *pNodeCalledNum = new TiXmlElement( "CalledNum" );
    pNodeCalledNum->LinkEndChild(new TiXmlText(szCalledNum));
    pNodeNewCall->LinkEndChild(pNodeCalledNum);

    // node dnis
    if(strlen(szDNIS) > 0)
    {
        TiXmlElement *pNodeDNIS = new TiXmlElement( "DNIS" );
        pNodeDNIS->LinkEndChild(new TiXmlText(szDNIS));
        pNodeNewCall->LinkEndChild(pNodeDNIS);
    }

    // node ani
    if(strlen(szANI) >0)
    {
        TiXmlElement *pNodeANI = new TiXmlElement( "ANI" );
        pNodeANI->LinkEndChild(new TiXmlText(szANI));
        pNodeNewCall->LinkEndChild(pNodeANI);
    }

    // node UDataEx
    if(strlen(szUDataEx) > 0)
        MakeXml_AppendUDataEx(pNodeNewCall, szUDataEx);

    // node ExtnsEx
    if(strlen(szExtnsEx) > 0)
        MakeXml_AppendExtnsEx(pNodeNewCall, szExtnsEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;

}


/****************************************************************************
 TITLE       : MakeXml_CallStatus
 DESCRIPTION : Make CallStatus Xml message
 PARAMS      : szCallId
               szStatus               
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_CallStatus(const char* szCallId, const char* szCallStatus)
{
    strcpy(m_szXmlCmd, "CallStatus");
    strcpy(m_szCallId, szCallId);
    
    // node Root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node NodeCallId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node CallStatus
    TiXmlElement *pNodeCallStatus = new TiXmlElement( m_szXmlCmd );
    pNodeCallStatus->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeCallStatus);

    pNodeCallStatus->SetAttribute("Event", szCallStatus);
    
    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_RouteRequest
 DESCRIPTION : Make RouteRequest Xml message
 PARAMS      : szCallId
               szRouteDN                            
               szCED
               szUDataEx
               szExtnsEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_RouteRequest( const char* szCallId
                            , const char* szRouteDN                            
                            , const char* szCED
                            , const char* szUDataEx
                            , const char* szExtnsEx)
{
    strcpy(m_szXmlCmd, "RouteRequest");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node RouteRequest
    TiXmlElement *pNodeRouteRequest = new TiXmlElement( m_szXmlCmd );
    pNodeRouteRequest->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeRouteRequest);

    pNodeRouteRequest->SetAttribute("RouteDN", szRouteDN);
    
    // node CED
    if(strlen(szCED) > 0)
    {
        TiXmlElement *pNodeCED = new TiXmlElement( "CED" );
        pNodeCED->LinkEndChild(new TiXmlText(szCED));
        pNodeRouteRequest->LinkEndChild(pNodeCED);
    }

    // node UDataEx
    if(strlen(szUDataEx) > 0)
        MakeXml_AppendUDataEx(pNodeRouteRequest, szUDataEx);

    // node ExtnsEx
    if(strlen(szExtnsEx) > 0)
        MakeXml_AppendExtnsEx(pNodeRouteRequest, szExtnsEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_RouteResponse
 DESCRIPTION : Make RouteResponse Xml message
 PARAMS      : szCallId
               szRouteType
               szDest
               szExtnsEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_RouteResponse(const char* szCallId
                                    , const char* szRouteType
                                    , const char* szDest
                                    , const char* szExtnsEx )
{
    strcpy(m_szXmlCmd, "RouteResponse");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node RouteResponse
    TiXmlElement *pNodeRouteResponse = new TiXmlElement( m_szXmlCmd );
    pNodeRouteResponse->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeRouteResponse);

    pNodeRouteResponse->SetAttribute("RouteType", szRouteType);

    TiXmlElement *pNodeDest = new TiXmlElement( "Dest" );
    pNodeDest->LinkEndChild(new TiXmlText(szDest));
    pNodeRouteResponse->LinkEndChild(pNodeDest);

    // node ExtnsEx
    if(strlen(szExtnsEx) > 0)
        MakeXml_AppendExtnsEx(pNodeRouteResponse, szExtnsEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_OneStepXfer
 DESCRIPTION : Make OneStepXfer Xml message
 PARAMS      : szCallId
               szDestDN
               szUDataEx
               szExtnsEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem:: MakeXml_OneStepXfer ( const char* szCallId
                                    , const char* szDestDN
                                    , const char* szUDataEx
                                    , const char* szExtnsEx )
{
    strcpy(m_szXmlCmd, "OneStepXfer");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node OneStepXfer
    TiXmlElement *pNodeOneStepXfer = new TiXmlElement( m_szXmlCmd );
    pNodeOneStepXfer->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeOneStepXfer);

    pNodeOneStepXfer->SetAttribute("DestDN", szDestDN);

    // node UDataEx
    if(strlen(szUDataEx) > 0)
        MakeXml_AppendUDataEx(pNodeOneStepXfer, szUDataEx);

    // node ExtnsEx
    if(strlen(szExtnsEx) > 0)
        MakeXml_AppendExtnsEx(pNodeOneStepXfer, szExtnsEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_CallInfoReq
 DESCRIPTION : Make CallInfoReq Xml message
 PARAMS      : szCallId
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_CallInfoReq ( const char* szCallId)
{
    strcpy(m_szXmlCmd, "CallInfoReq");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node CallInfoReq
    TiXmlElement *pNodeCallInfoReq = new TiXmlElement( m_szXmlCmd );
    pNodeCallInfoReq->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeCallInfoReq);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_CallInfoResp
 DESCRIPTION : Make CallInfoResp Xml message
 PARAMS      : szCallId
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_CallInfoResp( const char* szCallId
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
                                    , const char* szTSCallId )
{
    strcpy(m_szXmlCmd, "CallInfoResp");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node CallInfoResp
    TiXmlElement *pNodeCallInfoResp = new TiXmlElement( m_szXmlCmd );
    pNodeCallInfoResp->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeCallInfoResp);

    pNodeCallInfoResp->SetAttribute("ANI"              , szANI);
    pNodeCallInfoResp->SetAttribute("DNIS"             , szDNIS);
    pNodeCallInfoResp->SetAttribute("CalledNum"        , szCalledNum);
    pNodeCallInfoResp->SetAttribute("ConnId"           , szConnId);
    pNodeCallInfoResp->SetAttribute("FirstHomeLocation", szFirstHomeLocation);
    pNodeCallInfoResp->SetAttribute("LastEvent"        , szLastEvent);
    pNodeCallInfoResp->SetAttribute("OtherDN"          , szOtherDN);
    pNodeCallInfoResp->SetAttribute("OtherQueue"       , szOtherQueue);
    pNodeCallInfoResp->SetAttribute("OtherTrunk"       , szOtherTrunk);
    pNodeCallInfoResp->SetAttribute("PortDN"           , szPortDN);
    pNodeCallInfoResp->SetAttribute("PortQueue"        , szPortQueue);
    pNodeCallInfoResp->SetAttribute("PortTrunk"        , szPortTrunk);
    pNodeCallInfoResp->SetAttribute("ThirdPartyDN"     , szThirdPartyDN);
    pNodeCallInfoResp->SetAttribute("TSCallId"         , szTSCallId);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_EndCall
 DESCRIPTION : Make EndCall Xml message
 PARAMS      : szCallId
               szEndCause
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_EndCall( const char* szCallId, const char* szEndCause )
{
    strcpy(m_szXmlCmd, "EndCall");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node EndCall
    TiXmlElement *pNodeEndCall = new TiXmlElement( m_szXmlCmd );
    pNodeEndCall->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeEndCall);

    pNodeEndCall->SetAttribute("EndCause", szEndCause);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_UDataSet
 DESCRIPTION : Make UDataSet Xml message
 PARAMS      : szCallId
               szRequestId
               szAction
               szUDataEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/ 
int CXmlItem::MakeXml_UDataSet( const char* szCallId
                                , const char* szRequestId
                                , const char* szAction
                                , const char* szUDataEx)
{
    strcpy(m_szXmlCmd, "UDataSet");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node UDataSet
    TiXmlElement *pNodeUDataSet = new TiXmlElement( m_szXmlCmd );
    pNodeUDataSet->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeUDataSet);

    pNodeUDataSet->SetAttribute("Action", szAction);
    
    // node RequestId
    TiXmlElement *pNodeRequestId = new TiXmlElement( "RequestId" );
    pNodeRequestId->LinkEndChild(new TiXmlText(szRequestId));
    pNodeUDataSet->LinkEndChild(pNodeRequestId);

    // node UDataEx
    if(strlen(szUDataEx) > 0)
        MakeXml_AppendUDataEx(pNodeUDataSet, szUDataEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_UDataGet
 DESCRIPTION : Make UDataGet Xml message
 PARAMS      : szCallId
               szKeys
               szRequestId 
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_UDataGet( const char*  szCallId
                                , const char* szRequestId
                                , const char* szKeys
                                , const char* szDelemeter 
                                )
{
    strcpy(m_szXmlCmd, "UDataGet");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node UDataGet
    TiXmlElement *pNodeUDataGet = new TiXmlElement( m_szXmlCmd );
    pNodeUDataGet->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeUDataGet);


    // Format = key1|key2|key..n -> key1:key2:keyn..
    char* pKey  = NULL;
    
    char szData[128];
    memset(szData, 0x00, sizeof(szData));
    strcpy(szData, szKeys);

    char szNewKeys[1024];
    memset(szNewKeys, 0x00, sizeof(szNewKeys));

    // int nSeq = 0;
    pKey = strtok( szData, szDelemeter );
    do
    {   
        strcat(szNewKeys, pKey);
        pKey = strtok( NULL, szDelemeter );
        if(pKey != NULL)
        {
            strcat(szNewKeys, ":");
            //nSeq++;
        }

    }while(pKey != NULL);

    pNodeUDataGet->SetAttribute("Keys"  , szNewKeys);
    
    // node RequestId
    TiXmlElement *pNodeRequestId = new TiXmlElement( "RequestId" );
    pNodeRequestId->LinkEndChild(new TiXmlText(szRequestId));
    pNodeUDataGet->LinkEndChild(pNodeRequestId);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : MakeXml_UDataResp
 DESCRIPTION : Make UDataResp Xml message
 PARAMS      : szCallId
               szAction
               szKeys
               szRequestId
               szUDataEx
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_UDataResp( const char* szCallId
                                , const char* szRequestId
                                , const char* szResult
                                , const char* szUDataEx  )
{
    strcpy(m_szXmlCmd, "UDataResp");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);

    // node UDataResp
    TiXmlElement *pNodeUDataResp = new TiXmlElement( m_szXmlCmd );
    pNodeUDataResp->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeUDataResp);
        
    // node RequestId
    TiXmlElement *pNodeRequestId = new TiXmlElement( "RequestId" );
    pNodeRequestId->LinkEndChild(new TiXmlText(szRequestId));
    pNodeUDataResp->LinkEndChild(pNodeRequestId);

    // node UDataEx
    if(strlen(szUDataEx) > 0)
        MakeXml_AppendUDataEx(pNodeUDataResp, szUDataEx);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}



/****************************************************************************
 TITLE       : MakeXml_PeekStatReq
 DESCRIPTION : Make UDataGet Xml message
 PARAMS      : szCallId
               szKeys
               szRequestId 
 RETURN      : xml message length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int CXmlItem::MakeXml_PeekStatReq( const char*  szCallId
                                 , const char*  szRequestId
                                 , const char*  szStatName)
{
    strcpy(m_szXmlCmd, "PeekStatReq");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);
    
    // node PeekStatReq
    TiXmlElement *pNodePeekStatReq = new TiXmlElement( m_szXmlCmd );
    pNodePeekStatReq->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodePeekStatReq);

    // node RequestId
    TiXmlElement *pNodeRequestId = new TiXmlElement( "RequestId" );
    pNodeRequestId->LinkEndChild(new TiXmlText(szRequestId));
    pNodePeekStatReq->LinkEndChild(pNodeRequestId);

    // node RequestId
    TiXmlElement *pNodeStatName = new TiXmlElement( "StatName" );
    pNodeStatName->LinkEndChild(new TiXmlText(szStatName));
    pNodePeekStatReq->LinkEndChild(pNodeStatName);

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


int CXmlItem::MakeXml_StatResp( const char* szCallId
                                , const char* szRequestId
                                , const char* szResultCode
                                , const char* szResult )
{
    strcpy(m_szXmlCmd, "StatResp");
    strcpy(m_szCallId, szCallId);
    
    // node root
    TiXmlElement *pNodeRoot = new TiXmlElement( "GctiMsg" );  
    m_xmlDoc.LinkEndChild( pNodeRoot );

    // node callId
    TiXmlElement *pNodeCallId = new TiXmlElement( "CallId" );
    pNodeCallId->LinkEndChild(new TiXmlText( m_szCallId ));
    pNodeRoot->LinkEndChild(pNodeCallId);
    
    // node StatResp
    TiXmlElement *pNodeStatResp = new TiXmlElement( m_szXmlCmd );
    pNodeStatResp->LinkEndChild(new TiXmlText(""));
    pNodeRoot->LinkEndChild(pNodeStatResp);

    pNodeStatResp->SetAttribute("ResultCode", szResultCode);

    // node RequestId
    TiXmlElement *pNodeRequestId = new TiXmlElement( "RequestId" );
    pNodeRequestId->LinkEndChild(new TiXmlText(szRequestId));
    pNodeStatResp->LinkEndChild(pNodeRequestId);
    
    // node Result
    TiXmlElement *pNodeResult = NULL;
    if(strlen(szResult) > 0)
    {
        pNodeResult = new TiXmlElement( "Result" );
        pNodeResult->LinkEndChild(new TiXmlText(szResult));
        pNodeStatResp->LinkEndChild(pNodeResult);
    }

    m_xmlDoc.Accept(&m_xmlPrinter);

    strcpy(m_szXmlStr, m_xmlPrinter.CStr());
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}

/****************************************************************************
 TITLE       : MakeXml_AppendUDataEx
 DESCRIPTION : Make FlowControl Xml message
 PARAMS      : pNodeParent
               szUDataEx
               szDelemeter               
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
void CXmlItem::MakeXml_AppendUDataEx( TiXmlElement *pNodeParent, const char *szUDataEx, const char* szDelemeter )
{
    if(strlen(szUDataEx) <=0 )
        return;

    char szData[MAX_APPEND_DATA_SIZE];
    memset(szData, 0x00, sizeof(szData));
    strcpy(szData, szUDataEx);
    
    TiXmlElement *pNode = new TiXmlElement( "UDataEx" );
    pNode->LinkEndChild(new TiXmlText( "" ));
    pNodeParent->LinkEndChild(pNode);
    
    char szKey[128];
    char szValue[MAX_APPEND_DATA_SIZE];
    char* pKeyValue  = NULL;
    char* pPos = NULL;
    int nIdx = 0;

    pKeyValue = strtok( szData, szDelemeter );
    while(pKeyValue != NULL)
    {
        pPos = NULL; 
        memset(szKey  , 0x00, sizeof(szKey));
        memset(szValue, 0x00, sizeof(szValue));

        pPos = strchr(pKeyValue, '=');
        if(pPos == NULL)
        {
            pKeyValue = strtok( NULL, szDelemeter );
            continue;
        }

        nIdx = strlen(pKeyValue) - strlen(pPos);
        strncpy(szKey, pKeyValue, nIdx);
        strncpy(szValue, ((pKeyValue + nIdx) + 1), (strlen(pKeyValue) - nIdx));
        
        TiXmlElement *pNodeChild = new TiXmlElement( "Node" );
        pNodeChild->LinkEndChild(new TiXmlText( "" ));

        pNodeChild->SetAttribute("Name", szKey);
        pNodeChild->SetAttribute("Type", "Str");
        pNodeChild->SetAttribute("Val" , szValue);

        pNode->LinkEndChild(pNodeChild);

        pKeyValue = strtok( NULL, szDelemeter );
    }
}


/****************************************************************************
 TITLE       : MakeXml_AppendExtnsEx
 DESCRIPTION : Make FlowControl Xml message
 PARAMS      : pNodeParent
               szExtnsEx
               szDelemeter               
 RETURN      : 
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
void CXmlItem::MakeXml_AppendExtnsEx( TiXmlElement *pNodeParent, const char *szExtnsEx, const char* szDelemeter )
{
    if(strlen(szExtnsEx) <=0 )
        return;

    char szData[MAX_APPEND_DATA_SIZE];
    memset(szData, 0x00, sizeof(szData));
    strcpy(szData, szExtnsEx);
    
    TiXmlElement *pNode = new TiXmlElement( "ExtnsEx" );
    pNode->LinkEndChild(new TiXmlText( "" ));
    pNodeParent->LinkEndChild(pNode);
    
    char szKey[128];
    char szValue[MAX_APPEND_DATA_SIZE];
    char* pKeyValue  = NULL;
    char* pPos = NULL;
    int nIdx = 0;

    pKeyValue = strtok( szData, szDelemeter );
    while(pKeyValue != NULL)
    {
        pPos = NULL; 
        memset(szKey  , 0x00, sizeof(szKey));
        memset(szValue, 0x00, sizeof(szValue));

        pPos = strchr(pKeyValue, '=');
        if(pPos == NULL)
        {
            pKeyValue = strtok( NULL, szDelemeter );
            continue;
        }

        nIdx = strlen(pKeyValue) - strlen(pPos);
        strncpy(szKey, pKeyValue, nIdx);
        strncpy(szValue, ((pKeyValue + nIdx) + 1), (strlen(pKeyValue) - nIdx));
        
        TiXmlElement *pNodeChild = new TiXmlElement( "Node" );
        pNodeChild->LinkEndChild(new TiXmlText( "" ));

        pNodeChild->SetAttribute("Name", szKey);
        pNodeChild->SetAttribute("Type", "Str");
        pNodeChild->SetAttribute("Val" , szValue);

        pNode->LinkEndChild(pNodeChild);

        pKeyValue = strtok( NULL, szDelemeter );
    }
}




/****************************************************************************
 TITLE       : Parse
 DESCRIPTION : Parsing xml string
 PARAMS      : szXmlStr
 RETURN      : xml length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
int  CXmlItem::Parse(const char *szXmlStr)
{   
    // parsing
    m_xmlDoc.Parse(szXmlStr);
        
    // get Root
    TiXmlElement* pRoot = m_xmlDoc.FirstChildElement("GctiMsg");        
    if( pRoot == NULL ) 
        return 0;        
        
    strcpy(m_szCallId, pRoot->FirstChildElement()->GetText());
    strcpy(m_szXmlCmd, pRoot->LastChild()->Value());
    strcpy(m_szXmlStr, szXmlStr);
    m_nXmlLength = strlen(m_szXmlStr);

    return m_nXmlLength;
}


/****************************************************************************
 TITLE       : GetValue
 DESCRIPTION : Parsing xml string
 PARAMS      : szNodeName
               szBuff
               nBufLen
 RETURN      : xml length
 WRITER      : KIMCG
 DATE        : 2014.12.17
***************************************************************************/
void CXmlItem::GetValue(const char* szXPath, char* szBuff, int nBufLen)
{
    memset(szBuff, 0x00, sizeof(nBufLen));

    TIXML_STRING S_res;
    S_res = TinyXPath::S_xpath_string (m_xmlDoc.RootElement(), szXPath);    
    strcpy(szBuff, S_res.c_str());

}



/****************************************************************************
 TITLE       : ParseAttachData
 DESCRIPTION : Parsing Attach Data
 PARAMS      : szXPath
               szBuff
               nBufLen
               szDelemeter
 RETURN      : Attach data length
 WRITER      : KIMCG
 DATE        : 2014.12.19
***************************************************************************/
int CXmlItem::ParseAttachData(const char* szXPath, char* szBuff, int nBuffLen, const char *szDelemeter)
{    
    int nDataLen = 0;
    memset(szBuff, 0x00, sizeof(nBuffLen));

    TiXmlNode *pNodeParent = TinyXPath::XNp_xpath_node(m_xmlDoc.RootElement(), szXPath);
    if(pNodeParent == NULL)
        return nDataLen;

    if(pNodeParent->NoChildren())
        return nDataLen;

    TiXmlElement *pNodeChild = pNodeParent->FirstChildElement();
    if(pNodeChild == NULL)
        return nDataLen;

    do
    {   
        strcat(szBuff, pNodeChild->Attribute("Name"));
        strcat(szBuff, "=");
        strcat(szBuff, pNodeChild->Attribute("Val"));

        pNodeChild = pNodeChild->NextSiblingElement();
        if(pNodeChild != NULL)
            strcat(szBuff, szDelemeter);

    }while(pNodeChild != NULL);

    nDataLen = strlen(szBuff);
    return nDataLen;
}



