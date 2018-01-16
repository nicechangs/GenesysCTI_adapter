/*! 
 \file interface.h
 \brief IVR library data for drivers/applications
*/
/* ========================================================================= *\
|                                                                             |
|  File Name:         interface.h                                             |
|                                                                             |
|  Copyright.         Copyright (C) Genesys Labs 2002                         |
|                     LICENSED MATERIAL - PROGRAM PROPERTY OF Genesys Labs    |
|                                                                             |
|  Description:      Functions available with I library for the use of        |
|                                               IVR-Driver developers         |
|                                                                             |
\* ========================================================================= */
#ifndef __ILIBRARY_INTERFACE_H
#define __ILIBRARY_INTERFACE_H

/*
	Assume C declarations for C++
*/
#ifdef __cplusplus
	extern "C" {
#endif          

#if defined __OS2DEF__ || defined __wtypes_h__ || defined WIN32 || defined WIN64
	#define NDEF_BOOL
#endif

#if !defined DEF_BOOL && !defined NDEF_BOOL
	#define DEF_BOOL
	#ifdef  FALSE
		#undef FALSE
	#endif
	#ifdef  TRUE
		#undef TRUE
	#endif
	#define FALSE  0
	#define TRUE   1
	typedef int BOOL;
	typedef BOOL* PBOOL;
#endif

#if !defined __OS2DEF__
	typedef unsigned char          BYTE;
	typedef unsigned short       USHORT;
	typedef unsigned int           UINT;
	typedef unsigned long         ULONG;
	typedef void           *      PVOID;
	typedef BYTE           *      PBYTE;
	typedef int            *       PINT;
	typedef UINT           *      PUINT;
	typedef ULONG          *     PULONG;
	typedef BYTE           *      PUSTR;
#endif

typedef const void      *     CPVOID;
typedef PVOID           *     PPVOID;
typedef char            *       PSTR;
typedef const char      *      CPSTR;
typedef long            *      PLONG;

#if !defined WIN32 && !defined WIN64
	typedef long                   LONG;
	typedef long           *       LPTR;
#endif

#if defined WIN32 || WIN64
	typedef __int64              LONG64;
	typedef unsigned __int64    ULONG64;
#elif defined __osf__
	typedef long                 LONG64;
	typedef unsigned long       ULONG64;
#elif defined SCO_SV_3_2
#elif defined _ATT_UX
#elif defined _UW1_1
#elif defined _UW2_1
#elif defined OS2
	typedef struct tagLONG64
	{
		long  Lo;
		long  Hi;
	} LONG64;
	typedef struct tagULONG64
	{
		ULONG Lo;
		ULONG Hi;
	} ULONG64;
#else
	typedef long long            LONG64;
	typedef unsigned long long  ULONG64;
#endif

#if !defined NULL
	# define NULL 0
#endif

typedef unsigned short   SOCKET_PORT;
typedef ULONG            ilPORT;       /* Must be a long integer        */
typedef ULONG              ilRQ;       /* Request number type           */
#define ilRQ_ERR              0        /* ilSRq..  error return value   */
#define ilRQ_ANY              0        /* Argument used for functions:  */
                                       /* ilSRq.. will generate its     */
                                       /*  own request ID (ret.back)    */
                                       /* ilGetReply will take oldest   */
                                       /*  reply from internal queue    */

typedef long                   ilRET;  /* Return type (values see below)*/
#define ilRET_OK                   0L  /* or any positive value         */
#define ilRET_ERROR               -1L  /* a general error               */
#define ilRET_LIB_NOTREADY        -3L  /* the library has not been      */
									   /* initialized                   */
#define ilRET_CONN_CLOSED         -5L  /* the connections is closed     */
#define ilRET_BAD_ARGS            -7L  /* an argument is not valid      */ 
#define ilRET_FUNC_UNSUPPORTED    -9L  /* function is not supported     */
#define ilRET_TIMEOUT            -11L  /* Order still be in progress    */
#define ilRET_REQ_EXPIRED        -12L  /* There is no request with specified
                                          number or it has been expired */
#define ilRET_NO_REQUESTS        -13L  /* There is no request in the queue
                                          for processing. Generates by
                                          ilGetReplyAny                 */
#define ilRET_BAD_CONN_NAME      -15L  /* The connection name is bad    */
#define ilRET_REQ_FAILURE      -1000L  /* Reply from requested service
                                          contains failure code         */
/*
	GetCallInfo types
*/

/* FR 267139776 DEL support getcallinfo Call_UUID */
/* ER 278630141 DWS support getcallinfo ThirdPartyDN */
typedef enum
{
	ilCI_ALL				   = 100,
	ilCI_CONN_ID               = 101,
	ilCI_CALL_ID               = 102,
	ilCI_DN                    = 111,
	ilCI_ANI                   = 112,
	ilCI_DNIS                  = 113,
	ilCI_THIS_DN               = 121,
	ilCI_THIS_TRUNK            = 122,
	ilCI_THIS_QUEUE            = 123,
	ilCI_OTHER_DN              = 131,
	ilCI_OTHER_TRUNK           = 132,
	ilCI_OTHER_QUEUE           = 133,
	ilCI_LAST_EVENT_NAME       = 151,
	ilCI_FIRST_HOME_LOC		   = 152,
	ilCI_CALL_UUID  		   = 153,
	ilCI_3RD_PARTY_DN		   = 154,
	ilCI_UNKNOWN			   = 999
} ilCI_TYPE;

/*
	For display of call info parms
*/
#define ilCI_CONN_ID_CHAR			"ConnId"
#define ilCI_ALL_CHAR				"All"
#define ilCI_CALL_ID_CHAR			"TSCallId"
#define ilCI_DN_CHAR				"CalledNum"
#define ilCI_ANI_CHAR				"ANI"
#define ilCI_DNIS_CHAR				"DNIS"
#define ilCI_THIS_DN_CHAR			"PortDN"
#define ilCI_THIS_TRUNK_CHAR		"PortTrunk"
#define ilCI_THIS_QUEUE_CHAR		"PortQueue"
#define ilCI_OTHER_DN_CHAR			"OtherDN"
#define ilCI_OTHER_TRUNK_CHAR		"OtherTrunk"
#define ilCI_OTHER_QUEUE_CHAR		"OtherQueue"
#define ilCI_LAST_EVENT_NAME_CHAR	"LastEvent"
#define ilCI_FIRST_HOME_LOC_CHAR	"FirstHomeLocation"
/* FR 267139776 DEL support getcallinfo Call_UUID */
#define ilCI_CALL_UUID_CHAR			"UUID"
/* ER 278630141 DWS support getcallinfo ThirdPartyDN */
#define ilCI_3RD_PARTY_DN_CHAR		"ThirdPartyDN"
#define ilCI_UNKNOWN_CHAR			"Unknown"

typedef enum
{
  ilET_NUMBER                =   0,
  ilET_TEXT                  =  10
} ilERR_TYPE;

typedef enum
{
	ilDO_NOT_SUPPORTED	= 0,
	ilDO_NO_TRUNKS,
	ilDO_MISC
} ilDIALOUTERR;

typedef enum
{
	ilFAIL_BUSY=0,
	ilFAIL_NO_ANSWER,
	ilFAIL_CONNECT_FAILED
} ilFAILURE;

/*
	The call status is used to determine if an API call will succeed or not. 
	If the determination is made that it will not, the API call is rejected 
	with an appropriate return code. Certain call states are equivalent 
	to an established call which is one of the key criteria for determining
	whether to allow an API to flow. These are noted below.  Notice that
	two states have been added to the states defined in the IServer 
	documentation in order to properly determine whether a call is 
	established or not, specifically the eCallStatusDialingMakeCall and
	eCallStatusBusyMakeCall. If a make call is in the dialing or busy
	state, the call is not established yet.  However, if any already
	established call is in either of these states, the call is established
*/
enum CallStatus			
{
	eCallStatusUnknown=0,
	/*
		For a make call, if it's in the dialing or busy state, it's not 
		established
	*/
	eCallStatusDialingMakeCall,
	eCallStatusBusyMakeCall,
	eCallStatusRinging,
	/*
		The following states are equivalent to an established call
	*/
	eCallStatusHeld,			/* Established */
	eCallStatusBusy,			/* Established */
	eCallStatusDialing,			/* Established */
	eCallStatusEstablished,		/* Established */
	eCallStatusRetrieved,		/* Established */
	eCallStatusConfPartyDel,	/* Established */
	eCallStatusConfPartyAdd,	/* Established */
	eCallStatusXferComplete,	/* It is important to note that the XferComplete 
								   status does not indicate success or failure.  If it did, there 
								   would likely be a XferSucceeded/XferFailed status pair instead.  
								   There are only 2 statuses that indicate that the call has ended 
								   on the T-lib side that the XML side reports: XferComplete and 
								   Released.  Neither can nor do indicate who released or whether 
								   that should be interpreted as success or failure – as success 
								   and failure are often customer defined terms. More precisely, 
								   the two messages are used as follows: if a transfer has ever 
								   been requested, when the call ends XferComplete is the end status.
								   In all other cases, when a call ends Released is used.  This 
								 is the design of the protocol. */
	eCallStatusReleased,		/* Released means the call is no longer established */
	eCallStatusNoChange			/* If the call state is set to this value, it will cause no
								   change to occur to the call state on a port */
};	
//-----------------------------------------------------------------------------------------------------
// The state of the I-Library
// If in LegacyMode = true,
//		ilRET_ACTIVE				- This is the normal processing state
//		ilRET_SHUTDOWN_IN_PROCESS	- Indicates that a shutdown request has been received and calls
//									  are being monitored. The number of outstanding calls will continue
//									  to be displayed.
//									  It is the applications perogative to stop processing or not.
//	If in LegacyMOde = false, the sequence would be
//		ilRET_ACTIVE				- This is the normal processing state
//		ilRET_SHUTDOWN_IN_PROCESS	- indicates that a shutdown request has been received, and agents
//									  are being logged out.
//		ilRET_ALL_AGENTS_LOGGEDOUT	- indicates that all agents are logged out, and calls are being monitored.
//									  The number of outstanding calls will continue to be displayed.
//									  It is the applications perogative to stop processing or not.
//		ilRET_NO_CALLS_IN_PROGRESS	- indicates that all agents are logged out, and there are no calls in progress
//									  The number of outstanding calls will continue to be displayed.
//									  It is the applications perogative to stop processing or not.
//-----------------------------------------------------------------------------------------------------
enum IlibraryState
{
	ilRET_ACTIVE=0,				// no shutdown received, normal processing
	ilRET_SHUTDOWN_IN_PROCESS,	// shutdown request received, logging agents out
	ilRET_ALL_AGENTS_LOGGEDOUT,	// all agents logged out, monitoring calls
	ilRET_NO_CALLS_IN_PROGRESS	// no active calls, can change
};

/*
	I-Library API's
*/

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif
/*! The ilInitiate() function must be called before calling any 
	other function in the library with the exception of ilSetLogHeader.
	Otherwise the IVR Library rejects all function calls. As an 
	argument it takes configuration name of the IVR object resource. This
	function sets up all required services within the library.

	\param	psName Name of this IVR object resource as it is stored in
	Configuration Server. The resource is used to define the switches, DNs, and ports to be
	used during operations with this IVR. For Unixware and the ILibrary SDK, this resource
	is also used to acquire configuration information.
	
	\return The ilInitiate() function returns FALSE if there is
	a problem with the library in which case, the driver should abort.
*/
BOOL ilInitiate(CPSTR psName);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSetVersionNumber() function lets you specify 
	a version of your IVR driver/application. 

	\param pszVersionNumber Points to a string representation of the
	version number for this driver/application.

    \return The ilSetVersionNumber() function returns TRUE if it is 
	successful, FALSE if it fails.
*/
BOOL ilSetVersionNumber(CPSTR pszVersionNumber);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilConnectionOpen() function opens a connection
	and logs in to the IServer.

	\param psHost The name of the machine that hosts the IVR Server.
	\param sPort The port number that the IVR Server is listening on.
	\param ulTimeoutMS The maximum number of milliseconds allowed for
	connection to be established. This same value will also be used
	to determine when to close and re-open
	the socket if the socket appears to be connected to IServer but no login
	response is received. 

    \return The function returns TRUE if a successful login
	to the IServer succeeds. If a successful
	login does not occur within the timeout period, the 
	function returns FALSE. ILibrary will continue to attempt to log
	in to IServer, so the driver can try again.
	If a successful login to IVR Server does not occur, 
	any ilSRq request function call will return the error 
	ilRET_CONN_CLOSED.
*/
BOOL ilConnectionOpen(CPSTR psHost,SOCKET_PORT sPort,ULONG ulTimeoutMS);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilConnectionOpenConfigServer() function opens connections and 
	logs in to IVR Server.

	\param psHost The name of the machine that hosts Config Server.
	\param sPort The port number that the Config Server is listening on.
	\param szAppName The name of the IVR Driver application which contains
	configuration information. 
	\param ulTimeoutMS The maximum number of milliseconds allowed for
	connection to be established. This same value will also be used
	to determine when to close and re-open
	the socket if the socket appears to be connected to IServer but no login
	response is received. 

    \return The function returns TRUE if a successful login
	to at least one IServer succeeds. If a successful
	login does not occur within the timeout period, the 
	function returns FALSE. ILibrary will continue to attempt to log
	in to IServer, so the driver can try again.
	If a successful login to IVR Server does not occur, 
	any ilSRq request function call will return the error 
	ilRET_CONN_CLOSED.
*/
BOOL ilConnectionOpenConfigServer(CPSTR psHost,SOCKET_PORT sPort,CPSTR szAppName,ULONG ulTimeoutMS);

#if defined WIN32 || defined WIN64
	__declspec(dllexport)
#endif
/*!	The ilConnectionOpenConfigServer() function opens connections and 
	logs in to IVR Server.

	\param _host The primary Config Server host name
	\param _port The primary Config Server port number
	\param _port_transport The primary config server client side port number 
	\param _host_bu The backup Config Server host name
	\param _port_bu The backup Config Server port number
	\param szAppName The name of the IVR Driver application which contains configuration information. 
	\param ulTimeoutMS The maximum number of milliseconds allowed for
	connection to be established. This same value will also be used
	to determine when to close and re-open
	the socket if the socket appears to be connected to IServer but no login
	response is received. 

    \return The function returns TRUE if a successful login
	to at least one IServer succeeds. If a successful
	login does not occur within the timeout period, the 
	function returns FALSE. ILibrary will continue to attempt to log
	in to IServer, so the driver can try again.
	If a successful login to IVR Server does not occur, 
	any ilSRq request function call will return the error 
	ilRET_CONN_CLOSED.
*/
BOOL ilConnectionOpenConfigServer80( CPSTR _host, SOCKET_PORT _port, SOCKET_PORT _port_transport, CPSTR _host_bu, SOCKET_PORT _port_bu, CPSTR  appName, ULONG _ulTimeoutMS );

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilConnectionClose() function is provided for compatibility
	with previous versions of this library. It is a no-op
	and should not be used.
   
	\return The function returns TRUE if successful,FALSE if the 
	IVR library is not initialized.
*/
BOOL ilConnectionClose(void);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqNoteCallStart() function notifies the IVR Interface 
	Server that a new call resides on a particular IVR resource 
	specified by the Port argument.

	Call the ilSRqNoteCallStart() function each time that call
	processing is started.

	The IVR script should call this function before it calls 
	any other telephony requests to IVR Interface Server for 
	the particular IVR channel (except for the ilCallInit() 
	function, which performs the equivalent function). 
	Otherwise, the telephony request will be rejected and a corresponding
	error returned.<br>Of the six arguments, the last four are 
	optional and psDNIS and psANI are currently used. 
	If the IVR Interface Server connects to T-Server 
	(linked to a real switch, that is, IVR-Behind-Switch), the 
	driver should not change the DNIS, and ANI that are 
	provided by the T-Server.<br>If the IVR Interface Server is 
	configured for IVR-In-Front (connected to a virtual TServer), 
	then the IVR script may	populate the DNIS and ANI values if 
	the IVR can get that information. 

	\param RqID In general you should pass in ilRQ_ANY. If you 
	pass in your own ilRQ value, your code must manage any 
	contention possibilities. 
	\param Port is the IVR port where the call resides.
	\param psCallID (optional) not used
	\param psDNIS (optional) This value is placed as a DNIS in all 
	events generated by the IVR Server.
	\param psANI (optional) This value is placed as an ANI in 
	all events generated by the IVR Server. 
	\param psTagCDT (optional) not used

    \return If the function succeeds, and if you passed in a first 
	argument of ilRQ_ANY, it returns a valid ilRQ value you 
	should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return 
	that value.
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqNoteCallStart(ilRQ RqID,ilPORT Port,CPSTR psCallID,CPSTR psDNIS,CPSTR psANI,CPSTR psTagCDT);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! Call the ilSRqNoteCallEnd() function when call activity on a
	particular port has completed. If configured for IVR-Behind-Switch,
	the IVR Server will request that the 
	associated TServer release the call.

	\param RqID In general you should pass in ilRQ_ANY. If you 
	pass in your own ilRQ value, your code must manage any 
	contention possibilities. 
	\param Port is the IVR port where the call resides.

    \return If the function succeeds, and if you passed in a first 
	argument of ilRQ_ANY, it returns a valid ilRQ value 
	you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that 
	value. 
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqNoteCallEnd(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! Use the ilGetReply() function to obtain the reply to a 
	previous ilSRq request function. The ilGetReply() function 
	stores the text in a string buffer that your application maintains.

	\param RqID A value captured from the return of an ilSRq*() function.
	\param psRep A pointer to a string buffer.
	\param iRepLen An integer that specifies the size of the string 
	buffer in bytes.

    \return The function returns an ilRET value greater than 0
	for success. 
	<br> If the ilGetReply() function fails, it returns 
	a negative value that corresponds to an ilRET_ERROR defined constant. 
	<br>- INVALID_RQID, the input request id = 0
	<br>- ilRET_ERROR, an error has occurred, look in the logs for more information
	<br>- ilRET_LIB_NOTREADY, the library has not been initialized
	<br>- ilRET_TIMEOUT, IServer has not responded to this request yet 
	<br>- ilRET_REQ_EXPIRED, there is no record of this request, 
		                     an ilGetReply has already asked for it or it is from 
							 a call that is no longer in progress
*/
ilRET ilGetReply(ilRQ RqID, PSTR psRep, int iRepLen);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilWatch() function allows I-Library to 
	<br>1. check on IServer socket readiness 
	<br>2. check on Config Server readiness 
	<br>3. move agents to configured states if LegacyMode=false
	<br>4. satisfy KeepAlive requirements
	<br>The long value specifies the minimum time to wait before returning.

    \return The function returns an ilRET value of one
*/
ilRET ilWatch( long w );

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataAddKD() function attaches one data element
	specified by psKey and psData to the active call that resides 
	on the IVR port.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param port The IVR port where the call resides.
	\param psKey The key to be attached to the call.
	\param psData The data associated with the key.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataAddKD(ilRQ RqID,ilPORT port,CPSTR psKey,CPSTR psData);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqRouteGet() function gets the next service 
	(route) from the router. Immediately after getting 
	the next service, the IVR Server automatically responds 
	to the router with a route point (successfully) used 
	message. If there is a possibility that this may not be 
	the case, use RouteStart instead.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.
	\param psRP Specifies the route point where in the router a 
	valid strategy is loaded, with only a next service 
	(that is, target) within that strategy.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If you passed in your own 
	ilRQ value, it will return that value.  If it fails, 
	the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqRouteGet(ilRQ RqID,ilPORT Port,CPSTR psRP);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetReplyAny() function retrieves whatever is the next
	available reply with data and writes the ilRQ number for that 
	reply in the location specified by the first argument, a 
	pointer to an ilRQ variable. 

	\param	pRqID A pointer to an ilRQ variable that is to store the 
	returned request ID.
	\param psRep A pointer to a string buffer that will contain the 
	text of the reply.
	\param iRepLen Defines the length of psRep in bytes. If an actual 
	reply string is bigger than the number of bytes specified by 
	iRepLen, the function will only copy iRepLen bytes, truncating 
	the remainder of the response.

    \return The function returns an ilRET value of a positive value 
	for success. If the ilGetReplyAny() function fails, it returns 
	a negative value that corresponds to an ilRET_ERROR defined constant:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
    <br>- ilRET_NO_REQUESTS, there is no request in the queue with a response from IServer
*/
ilRET ilGetReplyAny(ilRQ* pRqID,PSTR psRep,int iRepLen);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallInit() function initiates a new call from the IVR 
	port to a destination DN specified by psDstDN.
	Note: This function will not work for the offsite-switch 
	option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you 
	pass in your own ilRQ value, your code must manage any 
	contention possibilities. 
	\param Port The IVR port where the call resides.
	\param psDstDN specifies the directory number of the party to be dialed.

    \return If the ilSRqCallInit() function succeeds, and if you passed 
	in a first argument of ilRQ_ANY, it returns a valid ilRQ value 
	you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that value.
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallInit(ilRQ RqID,ilPORT Port,CPSTR psDstDN);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallComplete() function releases a call associated 
	with the specified IVR port and is equivalent to 
	the ilSRqNoteCallEnd() function. 
	It may be called at any time there is an active call. 
	Note: This function will not work for the offsite-switch 
	option (IVR-In-Front). 

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you passed 
	in a first argument of ilRQ_ANY, it returns a valid ilRQ value 
	you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that 
	value. 
	If it fails, the function returns either ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallComplete(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilSRqCallConference() function makes a new conference call. 
	A new party, specified by the psDstDN parameter, is added to 
	the call.
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. This function does not 
	work for the offsite-switch option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides. 
	\param psDstDN specifies the directory number of the party 
	to be dialed.

    \return If the function succeeds, and if you 
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your   own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallConference(ilRQ RqID,ilPORT Port,CPSTR psDstDN);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallTransfer() function completes a call transfer. 
	This causes the call on the specified port parameter to be 
	released from both the original and consultation call. 
	Parties participating in the original and consultation call 
	that originally resided on this port are merged into one call.
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. This function does not 
	work for the offsite-switch option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.
	\param psDstDN Specifies the directory number of the party 
	to be dialed.

    \return If the function succeeds, and if you passed 
	in a first argument of ilRQ_ANY, it returns a valid ilRQ value 
	you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that value.
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallTransfer(ilRQ RqID,ilPORT Port,CPSTR psDstDN);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallTransferKVList() function completes a call transfer
	and transmits user data along with the call.
	This causes the call on the specified port parameter to be 
	released from both the original and consultation call. 
	Parties participating in the original and consultation call 
	that originally resided on this port are merged into one call.
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. This function does not 
	work for the offsite-switch option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.
	\param psDstDN Specifies the directory number of the party 
	to be dialed.
	\param psData Specifies the user data to be transmitted.

    \return If the function succeeds, and if you passed 
	in a first argument of ilRQ_ANY, it returns a valid ilRQ value 
	you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that value.
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallTransferKVList(ilRQ RqID,ilPORT Port,CPSTR psDstDN, CPSTR psData);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallConsultInit() function places the existing call 
	on hold and originates a consultation call. The new party, 
	specified by the psDstDN parameter, is connected to the 
	specified IVR port. The party(s) from the base call is (are) 
	placed on hold.
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. The function does not 
	work for the offsite-switch option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides. 
	\param psDstDN Specifies the directory number of the party 
	to be dialed.

    \return If the function succeeds, and if you 
	passed in a first argument of ilRQ_ANY, it returns a valid ilRQ 
	value you should capture for use with the ilGetReply() function. 
	If you passed in your own ilRQ value, it will return that value. 
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallConsultInit(ilRQ RqID,ilPORT Port,CPSTR psDstDN);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallConsultComplete() function releases a previously 
	initiated consult call. 
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. The function does not 
	work for the offsite-switch option (IVR-In-Front).

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if 
	you passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the ilGetReply()
	function. If you passed in your own ilRQ value, it will return 
	that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallConsultComplete(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallConsultConference() function merges the 
	original and consultation call that resides on this port 
	into conference. The new party from the consultation call is 
	added to the base call. 
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. The function does not 
	work for the offsite-switch option (IVR-In-Front).
	
	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.

    \return If the ilSRqCallConsultConference() function succeeds, and 
	if you passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function. If you passed in your own ilRQ value, 
	it will return that value. If it fails, the function returns 
	ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallConsultConference(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCallConsultTransfer() function completes a call 
	transfer. This causes the IVR port specified by the port 
	parameter to be released from both the original and consultation 
	call. Parties participating in the original and consultation 
	call that reside on this port are merged into one call. 
	Check the specific T-Server/Switch configuration to verify 
	that this functionality is supported. The function does not 
	work for the offsite-switch option (IVR-In-Front). 

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.

    \return If the ilSRqCallConsultTransfer() function succeeds, and 
	if you passed in a first argument of ilRQ_ANY, it returns 
	a valid ilRQ value you should capture for use with the 
	ilGetReply() function. If you passed in your own ilRQ value, 
	it will return that value. If it fails, the function returns 
	ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCallConsultTransfer(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataAddList() function adds the list of key-value 
	to the call residing on the IVR channel specified by Port.

	The IVR API supports functions that use a pointer to an 
	ordered list structure. This structure allows sending and 
	receiving a number of parameters presented as a single string. 

	An ordered list structure is a string composed of several 
	fields in strict order. Each field is separated from another 
	field by a delimiter character. The order of fields in this 
	structure is very important and must be specified. If a field 
	has a zero value (string is empty) - this string still retains 
	its place in the list.

	The first character in an ordered list is a delimiter. Each
	parameter begins after this delimiter and continues until 
	another delimiter is found or the string is terminated, 
	and so on.

	The following are examples of an ordered list.
	<UL>
	<LI>A list of key-value pairs:
 <PRE>;name;John Doe;address;301 Main Street;city;San Francisco;phone;415-555-1212</PRE>
	<BR>In this example, ";" is a delimiter, and the string 
	constants are four key-value pairs:
	<BR>name	== John Doe
	<BR>address 	== 301 Main Street
	<BR>city	== San Francisco
	<BR>phone	== 415-555-1212
	</LI>
	<LI>An ordered list of key-value pairs.
<PRE>#name##city</PRE>
	In this example the first key is name with the value being 
	null. The second key is city with the value also null.</LI>
	</UL>

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.
	\param psList Points to an ordered list of keys and values. 
    
	\return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataAddList(ilRQ RqID,ilPORT Port,CPSTR psList);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataGetKD() function requests the value of the 
	key-value pair specified by the key.
	Note: Calls to get the key-value data (ilSRqUDataGetKD) require the
	<B>exact</B> (including case) key string that was used when
	the key-value was attached.
	Use the GetReply() function to get the value in your 
	driver's reply buffer. 

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psKey The key for the key-data pair. 

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value   you should capture for use with the 
	ilGetReply() function. If you   passed in your own ilRQ 
	value, it will return that value. If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataGetKD(ilRQ RqID,ilPORT Port,CPSTR psKey);

#if defined WIN32 || defined WIN64
	__declspec(dllexport)
#endif
/*! The ilSRqUDataGetList() function requests the values of the
	list of keys specified by the psKkey.
	Note: Calls to (ilSRqUDataGetList) require the
	<B>exact</B> (including case) key value strings that was used when
	the key-value was attached.
	Use the GetReply() function to get the value in your 
	driver's reply buffer. 

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psKey The list of keys for the key list.
	              The first character is used for the list delimiter.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid
	ilRQ value   you should capture for use with the
	ilGetReply() function. If you   passed in your own ilRQ
	value, it will return that value. If it fails, the
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataGetList(ilRQ RqID,ilPORT Port,CPSTR psKey);


#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataGetAll() function requests the value of all the 
	key-value pairs for this call.
	Use the GetReply() function to get the value in your 
	driver's reply buffer. 

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value   you should capture for use with the 
	ilGetReply() function. If you   passed in your own ilRQ 
	value, it will return that value. If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataGetAll(ilRQ RqID,ilPORT Port );

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataDelKD() function requests deletion of the 
	key-value pair specified by the key.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.
	\param psKey The key of the key-value pair that is to be deleted. 
	The entire KVPair, including key and value, will be 
	deleted from the call data.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataDelKD(ilRQ RqID,ilPORT Port,CPSTR psKey);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqUDataDelAll() function requests deletion of all 
	user data attached to the call associated with the specified 
	IVR port.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqUDataDelAll(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqGetCallInfo() function requests information about 
	a type of call data, such as a connection ID, DN, and such. 
	Use the ilGetReply() function to get the information in 
	your driver's reply buffer.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.
	\param eCI is a type of data about the call on the Port for 
	which you want information. See the ilCI_TYPE enum.
	<br>ilCI_ALL
	<br>ilCI_CONN_ID 
	<br>ilCI_CALL_ID 
	<br>ilCI_DN 
	<br>ilCI_ANI 
	<br>ilCI_DNIS 
	<br>ilCI_THIS_DN 
	<br>ilCI_THIS_TRUNK 
	<br>ilCI_THIS_QUEUE 
	<br>ilCI_OTHER_DN 
	<br>ilCI_OTHER_TRUNK 
	<br>ilCI_OTHER_QUEUE 
	<br>ilCI_LAST_EVENT_NAME 
	<br>ilCI_FIRST_HOME_LOC
	<br>ilCI_CALL_UUID 

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will return 
	that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqGetCallInfo(ilRQ RqID,ilPORT Port,ilCI_TYPE eCI);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCDT_Init() function requests an access number or 
	tag to make a Call Data Transfer (CDT) to a remote destination. 
	Use the ilGetReply() function to get access number information, 
	an access number (indirect type), or tag value (depending on 
	the type of CDT) in your driver's reply buffer. The reply 
	buffer will store information needed to transfer the call, 
	including any routing digits (9 for outside line, 415 for CA, 
	and so forth). 
	After receiving the reply for this request, the user of the
	library is responsible for making the transfer of the call
	through PSTN.

	CDT requests can be:
	<UL>
	<LI>Indirect&#151The call is directed to an intermediate 
	routing	point to reach a destination DN. The remote T-Server 
	redirects the call from the routing point to the destination 
	DN.</LI>
	<LI>DirectNT&#151The call is transferred directly to the 
	destination DN. No tag is required.</LI>
	<LI>DirectTO&#151The call is transferred directly to the 
	destination DN. The tag has to be provided by a layer that 
	is outside of CDT, for example, the driver. (TO stands for 
	tag outside).</LI>
	<LI>DirectTI&#151The call is transferred directly to the 
	destination DN. The tag is generated inside the CDT layer 
	(TI means tag inside). To use this function you do not have 
	to generate this tag. In other words, The tags do not need 
	to be known if inside. The Tservers participating in the 
	transfer will use the tag to correlate the data transferred. </LI>
	<LI>Default&#151The call is handled according to whichever of the
	previous methods is configured as the default method. </LI>
	</UL>

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any contention 
	possibilities. 
	\param Port The IVR port where the call resides.
	\param psDstDN specifies the directory number of the party to be 
	dialed.
	\param psService Specifies the name of the remote T-Server that 
	receives a call and attaches data to it.
	\param psCDT_Type Specifies the type of Call Data Transfer request.
	\param psCDT_Tag Specifies the tag used to mark a call. This tag 
	should be transferred with the call through PSTN. The tag 
	is used by the remote T-Server to match an arriving call 
	(from PSTN) with a CDT request provided by the 
	ilSRqCDT_Init() function.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCDT_Init(ilRQ RqID,ilPORT Port,CPSTR psDstDN,CPSTR psService,CPSTR psCDT_Type,CPSTR psCDT_Tag);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqCDT_Cancel() function cancels a previous 
	ilSRqCDT_Init() function request.

	\param RqID In general you should pass in ilRQ_ANY. If you 
	pass in your own ilRQ value, your code must manage any 
	contention possibilities. 
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function. If you passed in your own ilRQ 
	value, it will return that value. If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqCDT_Cancel(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilSRqVersion() function requests the version number of 
	the IVR Library or of a named service.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psService A pointer to the name of the service. If the 
	value is NULL or " ", the IVR library version is returned.
	If the value is the name of the IVR resource, the driver version is returned.
	For any other value, the I Server version is returned.

   \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a valid 
	ilRQ value you should capture for use with the ilGetReply() 
	function. If you passed in your own ilRQ value, it will 
	return that value. If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqVersion(ilRQ RqID,ilPORT Port,CPSTR psService);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetVersion() function lets you retrieve the version 
	of the IVR Library.

    \return The ilGetVersion() function returns a pointer to a constant 
	string representation of the version of the IVR Library.
*/
CPSTR ilGetVersion(void);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqToLog() function inserts an information string 
	into log file of psService.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psService Specifies the name of the service that 
	has the log stream to be used.
	\param psStr A string that is the information to be placed 
	into the log stream.
	 
	\return If the ilSRqToLog() function succeeds, and if you passed 
	in a first argument of ilRQ_ANY, it returns a valid ilRQ 
	value you should capture for use with the ilGetReply() function.
	If you passed in your own ilRQ value, it will return that value.
	If it fails, the function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqToLog(ilRQ RqID,ilPORT Port,CPSTR psService,CPSTR psStr);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSetLogHeader() is a no-op and should not be called.

	\param pszLogHeader the header information for the log file.
    \return The ilSetLogHeader() function returns TRUE and FALSE if
	it fails.
*/
BOOL ilSetLogHeader(CPSTR pszLogHeader);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilSetTimeout() function sets the timeout for next calls 
	of the ilGetReply() and ilGetRequest() functions.

	\param ulMSec specifies the Timeout value in milliseconds.
	\return The ilSetTimeout() function returns TRUE if it succeeds, 
	and FALSE if it fails.
*/
BOOL ilSetTimeout(ULONG ulMSec);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetTimeout() function gets the current value of 
	timeout set by the previous ilSetTimeout call.
	
	\return The function returns the Timeout value 
	in milliseconds.
*/
ULONG ilGetTimeout(void);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetLastError() returns the latest error code from the library

   	\param eErrType A type that is either an ilET_NUMBER or ilET_TEXT. 
	If you pass in ilET_TEXT, you direct the library to print the 
	error to the log. If you pass in ilET_NUMBER, the return value 
	matches ilNum, which is associated with an error description.
   	Here is the declaration of the error descriptions.
       	<BR>ilRET_OK,                 "OK"
       	<BR>ilRET_ERROR,              "Error"
       	<BR>ilRET_LIB_NOTREADY,       "ILibrary not initialized properly"
       	<BR>ilRET_CONN_CLOSED,        "Connection to IVR Server is closed"
       	<BR>ilRET_BAD_ARGS,           "One or more aguments is invalid"
		<BR>ilRET_FUNC_UNSUPPORTED,   "Name of object is invalid"
       	<BR>ilRET_TIMEOUT,            "Request still in progress"
       	<BR>ilRET_REQ_EXPIRED,        "Request expired"
       	<BR>ilRET_NO_REQUESTS,        "No request in the queue"
	    <BR>ilRET_BAD_CONN_NAME,	  "Connection name is invalid"
       	<BR>ilRET_REQ_FAILURE,        "Request failed"
	    <BR>
   \return If the input arg is ilET_NUMBER, the return value matches the 
	error number for the error text.
*/
long  ilGetLastError(ilERR_TYPE eErrType);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetLastPortError() returns the latest error code for the named port

   	\param ilPort The IVR port where the call resides
   	\param eErrType A type that is either an ilET_NUMBER or ilET_TEXT. 
	If you pass in ilET_TEXT, you direct the library to print the 
	error to the log. If you pass in ilET_NUMBER, the return value 
	matches ilNum, which is associated with an error description.
   	Here is the declaration of the error descriptions.
       	<BR>ilRET_OK,                 "OK"
       	<BR>ilRET_ERROR,              "Error"
       	<BR>ilRET_LIB_NOTREADY,       "ILibrary not initialized properly"
       	<BR>ilRET_CONN_CLOSED,        "Connection to IVR Server is closed"
       	<BR>ilRET_BAD_ARGS,           "One or more aguments is invalid"
		<BR>ilRET_FUNC_UNSUPPORTED,   "Name of object is invalid"
       	<BR>ilRET_TIMEOUT,            "Request still in progress"
       	<BR>ilRET_REQ_EXPIRED,        "Request expired"
       	<BR>ilRET_NO_REQUESTS,        "No request in the queue"
	    <BR>ilRET_BAD_CONN_NAME,	  "Connection name is invalid"
       	<BR>ilRET_REQ_FAILURE,        "Request failed"
	    <BR>
   \return If the input arg is ilET_NUMBER, the return value matches the 
	error number for the error text.
*/
long  ilGetLastPortError(ilPORT ilPort, ilERR_TYPE eErrType);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilLocalPrn() function prints text directly to local log.

	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psFmt is a format string that follows the rules for the 
	printf() function.  Subsequent arguments match the format 
	specifiers in the format string.

    \return The function returns ilRET_OK (or some 
	positive value) if it succeeds. If it fails it returns 
	an ilRET error value
*/
ilRET ilLocalPrn(ilRQ RqID,ilPORT Port,CPSTR psFmt, ...);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilLocalPrnSelector() function prints text directly to local log.
	based upon a selection variable.

	\param selector A string of zeroes and ones which indicate when
	the message should be printed. The string is compared to a string
	specified in CME which indicates which messages the user wants
	to be printed. If the selector string has a one in a position
	which matches a one in a position in the configured value in CME,
	the message will be printed.
	\param RqID In general you should pass in ilRQ_ANY. If you pass in 
	your own ilRQ value, your code must manage any contention 
	possibilities.
	\param Port The IVR port where the call resides.
	\param psFmt is a format string that follows the rules for the 
	printf() function.  Subsequent arguments match the format 
	specifiers in the format string.

    \return The function returns ilRET_OK (or some 
	positive value) if it succeeds. If it fails it returns 
	an ilRET error value
*/
ilRET ilLocalPrnSelector(char* selector, ilRQ RqID,ilPORT Port,CPSTR psFmt, ...);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilGetCallStatus() function returns the current call status on
	the port.

	\param Port The port to provide call status for.

    \return The function returns the call status of the port using the 
	CallStatus enum.
*/
int ilGetCallStatus(ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilPrnError() function prints an error message 
	associated with an error code received from the 
	ilGetLastError() function.

	You should call the ilGetLastError() function first, 
	take an appropriate action (according to the returned 
	error code), and then either use the ilPrnError() 
	function to print an error message or call the 
	ilSRqToLog() function.

	\param lErrCode Matches an error code returned from the 
	ilGetLastError() function.

    \return The function returns a pointer to a 
	constant string error message.
*/
CPSTR ilPrnError(long lErrCode);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqRouteDone() function is for compatibility with
	previous versions of this library, it is no longer supported.

	\param RqID In general you should pass in ilRQ_ANY. If you 
	pass in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If you passed in your own ilRQ 
	value, it will return that value.  If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqRouteDone(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*!	The ilSRqRouteStart() function indicates to URS that 
	a route sequence for psRP has started.  The IVR Server 
	will return treatments to the application via the 
	ilGetRequest() function. For each treatment, the IVR 
	application (this driver if this is an interface for
	an application) should apply and respond with the 
	results via the ilSendReply() function.
	The ilGetRequest() function will fail upon strategy 
	termination.  Call the ilSRqRouteAbort() function to 
	end the current strategy.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.
	\param psRP Specifies the route point where in the router a 
	valid strategy is loaded, with only a next service 
	(that is, target) within that strategy.

    \return If the function succeeds, and if you 
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If you passed in your own ilRQ 
	value, it will return that value. If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqRouteStart(ilRQ RqID,ilPORT Port,CPSTR psRP);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! 	The ilGetRequest() function checks for a request 
	(treatment) from URS.  It accepts (for a specified 
	port) URS treatment through an IVR server. 

	\param Port The IVR port where the call resides.
	\param psReq Points to a string buffer that stores the text 
	of the treatment information.
	\param iReqLen Specifies the size of the psReq buffer in bytes.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilSendReply() function. If you passed in your own 
	ilRQ value, it will return that value. If it fails, 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilGetRequest(ilPORT Port,PSTR psReq,int iReqLen);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSendReply() function sends a reply for an earlier 
	received treatment request (URS via Iserver). 

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param bResult if TRUE prints an OK message to the log; if FALSE, 
	prints an ERROR message to the log.
	\param psReply is a pointer to the message to be sent as a reply, 
	the actual contents of the reply buffer depend upon the 
	treatment request, for example, if CollectDigits was the 
	treatment request, then a valid string of numbers would 
	be returned.

    \return The function returns ilRET_OK (or some 
	positive value) if it succeeds. If it fails it returns:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
	<br>- ilRET_BAD_ARGS, one of the input parms was invalid, look in the log for more information
*/
ilRET ilSendReply(ilRQ RqID,BOOL bResult,CPSTR psReply);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqRouteAbort() function indicates to the Universal 
	Routing Server (URS) that the route sequence associated 
	with RqID has been aborted.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If you passed in your own ilRQ 
	value, it will return that value.  If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqRouteAbort(ilRQ RqID,ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqStatPeek() function requests statistics from 
	StatServer.  The statistics, CurrNumberWaitingCalls and 
	ExpectedWaitTime, are supported.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port where the call resides.
	psSS_Name specifies the name of a StatServer as defined 
	in the Configuration Server.
	\param psStatName Specifies the name of the statistic.

    \return If the function succeeds, and if you
	passed in a first argument of ilRQ_ANY, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If you passed in your own ilRQ 
	value, it will return that value.  If it fails, the 
	function returns ilRQ_ERR, an error has occurred, look in the log for the reason
*/
ilRQ  ilSRqStatPeek(ilRQ RqID,ilPORT Port,CPSTR psStatName);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilSRqStatGet() function is provided for compatibility with 
	previous versions of this library.  It is no longer supported 
	in 7.0. ilSRqStatPeek should be used instead.

*/
ilRQ  ilSRqStatGet(ilRQ RqID,ilPORT Port,CPSTR psSS_Name,CPSTR psObjectType,CPSTR psObjectID,CPSTR psStatType);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilDeinitiate() function is available only for compatibility
	with previous versions of this library.  It is not intended
	that the IVR Library be de-initiated in order to be initiated
	again.  It is recommended that it not be called.

    \return The function always returns TRUE.
*/
BOOL  ilDeinitiate(void);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilDialOutError() function indicates an outbound call 
	could not be made.  

	\param Port The IVR port that made the call.
	\param _ilDialOutErr The type of error.
	
    \return The function returns ilRET_OK (or some 
	positive value) if it succeeds. If it fails it returns:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
	<br>- ilRQ_ERR, an error has occurred, look in the log for the reason

	This is part of OutBound Calling support and is available only through Genesys Professional Services
*/
ilRET	ilDialOutError(ilPORT Port, ilDIALOUTERR _ilDialOutErr);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	

/*! The ilSRqGetDialOutData() function requests outbound call data from 
	IServer.  

    \return If the function succeeds, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If it fails, the 
	function returns:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
    <br>- ilRET_NO_REQUESTS, there is no request for an outbound call to be made
	<br>- ilRQ_ERR, an error has occurred, look in the log for the reason

	This is part of OutBound Calling support and is available only through Genesys Professional Services

*/
ilRQ  ilSRqGetDialOutData();

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	

/*! The ilSRqDialOutDataInit() function updates status on the outbound call
	indicating that the application/driver has made the call.

	\param RqID In general you should pass in ilRQ_ANY. If you pass 
	in your own ilRQ value, your code must manage any 
	contention possibilities.
	\param Port The IVR port that made the call.
	\param RqID_DialOut The request id of the dial out data
    \return If the function succeeds, it returns a 
	valid ilRQ value you should capture for use with the 
	ilGetReply() function.  If it fails, the 
	function returns:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
	<br>- ilRQ_ERR, an error has occurred, look in the log for the reason

	This is part of OutBound Calling support and is available only through Genesys Professional Services

*/
ilRQ  ilSRqDialOutDataInit(ilRQ RqID, ilPORT Port, ilRQ RqID_DialOut);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	

/*! The ilCallEstablished() function indicates that an outbound
	call has been established.

	\param Port The IVR port that made the call.
    \return The function returns ilRET_OK if a CallStatus(Established) msg
	was successfully sent to IServer. If it fails it returns:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
	<br>- ilRQ_ERR, an error has occurred, look in the log for the reason

  	This is part of OutBound Calling support and is available only through Genesys Professional Services

*/
ilRET  ilCallEstablished(ilPORT Port);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilFailure() function indicates that an outbound
	call could not be made

	\param Port The IVR port that made the call.
	\param _ilFailure The type of error.
	
    \return The function returns ilRET_OK (or some 
	positive value) if it succeeds. If it fails it returns 
	an ilRET error value:
	<br>- ilRET_LIB_NOTREADY, the ILibrary has not been initialized
	<br>- ilRET_CONN_CLOSED, there is no connection to IServer available.
	<br>- ilRQ_ERR, an error has occurred, look in the log for the reason

   	This is part of OutBound Calling support and is available only through Genesys Professional Services

*/
ilRET ilFailure(ilPORT Port, ilFAILURE _ilFailure);

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetProcessingState() function returns the current state of I-Library

    \return The function returns an ilRET value
	<br>- ilRET_ACTIVE					no shutdown received, normal processing
	<br>- ilRET_SHUTDOWN_IN_PROCESS,	shutdown request received, logging agents out
	<br>- ilRET_ALL_AGENTS_LOGGEDOUT,	all agents logged out, monitoring calls
	<br>- ilRET_NO_CALLS_IN_PROGRESS,	no active calls

*/
ilRET ilGetProcessingState();

#if defined WIN32 || defined WIN64
	__declspec(dllexport) 
#endif	
/*! The ilGetParmValue() function returns the value for the key. The key must
	be configured in a section in the IVRDriver application which is created by
	the user and cannot be named "ivr_server_interface", "log_content", 
	"security", or "log".
	\param _key is the key to return a value for.
	\param _iConvert determines whether to convert a boolean value.
	if _iConvert is 1 and the value for the key is a boolean, the value will be 
	converted to "true" or "false". 
	A boolean is true if it's value is 1, on, yes, or true. 
*/
char* ilGetParmValue(char* _key, int _iConvert);

/*
	Complete the C declarations for C++
*/
#ifdef __cplusplus
	}
#endif

#endif /* __ILIBRARY_INTERFACE_H */

