//
//  iShMem.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 15..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iShMem.h
 * @brief   Shared Memory 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.15
 */

#pragma once

#include "iObj.h"

#ifdef IWINDOWS
typedef int32						key_t;
#define STRFMT_SHMFILE				("Global\\iShMem%8.8X")
#else
#include <sys/ipc.h>
#include <sys/shm.h>
typedef int32						HANDLE;
#define INVALID_HANDLE_VALUE		(-1)
#endif

/**
 * @class    iShMem
 * @brief    Shared Memory 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.15
 */
class ILIBAPI iShMem : public iObj
{
public:
	iShMem();				///< 생성자.
	virtual ~iShMem();		///< 소멸자.

	/**
	 * @brief  공유 메모리 생성 함수
	 * @return                성공 여부
	 * @param  nKey           Shared Memory Key
	 * @param  nSize          Shared Memory 크기
	 * @param  bExistAsErr    이미 공유 메모리가 존재할 경우 Error로 처리 할지 여부
	 */
	bool CreateMem(key_t nKey, int32 nSize, bool bExistAsErr = true);

	/**
	 * @brief  공유 메모리 Open 함수
	 * @return              성공 여부
	 * @param  nKey         Shared Memory Key
	 * @param  nSize        Shared Memory 크기(기존에 공유 메모리가 존재 할 경우 0으로 설정)
	 * @note                nKey가 존재하지 않을 경우 Share Memory를 생성함
	 */
	bool OpenMem(key_t nKey, int32 nSize = 0);

	/**
	 * @brief 공유 메모리 해제 함수
	 * @param bDelete       해제 시 공유 메모리 삭제 여부
	 */
	void CloseMem(bool bDelete = false);

	/**
	 * @brief  공유 메모리 Open 상태 
	 * @return               Open 상태
	 */
	bool IsOpened() { return bool(mpMem!=NULL); };

	HANDLE GetShMemHandle() { return mhShMem; };

	/**
	 * @brief  공유 메모리 Key 반환
	 * @return 공유 메모리 Key
	 */
	key_t GetShMemKey() { return mnShMemKey; };

	/**
	 * @brief  공유 메모리 Pointer를 반환
	 * @return 공유 메모리 Pointer
	 */
	NPVOID GetMem() { return mpMem; };

	/**
	 * @brief  공유 메모리 크기를 반환
	 * @return 공유 메모리 크기
	 */
	size_t GetSize();

#ifdef ILINUX
	bool IsLocked();
	bool IsDestroy();
#endif
#ifndef IWINDOWS
	uint16 GetAttachCnt();
#endif

protected:
	/**
	 * @brief iShMem을 상속 구현 시 Shared Memory Open후 호출되는 Callback 함수
	 * @param nKey        공유 메모리 Key
	 * @param nSize       공유 메모리 크기
	 */
	virtual void OnOpen(key_t nKey, int32 nSize) {};

	/**
	 * @brief iShMem을 상속 구현 시 Shared Memory Close후 호출되는 Callback 함수
	 * @param nKey         공유 메모리 Key
	 */
	virtual void OnClose(key_t nKey) {};

private:
	HANDLE mhShMem;
	key_t mnShMemKey;
	NPVOID mpMem;
#ifdef IWINDOWS
	char mszShmFileName[MAXLEN_PATH];
	size_t mnSize;
#else
	shmid_ds mstDs;
#endif
};











