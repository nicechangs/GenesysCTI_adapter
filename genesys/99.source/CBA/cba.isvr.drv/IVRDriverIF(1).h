#pragma once

#define SOCK_BUFFER_SIZE               (8129)

#define GLI_HEADER_LENGTH              6
#define GLI_VERSION                    0x02
#define GLI_DEFAULT_APP                0x00


typedef struct _GLI_HEADER
{
    char cNull;

    // 0: Alive Req, 1:Alive Ack, 2:Error Ack, 3:Request xml,Response xml
    char cMsgType;                      
    
    unsigned short htons_len;
    char cGLI_VERSION;
    char cGLI_DEFAULT_APP;
}GLI_HEADER;

typedef enum
{
    GLI_MSGTYPE_KEEP_ALIVE_REQ = 0x0,
    GLI_MSGTYPE_KEEP_ALIVE_ACK = 0x1,
    GLI_MSGTYPE_ERROR_ACK      = 0x2,
    GLI_MSGTYPE_XML_DATA       = 0x3,
}GLI_MSGTYPE;

typedef enum
{
    // common
    ilRET_NONE                          = 0,
    ilRET_SUCCESS                       = 1,
    ilRET_UNKNOWN                       = 99999,

    // log part
    ilRET_INVALID_LOG_PATH              = 10001,
    ilRET_INVALID_LOG_LINK_PATH         = 10002,
    ilRET_INVALID_LOG_PREFIX            = 10003,
    ilRET_INVALID_LOG_LEVEL             = 10004,

    // driver part
    ilRET_NOT_INITIALIZED               = 20001,
    ilRET_INVALID_REMOTE_IP             = 20004,
    ilRET_INVALID_REMOTE_PORT           = 20005,
    ilRET_INVALID_VERSION               = 20007,
    ilRET_FAIL_CONBINE_GLI_HEADER       = 20008,
    ilRET_NOT_CONNECTED_REMOTE          = 20009,
    ilRET_FAIL_SOCKET_OPEN              = 20010,
    ilRET_FAIL_SOCKET_SEND_DATA         = 20011,
    ilRET_FAIL_SOCKET_RECEIVE_DATA      = 20012,

    ilRET_FAIL_MAKE_XML_LOGINREQ        = 30001,
    ilRET_FAIL_MAKE_XML_NEWCALL         = 30002,
    ilRET_FAIL_MAKE_XML_ROUTEREQUEST    = 30003,
    ilRET_FAIL_MAKE_XML_ONESTEPXFER     = 30004,
    ilRET_FAIL_MAKE_XML_CALLINFOREQ     = 30005,
    ilRET_FAIL_MAKE_XML_ENDCALL         = 30006,
    ilRET_FAIL_MAKE_XML_UDATASET        = 30007,
    ilRET_FAIL_MAKE_XML_UDATAGET        = 30008,
}ilRET;

typedef enum 
{
    eCallStatusUnknown                  = 10001, // The call state is unknown. This usually indicatesthere is no call on the port.
    eCallStatusDialingMakeCall          = 10002, // A make call is in progress, but it has not yet been answered.
    eCallStatusBusyMakeCall             = 10003, // A make call was made to a busy number.
    eCallStatusRinging                  = 10004, // A call has been made, and the called party’s phone is ringing.
    eCallStatusHeld                     = 10005, // A call is on hold.
    eCallStatusBusy                     = 10006, // A call was made to a busy number.
    eCallStatusDialing                  = 10007, // A call is being dialed.
    eCallStatusEstablished              = 10008, // A call has been connected to the called party.
    eCallStatusRetrieved                = 10009, // A previous consult call has been retrieved. 
    eCallStatusConfPartyDel             = 10010, // A party that was brought into a conference call has been deleted from the call.
    eCallStatusConfPartyAdd             = 10011, // A party has been added to the call.
    eCallStatusXferComplete             = 10012, // A call has been transferred.
    eCallStatusReleased                 = 10013, // A call has ended. 
    eCallStatusNoChange                 = 10014, // Internal state used by I-Library. This would never be returned to the application.
}ilCallStatus;





