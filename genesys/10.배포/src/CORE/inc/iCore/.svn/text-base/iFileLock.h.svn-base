//
//  iFileLock.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 15..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iFileLock.h
 * @brief   File Lock 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.15
 */

#pragma once

#include "iMutex.h"

/**
 * @class    iFileLock
 * @brief    File을 이용하여 프로세스 간 Read/Write Lock 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.15
 * @note     해당 Class는 Unix 계열 OS에서만 동작합니다.
 * - Lock/Unlock을 위해 실제 Bulk 파일을 생성합니다.
 */
class ILIBAPI iFileLock : public iObj
{
public:
	iFileLock();
	virtual ~iFileLock();

	/**
	 * @brief  Lock/Unlock을 위한 파일 Open
	 * @return               성공 여부
	 * @param  szFilePath    Lock/Unlock을 위한 파일 명
	 * @param  nCnt          Lock/Unlock 갯수
	 */
	bool OpenLock(NPCSTR szFilePath, uint32 nCnt);

	/**
	 * @brief  Lock/Unlock을 위한 파일 Close
	 * @return 항상 false를 반환함
	 */
	bool CloseLock();

	/**
	 * @brief  FileLock의 Open 여부를 반환
	 * @return FileLock의 Open 여부
	 */
	bool IsOpened();

	/**
	 * @brief  Read Lock
	 * @return      성공 여부
	 * @param  nIdx Locking Index
	 */
	bool RLock(uint32 nIdx);

	/**
	 * @brief  Write Lock
	 * @return      성공 여부
	 * @param  nIdx Locking Index
	 */
	bool WLock(uint32 nIdx);

	/**
	 * @brief  Read Lock Try
	 * @return      성공 시 Locking 됨, 실패 시 Write Lock이 걸려 있는 상태임
	 * @param  nIdx Locking Index
	 */
	bool RLockT(uint32 nIdx);

	/**
	 * @brief  Write Lock
	 * @return      성공 시 Locking 됨, 실패 시 Read/Write Lock이 걸려 있는 상태임
	 * @param  nIdx Locking Index
	 */
	bool WLockT(uint32 nIdx);

	/**
	 * @brief Unlock을 수행
	 * @param nIdx Locking Index
	 */
	void Unlock(uint32 nIdx);

protected:
#ifdef IWINDOWS
	HANDLE mnFd;
#else
	int32 mnFd;
#endif
	uint32 mnSize;
	iRwLock* mpCs;
};


/**
 * @class    iRLock
 * @brief    C++의 생성자/소멸자의 특성을 이용한 Read Lock Helper Class
 * @note     해당 Class는 Unix 계열 OS에서만 동작합니다.
 * - Lock/Unlock을 생성자/소멸자에서 수행함
 */
class ILIBAPI iRLock : public iObj
{
public:
	iRLock(iFileLock& stLock, uint32 nIdx);
	virtual ~iRLock();
protected:
	uint32 mnIdx;
	iFileLock& mstLock;
};

/**
 * @class    iWLock
 * @brief    C++의 생성자/소멸자의 특성을 이용한 Write Lock Helper Class
 * @note     해당 Class는 Unix 계열 OS에서만 동작합니다.
 * - Lock/Unlock을 생성자/소멸자에서 수행함
 */
class ILIBAPI iWLock : public iObj
{
public:
	iWLock(iFileLock& stLock, uint32 nIdx);
	virtual ~iWLock();
protected:
	uint32 mnIdx;
	iFileLock& mstLock;
};

/**
 * @note Sample Source Code
@code
iFileLock stLock;
...
void Function()
{
	iRLock stCs(stLock, 0);
	// Critical Section
	return;		// stCs의 소멸자에 의해 stLock.Unlock이 호출됨
}
@endcode
 */


























