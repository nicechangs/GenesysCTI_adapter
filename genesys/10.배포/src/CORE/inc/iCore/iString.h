//
//  iString.h
//
//
//  Created by Tony Ryu on 2013.8.19.
//  Copyright (c) 2013 Tony Ryu. All rights reserved.
//

/**
 * @file    iString.h
 * @brief   String 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013.8.19
 */

#pragma once

#include "iObj.h"
#include "iLib.h"

/**
 * @class    iUprStr
 * @brief    문자열 Upper Case 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.8.19
 * @note     소멸자에서 메모리를 자동으로 해제함
 */
class ILIBAPI iUprStr : public iObj
{
public:
	iUprStr(NPCSTR szData = NULL)
	{	mpData = ILIB_NULLSTR;	mbAlloc = false;
		SetStr(szData);
	}
	iUprStr(const iUprStr& rStr)
	{	mpData = ILIB_NULLSTR;	mbAlloc = false;
		SetStr(rStr.mpData);
	}
	virtual ~iUprStr() { FreeStr(); }

	iUprStr& operator=(const iUprStr& rStr);
	operator NPCSTR() const { return mpData; };

	inline NPCSTR GetStr() { return mpData; };
	NPCSTR SetStr(NPCSTR szData = NULL);

protected:
	NPCSTR mpData;
	bool mbAlloc;
	void FreeStr();
};

/**
 * @class    iLowStr
 * @brief    문자열 Lowwer Case 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.8.19
 * @note     소멸자에서 메모리를 자동으로 해제함
 */
class ILIBAPI iLowStr : public iObj
{
public:
	iLowStr(NPCSTR szData = NULL)
	{	mpData = ILIB_NULLSTR;	mbAlloc = false;
		SetStr(szData);
	}
	iLowStr(const iLowStr& rStr)
	{	mpData = ILIB_NULLSTR;	mbAlloc = false;
		SetStr(rStr.mpData);
	}
	virtual ~iLowStr() { FreeStr(); }

	iLowStr& operator=(const iLowStr& rStr);
	operator NPCSTR() const { return mpData; };

	inline NPCSTR GetStr() { return mpData; };
	NPCSTR SetStr(NPCSTR szData = NULL);

protected:
	NPCSTR mpData;
	bool mbAlloc;
	void FreeStr();
};
