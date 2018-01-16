//
//  iQue.h
//  iCore
//
//  Created by Tony Ryu on 2012. 10. 4..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    iQue.h
 * @brief   Template 기반 Queue 처리 
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 4.
 */

#pragma once

#include "iObj.h"

#define DEFCNT_NODEUNIT				(10)

/**
 * @class    iQue
 * @brief    Template 기반 Queue 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.10.4
 *
 * @param    TQueVal   Queue의 저장될 Value Type
 * @param    nBlkSize  저장소 크기 확장 단위
 */
template <typename TQueVal, uint32 nBlkSize = DEFCNT_NODEUNIT>
class ILIBAPI iQue : public iObj
{
	typedef struct iQueNode
	{
		iQueNode* pNext;
		iQueNode* pPrev;
		TQueVal qVal;
	} IQUENODE, *PQUENODE, **PPQUENODE;
public:
	iQue()									///< 생성자.
	{
		mnCnt = 0;
		mpHead = mpTail = mpFree = NULL;
		mpBlks = NULL;
		mnBlkSize = nBlkSize;
	}
	virtual ~iQue()							///< 소멸자.
	{
		Clear();
	}
	inline uint32 GetCnt() const { return mnCnt; };
	inline bool IsEmpty() const { return mnCnt == 0; };

	/**
	 * @brief  Head 값을 반환 하고 Pop 한다.
	 * @return              성공 여부
	 * @param  qRet         값을 받아올 버퍼
	 */
	bool PopHead(TQueVal& qRet)
	{
		if(mpHead)
		{
			PQUENODE pOldNode = mpHead;
			qRet = pOldNode->qVal;

			mpHead = pOldNode->pNext;
			if(mpHead != NULL) mpHead->pPrev = NULL;
			else mpTail = NULL;
			FreeNode(pOldNode);
			return true;
		}
		return false;
	}

	/**
	 * @brief   Head 값만 반환한다.
	 * @return              성공 여부
	 * @param  qRet         값을 받아올 버퍼
	 */
	bool Head(TQueVal& qRet)
	{
		if(mpHead)
		{
			PQUENODE pOldNode = mpHead;
			qRet = pOldNode->qVal;
			return true;
		}
		return false;
	}

	/**
	 * @brief  Head 값을 Pop만 한다.
	 * @return 성공 여부
	 */
	bool PopHead()
	{
		if(mpHead)
		{
			PQUENODE pOldNode = mpHead;
			mpHead = pOldNode->pNext;
			if(mpHead != NULL) mpHead->pPrev = NULL;
			else mpTail = NULL;
			FreeNode(pOldNode);
			return true;
		}
		return false;
	}

	/**
	 * @brief  Tail 값을 반환 하고 Pop 한다.
	 * @return              성공 여부
	 * @param  qRet         값을 받아올 버퍼
	 */
	bool PopTail(TQueVal& qRet)
	{
		if(mpTail)
		{
			PQUENODE pOldNode = mpTail;
			qRet = pOldNode->qVal;

			mpTail = pOldNode->pPrev;
			if(mpTail != NULL) mpTail->pNext = NULL;
			else mpHead = NULL;
			FreeNode(pOldNode);
			return true;
		}
		return false;
	}

	/**
	 * @brief   Tail 값만 반환한다.
	 * @return              성공 여부
	 * @param  qRet         값을 받아올 버퍼
	 */
	bool Tail(TQueVal& qRet)
	{
		if(mpTail)
		{
			PQUENODE pOldNode = mpTail;
			qRet = pOldNode->qVal;
			return true;
		}
		return false;
	}

	/**
	 * @brief  Tail 값을 Pop만 한다.
	 * @return 성공 여부
	 */
	bool PopTail()
	{
		if(mpTail)
		{
			PQUENODE pOldNode = mpTail;
			mpTail = pOldNode->pPrev;
			if(mpTail != NULL) mpTail->pNext = NULL;
			else mpHead = NULL;
			FreeNode(pOldNode);
			return true;
		}
		return false;
	}

	/**
	 * @brief  Head 측에 값을 Push 한다.
	 * @return               Push된 값의 Position Value
	 * @param  qVal          Push할 값
	 */
	NPos PushHead(TQueVal qVal)
	{
		PQUENODE pNewNode = NewNode(NULL, mpHead);
		pNewNode->qVal = qVal;
		if(mpHead != NULL) mpHead->pPrev = pNewNode;
		else mpTail = pNewNode;
		mpHead = pNewNode;
		return (NPos)pNewNode;
	}

	/**
	 * @brief  Tail 측에 값을 Push 한다.
	 * @return               Push된 값의 Position Value
	 * @param  qVal          Push할 값
	 */
	NPos PushTail(TQueVal qVal)
	{
		PQUENODE pNewNode = NewNode(mpTail, NULL);
		pNewNode->qVal = qVal;
		if(mpTail != NULL) mpTail->pNext = pNewNode;
		else mpHead = pNewNode;
		mpTail = pNewNode;
		return (NPos)pNewNode;
	}

	/**
	 * @brief 모든 값을 초기화 한다.
	 */
	void Clear()
	{
		PQUENODE pTemp;
		mnCnt = 0;
		mpHead = mpTail = mpFree = NULL;
		while(mpBlks)
		{
			pTemp = mpBlks;
			mpBlks = mpBlks->pNext;
			delete []pTemp;
		}
	}

	/**
	 * @brief 특정 위치의 값을 제거한다.
	 * @param pos           값의 Position Value
	 */
	void RemoveAt(NPos pos)
	{
		PQUENODE pOldNode = (PQUENODE)pos;
		if(pOldNode == mpHead) mpHead = pOldNode->pNext;
		else pOldNode->pPrev->pNext = pOldNode->pNext;
		if(pOldNode == mpTail) mpTail = pOldNode->pPrev;
		else pOldNode->pNext->pPrev = pOldNode->pPrev;
		FreeNode(pOldNode);
	}

	/**
	 * @brief  특정 위치 앞에 값을 추가하는 함수
	 * @return                   Insert 된 값의 Position Value
	 * @param  pos               Insert 할 Position Value
	 * @param  qVal              Insert 할 값
	 */
	NPos InsertBefore(NPos pos, TQueVal& qVal)
	{
		if(pos == NULL) return PushHead(qVal);
		PQUENODE pOldNode = (PQUENODE)pos;
		PQUENODE pNewNode = NewNode(pOldNode->pPrev, pOldNode);
		pNewNode->qVal = qVal;
		if(pOldNode->pPrev != NULL) pOldNode->pPrev->pNext = pNewNode;
		else mpHead = pNewNode;
		pOldNode->pPrev = pNewNode;
		return (NPos)pNewNode;
	}

	/**
	 * @brief  특정 위치 뒤에 값을 추가하는 함수
	 * @return                   Insert 된 값의 Position Value
	 * @param  pos               Insert 할 Position Value
	 * @param  qVal              Insert 할 값
	 */
	NPos InsertAfter(NPos pos, TQueVal& qVal)
	{
		if(pos == NULL) return PushTail(qVal);
		PQUENODE pOldNode = (PQUENODE)pos;
		PQUENODE pNewNode = NewNode(pOldNode, pOldNode->pNext);
		pNewNode->qVal = qVal;
		if(pOldNode->pNext != NULL) pOldNode->pNext->pPrev = pNewNode;
		else mpTail = pNewNode;
		pOldNode->pNext = pNewNode;
		return (NPos)pNewNode;
	}

	/**
	 * @brief  값으로 특정 위치를 찾는 함수
	 * @return            검색된 값의 Position Value
	 * @param  qSearch    검색 할 값
	 * @param  startAfter 지정 위치 이후 부터 검색(NULL일 경우 처음부터 검색)
	 */
	NPos Find(TQueVal& qSearch, NPos startAfter = NULL) const
	{
		PQUENODE pNode = (PQUENODE)startAfter;
		if(pNode == NULL) pNode = mpHead;
		else pNode = pNode->pNext;
		for(; pNode != NULL; pNode = pNode->pNext)
			if(pNode->qVal == qSearch)
				return (NPos)pNode;
		return NULL;
	}

	/**
	 * @brief  특정 Index(Head기반)의 위치를 찾는 함수
	 * @return                해당 Index의 Position Value
	 * @param  nIdx           특정 Index(Head 0 Based Index)
	 */
	NPos FindIndex(int32 nIdx) const
	{
		if(nIdx >= mnCnt || nIdx < 0) return NULL;
		PQUENODE pNode = mpHead;
		while(nIdx--) pNode = pNode->pNext;
		return (NPos) pNode;
	}

	/**
	 * @brief  Head 값을 반환(참조형)하는 함수
	 * @return Head 값(참조형)
	 * @warning Head가 반드시 존재하는것이 확인되었을때 호출할 것
	 */
	inline TQueVal& GetHead() { return mpHead->qVal; };

	/**
	 * @brief  Head 값을 반환하는 함수
	 * @return Head 값
	 * @warning Head가 반드시 존재하는것이 확인되었을때 호출할 것
	 */
	inline TQueVal GetHead() const { return mpHead->qVal; };

	/**
	 * @brief  Tail 값을 반환(참조형)하는 함수
	 * @return Tail 값(참조형)
	 * @warning Tail이 반드시 존재하는것이 확인되었을때 호출할 것
	 */
	inline TQueVal& GetTail() { return mpTail->qVal; };

	/**
	 * @brief  Tail 값을 반환하는 함수
	 * @return Tail 값
	 * @warning Tail이 반드시 존재하는것이 확인되었을때 호출할 것
	 */
	inline TQueVal GetTail() const { return mpTail->qVal; };

	/**
	 * @brief  Head의 Position Value 반환
	 * @return                 Head Position Value
	 */
	inline NPos GetHeadPos() const { return (NPos)mpHead; };

	/**
	 * @brief  Tail의 Position Value 반환
	 * @return                 Tail Position Value
	 */
	inline NPos GetTailPos() const { return (NPos)mpTail; };

	/**
	 * @brief  특정 Position 값 반환 후 다음(Next) Position 반환
	 * @return              Next Position Value
	 * @param  rPos         현재 Position Value
	 * @param  qVal         현재 Position에 해당하는 값
@code
iQue<int32> queInt;
...
int32 nVal;
NPos pos = queInt.GetHeadPos();
while(queInt.GetNext(pos, nKey, nVal))
{
	...
}
@endcode
	 */
	inline bool GetNext(NPos& rPos, TQueVal& qVal)
	{
		PQUENODE pNode = (PQUENODE)rPos;
		if(pNode)
		{
			rPos = (NPos)pNode->pNext;
			qVal = pNode->qVal;
			return true;
		}
		return false;
	}

	/**
	 * @brief  특정 Position 값 반환 후 이전(Prev) Position 반환
	 * @return              Prev Position Value
	 * @param  rPos         현재 Position Value
	 * @param  qVal         현재 Position에 해당하는 값
	 */
	inline bool GetPrev(NPos& rPos, TQueVal& qVal)
	{
		PQUENODE pNode = (PQUENODE)rPos;
		if(pNode)
		{
			rPos = (NPos)pNode->pPrev;
			qVal = pNode->qVal;
			return true;
		}
		return false;
	}

	/**
	 * @brief  특정 Position의 값을 반환
	 * @return            성공 여부
	 * @param  pos        Position Value
	 * @param  qVal       반환 값 저장 버퍼
	 */
	inline bool GetAt(NPos pos, TQueVal& qVal)
	{
		PQUENODE pNode = (PQUENODE)pos;
		if(pNode)
		{
			qVal = pNode->qVal;
			return true;
		}
		return false;
	}

	/**
	 * @brief  특정 Position에 값을 설정
	 * @return            성공 여부
	 * @param  pos        Position Value
	 * @param  qVal       저장할 값 지정
	 */
	inline bool SetAt(NPos pos, TQueVal& qVal)
	{
		PQUENODE pNode = (PQUENODE)pos;
		if(pNode)
		{
			pNode->qVal = qVal;
			return true;
		}
		return false;
	}

protected:
	PQUENODE mpHead;
	PQUENODE mpTail;
	uint32 mnCnt;
	PQUENODE mpFree;
	PQUENODE mpBlks;
	uint32 mnBlkSize;

	PQUENODE NewNode(PQUENODE pPrev, PQUENODE pNext)
	{
		int32 i;
		PQUENODE pNode;
		if(mpFree == NULL)
		{
			pNode = new IQUENODE[mnBlkSize+1];
			pNode->pNext = mpBlks;
			mpBlks = pNode;
			pNode += mnBlkSize;
			for(i=mnBlkSize-1; i>=0; i--, pNode--)
			{
				pNode->pNext = mpFree;
				mpFree = pNode;
			}
		}
		pNode = mpFree;
		mpFree = mpFree->pNext;
		pNode->pPrev = pPrev;
		pNode->pNext = pNext;
		mnCnt++;
		return pNode;
	}

	void FreeNode(PQUENODE pNode)
	{
		pNode->pNext = mpFree;
		mpFree = pNode;
		mnCnt--;
		if(mnCnt == 0) Clear();
	}
};
