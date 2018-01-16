//
//  iSem.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 15..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iSem.h
 * @brief   Semaphore 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.15
 */

#pragma once

#include "iObj.h"

#ifdef IWINDOWS
typedef int32					key_t;
#else
typedef int32					HANDLE;
#endif

/**
 * @class    iSem
 * @brief    Semaphore 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.15
 */
class ILIBAPI iSem : public iObj
{
public:
	iSem();					///< 생성자.
	virtual ~iSem();		///< 소멸자.

	/**
	 * @brief  세마포어 생성 함수
	 * @return                성공 여부
	 * @param  nKey           Semaphore Key
	 * @param  nInit          임계 영역 동시 접근 수
	 * @param  bExistAsErr    이미 세마포어가 존재할 경우 Error로 처리 할지 여부
	 */
	bool CreateSem(key_t nKey, int32 nInit, bool bExistAsErr = true);

	/**
	 * @brief  세마포어 Open 함수
	 * @return              성공 여부
	 * @param  nKey         Semaphore Key
	 */
	bool OpenSem(key_t nKey);

	/**
	 * @brief 세마포어 해제 함수
	 */
	bool CloseSem();

	/**
	 * @brief 임계영역 진입 시 Locking 함수
	 */
	void Lock();

	/**
	 * @brief 임계영역 탈출 시 Unlocking 함수
	 */
	void Unlock();

	HANDLE GetSemHandle() { return mhSem; };
	key_t GetSemKey() { return mnSemKey; };

protected:
	HANDLE mhSem;
	key_t mnSemKey;
#ifdef IWINDOWS
	char mszSemFileName[MAXLEN_PATH];
#endif
};
