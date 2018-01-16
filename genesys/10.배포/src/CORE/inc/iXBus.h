#ifndef _BTXBUS_H
#define _BTXBUS_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iType.h>

#define MAX_XBUS_DATA		(4096)		// 
#define MAXWAIT_SHMTRUST	(5)			// 5초동안 XBus HBit이 실행되지 않는다면
										// XBus SHM 메모리를 확신할 수 없음

enum
{
	MAX_MODLID			= 0x03fd,
	SELF   				= 0x03fe,
	BROAD  				= 0x03ff
};

// xbus_t return value
#define SUCCESS				(0x00)
#define MSG_TOO_BIG			(0x01)
#define MSG_TOO_SHORT		(0x02)
#define DEST_UNDEFINED		(0x03)
#define SOURCE_UNDEFINED	(0x04)
#define DEST_PROCESS_DOWN	(0x05)
#define SOCKET_WRITE_FAIL	(0x06)
#define SOCKET_READ_FAIL	(0x07)
#define PARAMETER_ERROR		(0x08)
#define SHM_ERROR			(0x09)
#define MAKE_SOCKET_ERROR	(0x0A)
#define CONFIG_ERROR		(0x0B)
#define STATE_MISMATCH		(0x0C)
#define UNDEFINED_ERROR		(0xFF)

#define	XB_HEAD_LEN			(sizeof(XB_HEAD))
#define	XBUS_MSG_HDR 		XB_HEAD
#define XBUS_MSG_HDR_LEN	XB_HEAD_LEN

// for Old Type Structure
typedef struct
{
	char			version[8];
	uint16			Length;
	uint8 		 	Bustype;
	uint8 		 	nRsvd1;
	uint16 			From;
	uint16 			To;
	uint16			MsgId;
	uint16			nRsvd2;
} XB_HEAD;

typedef struct
{
	XB_HEAD 	hdr;
	char		Data[MAX_XBUS_DATA];
} XBUS_MSG;

// Call Back ID
enum
{
	HA_UnAssign				= (1),
	HA_Active				= (2),
	HA_Standby				= (3),
	HA_AddStandby			= (4),
	HA_RemoveStandby		= (5),
	HA_END					= (6),
};

enum
{
	MD_TYPE_REAL			= (0),
	MD_TYPE_VIRTUAL			= (1),
};

enum
{
	HB_ROLE_SLAVE			= (0),
	HB_ROLE_MASTER			= (1),
};

enum
{
	HA_ROLE_NONE			= (0),
	HA_ROLE_DEPENDENCY		= (1),
	HA_ROLE_MASTER			= (2),
};

enum
{
	HB_STATE_DOWN			= (0),
	HB_STATE_ALIVE			= (1),
};

// Add for xb_get_ha_info  2005/05/02
// XBusGetHaStMine/XBusGetHaStPeer 2013/10/21
enum
{
	HA_STATE_NULL			= (0),		// Unused
	HA_STATE_UNASSIGN		= (1),		// Master + Dependency
	HA_STATE_PRE_ACTIVE		= (2),		// Master only
	HA_STATE_ACTIVE			= (3),		// Master + Dependency
	HA_STATE_PRE_STANDBY	= (4),		// Master only
	HA_STATE_STANDBY		= (5),		// Master + Dependency
};

typedef int32 	(*CALLBACK)(int32, uchar*);
typedef void 	(*xbMoSt_t)(uint16, int32);
typedef void 	(*xbHaCB_t)(int32);
typedef void	(*xbMoCB_t)(char*);
typedef int32	xbus_t;

/*****************************************************************************
 * XBUS New Interface Methods
 *****************************************************************************/
xbus_t XBusInit(uint16 idMine, NPCSTR szCfg, NPCSTR szIpHb0 = NULL, NPCSTR szIpHb1 = NULL);
xbus_t XBusStart(CALLBACK fpSBus, CALLBACK fpMBus, bool bSvc = true);
xbus_t XBusStop();

int32 XBusGetHaStMine();
int32 XBusGetHaStPeer();

void XBusSetService(bool bSvc);
bool XBusGetService(uint16 idModl);

NPCSTR XBusGetModlName(uint16 idModl = 0);
pid_t XBusGetModlPid(uint16 idModl = 0);

bool XBusSetPkgName(NPCSTR szPkgName, uint32 nMajor, uint32 nMinor, uint32 nPatch, uint32 nBuild, uint32 nHotfix);
NPCSTR XBusGetPkgName();

uint32 XBusGetLogLvl(uint16 idModl = 0);
uint32* XBusGetLvlPtr(uint16 idModl = 0);
void XBusSetLogLvl(uint32 nLogLvl, uint16 idModl = 0);

bool XBusMsgSend(uint16 idTo, uint16 nMsgId, iByte* pData, uint16 nData);
bool XBusSyncSend(uint16 nMsgId, iByte* pData, uint16 nData);

NPSTR XBusGetUsr(uint16 idModl = 0);

bool IsTrustShm();



/*****************************************************************************
 * XBUS Old Interface Methods
 *****************************************************************************/
// HA Call Back
extern xbus_t 	reg_callback(uint8 cb_id, xbHaCB_t function);

// Module State Change Call Back
extern xbus_t 	reg_mod_state_change(xbMoSt_t function);

// Module CLI Callback
extern xbus_t 	reg_mod_callback(xbMoCB_t pFunc);

// XBus Initialize Methods v1
extern xbus_t	init_btxbus2(uint16 idMine, CALLBACK fpSBus, CALLBACK fpMBus, NPCSTR szCfg, bool bSvc = true);
extern xbus_t   term_btxbus();

// Send Functions
// Send XBus to my peer module
extern xbus_t	ssendsig(uint16 nLength, uint16 nMsgId, uchar* pMsg);

// Send XBus to specified module(s)
extern xbus_t	msendsig(uint16 nLength, int32 idTo, uint16 nMsgId, uchar* pMsg);
// same function as msendsig
extern xbus_t	xsendsig(uint16 nLength, int32 idTo, uint16 nMsgId, uchar* pMsg);

// get_module_state : return value = -1/0/1
extern int 		get_module_state(uint16 moId);
extern void 	get_all_module_state(char *state);
extern NPCSTR	get_module_name(uint16 moId);
extern pid_t	get_module_pid(uint16 moId);
extern NPCSTR	get_error_msg(xbus_t nRetCode);

// init_btxbus 이후에 실행 할것(default = LV1, CRT)
extern xbus_t	set_log_out(int out);
extern xbus_t	set_log_level(int level);
extern xbus_t	set_log_filename(const char *filename);

extern void		xb_get_ha_info(int *MyHaState, int *PeerHaState);
extern xbus_t	xb_change_active_standby();

// 20130412 TonyRyu
// extern bool get_service_state_all();
extern bool get_service_state(uint16 moId);
extern void set_service_state(bool bService);

//20130807 TonyRyu
extern uint32 get_log_level(uint16 moId);
extern uint32* get_log_level_ptr(uint16 moId);
extern bool set_log_level(uint16 moId, uint32 nLogLvl);

/**
 * @owner : chiwoo
 * GetCpuIdle() 수집없이 기존에 수집된 정보를 기반으로
 * CPU IDLE 상태를 확인한다.
 */
extern bool IsCpuIdleOverWithNoneCollect(int32 nLimit, int32 nCount);
extern int32 getCpuIdleOverWithNoneCollect(int32 nLimit, int32 nCount);


















// ~IPRONv3.1.2-b0019 : (key_t)0xA0077700
// ~IPRONv3.1.2-b0022 : (key_t)0xA0077701
// IPRONv3.1.2-b0024~ : (key_t)0xBBBB????
#define	XB_SHMID_KEY		(key_t)0xbbbb0000

#define	MAXLEN_MODLNAME		(16)
#define MAX_FILENAME		(128)
#define MAXBUF_MODULE		(128)
#define MAXLEN_MODLBUF		(512)

#define	MAXCNT_MODLID		(0x0400)				//
#define MAX_PKGNAME_LEN		(32)
#define MAX_IDLEDATA		(128)

typedef struct
{
	pid_t			pidMo;						// Process Id
	time_t			tModl;						// Process 생성 Time
	uint16			idModl;						// Module Id
	uint8			nHBState;					// Alive Status
	bool			bService;					// Service State - 20130412 // SSC <=> SLC 간 서비스 상태
	uint32			nLogLvl;					// OFF(0), LV1(1), LV2(2), LV3(3)
	struct timeval	tStart;						// Module Start Time 참조용
	union
	{
		struct
		{
			char 	szName[MAXLEN_MODLNAME];	// Module Name
			char	szExec[MAX_FILENAME];		// CmdLine Info
			char	szBuff[MAXBUF_MODULE];		// Maximum Shared Buffer per Module
			char	szLine[MAXBUF_MODULE];		// Maximum Shared Buffer for CLI per Module
			uint8 	nHBRole;					// My HB Role
			uint8 	nHBGroup;					// index of HB_INFO
			uint8 	nHARole;					// My HA Role (M)ASTER or (S)LAVE
			uint8 	nHAGroup;					// index of HA_INFO
		};
		char		szRsvd[MAXLEN_MODLBUF];
	};
} PROCINFO;

// shared memory define
typedef struct
{
	uint32			nShmKey;
	time_t			tAlive;
	time_t			tHAState;
	int16			nHAState;
	int16 			nHAPeer;
	PROCINFO		stMod[MAXCNT_MODLID];
	char			szPkgName[MAX_PKGNAME_LEN];
	uint8			nSlve;					// Slave Process Count
	uint8			nAlive;
	bool			bHaMode;
	bool			bDependancy;
	char			nIdle[MAX_IDLEDATA];
	union
	{
		struct
		{
			bool 	bLocalOnly;
		};
		char 		szRsvd[MAXLEN_MODLBUF];
	};
} SHM_MOST;

#endif	/* btxbus.h */

