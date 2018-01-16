//
//  iGrid.h
//  
//
//  Created by Tony Ryu on 2014.  .  .
//  Copyright (c) 2014 Tony Ryu. All rights reserved.
//

/**
 * @file     iGrid.h
 * @brief    
 *
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @version  v0.1
 * @see      None
 * @date     2014.  .  .
 */

#pragma once

#include "iObj.h"

typedef struct iGridIdx
{
	uint32			xPos;
	uint32			yPos;
	uint32			zPos;
	uint32			tPos;
	iGridIdx(uint32 x=0, uint32 y=0, uint32 z=0, uint32 t=0)
	{ xPos=x; yPos=y; zPos=z; tPos=t; };
} iGridIdx;

/**
 * @class    iGrid
 * @brief    
 */
class ILIBAPI iGrid : public iObj
{
public:
	enum eType					///< Sqlite와 Type을 맞춤.
	{
		ITYPE_NONE		= 0,	///< 해당 Data가 존재하지 않음.
		ITYPE_INT		,		///< Integer Data.
		ITYPE_FLOAT		,		///< Float Data.
		ITYPE_TEXT		,		///< Text Data(NULL로 종료하는).
		ITYPE_BLOB		,		///< Binary Data.
		ITYPE_NULL				///< Null Data.
	};

	iGrid();
	virtual ~iGrid();

	void Reset();
	inline bool IsInit() { return bool(mpRecs!=NULL); };

	size_t Total();

	inline uint32 MaxX() { return msMax.xPos; };
	inline uint32 MaxY() { return msMax.yPos; };
	inline uint32 MaxZ() { return msMax.zPos; };
	inline uint32 MaxT() { return msMax.tPos; };

	bool IsExist(iGridIdx sIdx);
	eType Type(iGridIdx sIdx);
	NPCSTR Types(iGridIdx sIdx);

	int64 Int(iGridIdx sIdx, int64 nDef = 0);
	double Flt(iGridIdx sIdx, double fDef = 0.0);
	NPCSTR Txt(iGridIdx sIdx);
	NPVOID Blb(iGridIdx sIdx, size_t *pSize = NULL);

	bool SetInt(iGridIdx sIdx, int64 nVal);
	bool SetFlt(iGridIdx sIdx, double fVal);
	bool SetTxt(iGridIdx sIdx, NPCSTR szVal);
	bool SetTxts(iGridIdx sIdx, NPCSTR szVal, ...);
	bool SetBlb(iGridIdx sIdx, NPVOID sbVal, size_t nSize);
	bool SetNil(iGridIdx sIdx);

protected:
	iGridIdx msMax;
	NPVOID mpRecs;

	NPVOID GetData(iGridIdx sIdx);
	bool SetData(iGridIdx sIdx, NPVOID pData);
	bool Compare(iGridIdx sIdx0, iGridIdx sIdx1);

	DISALLOW_COPY_AND_ASSIGN(iGrid);
};


