//
//  NsObj.h
//  NsLib
//
//  Created by Tony Ryu on 2012. 10. 29..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    NsObj.h
 * @brief
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 29.
 */

#pragma once

#include "iLib.h"

typedef uint32					NsBits;
static const uint32 NcKey		= (0x1004face);
static const uint32 theBckts[]	= { 1, 1, 1, 1, 2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381 };

#define NcNodeLimit				(64*1024)					// 최대 65536개 이하 노드만을 가질 수 있음

#define NcBitCnt				(sizeof(NsBits)*8)
#define NcBitArr(x)				(x/NcBitCnt)				// 주의!! : theMax는 항상 32의 배수가 되어야 함 !!!!
#define NsBcktMax(x)			(x/8)						// 평균 최대 8회 Key 비교를 위한 Bucket 수

template <uint32 theMax>
class NsObj
{
public:
	NsObj() {};
	~NsObj() {};
	bool Init()
	{
		// Node 최대 개수는 32의 배수여야만 함
		if(IALIGN32(theMax)!=theMax || theMax>NcNodeLimit)
		{
			iTrace("NsObj::Init Failed(ALIGN32=%d/NodeCnt=%d/NodeLimit=%d)", IALIGN32(theMax), theMax, NcNodeLimit);
			return false;
		}
		mnCnt = 0;
		mnLast = -1;
		memset(mnBits, 0x00, sizeof(mnBits));
		mKey = NcKey;
		return true;
	}

	inline void SetFree() { mKey++; };
	inline bool IsInit() { return bool(mKey==NcKey); };
	inline bool IsFull() { return bool(mnCnt==theMax); };
	inline bool IsEmpty() { return bool(mnCnt==0); };
	inline uint32 GetCnt() { return mnCnt; };
	inline uint32 GetMemSize() { return sizeof(*this); };
	inline uint32 GetMaxCnt() { return theMax; };

	uint32 GetCntAudit()
	{
		uint32 i, nCnt=0;
		for(i=0; i<theMax; i++) if(GetBit(i)) nCnt++;
			return nCnt;
	}

	int32 AllocNode()
	{
		int32 nIdx = FindBit0(mnLast+1);
		if(nIdx>=0)
		{
			IncCnt();
			mnLast = nIdx;
			SetBit1(nIdx);
		}
		return nIdx;
	}
	int32 AllocNode(int32 nIdx)
	{
		if(!GetBit(nIdx))
		{   IncCnt();
			mnLast = nIdx;
			SetBit1(nIdx);
			return nIdx;
		}
		return -1;
	}
	bool FreeNode(int32 nIdx)
	{
		if(nIdx>=0 && GetBit(nIdx))
		{
			SetBit0(nIdx);
			DecCnt();
			return true;
		}
		return false;
	}

protected:
	inline int32 FirstBit0()
	{
		return NextBit0();
	}
	inline int32 FirstBit1()
	{
		return NextBit1();
	}
	inline int32 NextBit0(uint32 nIdx = 0)
	{
		if(nIdx<theMax)
		{
			uint32 nArr = uint32(nIdx/NcBitCnt)%NcBitArr(theMax);
			uint32 nBit = uint32(nIdx%NcBitCnt);
			int32 nCnt = theMax-(nArr*NcBitCnt);
			while(nCnt>0 && (mnBits[nArr]==~0U || !FindBit0(mnBits[nArr], nBit, nCnt)))
			{
				nArr = (nArr+1)%NcBitArr(theMax);
				nBit = 0;
				nCnt -= NcBitCnt;
			}
			if(nCnt>0) return nArr*NcBitCnt+nBit;
		}
		return -1;
	}
	inline int32 NextBit1(uint32 nIdx = 0)
	{
		if(nIdx<theMax)
		{
			uint32 nArr = uint32(nIdx/NcBitCnt)%NcBitArr(theMax);
			uint32 nBit = uint32(nIdx%NcBitCnt);
			int32 nCnt = theMax-(nArr*NcBitCnt);
			while(nCnt>0 && (mnBits[nArr]==0U || !FindBit1(mnBits[nArr], nBit, nCnt)))
			{
				nArr = (nArr+1)%NcBitArr(theMax);
				nBit = 0;
				nCnt -= NcBitCnt;
			}
			if(nCnt>0) return nArr*NcBitCnt+nBit;
		}
		return -1;
	}

	inline int32 FindBit0(uint32 nIdx = 0)
	{
		nIdx = nIdx%theMax;
		uint32 nArr = uint32(nIdx/NcBitCnt)%NcBitArr(theMax);
		uint32 nBit = uint32(nIdx%NcBitCnt);
		int32 nCnt = theMax+nBit;
		while(nCnt>0 && (mnBits[nArr]==~0U || !FindBit0(mnBits[nArr], nBit, nCnt)))
		{
			nArr = (nArr+1)%NcBitArr(theMax);
			nBit = 0;
			nCnt -= NcBitCnt;
		}
		if(nCnt>0) return nArr*NcBitCnt+nBit;
		return -1;
	}
	inline int32 FindBit1(uint32 nIdx = 0)
	{
		nIdx = nIdx%theMax;
		uint32 nArr = uint32(nIdx/NcBitCnt)%NcBitArr(theMax);
		uint32 nBit = uint32(nIdx%NcBitCnt);
		int32 nCnt = theMax+nBit;
		while(nCnt>0 && (mnBits[nArr]==0U || !FindBit1(mnBits[nArr], nBit, nCnt)))
		{
			nArr = (nArr+1)%NcBitArr(theMax);
			nBit = 0;
			nCnt -= NcBitCnt;
		}
		if(nCnt>0) return nArr*NcBitCnt+nBit;
		return -1;
	}

	inline bool GetBit(uint32 nIdx)
	{
		nIdx = nIdx%theMax;
		return GetBit(mnBits[nIdx/NcBitCnt], nIdx%NcBitCnt);
	}
	inline bool GetBit(uint32 nIdx, bool& bBit)
	{
		if(nIdx<theMax && nIdx/NcBitCnt<NcBitArr(theMax))
		{
			bBit = GetBit(mnBits[nIdx/NcBitCnt], nIdx%NcBitCnt);
			return true;
		}
		return false;
	}
	inline bool SetBit0(uint32 nIdx)
	{
		if(nIdx<theMax && nIdx/NcBitCnt<NcBitArr(theMax))
		{
			SetBit0(mnBits[nIdx/NcBitCnt], nIdx%NcBitCnt);
			return true;
		}
		return false;
	}
	inline bool SetBit1(uint32 nIdx)
	{
		if(nIdx<theMax && nIdx/NcBitCnt<NcBitArr(theMax))
		{
			SetBit1(mnBits[nIdx/NcBitCnt], nIdx%NcBitCnt);
			return true;
		}
		return false;
	}

protected:

	uint32 CalcBcktCnt(uint32 nMaxNode)
	{
		uint32 nBckt = 1;
		for(uint32 i=1; i<(sizeof(theBckts)/sizeof(theBckts[0])) && theBckts[i]<NsBcktMax(nMaxNode); i++)
			nBckt = theBckts[i];
		return nBckt;
	}

private:
	uint32	mKey;
	uint32	mnCnt;
	int32	mnLast;
	uint8	mnBitCnt;
	uint8	mnReserved;
	uint16	mnMaxBits;
	NsBits	mnBits[NcBitArr(theMax)];

	inline uint32 IncCnt() { return ++mnCnt; };
	inline uint32 DecCnt() { return --mnCnt; };

	// nStep : 0~31(2^5)
	inline bool GetBit(NsBits& nBit, uint32 nStep) { return bool(nBit&(1<<nStep)); };
	inline void SetBit0(NsBits& nBit, uint32 nStep) { nBit&=~(1<<nStep); };
	inline void SetBit1(NsBits& nBit, uint32 nStep) { nBit|=(1<<nStep); };
	inline bool FindBit0(NsBits nBit, uint32& nSPos, int32 nRange = NcBitCnt)
	{
		if(nRange>(int32)NcBitCnt) nRange = NcBitCnt;
		for(; nSPos<(uint32)nRange; nSPos++)
		{
			if(!(nBit&(1<<nSPos))) return true;
		}
		return false;
	}
	inline bool FindBit1(NsBits nBit, uint32& nSPos, int32 nRange = NcBitCnt)
	{
		if(nRange>(int32)NcBitCnt) nRange = NcBitCnt;
		for(; nSPos<(uint32)nRange; nSPos++)
		{
			if(nBit&(1<<nSPos)) return true;
		}
		return false;
	}
};
