//
//  iBase64.h
//
//
//  Created by Tony Ryu on 2013.2.27.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iBase64.h
 * @brief   Base64 방식으로 Encoding / Decoding을 처리하는 함수 모음
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.27
 */

#pragma once

#include "iType.h"

/**
 * @brief  Base64 방식에서 특수기호를 대체한 Encoding 함수
 * @return                 Encoding 데이터 크기
 * @param  pData           Encoding 할 데이터
 * @param  nData           Encoding 할 데이터 크기
 * @param  szEnc           Encoding 된 데이터 저장 버퍼(String)
 * @param  nEnc            Encoding 된 데이터 저장 버퍼 크기
 * @note 주의 사항
 * - 원래 Base64에서 사용하는 '+', '/' 대신 '$' / '@'를 사용함
 */
ILIBAPI size_t iCore64Enc(const iByte* pData, size_t nData, NPSTR szEnc, size_t nEnc);

/**
 * @brief  Base64 방식에서 특수기호를 대체한 Decoding 함수
 * @return                 Decoding 데이터 크기
 * @param  szEnc           Decoding 할 데이터
 * @param  nEnc            Decoding 할 데이터 크기
 * @param  pData           Decoding 된 데이터 저장 버퍼(String)
 * @param  nData           Decoding 된 데이터 저장 버퍼 크기
 * @note 주의 사항
 * - 원래 Base64에서 사용하는 '+', '/' 대신 '$' / '@'를 사용함
 */
ILIBAPI size_t iCore64Dec(NPCSTR szEnc, size_t nEnc, iByte* pData, size_t nData);

/**
 * @brief  Base64 Encoding 함수
 * @return                 Encoding 데이터 크기
 * @param  pData           Encoding 할 데이터
 * @param  nData           Encoding 할 데이터 크기
 * @param  szEnc           Encoding 된 데이터 저장 버퍼(String)
 * @param  nEnc            Encoding 된 데이터 저장 버퍼 크기
 */
ILIBAPI size_t iBase64Enc(const iByte* pData, size_t nData, NPSTR szEnc, size_t nEnc);

/**
 * @brief  Base64 Decoding 함수
 * @return                 Decoding 데이터 크기
 * @param  szEnc           Decoding 할 데이터
 * @param  nEnc            Decoding 할 데이터 크기
 * @param  pData           Decoding 된 데이터 저장 버퍼(String)
 * @param  nData           Decoding 된 데이터 저장 버퍼 크기
 */
ILIBAPI size_t iBase64Dec(NPCSTR szEnc, size_t nEnc, iByte* pData, size_t nData);

