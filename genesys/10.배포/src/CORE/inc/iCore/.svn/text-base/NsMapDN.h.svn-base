//
//  NsMapDN.h
//  NsLib
//
//  Created by Tony Ryu on 2012. 11. 1..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    NsMapDN.h
 * @brief   
 *
 * @version v0.1
 * @see     None
 * @date    2012. 11. 1.
 */

#pragma once

#include "NsObj.h"

template <uint32 cKeyLen, uint32 cValLen, uint32 theMax, uint32 theEntry = 1, uint32 theDept = theMax>
class NsMapDN : public NsObj<theMax>
{
	typedef struct
	{
		uint32  nEntry;
		char    szKey[cKeyLen];
		char    szVal[cValLen];
	} NsData;
public:
	NsMapDN() {};
	~NsMapDN() {};
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

	inline NPCSTR GetKey(uint32 nIdx) { return this->GetBit(nIdx%theMax)?mstData[nIdx%theMax].szKey:NULL; };
	inline NPSTR GetVal(uint32 nIdx) { return this->GetBit(nIdx%theMax)?mstData[nIdx%theMax].szVal:NULL; };

	inline int32 GetIdx(uint32 nEntry, NPCSTR szKey)
	{
		int32 nLast = GetHashIdx(nEntry, szKey);
		return FindKey(szKey, nLast);
	}
	inline bool IsExist(uint32 nEntry, NPCSTR szKey)
	{
		if(GetIdx(nEntry, szKey)<0) return false;
		else return true;
	}

	inline bool Insert(uint32 nEntry, NPCSTR szKey, NPCSTR szVal, int32* pIdx = NULL, bool bIsExistErr = true)
	{
		int32 nHash = GetHashIdx(nEntry, szKey);
		int32 nIdx, nLast = nHash;
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(szKey, nLast);
		if(*pIdx<0)
		{
			*pIdx = this->AllocNode();
			if(*pIdx<0) return false;
			mstData[*pIdx].nEntry = nEntry;
			memcpy(mstData[*pIdx].szKey, szKey, sizeof(mstData[*pIdx].szKey));
			memcpy(mstData[*pIdx].szVal, szVal, sizeof(mstData[*pIdx].szVal));
			mnNodes[*pIdx] = mnNodes[nHash];
			mnNodes[nHash] = *pIdx;
		}
		else
		{
			if(bIsExistErr) return false;
			memcpy(mstData[*pIdx].szVal, szVal, sizeof(mstData[*pIdx].szVal));
		}
		return true;
	}
	inline bool Update(uint32 nEntry, NPCSTR szKey, NPCSTR szVal, int32* pIdx = NULL, bool bNoExistErr = true)
	{
		int32 nHash = GetHashIdx(nEntry, szKey);
		int32 nIdx, nLast = nHash;
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(szKey, nLast);
		if(*pIdx<0)
		{
			if(bNoExistErr) return false;
			*pIdx = this->AllocNode();
			if(*pIdx<0) return false;
			memcpy(mstData[*pIdx].szKey, szKey, sizeof(mstData[*pIdx].szKey));
			memcpy(mstData[*pIdx].szVal, szVal, sizeof(mstData[*pIdx].szVal));
			mnNodes[*pIdx] = mnNodes[nHash];
			mnNodes[nHash] = *pIdx;
		}
		else memcpy(mstData[*pIdx].szVal, szVal, sizeof(mstData[*pIdx].szVal));
		return true;
	}
	inline bool Select(uint32 nEntry, NPCSTR szKey, NPSTR szVal, uint32 nVal = cValLen, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, szKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(szKey, nLast);
		if(*pIdx<0 || nVal<1) return false;
		nVal = IMIN(nVal, sizeof(mstData[*pIdx].szVal));
		memcpy(szVal, mstData[*pIdx].szVal, nVal);
		szVal[nVal-1] = '\0';
		return true;
	}
	inline NPSTR Select(uint32 nEntry, NPCSTR szKey, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, szKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(szKey, nLast);
		if(*pIdx<0) return NULL;
		return mstData[*pIdx].szVal;
	}
	inline bool Delete(uint32 nEntry, NPCSTR szKey, int32* pIdx = NULL)
	{
		int32 nIdx, nLast = GetHashIdx(nEntry, szKey);
		if(!pIdx) pIdx = &nIdx;
		*pIdx = FindKey(szKey, nLast);
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
	bool Next(int32& rPos, NPSTR szKey, NPSTR szVal, int32* pIdx = NULL)
	{
		if(rPos>=0 && rPos<theMax)
		{
			if(pIdx) *pIdx = rPos;
			if(szKey) strcpy(szKey, mstData[rPos].szKey);
			if(szVal) strcpy(szVal, mstData[rPos].szVal);
			rPos = this->NextBit1(rPos+1);
			return true;
		}
		return false;
	}
	int32 FindEntry(int32 nPos, uint32 nEntry, NPSTR szKey = NULL, NPSTR szVal = NULL)
	{
		while(!this->IsEmpty())
		{
			nPos = this->NextBit1(nPos+1);
			if(nPos>=0 && nPos<theMax)
			{
				if(nEntry==mstData[nPos].nEntry)
				{
					if(szKey) strcpy(szKey, mstData[nPos].szKey);
					if(szVal) strcpy(szVal, mstData[nPos].szVal);
					return nPos;
				}
			}
			else break;
		}
		return -1;
	}
	bool Next(int32& rPos, uint32 nEntry, NPSTR szKey, NPSTR szVal, int32* pIdx = NULL)
	{
		while(rPos>=0 && rPos<theMax)
		{
			if(nEntry==mstData[rPos].nEntry)
			{
				if(pIdx) *pIdx = rPos;
				strcpy(szKey, mstData[rPos].szKey);
				strcpy(szVal, mstData[rPos].szVal);
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

	inline int32 GetHashIdx(uint32 nEntry, NPCSTR szKey) { return theMax+nEntry*mnBckt+Hash(szKey); };
	inline uint32 Hash(NPCSTR szKey)
	{
		uint32 nHash = 2166136261U;
		while(*szKey) nHash = 16777619U*nHash^(uint32)*szKey++;
		return (nHash)%mnBckt;
	}
	inline bool IsEqual(NPCSTR szKey1, NPCSTR szKey2) { return bool(strcmp(szKey1, szKey2)==0); };
	int32 FindKey(NPCSTR szKey, int32& nLast)
	{
		int32 nIdx = mnNodes[nLast];
		while(nIdx>=0)
		{
			if(IsEqual(szKey, mstData[nIdx].szKey)) return nIdx;
			nLast = nIdx;
			nIdx = mnNodes[nLast];
		}
		return -1;
	}
};
