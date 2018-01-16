//
//  intfuncs.h
//
//
//  Created by Tony Ryu on 2013.10.28.
//  Copyright (c) 2013 Tony Ryu. All rights reserved.
//

/**
 * @file    intfuncs.h
 * @brief
 *
 * @version v0.1
 * @see     None
 * @date    2013.10.28
 */

#pragma once

#include <iType.h>
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

int intfuncs_extension_init(sqlite3 *pDb);

/**
 * Utility Functions
 */

bool tChkRange(int64 nVal, int64 nFrom, int64 nTo);
bool tMakePattern(NPCSTR szInput, NPSTR szOutput, uint32 nOutput);
bool tMatchPattern(NPCSTR szDigit, NPCSTR szPattern);

void tSetBit(uint32* pAlloc, uint16 nArg);
uint16 tFindBit(uint32* pAlloc);

#ifdef __cplusplus
}
#endif

