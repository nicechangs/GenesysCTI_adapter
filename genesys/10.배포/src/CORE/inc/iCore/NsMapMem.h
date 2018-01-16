//
//  NsMapMem.h
//  NsLib
//
//  Created by Tony Ryu on 2012. 11. 1..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    NsMapMem.h
 * @brief   
 *
 * @version v0.1
 * @see     None
 * @date    2012. 11. 1.
 */

#pragma once

#include "NsObj.h"

template <typename TKey, typename TVal, uint32 theMax, uint32 theEntry = 1, uint32 theDept = theMax>
class NsMapMem : public NsObj<theMax>
{
	typedef struct
	{
		uint32  nEntry;
		TKey    kKey;
		TVal    vVal;
	} NsData;
public:
	NsMapMem() {};
	~NsMapMem() {};
	bool InitMap()
	{
		if(this->Init())
		{
			memset(mnNodes, 0xff, sizeof(mnNodes));
			memset(mstData, 0x00, sizeof(mstData));
			mnBckt = this->CalcBcktCnt(theDept);
			return true;
		}
		return false;
	}

	inline const TKey* GetKey(uint32 nIdx) { return this->GetBit(nIdx%theMax)?&mstData[nIdx%theMax].vKey:NULL; };
	inline TVal* GetVal(uint32 nIdx) { return this->GetBit(nIdx%theMax)?&mstData[nIdx%theMax].vVal:NULL; };

	inline int32 GetIdx(uint32 nEntry, TKey kKey)
	{
		int32 nLast = GetHashIdx(nEntry, kKey);
		return FindKey(kKey, nLast);
	}
	inline bool IsExist(uint32 nEntry, TKey kKey)
	{
		if(GetIdx(nEntry, kKey)<0) return false;
		else return true;
	}

	inline bool Insert(uint32 nEntry, TKey kKey, TVal& vVal, int32* pIdx = NULL, bool bIsExistErr = true)
	{
		int32 nHash = GetHashIdx(nEntry, kKey);
		int32 nIdx, nLast = nHash;
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(kKey, nLast);
		if(*pIdx<0)
		{
			*pIdx = this->AllocNode();
			if(*pIdx<0) return false;
			mstData[*pIdx].nEntry = nEntry;
			mstData[*pIdx].kKey = kKey;
			mstData[*pIdx].vVal = vVal;
			mnNodes[*pIdx] = mnNodes[nHash];
			mnNodes[nHash] = *pIdx;
		}
		else
		{
			if(bIsExistErr) return false;
			mstData[*pIdx].vVal = vVal;
		}
		return true;
	}
	inline bool Update(uint32 nEntry, TKey kKey, TVal& vVal, int32* pIdx = NULL, bool bNoExistErr = true)
	{
		int32 nHash = GetHashIdx(nEntry, kKey);
		int32 nIdx, nLast = nHash;
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(kKey, nLast);
		if(*pIdx<0)
		{
			if(bNoExistErr) return false;
			*pIdx = this->AllocNode();
			if(*pIdx<0) return false;
			mstData[*pIdx].kKey = kKey;
			mstData[*pIdx].vVal = vVal;
			mnNodes[*pIdx] = mnNodes[nHash];
			mnNodes[nHash] = *pIdx;
		}
		else mstData[*pIdx].vVal = vVal;
		return true;
	}
	inline bool Select(uint32 nEntry, TKey kKey, TVal& vVal, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, kKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(kKey, nLast);
		if(*pIdx<0) return false;
		vVal = mstData[*pIdx].vVal;
		return true;
	}
	inline TVal* Select(uint32 nEntry, TKey kKey, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, kKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(kKey, nLast);
		if(*pIdx<0) return NULL;
		return &mstData[*pIdx].vVal;
	}
	inline bool Delete(uint32 nEntry, TKey kKey, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, kKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(kKey, nLast);
		if(*pIdx>=0)
		{
			mnNodes[nLast] = mnNodes[*pIdx];
			mnNodes[*pIdx] = -1;
			memset(&mstData[*pIdx], 0x00, sizeof(mstData[*pIdx]));
			this->FreeNode(*pIdx);
			return true;
		}
		return false;
	}

	int32 First() { return this->IsEmpty()?-1:this->FirstBit1(); };
	bool Next(int32& rPos, TKey& kKey, TVal& vVal, int32* pIdx = NULL)
	{
		if(rPos>=0 && rPos<theMax)
		{
			if(pIdx) *pIdx = rPos;
			kKey = mstData[rPos].kKey;
			vVal = mstData[rPos].vVal;
			rPos = this->NextBit1(rPos+1);
			return true;
		}
		return false;
	}
	int32 FindEntry(int32 nPos, uint32 nEntry, TKey* pKey = NULL, TVal* pVal = NULL)
	{
		while(!this->IsEmpty())
		{
			nPos = this->NextBit1(nPos+1);
			if(nPos>=0 && nPos<theMax)
			{
				if(nEntry==mstData[nPos].nEntry)
				{
					if(pKey) *pKey = mstData[nPos].kKey;
					if(pVal) *pVal = mstData[nPos].vVal;
					return nPos;
				}
			}
			else break;
		}
		return -1;
	}

	bool Next(int32& rPos, uint32 nEntry, TKey& kKey, TVal& vVal, int32* pIdx = NULL)
	{
		while(rPos>=0 && rPos<theMax)
		{
			if(nEntry==mstData[rPos].nEntry)
			{
				if(pIdx) *pIdx = rPos;
				kKey = mstData[rPos].kKey;
				vVal = mstData[rPos].vVal;
				rPos = this->NextBit1(rPos+1);
				return true;
			}
			rPos = this->NextBit1(rPos+1);
		}
		return false;
	}

protected:
	uint32 mnBckt;
	int32 mnNodes[theMax+NsBcktMax(theDept)*theEntry];
	NsData mstData[theMax];

	inline int32 GetHashIdx(uint32 nEntry, TKey kKey) { return theMax+nEntry*mnBckt+Hash(kKey); };
	inline uint32 Hash(TKey kKey)
	{
		uint32 nHash = 2166136261U;
		NPCSTR szKey = (NPCSTR)&kKey;
		for(uint32 i=0; i<sizeof(kKey); i++)
			nHash = 16777619U*nHash^(uint32)*szKey++;
		return (nHash)%mnBckt;
	}
	inline bool IsEqual(TKey& kKey1, TKey& kKey2) { return bool(memcmp(&kKey1, &kKey2, sizeof(kKey1))==0); };

	int32 FindKey(TKey kKey, int32& nLast)
	{
		int32 nIdx = mnNodes[nLast];
		while(nIdx>=0)
		{
			if(IsEqual(kKey, mstData[nIdx].kKey)) return nIdx;
			nLast = nIdx;
			nIdx = mnNodes[nLast];
		}
		return -1;
	}
};
