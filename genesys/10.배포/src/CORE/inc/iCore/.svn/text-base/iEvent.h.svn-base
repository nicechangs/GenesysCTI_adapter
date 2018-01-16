//
//  iEvent.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iEvent.h
 * @brief   Event 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.14
 */

#pragma once

#include "iType.h"
#include "iObj.h"

#ifdef IUNIX
#include <pthread.h>
#include <errno.h>
#define WAIT_TIMEOUT            ETIMEDOUT
#define WAIT_OBJECT_0			(0x00000000)
#define ONESEC_PER_NANOSEC      (1000000000)
#define ONESEC_PER_MICROSEC     (1000000)
#define ONESEC_PER_MILISEC      (1000)
#endif

#define WAIT_SIGNALED			WAIT_OBJECT_0

/**
 * @class    iEvent
 * @brief    Event 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.14
 */
class ILIBAPI iEvent : public iObj
{
public:
	iEvent();
	virtual ~iEvent();

	/**
	 * @brief  Event 생성 함수
	 * @return                성공 여부
	 * @param  bManualReset   수동 Reset 여부(기본 false)
	 */
	bool OpenEvent(bool bManualReset = false);

	/**
	 * @brief Event 종료 함수
	 */
	void CloseEvent();

	/**
	 * @brief  Event를 기다리는 함수
	 * @return                Time out 시에는 WAIT_TIMEOUT을 반환
	 * @param  nWaitTime      Time out Time(ILIB_INFINITE 시에는 무한대로 기다림)
	 */
	virtual int32 WaitEvent(int32 nWaitTime = ILIB_INFINITE);

	/**
	 * @brief  Event를 Set하는 함수
	 * @return 성공 여부
	 */
	virtual bool SetEvent();

	/**
	 * @brief  Event를 Reset하는 함수(Manual Event일 경우에만 사용)
	 * @return Reset하기 전 Signal 상태
	 */
	virtual bool ResetEvent();

protected:
#ifdef IWINDOWS
	HANDLE mhEvent;
#else
	pthread_cond_t mhEvent;
	pthread_mutex_t mhMutex;
	bool mbSignal;
	bool mbManual;
#endif
	bool mbOpen;
};

