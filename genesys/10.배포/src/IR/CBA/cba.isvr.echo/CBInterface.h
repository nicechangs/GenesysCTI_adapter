#pragma once

#define MAX_CALLID_SIZE                         41
#define MAX_SERVICE_SIZE                        16
#define MAX_COMMANDTYPE_SIZE                    5
#define MAX_CHOSTID_SIZE                        10
#define MAX_CHANSEQ_SIZE                        5
#define MAX_COMMAND_SIZE                        16
#define MAX_UKEY_SIZE                           24
#define MAX_TIMEOUT                             4
#define MAX_CTISEND_BUFFER                      4096

#define SOCK_BUFFER_SIZE                        8192
#define PACKET_STX                              0x02
#define PACKET_ETX                              0x03

// link 상태
#define LINK_DISCONNECTED                       0                                // 끊어짐
#define LINK_CONNECTED                          1                                // 연결됨

#define PACKET_KEY_I_CALL_EX                    "I_CALL_EX_"                     // 확장 콜 정보
#define PACKET_KEY_I_MAKECALL                   "I_MAKECALL"                     // CTMP MakeCall 요청
#define PACKET_KEY_I_CLRCALL_                   "I_CLRCALL_"                     // CTMP MakeCall 요청콜 종료 

#define PACKET_KEY_I_CALLSTAT                   "I_UPDATE_UEI"                   // Call 상태체크
#define PACKET_KEY_I_WAITINFO                   "I_WAITINFO"                     // Queue Wait정보 얻기
#define PACKET_KEY_I_SSTRANS_                   "I_SS_TRANS"                     // SingleStepTransfer 요청

// XCIP 헤더 길이
#define XCIP_LEN_SIZE                           5
#define XCIP_FROM_SIZE                          8
#define XCIP_TO_SIZE                            8
#define XCIP_KEY_SIZE                           9

// cb의 XCCP의 sCommand
#define CB_CMDTYPE_REGISTER_REQ                 10000                            // register 요청             (ADAPTER <-- CB)
#define CB_CMDTYPE_REGISTER_SUCCESS             10001                            // register 성공 응답        (ADAPTER --> CB)
#define CB_CMDTYPE_REGISTER_FAIL                10002                            // register 실패 응답        (ADAPTER --> CB)
#define CB_CMDTYPE_STATUS_QUERY                 11100                            // adapter 상태 요청         (ADAPTER <-- CB)
#define CB_CMDTYPE_STATUS_OK                    11101                            // adapter 상태 정상 응답    (ADAPTER --> CB)
#define CB_CMDTYPE_STATUS_NOK                   11102                            // adapter 상태 비정상 응답  (ADAPTER --> CB)
#define CB_CMDTYPE_STATUS_NOTIFY                11103                            // adapter 상태 통보         (ADAPTER --> CB)
#define CB_CMDTYPE_LINK_QUERY                   11200                            // link 상태 요청            (ADAPTER <-- CB)
#define CB_CMDTYPE_LINK_OK                      11201                            // link 상태 정상 응답       (ADAPTER --> CB)
#define CB_CMDTYPE_LINK_NOK                     11202                            // link 상태 비정상 응답     (ADAPTER --> CB)
#define CB_CMDTYPE_LINK_NOTIFY                  11203                            // link 상태 통보            (ADAPTER --> CB)
#define CB_CMDTYPE_DATA_REQ                     12000                            // data 처리 요청            (ADAPTER <-- CB)
#define CB_CMDTYPE_DATA_SUCCESS                 12001                            // data 처리 성공 응답       (ADAPTER --> CB)
#define CB_CMDTYPE_DATA_FAIL                    12002                            // data 처리 실패 응답       (ADAPTER --> CB)

// cb의 XCCP error
#define CB_ERRCODE_NOERROR                      "00000"                          // 성공
#define CB_ERRDESC_NOERROR                      "NOERROR"                        // 성공
#define CB_ERRCODE_SERVER_CONNECTION_FAILED     "63001"                          // server 연결 실패
#define CB_ERRDESC_SERVER_CONNECTION_FAILED     "SERVER_CONNECTION_FAILED"       // server 연결 실패
#define CB_ERRCODE_SERVER_CONNECTION_DOWNED     "63002"                          // server 연결 끊어짐
#define CB_ERRDESC_SERVER_CONNECTION_DOWNED     "SERVER_CONNECTION_DOWNED"       // server 연결 끊어짐
#define CB_ERRCODE_SERVER_INVALID_XCCP_COMMAND  "63003"                          // 정의되지 않은 xccp command
#define CB_ERRDESC_SERVER_INVALID_XCCP_COMMAND  "SERVER_INVALID_XCCP_COMMAND"    // 정의되지 않은 xccp command
#define CB_ERRCODE_PKT_INVALID_DATASIZE         "64102"                          // 잘못된 data 크기
#define CB_ERRDESC_PKT_INVALID_DATASIZE         "PKT_INVALID_DATASIZE"           // 잘못된 data 크기
#define CB_ERRCODE_PKT_INVALID_DATA             "64103"                          // 잘못된 data
#define CB_ERRDESC_PKT_INVALID_DATA             "PKT_INVALID_DATA"               // 잘못된 data
#define CB_ERRCODE_PKT_HOST_REQUEST_FAILED      "64105"                          // host로 요청 실패
#define CB_ERRDESC_PKT_HOST_REQUEST_FAILED      "PKT_HOST_REQUEST_FAILED"        // host로 요청 실패
#define CB_ERRCODE_REQUEST_PUSH_QUEUE           "64106"                          // 요청 data를 queue에 넣기 실패
#define CB_ERRDESC_REQUEST_PUSH_QUEUE           "REQUEST_PUSH_QUEUE"             // 요청 data를 queue에 넣기 실패
#define CB_ERRCODE_TIMEOUT_REQUEST              "64107"                          // 요청 data timeout
#define CB_ERRDESC_TIMEOUT_REQUEST              "TIMEOUT_REQUEST"                // 요청 data timeout

typedef enum
{
    XCCP_METHOD_REQ_REGISTER                = 10000,
    XCCP_METHOD_REGISTER_SUCC,
    XCCP_METHOD_REGISTER_FAIL,
    XCCP_METHOD_REQ_UNREG                   = 10100,
    XCCP_METHOD_UNREG_SUCC,
    XCCP_METHOD_UNREG_FAIL,
    XCCP_METHOD_QUERY_ADAPTOR_LIST          = 11000,
    XCCP_METHOD_QUERY_ADAPTOR_LIST_SUCC,
    XCCP_METHOD_QUERY_ADAPTOR_LIST_FAIL,
    XCCP_METHOD_ADAPTOR_NOTI_STATUS_QUERY   = 11100,
    XCCP_METHOD_ADAPTOR_STATUS_OK,
    XCCP_METHOD_ADAPTOR_STATUS_NOK,
    XCCP_METHOD_ADAPTOR_STATUS_NOTIFY,
    XCCP_METHOD_ADAPTOR_LINK_STATUS_QUERY   = 11200,
    XCCP_METHOD_ADAPTOR_LINK_STATUS_OK,
    XCCP_METHOD_ADAPTOR_LINK_STATUS_FAIL,
    XCCP_METHOD_ADAPTOR_LINK_STATUS_NOTIFY,
    XCCP_METHOD_USER_REQ                    = 12000,
    XCCP_METHOD_USER_REQ_SUCC,
    XCCP_METHOD_USER_REQ_FAIL,
    
}eXCCP_METHOD;

typedef enum
{
    XCCP_ERR_UNKNOWN_SERVCE             = 50001,
    XCCP_ERR_SERVICE_NOT_AVAILABLE,
    XCCP_ERR_HEARTBEAT_FAILED,
    XCCP_ERR_USERDATA_NOT_DEFINED,
    XCCP_ERR_ERRORDATA_NOT_DEFINED,
}eXCCP_ERROR;

typedef struct PKT_XCIP_HEADER 
{
    PKT_XCIP_HEADER()   {;}
    ~PKT_XCIP_HEADER()  {;}
    char            stx;
    char            sLen[XCIP_LEN_SIZE];    //전문 전체 길이
    char            sFrom[XCIP_FROM_SIZE];  //ProcGroup(2) + SubGroup(2) + UserDef(4)
    char            sTo[XCIP_TO_SIZE];      //ProcGroup(2) + SubGroup(2) + UserDef(4)
    char            sKey[XCIP_KEY_SIZE];    //Unique Key
    char            cMsgFlag;               //'1':Message End, '0': Message Continue(Not Support), 'H' : Heartbeat
} pktXCIPHeader;

typedef struct PKT_XCCP_HEADER 
{
    PKT_XCCP_HEADER()   {;}
    ~PKT_XCCP_HEADER()  {;}

    pktXCIPHeader   header;
    char            sCallID[MAX_CALLID_SIZE];               //Call ID
    char            sServiceType[MAX_SERVICE_SIZE];         //Service
    char            sCommandType[MAX_COMMANDTYPE_SIZE];     //Request Type
    char            sCHostId[MAX_CHOSTID_SIZE];             //Call Host ID
    char            sChanSeq[MAX_CHANSEQ_SIZE];             //Channel Sequence
    char            sCommand[MAX_COMMAND_SIZE];             //Request Command
    char            sUniqueKey[MAX_UKEY_SIZE];              //Unique Key
    char            sTimeout[MAX_TIMEOUT];                  //Timeout(초)
} pktXCCPHeader;
