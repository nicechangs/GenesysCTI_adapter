//
//  iDblQue.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 15..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iDblQue.h
 * @brief   Double Locked Queue
 * @warning Unix 계열에서는 Double Lock을 사용, Windows에서는 Event 사용.
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.15
 */

#pragma once

#include "iQue.h"
#include "iMutex.h"

template <typename TQueVal, uint32 nBlkSize = DEFCNT_NODEUNIT>
class ILIBAPI iDblQue : public iObj
{
public:
	iDblQue()
	{
		SetOnOff(true);
	}
	virtual ~iDblQue()
	{
		Free();
	}

	inline uint32 GetCnt() { return mqData.GetCnt(); };
	inline bool IsOn() { return mbOn; };
	inline void SetOnOff(bool bOn) { iAutex stCs(mstCsQue); mbOn = bOn; };

	void Init()
	{
		TQueVal qVal;
		mstCsQue.Lock();
		while(mqData.PopHead(qVal)) OnClear(qVal);
		mqData.Clear();
		mbOn = true;
		mstCsQue.Unlock();
#ifndef IWINDOWS
		mstCsWait.LockT();
#endif
	}

	void Free()
	{
		TQueVal qVal;
		mstCsQue.Lock();
		mbOn = false;
		while(mqData.PopHead(qVal)) OnClear(qVal);
		mqData.Clear();
		mstCsQue.Unlock();
#ifdef IWINDOWS
		mstEvWait.SetEvent();
#else
		mstCsWait.Unlock();
#endif
	}

	bool Push(TQueVal qVal)
	{
		mstCsQue.Lock();
		bool bRet = false, bUnlock = !mbOn || mqData.IsEmpty();
		if(mbOn)
		{
			mqData.PushTail(qVal);
			bRet = true;
		}
		mstCsQue.Unlock();
#ifdef IWINDOWS
		if(bUnlock) mstEvWait.SetEvent();
#else
		if(bUnlock) mstCsWait.Unlock();
#endif
		return bRet;
	}

	bool Pop(TQueVal& qVal)
	{
#ifdef IWINDOWS
		mstEvWait.WaitEvent();
#else
		mstCsWait.Lock();
#endif
		mstCsQue.Lock();
		bool bRet = mqData.PopHead(qVal);
		bool bLock = mbOn && mqData.IsEmpty();
		mstCsQue.Unlock();
#ifdef IWINDOWS
		if(!bLock) mstEvWait.SetEvent();
#else
		if(!bLock) mstCsWait.Unlock();
#endif
		return bRet;
	}

protected:
	bool mbOn;
	iMutex mstCsQue;
#ifdef IWINDOWS
	iEvent mstEvWait;
#else
	iMutex mstCsWait;
#endif
	iQue<TQueVal, nBlkSize> mqData;
	virtual void OnClear(TQueVal& qVal) {};
};


