//
//  iObj.h
//  iCore
//
//  Created by Tony Ryu on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iObj.h
 * @brief   iCore Library의 최상위 Class
 *
 * @version v0.1
 * @see     없음
 * @date    2012.5.14.
 */

#pragma once
#include <iType.h>

/**
 * @class    iObj
 * @brief    iCore Library의 최상위 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.14
 */
class ILIBAPI iObj
{
public:
	iObj();
	virtual ~iObj();

	/**
	 * @brief  해당 프로세스내에서 Unique한 Object ID
	 * @return Object ID
	 */
	inline uint32 getObjId() { return mnObjId; };
	static uint32 getObjCnt();

private:
	uint32 mnObjId;
};
