//
//  NsLib.h
//  NsLib
//
//  Created by Tony Ryu on 2012. 10. 29..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    NsLib.h
 * @brief   
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 29.
 */

#pragma once

#include <iType.h>
#include <iShMem.h>

typedef uint32                  NsBits;
static const uint32 NcKey       = (0x1004face);
static const uint32 theBckts[]  = { 1, 1, 1, 1, 2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381 };

#define NcNodeLimit             (64*1024)                   // 최대 65536개 이하 노드만을 가질 수 있음

#define NcBitCnt                (sizeof(NsBits)*8)
#define NcBitArr(x)             (x/NcBitCnt)
#define NsBcktMax(x)            (x/8)                       // 평균 최대 8회 Key 비교를 위한 Bucket 수

enum
{
    NSLIB_NEEDINIT      = 1,
    NSLIB_SUCCESS       = 0,
    NSLIB_OPENFAIL      = -1,
    NSLIB_SIZEDIFF      = -2,
};

template <typename TStruct>
class NsLib : public iShMem
{
	typedef struct
	{
		uint32      nShmKey;
		uint32      nShmSize;
		TStruct     mstData;
	} NsData;
public:
	NsLib() { mpData = NULL; };
	~NsLib() { Close(); };
	
	int32 InitShm(uint32 nShmKey)
	{	
		if(IsOpened()) CloseMem();
		if(!CreateMem(nShmKey, sizeof(NsData), false)) return NSLIB_OPENFAIL;
		mpData = (NsData*)GetMem();
		if(mpData)
		{
			if(mpData->nShmKey!=nShmKey) return NSLIB_NEEDINIT;
			if(mpData->nShmSize==sizeof(NsData)) return NSLIB_SUCCESS;
			return NSLIB_SIZEDIFF;
		}
		Close();
		return NSLIB_OPENFAIL;
	}

	int32 OpenShm(uint32 nShmKey)
	{
		if(IsOpened()) CloseMem();
		if(!CreateMem(nShmKey, sizeof(NsData), false)) return NSLIB_OPENFAIL;
		mpData = (NsData*)GetMem();
		if(mpData)
		{
			if(mpData->nShmKey!=nShmKey) return NSLIB_NEEDINIT;
			if(mpData->nShmSize==sizeof(NsData)) return NSLIB_SUCCESS;
			return NSLIB_SIZEDIFF;
		}
		Close();
		return NSLIB_OPENFAIL;
	}
	
	void InitComplete()
	{
		if(mpData)
		{
			mpData->nShmSize = sizeof(NsData);
			mpData->nShmKey = GetShMemKey();
		}
	}
	
	void Close()
	{
		mpData = NULL;
		CloseMem();
	}
	
	TStruct* operator->() { return mpData?&mpData->mstData:NULL; };
	
protected:
	NsData* mpData;
};
