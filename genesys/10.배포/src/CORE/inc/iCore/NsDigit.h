//
//  NsDigit.h
//  NsLib
//
//  Created by Tony Ryu on 2012. 10. 31..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    NsDigit.h
 * @brief   
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 31.
 */

#pragma once

#include "NsObj.h"

enum
{
	DGTYPE_NULL = 0
};
enum
{	DIGIT_NULL=-1,	DIGIT_0=0,	DIGIT_1,	DIGIT_2,	DIGIT_3,	DIGIT_4,	DIGIT_5,
	DIGIT_6,		DIGIT_7,	DIGIT_8,	DIGIT_9,	DIGIT_A,	DIGIT_S,	MAXCNT_DIGITS
};

static char theDgts[MAXCNT_DIGITS] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

template <uint32 cDgtLen, uint32 theMax, uint32 theEntry = 1>
class NsDigit : public NsObj<theMax>
{
	typedef struct
	{
		int16   nChld;		// Child Digit Index(-1:None)
		uint16  nType;		// Digit Type (NULL, EXTDN, TRUNK, AGTDN, GRPDN, FCODE, ACODE, EMGCY)
		uint16  nAIdx;		// Instance Index
		uint16  nChId;		// Channel ID
	} NsItem;
	typedef struct
	{
		NsItem  iItem[MAXCNT_DIGITS];
	} NsData;
public:
	NsDigit() {};
	~NsDigit() {};
	bool InitMap()
	{
		uint16 i, j;
		if(theMax>IMAX_INT16 || !this->Init()) return false;
		memset(mstEtry, 0x00, sizeof(mstEtry));
		memset(mstData, 0x00, sizeof(mstData));
		for(i=0; i<(sizeof(mstEtry)/sizeof(mstEtry[0])); i++)
			{ for(j=0; j<MAXCNT_DIGITS; j++) mstEtry[i].iItem[j].nChld = -1; };
		for(i=0; i<(sizeof(mstData)/sizeof(mstData[0])); i++)
			{ for(j=0; j<MAXCNT_DIGITS; j++) mstData[i].iItem[j].nChld = -1; };
		return true;
	}
	bool AddDigits(uint16 nEntry, NPCSTR szDgt, uint16 nType, uint16 nAIdx, uint16 nChId)
	{
		int8 nDgt[cDgtLen];
		if(CvtDigits(szDgt, nDgt)) return Insert(&mstEtry[nEntry], nDgt, nType, nAIdx, nChId);
		return false;
	}
	bool GetDigits(uint16 nEntry, NPCSTR szDgt, uint16& nType, uint16& nAIdx, uint16& nChId)
	{
		int8 nDgt[cDgtLen];
		const NsItem* pItem = NULL;
		if(CvtDigits(szDgt, nDgt)) pItem = Select(&mstEtry[nEntry], nDgt);
		if(pItem)
		{
			nType = pItem->nType;
			nAIdx = pItem->nAIdx;
			nChId = pItem->nChId;
			return true;
		}
		return false;
	}
	bool DelDigits(uint16 nEntry, NPCSTR szDgt)
	{
		int8 nDgt[cDgtLen];
		if(CvtDigits(szDgt, nDgt)) return Delete(&mstEtry[nEntry], nDgt);
		return false;
	}

	inline void DispDigits(uint16 nEntry) { DispDigits(&mstEtry[nEntry]); };

protected:
	NsData mstEtry[theEntry];
	NsData mstData[theMax];

	inline bool CvtDigits(NPCSTR szSrc, int8* nDst)
	{
		int32 i;
		for(i=0; i<(cDgtLen-1); i++)
		{
			switch(szSrc[i])
			{	case '0':	nDst[i] = DIGIT_0;	break;	case '1':	nDst[i] = DIGIT_1;	break;
			case '2':	nDst[i] = DIGIT_2;	break;	case '3':	nDst[i] = DIGIT_3;	break;
			case '4':	nDst[i] = DIGIT_4;	break;	case '5':	nDst[i] = DIGIT_5;	break;
			case '6':	nDst[i] = DIGIT_6;	break;	case '7':	nDst[i] = DIGIT_7;	break;
			case '8':	nDst[i] = DIGIT_8;	break;	case '9':	nDst[i] = DIGIT_9;	break;
			case '*':	nDst[i] = DIGIT_A;	break;	case '#':	nDst[i] = DIGIT_S;	break;
			case '\0':	nDst[i] = DIGIT_NULL;	return bool(i>0);
			default:	nDst[i] = DIGIT_NULL;	return false;
			}
		}
		nDst[i] = DIGIT_NULL;
		return bool(i>0);
	}

	bool Insert(NsData* pData, int8* nDgt, uint16 nType, uint16 nAIdx, uint16 nChId)
	{
		NsItem *pItem = NULL;
		while((*nDgt)!=DIGIT_NULL)
		{
			pItem = &pData->iItem[*nDgt++];
			if((*nDgt)!=DIGIT_NULL)
			{
				if(pItem->nChld<0)
				{
					if((pItem->nChld = this->AllocNode())<0) return false;
				}
				pData = &mstData[pItem->nChld];
			}
		}
		if(pItem && pItem->nType==DGTYPE_NULL)
		{
			pItem->nType = nType; pItem->nAIdx = nAIdx; pItem->nChId = nChId;
			return true;
		}
		return false;
	}

	const NsItem* Select(NsData* pData, int8* nDgt)
	{
		NsItem *pItem, *pRet = NULL;
		while((*nDgt)!=DIGIT_NULL)
		{
			pItem = &pData->iItem[*nDgt++];
			if(pItem->nType!=DGTYPE_NULL) pRet = pItem;
			if(pItem->nChld>=0) pData = &mstData[pItem->nChld];
			else break;
		}
		return pRet;
	}

	bool Delete(NsData* pData, int8* nDgt)
	{
		NsItem* pItem = &pData->iItem[*nDgt++];
		if((*nDgt)==DIGIT_NULL)
		{
			if(pItem->nType==DGTYPE_NULL) return false;
			pItem->nType = DGTYPE_NULL;
			return true;
		}
		if(pItem->nChld>=0)
		{
			pData = &mstData[pItem->nChld];
			if(Delete(pData, nDgt))
			{
				int32 i;
				for(i=0; i<MAXCNT_DIGITS; i++)
				{
					if(pData->iItem[i].nChld>=0 || pData->iItem[i].nType!=DGTYPE_NULL) break;
				}
				if(i==MAXCNT_DIGITS)
				{
					int32 nChld = pItem->nChld;
					pItem->nChld = -1;
					this->FreeNode(nChld);
				}
				return true;
			}
		}
		return false;
	}

	void DispDigits(NsData* pData, int32 nDepth = 0, char* szDepth = (char*)"")
	{
		char szLine[MAXLEN_LINE] = {0,};
		NsItem* pItem = &pData->iItem[0];
		for(int32 i=0; i<MAXCNT_DIGITS; i++)
		{
			pItem = &pData->iItem[i];
			switch(pItem->nType)
			{
				case DGTYPE_NULL:
				if(pItem->nChld>=0)
				{
					iTrace("%s+[%c]", szDepth, theDgts[i]);
					sprintf(szLine, "%s  ", szDepth);
					DispDigits(&mstData[pItem->nChld], nDepth+1, szLine);
				}
				break;
				default:
				iTrace("%s+[%c] Exist [%d/%d/%d]", szDepth, theDgts[i], pItem->nType, pItem->nAIdx, pItem->nChId);
				if(pItem->nChld>=0)
				{
					sprintf(szLine, "%s  ", szDepth);
					DispDigits(&mstData[pItem->nChld], nDepth+1, szLine);
				}
				break;
			}
		}
	}
};
