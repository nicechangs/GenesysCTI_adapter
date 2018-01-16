//
//  NsCode.h
//
//
//  Created by Tony Ryu on 2013.2.27.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    NsCode.h
 * @brief
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.27
 */

#pragma once

#include "NsObj.h"

enum
{
	CDTYPE_NULL = 0
};
enum
{	CODE_NULL=-1,	CODE_0=0,	CODE_1,		CODE_2,		CODE_3,		CODE_4,		CODE_5,
	CODE_6,			CODE_7,		CODE_8,		CODE_9,		CODE_A,		CODE_S,		MAXCNT_CODES
};

static char theCodes[MAXCNT_CODES] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

template <typename UqKey, uint32 cCodeLen, uint32 theMax, uint32 theEntry = 1>
class NsCode : public NsObj<theMax>
{
	typedef struct
	{
		int16	nChld;		// Child Code Index(-1:None)
		uint16	nType;		// Code Type (Feature/Access/Emergency Code)
		UqKey	nUqId;		// Code Id Value
	} NsItem;
	typedef struct
	{
		NsItem  iItem[MAXCNT_CODES];
	} NsData;
public:
	NsCode() {};
	~NsCode() {};
	bool InitMap()
	{
		uint16 i, j;
		if(theMax>IMAX_INT16 || !this->Init()) return false;
		memset(mstEtry, 0x00, sizeof(mstEtry));
		memset(mstData, 0x00, sizeof(mstData));
		for(i=0; i<(sizeof(mstEtry)/sizeof(mstEtry[0])); i++)
			{ for(j=0; j<MAXCNT_CODES; j++) mstEtry[i].iItem[j].nChld = -1; };
		for(i=0; i<(sizeof(mstData)/sizeof(mstData[0])); i++)
			{ for(j=0; j<MAXCNT_CODES; j++) mstData[i].iItem[j].nChld = -1; };
		return true;
	}
	bool AddCodes(uint16 nEntry, NPCSTR szDgt, uint16 nType, UqKey nUqId)
	{
		int8 nCode[cCodeLen];
		if(CvtCodes(szDgt, nCode)) return Insert(&mstEtry[nEntry], nCode, nType, nUqId);
		return false;
	}
	bool GetCodes(uint16 nEntry, NPCSTR szDgt, uint16& nType, UqKey& nUqId)
	{
		int8 nCode[cCodeLen];
		const NsItem* pItem = NULL;
		if(CvtCodes(szDgt, nCode)) pItem = Select(&mstEtry[nEntry], nCode);
		if(pItem)
		{
			nType = pItem->nType;
			nUqId = pItem->nUqId;
			return true;
		}
		return false;
	}
	bool DelCodes(uint16 nEntry, NPCSTR szDgt)
	{
		int8 nCode[cCodeLen];
		if(CvtCodes(szDgt, nCode)) return Delete(&mstEtry[nEntry], nCode);
		return false;
	}

	inline void DispCodes(uint16 nEntry) { DispCodes(&mstEtry[nEntry]); };

protected:
	NsData mstEtry[theEntry];
	NsData mstData[theMax];

	inline bool CvtCodes(NPCSTR szSrc, int8* nDst)
	{
		int32 i;
		for(i=0; i<(cCodeLen-1); i++)
		{
			switch(szSrc[i])
			{	case '0':	nDst[i] = CODE_0;	break;	case '1':	nDst[i] = CODE_1;	break;
			case '2':	nDst[i] = CODE_2;	break;	case '3':	nDst[i] = CODE_3;	break;
			case '4':	nDst[i] = CODE_4;	break;	case '5':	nDst[i] = CODE_5;	break;
			case '6':	nDst[i] = CODE_6;	break;	case '7':	nDst[i] = CODE_7;	break;
			case '8':	nDst[i] = CODE_8;	break;	case '9':	nDst[i] = CODE_9;	break;
			case '*':	nDst[i] = CODE_A;	break;	case '#':	nDst[i] = CODE_S;	break;
			case '\0':	nDst[i] = CODE_NULL;	return bool(i>0);
			default:	nDst[i] = CODE_NULL;	return false;
			}
		}
		nDst[i] = CODE_NULL;
		return bool(i>0);
	}

	bool Insert(NsData* pData, int8* nCode, uint16 nType, UqKey nUqId)
	{
		NsItem *pItem = NULL;
		while((*nCode)!=CODE_NULL)
		{
			pItem = &pData->iItem[*nCode++];
			if((*nCode)!=CODE_NULL)
			{
				if(pItem->nChld<0)
				{
					if((pItem->nChld = this->AllocNode())<0) return false;
				}
				pData = &mstData[pItem->nChld];
			}
		}
		if(pItem && pItem->nType==CDTYPE_NULL)
		{
			pItem->nType = nType; pItem->nUqId = nUqId;
			return true;
		}
		return false;
	}

	const NsItem* Select(NsData* pData, int8* nCode)
	{
		NsItem *pItem, *pRet = NULL;
		while((*nCode)!=CODE_NULL)
		{
			pItem = &pData->iItem[*nCode++];
			if(pItem->nType!=CDTYPE_NULL) pRet = pItem;
			if(pItem->nChld>=0) pData = &mstData[pItem->nChld];
			else break;
		}
		return pRet;
	}

	bool Delete(NsData* pData, int8* nCode)
	{
		NsItem* pItem = &pData->iItem[*nCode++];
		if((*nCode)==CODE_NULL)
		{
			if(pItem->nType==CDTYPE_NULL) return false;
			pItem->nType = CDTYPE_NULL;
			return true;
		}
		if(pItem->nChld>=0)
		{
			pData = &mstData[pItem->nChld];
			if(Delete(pData, nCode))
			{
				int32 i;
				for(i=0; i<MAXCNT_CODES; i++)
				{
					if(pData->iItem[i].nChld>=0 || pData->iItem[i].nType!=CDTYPE_NULL) break;
				}
				if(i==MAXCNT_CODES)
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

	void DispCodes(NsData* pData, int32 nDepth = 0, char* szDepth = (char*)"")
	{
		char szLine[MAXLEN_LINE] = {0,};
		NsItem* pItem = &pData->iItem[0];
		for(int32 i=0; i<MAXCNT_CODES; i++)
		{
			pItem = &pData->iItem[i];
			switch(pItem->nType)
			{
				case CDTYPE_NULL:
				if(pItem->nChld>=0)
				{
					iTrace("%s+[%c]", szDepth, theCodes[i]);
					sprintf(szLine, "%s  ", szDepth);
					DispCodes(&mstData[pItem->nChld], nDepth+1, szLine);
				}
				break;
				default:
				iTrace("%s+[%c] Exist [%d/%d/%lld]", szDepth, theCodes[i], pItem->nType, (int64)pItem->nUqId);
				if(pItem->nChld>=0)
				{
					sprintf(szLine, "%s  ", szDepth);
					DispCodes(&mstData[pItem->nChld], nDepth+1, szLine);
				}
				break;
			}
		}
	}
};
