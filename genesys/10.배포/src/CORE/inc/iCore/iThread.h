//
//  iThread.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 15..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iThread.h
 * @brief   Threading 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.15
 */

#pragma once

#include "iEvent.h"

#define DEFLEN_THREAD_STACK			(2048*1024)		// 2MB

/**
 * @brief  Thread 함수 정의
 * @return             특별한 일이 없는 경우 0이나 NULL을 반환
 * @param  NPVOID      Start 함수 호출 시 설정한 User Pointer
 */
typedef NPVOID (*ITHREADFUNC)(NPVOID);

/**
 * @class    iThread
 * @brief    Threading 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.15
 */
class ILIBAPI iThread : public iEvent
{
public:
	iThread();				///< 생성자.
	virtual ~iThread();		///< 소멸자.

	/**
	 * @brief  Thread 시작 함수
	 * @return            성공 여부
	 * @param  pArg       User Pointer
	 * @param  pFunc      Thread 함수 Pointer(NULL 일 경우 상속받아 구현한 ThreadRun 함수가 실행)
	 * @param  nStack     Thread Stack Size(0일 경우 이미 설정된 Stack Size로 지정됨)
	 * @note              SetStackSize로 미 지정 시 기본 Stack Size인 DEFLEN_THREAD_STACK로 지정됨
	 */
	bool Start(NPVOID pArg = NULL, ITHREADFUNC pFunc = NULL, size_t nStack = 0);

	/**
	 * @brief  Thread 중지 함수
	 * @return           중지 완료 시 true 반환, 이미 중지되었을 경우 false를 반환
	 * @param  bWait     true일 경우 Thread 종료 이후 Return, false일 경우 바로 Return 됨
	 */
	bool Stop(bool bWait = false);

	/**
	 * @brief  Thread Stack Size 변환 함수(Start 이전에 호출하여야 함)
	 * @return                   기존 설정된 Stack Size 반환 
	 * @param  nStack            설정할 Stack Size(Byte 단위)
	 */
	size_t SetStackSize(size_t nStack);

	/**
	 * @brief  현재 Thread 실행중인지 여부 확인 함수
	 * @return 현재 실행중인지 여부
	 */
	inline bool IsRunning() { return mbRun; };

	/**
	 * @brief  Thread 함수 내부에서 IsOn() 함수를 마지막으로 호출한 시각
	 * @return 시각 정보
	 */
	inline time_t GetLastTime() { return mtLast; };

	/**
	 * @brief Thread 함수를 재지정(Start 이전에 설정하여야 함)
	 * @param pFunc          Thread 함수 Pointer
	 */
	inline void SetThFunc(ITHREADFUNC pFunc) { mpFunc = pFunc; };

	/**
	 * @brief  설정되어 있는 Thread 함수 Pointer를 반환
	 * @return Thread 함수 Pointer
	 */
	inline ITHREADFUNC GetThFunc() { return mpFunc; };

#ifdef IWINDOWS
	HANDLE GetHandle() { return mhThread; };
	inline dword GetThreadId() { return mIdThread; };
protected:
	HANDLE mhThread;
	dword mIdThread;
#else
	pthread_t GetHandle() { return mhThread; };
	inline pthread_t GetThreadId() { return mhThread; };
protected:
	pthread_t mhThread;
	pthread_attr_t mhAttr;
#endif

	size_t mnStack;
	NPVOID mpArg;
	ITHREADFUNC mpFunc;
	bool mbFlag;

	/**
	 * @brief   Thread 함수 내에서 계속 실행 여부를 반환
	 * @return  실행 여부
	 * @note    외부에서 Thread를 Stop 시켰는지 여부를 알 수 있음
	 * @warning iThread에서 현재 생성된 Thread에서만 호출하여야 함
	 */
	virtual bool IsOn();

	/**
	 * @brief  iThread 상속 시 구현 함수
	 * @return                특별한 일이 없는 경우 0이나 NULL을 반환
	 * @param  pArg           Start 함수 호출 시 설정한 User Pointer
	 */
	virtual NPVOID ThreadRun(NPVOID pArg);

	/**
	 * @brief iThread 상속 구현 시 Thread 함수(ThreadRun) 호출 전에 호출되는 함수
	 * @param pArg         Start 함수 호출 시 설정한 User Pointer
	 */
	virtual void OnStart(NPVOID pArg) {};

	/**
	 * @brief iThread 상속 구현 시 Thread 함수(ThreadRun) 종료 후에 호출되는 함수
	 * @param pArg         Start 함수 호출 시 설정한 User Pointer
	 */
	virtual void OnStop(NPVOID pArg) {};

private:
	bool mbRun;
	time_t mtLast;
	friend NPVOID iThreadFunc(NPVOID pArg);
};

