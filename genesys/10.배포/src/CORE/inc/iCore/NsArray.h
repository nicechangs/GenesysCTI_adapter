//
//  NsArray.h
//  
//
//  Created by Tony Ryu on 2012.12.7.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    NsArray.h
 * @brief
 *
 * @version v0.1
 * @see     None
 * @date    2012.12.7
 */

#pragma once

#include "NsObj.h"

template <typename TVal, uint32 theMax>
class NsArray : public NsObj<theMax>
{
	typedef struct
	{
		TVal    vVal;
	} NsData;
public:
	NsArray() {};
	~NsArray() {};
	bool InitMap()
	{
		if(this->Init())
		{
			memset(mstData, 0x00, sizeof(mstData));
			return true;
		}
		return false;
	}
	inline bool Insert(TVal& vVal, int32* pIdx = NULL)
	{
		int32 nIdx = this->AllocNode();
		if(nIdx>=0) mstData[nIdx].vVal = vVal;
		if(pIdx) *pIdx = nIdx;
		return nIdx<0?false:true;
	}
	inline bool Update(uint16 nIdx, TVal& vVal, bool bNoExistErr = true)
	{
		if(nIdx<theMax)
		{
			if(!this->GetBit(nIdx))
			{
				if(!bNoExistErr) this->AllocNode(nIdx);
				else return false;
			}
			mstData[nIdx].vVal = vVal;
			return true;
		}
		return false;
	}
	inline bool Select(uint16 nIdx, TVal& vVal)
	{
		if(nIdx<theMax)
		{
			vVal = mstData[nIdx].vVal;
			return true;
		}
		return false;
	}
	inline TVal* Select(uint16 nIdx) { return nIdx<theMax?&mstData[nIdx].vVal:NULL; }
	inline bool Delete(uint16 nIdx) { return nIdx<theMax?this->FreeNode(nIdx):false; }

	int32 First() { return this->IsEmpty()?-1:this->FirstBit1(); };
	bool Next(int32& rPos, TVal& vVal, int32* pIdx = NULL)
	{
		if(rPos>=0 && rPos<(int32)theMax)
		{
			if(pIdx) *pIdx = rPos;
			vVal = mstData[rPos].vVal;
			rPos = this->NextBit1(rPos+1);
			return true;
		}
		return false;
	}
protected:
	NsData mstData[theMax];
};
