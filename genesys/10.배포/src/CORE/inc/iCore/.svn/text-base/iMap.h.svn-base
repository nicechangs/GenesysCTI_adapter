//
//  iMap.h
//
//
//  Created by Tony Ryu on 2012.12.7.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iMap.h
 * @brief   Template 기반 Map 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.12.7
 */

#pragma once

#include "iObj.h"

#define DEFCNT_NODEUNIT				(10)

/**
 * @class    iMap
 * @brief    Template 기반 Map 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.12.7
 *
 * @param    TMapKey   Map의 Key Type
 * @param    TMapVal   Map의 저장될 Value Type
 * @param    nBlkSize  저장소 크기 확장 단위
 */
template <typename TMapKey, typename TMapVal, int32 nBlkSize = DEFCNT_NODEUNIT>
class ILIBAPI iMap : public iObj
{
	typedef struct iMapNode
	{
		iMapNode* pNext;
		TMapKey mKey;
		TMapVal mValue;
	} IMAPNODE, *PMAPNODE, **PPMAPNODE;
public:
	iMap()							///< 생성자.
	{
		mpHashTable = NULL;
		mnHashTableSize = 17;
		mnCnt = 0;
		mpFree = NULL;
		mpBlocks = NULL;
		mnBlkSize = nBlkSize;
	}
	virtual ~iMap()					///< 소멸자.
	{
		Clear();
	}

	inline int32 GetCnt() { return mnCnt; };
	bool IsEmpty() { return bool(mnCnt==0); };

	/**
	 * @brief  특정 Key가 존재하는지 확인 함수
	 * @return             존재 여부
	 * @param  mKey        특정 Key
	 */
	bool Lookup(TMapKey mKey)
	{
		uint32 nHash;
		PMAPNODE pNode = GetNodeAt(mKey, nHash);
		if(pNode == NULL) return false;
		return true;
	}

	/**
	 * @brief  특정 Key가 존재하는지 확인 후 값을 반환하는 함수
	 * @return             성공 여부
	 * @param  mKey        특정 Key
	 * @param  rValue      반환할 값의 버퍼
	 */
	bool Lookup(TMapKey mKey, TMapVal& rValue)
	{
		uint32 nHash;
		PMAPNODE pNode = GetNodeAt(mKey, nHash);
		if(pNode == NULL) return false;
		rValue = pNode->mValue;
		return true;
	}

	/**
	 * @brief  배열 연산자 함수
	 * @return                 해당 값(참조형)을 반환
	 * @param  mKey            특정 Key
	 * @note 주의 사항
	 * - 특정 키가 존재하지 않을 경우 새로 Key를 만듬
	 */
	TMapVal& operator[](TMapKey mKey)
	{
		uint32 nHash;
		PMAPNODE pNode;
		if((pNode = GetNodeAt(mKey, nHash)) == NULL)
		{
			if(mpHashTable == NULL)
				InitHashTable(mnHashTableSize);
			pNode = NewNode();

			pNode->mKey = mKey;
			pNode->pNext = mpHashTable[nHash];
			mpHashTable[nHash] = pNode;
		}
		return pNode->mValue;
	}

	/**
	 * @brief 특정 Key에 값을 설정하는 함수
	 * @param mKey       Key
	 * @param newValue   설정할 값
	 */
	void SetAt(TMapKey mKey, TMapVal newValue) { (*this)[mKey] = newValue; };

	/**
	 * @brief  특정 Key를 삭제하는 함수
	 * @return                삭제 여부
	 * @param  mKey           특정 Key
	 */
	bool RemoveKey(TMapKey mKey)
	{
		if(mpHashTable == NULL) return false;

		PPMAPNODE ppNodePrev;
		ppNodePrev = &mpHashTable[HashKey(mKey) % mnHashTableSize];

		PMAPNODE pNode;
		for(pNode = *ppNodePrev; pNode != NULL; pNode = pNode->pNext)
		{
			if(CompareKey(pNode->mKey, mKey))
			{
				*ppNodePrev = pNode->pNext;
				FreeNode(pNode);
				return true;
			}
			ppNodePrev = &pNode->pNext;
		}
		return false;
	}

	/**
	 * @brief Map을 초기화 하는 함수
	 */
	void Clear()
	{
		PMAPNODE pTemp;
		if(mpHashTable != NULL)
		{
			delete[] mpHashTable;
			mpHashTable = NULL;
		}

		mnCnt = 0;
		mpFree = NULL;
		while(mpBlocks)
		{
			pTemp = mpBlocks;
			mpBlocks = mpBlocks->pNext;
			delete []pTemp;
		}
	}

	/**
	 * @brief   Map의 최초 값의 Position Value를 반환 
	 * @return  Position Value
	 */
	NPos GetStartPos()
	{
		return (mnCnt == 0) ? NULL : NBEFORE_STARTPOS;
	}

	/**
	 * @brief  Map의 현재 값을 반환하고 rNextPos를 다음 Position값으로 지정
	 * @return                  다음 값의 존재 여부
	 * @param  rNextPos         다음 값이 저장될 Position Value
	 * @param  rKey             Key가 저장될 버퍼
	 * @param  rValue           값이 저장될 버퍼
@code
iMap<int32, int32> mapInt;
...
int32 nKey, nVal;
NPos pos = mapInt.GetStartPos();
while(mapInt.GetNextNode(pos, nKey, nVal))
{
	...
}
@endcode
	 */
	bool GetNextNode(NPos& rNextPos, TMapKey& rKey, TMapVal& rValue)
	{
		uint32 nBucket;
		PMAPNODE pNodeRet = (PMAPNODE)rNextPos;
		if(pNodeRet == (PMAPNODE)NBEFORE_STARTPOS)
		{
			for(nBucket = 0; nBucket < mnHashTableSize; nBucket++)
				if((pNodeRet = mpHashTable[nBucket]) != NULL) break;
		}
		if(pNodeRet)
		{
			PMAPNODE pNodeNext;
			if((pNodeNext = pNodeRet->pNext) == NULL)
			{
				for(nBucket = (HashKey(pNodeRet->mKey) % mnHashTableSize) + 1; nBucket < mnHashTableSize; nBucket++)
					if((pNodeNext = mpHashTable[nBucket]) != NULL) break;
			}
			rNextPos = (NPos)pNodeNext;
			rKey = pNodeRet->mKey;
			rValue = pNodeRet->mValue;
			return true;
		}
		return false;
	}

protected:
	PPMAPNODE mpHashTable;
	uint32 mnHashTableSize;
	int32 mnCnt;
	PMAPNODE mpFree;
	PMAPNODE mpBlocks;
	int32 mnBlkSize;

	uint32 GetHashTableSize() { return mnHashTableSize; };
	void InitHashTable(uint32 nHashSize, bool bAllocNow = true)
	{
		if(mnCnt == 0 && nHashSize>0)
		{
			if(mpHashTable != NULL)
			{
				delete[] mpHashTable;
				mpHashTable = NULL;
			}

			if(bAllocNow)
			{
				mpHashTable = new PMAPNODE[nHashSize];
				memset(mpHashTable, 0, sizeof(PMAPNODE) * nHashSize);
			}
			mnHashTableSize = nHashSize;
		}
	}

	virtual uint32 HashKey(TMapKey mKey) { return ((uint64)mKey)>>4; };
	virtual bool CompareKey(TMapKey mKey1, TMapKey mKey2) { return bool(mKey1==mKey2); };

	PMAPNODE NewNode()
	{
		int32 i;
		PMAPNODE pNode;
		if(mpFree == NULL)
		{
			pNode = new IMAPNODE[mnBlkSize+1];
			pNode->pNext = mpBlocks;
			mpBlocks = pNode;
			pNode += mnBlkSize;
			for(i=mnBlkSize-1; i>=0; i--, pNode--)
			{
				pNode->pNext = mpFree;
				mpFree = pNode;
			}
		}
		pNode = mpFree;
		mpFree = mpFree->pNext;
		mnCnt++;
		return pNode;
	}
	void FreeNode(PMAPNODE pNode)
	{
		pNode->pNext = mpFree;
		mpFree = pNode;
		mnCnt--;
		if(mnCnt == 0) Clear();
	}
	PMAPNODE GetNodeAt(TMapKey mKey, uint32& nHash)
	{
		nHash = HashKey(mKey) % mnHashTableSize;
		if(mpHashTable == NULL) return NULL;
		PMAPNODE pNode;
		for(pNode = mpHashTable[nHash]; pNode != NULL; pNode = pNode->pNext)
		{
			if(CompareKey(pNode->mKey, mKey)) return pNode;
		}
		return NULL;
	}
public:
	bool GetValueAt(TMapKey mKey, TMapVal& mValue)
	{
		if(mpHashTable == NULL) return false;
		uint32 nHash = HashKey(mKey) % mnHashTableSize;
		PMAPNODE pNode = mpHashTable[nHash];
		for(; pNode != NULL; pNode = pNode->pNext)
		{
			if(CompareKey(pNode->mKey, mKey))
			{
				mValue = pNode->mValue;
				return true;
			}
		}
		return false;
	}
};
