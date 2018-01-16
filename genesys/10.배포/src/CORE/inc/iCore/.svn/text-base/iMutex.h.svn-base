//
//  iMutex.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iMutex.h
 * @brief   Mutex 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.14
 */

#pragma once

#include "iObj.h"

/**
 * @class    iMutex
 * @brief    Mutex Lock 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.15
 * @note     Unix 계열은 pthread_mutex, Windows는 Critical Section을 사용
 * - pthread_mutex는 동일 Thread에서 Locking시 재진입을 허용하지 않음
 * - CRITICAL_SECTION은 동일 Thread에 대해서 Locking 재진입을 허용함
 */
class ILIBAPI iMutex : public iObj
{
public:
	iMutex();
	virtual ~iMutex();

#ifdef IWINDOWS
	CRITICAL_SECTION* GetMutex() { return &mstCs; };

	/**
	 * @brief Locking 함수
	 */
	inline void Lock() { EnterCriticalSection(&mstCs); };

	/**
	 * @brief Lock Trying 함수
	 * @return Lock 성공시 true 반환, 이미 Locking 상태일 경우 false를 반환
	 */
	inline bool LockT() { return (TryEnterCriticalSection(&mstCs))?true:false; };

	/**
	 * @brief Unlocking 함수 
	 */
	inline void Unlock() { LeaveCriticalSection(&mstCs); };
protected:
	CRITICAL_SECTION mstCs;
#else
	pthread_mutex_t* GetMutex() { return &mstCs; };

	/**
	 * @brief Locking 함수
	 */
	inline void Lock() { pthread_mutex_lock(&mstCs); };

	/**
	 * @brief  Lock Trying 함수
	 * @return Lock 성공시 true 반환, 이미 Locking 상태일 경우 false를 반환
	 */
	inline bool LockT() { return (pthread_mutex_trylock(&mstCs)==0)?true:false; };

	/**
	 * @brief Unlocking 함수 
	 */
	inline void Unlock() { pthread_mutex_unlock(&mstCs); };
protected:
	pthread_mutex_t mstCs;
#endif//IWINDOWS
};


/**
 * @class    iAutex
 * @brief    C++의 생성자/소멸자의 특성을 이용한 Mutex Lock Helper Class
 * @note Sample Source Code
@code
iMutex stMutex;
...
void Function()
{
	iAutex stCs(stMutex);
	// Critical Section
	return;		// stCs의 소멸자에 의해 stMutex.Unlock이 호출됨
}
@endcode
 */
class ILIBAPI iAutex : public iObj
{
public:
	iAutex(iMutex& stCs);
	virtual ~iAutex();
	inline void Lock() { mstCs.Lock(); mbLocked = true; };
	inline void Unlock() { mbLocked = false; mstCs.Unlock(); };
private:
	bool mbLocked;
	iMutex& mstCs;
};



/**
 * @class    iRwLock
 * @brief    Read/Write Lock 처리 Class
 * @warning
 * - UNIX 계열은 정상 동작 하나 WINDOWS는 CRITICAL_SECTION을 사용하여 Read Lock이 제대로 수행되지 않음
 */
class ILIBAPI iRwLock : public iObj
{
public:
	iRwLock();
	virtual ~iRwLock();

#ifdef IWINDOWS
	CRITICAL_SECTION* GetMutex() { return &mstCs; };
	inline void RLock() { Lock(); };
	inline void WLock() { Lock(); };

	inline BOOL RLockT() { return LockT(); };
	inline BOOL WLockT() { return LockT(); };

	inline void Lock() { EnterCriticalSection(&mstCs); };
	inline BOOL LockT() { return TryEnterCriticalSection(&mstCs); };
	inline void Unlock() { LeaveCriticalSection(&mstCs); };
protected:
	CRITICAL_SECTION mstCs;
#else
	pthread_rwlock_t* GetMutex() { return &mstCs; };
	inline void RLock() { pthread_rwlock_rdlock(&mstCs); };
	inline void WLock() { pthread_rwlock_wrlock(&mstCs); };

	inline bool RLockT() { return bool(pthread_rwlock_tryrdlock(&mstCs)==0); };
	inline bool WLockT() { return bool(pthread_rwlock_trywrlock(&mstCs)==0); };

	inline void Lock() { pthread_rwlock_wrlock(&mstCs); };
	inline bool LockT() { return WLockT(); };
	inline void Unlock() { pthread_rwlock_unlock(&mstCs); };
protected:
	pthread_rwlock_t mstCs;
#endif//IWINDOWS
};



/**
 * @class    iRwAuto
 * @brief    C++의 생성자/소멸자의 특성을 이용한 iRWLock Helper Class
 * @note Sample Source Code
@code
iRwLock stRwLock;
...
void Function()
{
	iRwAuto stCs(stRwLock, false);	// Read Locking
	// Critical Section
	return;		// stCs의 소멸자에 의해 stRwLock.Unlock이 호출됨
}
@endcode
 */
class ILIBAPI iRwAuto : public iObj
{
public:
	iRwAuto(iRwLock& stCs, bool bWrite = true);
	virtual ~iRwAuto();
	inline void Lock() { mstCs.Lock(); mbLocked = true; };
	inline void Unlock() { mbLocked = false; mstCs.Unlock(); };
private:
	bool mbLocked;
	iRwLock& mstCs;
};










