//
//  iLog.h
//  iLib3
//
//  Created by Tony Ryu on 11. 5. 6..
//  Copyright 2011 TonyAppz. All rights reserved.
//

/**
 * @file    iLog.h
 * @brief   Log 처리
 *
 * @version v0.1
 * @see     None
 * @date    2011.5.6
 */

#pragma once
#include <stdio.h>
#include "iMutex.h"

/**
 * @enum  Log Lable을 지정
 */
enum {
	IOUT_FILE				= 0x01000000,			///< File에 로그를 적재.
	IOUT_CONS				= 0x02000000,			///< Console에 로그를 표기.
	IOUT_SYSL				= 0x04000000,			///< syslogd에 로그를 표기.
	IOUT_FUNC				= 0x08000000,			///< Callback Function을 호출.
	ILOG_ADMIN				= 0x000000f0|IOUT_CONS,	///< Admin Log, Console에 로그 표시.
	ILOG_INFO				= 0x000000f1,			///< Info Log.
	ILOG_ERR				= 0x000000f2|IOUT_SYSL,	///< Error Log, Syslog에 로그 표시.
	ILOG_WARN				= 0x000000f3|IOUT_SYSL,	///< Warning Log, Syslog에 로그 표시.
	ILOG_CINFO				= ILOG_INFO|IOUT_CONS,	///< Info Log, Console에 로그 표시.
	ILOG_CERR				= ILOG_ERR|IOUT_CONS,	///< Error Log, Console에 로그 표시.
	ILOG_CWARN				= ILOG_WARN|IOUT_CONS,	///< Warning Log, Console에 로그 표시.
	ILOG_CHK				= 0x000000f4,			///< Check Log.
	ILOG_EXT				= 0x000000e5,			///< External Log.
	ILOG_LV1				= 0x000000e6,			///< Level 1 Log.
	ILOG_INT				= 0x000000c7,			///< Internal Log.
	ILOG_LV2				= 0x000000c8,			///< Level 2 Log.
	ILOG_TRC				= 0x00000089,			///< Trace Log.
	ILOG_LV3				= 0x0000008a,			///< Level 3 Log.
	ILOG_DBG				= 0x0000008b,			///< Debug Log.
	ILOG_USRDEF1			= 0x0000008c,			///< User Defined Log #1.
	ILOG_USRDEF2			= 0x0000008d,			///< User Defined Log #2.
	ILOG_USRDEF3			= 0x0000008e,			///< User Defined Log #3.
	ILOG_USRDEF4			= 0x0000008f,			///< User Defined Log #4.
	ILOG_MAX				= 0x00000010
};

/**
 * @enum  Log Level을 지정
 */
enum
{
	NLVL_OFF				= 0,					///< Log Off.
	NLVL_LV1				,						///< Log Level 1.
	NLVL_LV2				,						///< Log Level 2.
	NLVL_LV3				,						///< Log Level 3.
	NLVL_DBG				= NLVL_LV3,				///< Log Level 3.
	NLVL_ALL				= NLVL_LV3,				///< Log Level 3.
	NLVL_MAX
};

#define NLVL_DEFLVL			(NLVL_OFF)

#define NLOGLEN_PREFIX		(8)						///< Log Prefix의 최대 길이(8-1).
#define MAXCNT_LOGFILE		(1000)					///< 최대 1000개의 Log 파일 생성.

#define NLOGSTR_PREFIX		("iLog")
#define NLOGSTR_PATH		("./log")

#define NLOGFMT_LOGDATE		("YMD")					///< YYYYMMDD 형식으로 Log File명 생성.
#define NLOGLEN_LBL			(32)

/**
 * @class iLog
 * @brief Log 저장 Class
 * 
 * @note [Log Label 관련 정보]
 * - 출력 지정 Part(0xff000000) : 입력되는 로그를 출력할 Media(Console/File/Network/syslogd)
 * - 로그 라벨 Part(0x00ffffff) : 입력되는 로그 라인의 앞에 찍힐 Label을 지정
 *
 * @note [Log Level 지정]
 * - NLVL_ALL : 모든 로그를 지정된 출력 측으로 전달함
 * - NLVL_DBG : NLVL_ALL과 동일
 * - NLVL_LV3 : Log Level 3로서 DBG를 제외한 로그를 출력 측으로 전달
 * - NLVL_LV2 : Log Level 2로서 DBG,LV3,TRACE를 제외한 로그를 출력 측으로 전달
 * - NLVL_LV1 : Log Level 1로서 DBG,LV3,TRACE,LV2,INT를 제외한 로그를 출력 측으로 전달
 * - NLVL_OFF : Log Level Off로서 WARN,ERR,INFO,CONS 로그를 출력 측으로 전달
 *
 * @note [출력 지정]
 * - Default IOUT_FILE로만 출력
 * - SetOutMedia 함수에 IOUT_XXXX 상수를 통해 지정할 수 있음
 * - Log() 함수에서 IOUT_XXXX 상수를 통해 직접 출력을 지정할 수 있음
 */
class ILIBAPI iLog : public iMutex
{
public:
	/**
	 * @brief Unix계열의 syslogd를 통한 logging
	 * @param szProc Process Name
	 */
	static void SysLogInit(NPCSTR szProc);
	static void SysLogFree();
	static void SysLog(uint32 nLogLbl, NPCSTR szFmt, ...);

	/**
	 * @brief iLog() 생성자 : Filename Format 지정
	 * @param szFmt "YMDh"로 지정 가능 예) "MD":파일명에 월,일만 지정됨 ieslc0716000.log => 7월 16일
	 *              					 "Dh":파일명에 일,시만 지정됨 ieslc1613000.log => 16일 13시
	 */
	iLog(NPCSTR szFmt = NLOGFMT_LOGDATE);
	virtual ~iLog();

	inline bool IsInit() { return mbInit; };

	/**
	 * @brief Log 초기화 함수
	 * @param szPrefix  Log Prefix로 로그 파일명 앞부분
	 * @param szLogPath Log 저장될 Path
	 * @param nLogLvl   초기 Log Level(미지정 시 NLVL_OFF)
	 */
	void Init(NPCSTR szPrefix, NPCSTR szLogPath, uint32 nLogLvl = NLVL_DEFLVL);
	void Free();

	/**
	 * @brief Log File Link 지정 함수 
	 * @param szLink        Link 파일명
	 * @param szPath        Link 저장 Path
	 */
	void MakeLink(NPCSTR szLink, NPCSTR szPath);

	/**
	 * @brief 하나의 Log File의 Max Size 지정
	 * @param nMaxSize        Log File Max Size
	 */
	inline void SetMaxSize(uint32 nMaxSize) { mnMaxSize = nMaxSize; };

	/**
	 * @brief Log Level 지정 함수
	 * @param szLogLvl       String Log Level
	 * @note String Log Level
	 * - OFF, Off, off, '0' : NLVL_OFF
	 * - '1' : NLVL_LV1
	 * - '2' : NLVL_LV2
	 * - ON, On, on, '3' : NLVL_LV3 
	 * - ALL, '4' : NLVL_ALL
	 * - DBG, dbg : NLVL_DBG
	 */
	void SetLogLvl(NPCSTR szLogLvl);

	/**
	 * @brief Log Level 지정 함수
	 * @param nLogLvl        Log Level
	 * @note Log Level
	 * - 0 : NLVL_OFF
	 * - 1 : NLVL_LV1
	 * - 2 : NLVL_LV2
	 * - 3 : NLVL_LV3 
	 */
	void SetLogLvl(uint32 nLogLvl);

	/**
	 * @brief 외부 Log Level을 연동하기 위한 함수
	 * @param pLogLvl 외부에서 로그 레벨값을 저장하는 uint32 형 Integer Pointer
	 */
	void SetLvlPtr(uint32* pLogLvl);

	/**
	 * @brief  User Defined Label을 지정/설정 하는 함수
	 * @param  nLogLbl  지정하고자 하는 Label(ILOG_USRDEF1 ~ ILOG_USRDEF5 + External Flag)
	 * @param  szLogLbl Log 저장시 Label에 쓰여질 String
	 * @return          수정 성공 시 true, 수정 실패 시 false(지정 불가능한 Label일 경우)
	 */
	bool EditLogLbl(uint32 nLogLbl, NPCSTR szLogLbl);

	inline uint32 GetLogLvl() { return *mpExtLvl; };

	void SetOutMedia(int32 nOutMedia, bool bEnable);
	inline void SetOutFile(bool bEnable) { SetOutMedia(IOUT_FILE, bEnable); };
	inline void SetOutCons(bool bEnable) { SetOutMedia(IOUT_CONS, bEnable); };
	inline void SetOutSysL(bool bEnable) { SetOutMedia(IOUT_SYSL, bEnable); };
	inline void SetOutFunc(bool bEnable) { SetOutMedia(IOUT_FUNC, bEnable); };

	void Log(uint32 nLogLbl, NPCSTR szFmt, ...);
	void LogLine(uint32 nLogLbl, uint32 nLine, NPCSTR szFmt, ...);
	void LogChan(uint32 nLogLbl, uint16 nChnnel, NPCSTR szFmt, ...);

	void Disp(NPCSTR szFmt, ...);
	void DispTm(NPCSTR szFmt, ...);
	void HexDump(uint32 nLogLbl, NPSTR szBuf, uint32 nLen, NPCSTR szDescFmt, ...);

protected:
	uint32* mpExtLvl;
	uint32 mnLogLvl;
	uint32 mnCurDate;
	uint32 mnLogIdx;

	uint32 mnList;
	uint32 mnMaxSize;

	FILE* mpLogFile;
	char mszPath[MAXLEN_PATH];
	char mszFile[MAXLEN_PATH];
	char mszLink[MAXLEN_PATH];
	char mszPrefix[NLOGLEN_PREFIX];
	char mszDateFmt[MAXLEN_DATETIME];

	uint32 mnLenDate;
	uint32 mnOutMedia;

	char mszLogLbl[ILOG_MAX][NLOGLEN_LBL];
	bool mbInit;

	virtual void OnInit(NPCSTR szPrefix, NPCSTR szLogPath, uint32 nLogLvl) {};
	void InitLogLbl();
	FILE* FileOpen(uint32 nCurDate, uint32& nLogIdx);
	NPSTR MakeRowHeader(NPSTR szMsg, uint32 nLogLbl);
	NPCSTR GetLvlLbl(uint32 nLogLbl);
	bool ChkLogLvl(uint32 nLogLbl);
	void MakeLinkFile();

	void LogOutput(NPCSTR szMsg, uint32 nLogLbl);
	virtual void Out2File(NPCSTR szMsg, uint32 nLogLbl);
	virtual void Out2Cons(NPCSTR szMsg, uint32 nLogLbl);
	virtual void Out2SysL(NPCSTR szMsg, uint32 nLogLbl);
	virtual void Out2Func(NPCSTR szMsg, uint32 nLogLbl) {};
};


