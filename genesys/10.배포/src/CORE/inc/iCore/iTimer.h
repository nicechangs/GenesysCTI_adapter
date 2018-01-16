//
//  iTimer.h
//
//
//  Created by Tony Ryu on 2012.12.7.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iTimer.h
 * @brief   Timer 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.12.7
 */

#pragma once

#include "iType.h"
#include "iMutex.h"
#include "iThread.h"

#define DEFCNT_TIMER				(1024)			///< 기본 최대 수용 가능 Timer 개수.
#define DEFSTR_TRNSFMT				("YMDhms")		///< TrnsTime 함수의 기본 형식(ex:20131127095112).

/**
 * @brief  Timer Callback 함수 Pointer 정의
 * @return           함수에서 false를 반환 시 해당 Timer는 그냥 종료됨(KillTimer 호출이 필요 없음)
 * @param  hTimer    Timer Handle
 * @param  nTimerEvt Timer Event
 * @param  tTime     Timer가 실행되는 시각
 * @param  pArg      SetTimer시 지정한 User Data Pointer
 * @warning 
 * - tTime은 Callback 함수가 호출된 시각이 아니라 지정한 호출 시각 정보이다.
 * - Callback 함수 구현 시 반환 값을 주의 할것
 * -- return true; 시 지정한 Interval 마다 호출
 * -- return false; 시 해당 Timer가 삭제(Kill Timer 호출이 필요 없음)
 * -- Callback 함수에서 동일 Instance의 iTimer SetTimer/KillTimer 함수는 호출할 수 없음
 */
typedef bool(*iTimerFunc)(int32 hTimer, int32 nTimerEvt, uint64 tTime, NPVOID pArg);

/**
 * @class    iTimer
 * @brief    Timer 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.12.7
 */
class ILIBAPI iTimer : public iThread
{
public:
	iTimer();					///< 생성자.
	virtual ~iTimer();			///< 소멸자.

	/**
	 * @brief  Timer 서비스 시작 함수
	 * @return                 성공 여부
	 * @param  nMaxItem        최대 설정 가능 Timer 개수
	 */
	bool StartTimer(uint32 nMaxItem = DEFCNT_TIMER);

	/**
	 * @brief Timer 서비스 중지 함수
	 */
	void StopTimer();

	/**
	 * SetTimer Timer를 절대 시각 이후부터 수행하도록 지정함
	 * @return              Timer Handle(Timer Event와는 다름, Timer 중지 시 필요)
	 * @param  nTimerEvt    Timer Event
	 * @param  nInterval    Timer 주기(ms 단위)
	 * @param  pArg         User Argument
	 * @param  nAbsTime     Timer 시작 절대 시각(time_t Sec)
	 * @param  bIgnPastEvt  지정한 절대 시각으로부터 현재 시각까지 누락된 Timer Event를 무시할지 여부
	 * @see    KillTimer
	 */
	int32 SetTimer(int32 nTimerEvt, uint64 nInterval, NPVOID pArg, time_t nAbsTime = 0, bool bIgnPastEvt = true);

	/**
	 * @brief  실행 시킨 Timer를 중지 시키는 함수
	 * @return                성공 시 hTimer값 반환, 실패 시 -1 반환(이미 종료된 Timer일 경우)
	 * @param  hTimer         중지를 원하는 Timer Handle(SetTimer의 반환값)
	 */
	int32 KillTimer(int32 hTimer);

	/**
	 * @brief Timer Callback 함수를 외부 함수로 지정(iTimer를 상속받아 처리 시 OnTimerEvent 함수로 처리)
	 * @param pFunc             Timer Callback 함수 Pointer
	 */
	void SetTimerFunc(iTimerFunc pFunc) { mpTmFunc = pFunc; };

	/**
	 * @brief  년,월,일,시,분,초 값으로 time_t 값을 계산해주는 함수
	 * @return               time_t 값
	 * @param  nSec          초(-1 일 경우 현재 시각을 기준으로 함)
	 * @param  nMin          분(-1 일 경우 현재 시각을 기준으로 함)
	 * @param  nHour         시간(-1 일 경우 현재 시각을 기준으로 함)
	 * @param  nDay          일(-1 일 경우 현재 시각을 기준으로 함)
	 * @param  nMonth        월(-1 일 경우 현재 시각을 기준으로 함)
	 * @param  nYear         년(-1 일 경우 현재 시각을 기준으로 함)
@code
// 현재 시각이 2013년 11월 27일 10시 5분 17초 일때
time_t tTime1 = iTimer::CalcTime(0, 0, 0); // 2013년 11월 27일 00시 00분 00초의 time_t 값 반환
time_t tTime2 = iTimer::CalcTime();        // 2013년 11월 27일 10시 05분 17초의 time_t 값 반환
time_t tTime3 = iTimer::CalcTime(0);       // 2013년 11월 27일 10시 05분 00초의 time_t 값 반환
@endcode
	 */
	static time_t CalcTime(int8 nSec = -1, int8 nMin = -1, int8 nHour = -1, int8 nDay = -1, int8 nMonth = -1, int16 nYear = -1);

	/**
	 * @brief  Timer Callback 함수에서 제공하는 uint64 tTime을 Human Readable 형태로 변환하는 함수
	 * @return               Human Readable 시각 정보(예:20131127100517)
	 * @param  tTime         Timer Callback에서 주어진 tTime 값
	 * @param  szFmt         출력 형태 지정("YMDhms$")
@code
// tTime 시각이 2013년 11월 27일 10시 5분 17.456 초 일때
uint64 nTime1 = iTimer::TrnsTime(tTime, "YMDhms");    // 20131127100517 값 반환
uint64 nTime2 = iTimer::TrnsTime(tTime, "YMD");       // 20131127 값 반환
uint64 nTime3 = iTimer::TrnsTime(tTime, "hms$");      // 100517456 값 반환
@endcode
	 */
	static uint64 TrnsTime(uint64 tTime, NPCSTR szFmt = DEFSTR_TRNSFMT);

protected:
	typedef struct _iTimerProc
	{
		int32			hTimer;
		int32			nTimerEvt;
		uint64			nInterval;
		uint64			evtTime;
		NPVOID			pArg;
		_iTimerProc*	pNext;
		bool			bUse;	// If this flag is FALSE, Timer Item is already killed !!
	} iTimerProc;

	iMutex mstCs;
	uint32 mnMaxItem;

	iTimerFunc mpTmFunc;
	iTimerProc*	mpTimers;
	iTimerProc*	mpSetTm;
	iTimerProc*	mpHead;
	iTimerProc*	mpTail;
	iTimerProc*	mpCurTm;

	iTimerProc* PopEmpty();
	void PushEmpty(iTimerProc* pTimer);

	iTimerProc* GetTimer();
	iTimerProc* PopTimer(int32 hTimer = -1);
	bool PushTimer(iTimerProc* pNewTimer);

	iTimerProc* CalcNextTime(iTimerProc* pTimer);
	int32 WaitTimerEvent(iTimerProc* pTimer);
	static uint64 GetCurTime();
	virtual bool OnTimerEvent(int32 hTimer, int32 nTimerEvt, uint64 tTime, NPVOID pArg);
	virtual NPVOID ThreadRun(NPVOID pArg);

};
