//
//  iAria.h
//  
//
//  Created by Tony Ryu on 2013.2.27.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iAria.h
 * @brief   Aria 암호화 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.27
 */

#pragma once

#include "iObj.h"

#define LEN_ARIAUNIT			(16)		///< 암호화 데이터 크기 단위.
#define MAX_ARIAROUND			((16)+1)	///< 암호화 Round 횟수.
#define DEF_ARIA_KEYBITS		(128)		///< 기본 암호화 Key Bit Count.
#define MAXLEN_MASTERKEY		(32)		///< 암호화 Seed의 최대 크기.

/**
 * @class    iAria
 * @brief    Aria 암호화 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.2.27
 * @warning 주의 사항
 * - 암호화 및 복호화 데이터의 크기는 반드시 LEN_ARIAUNIT의 배수이어야 함
 * - 만약 크기가 LEN_ARIAUNIT의 배수가 아니면 Padding을 넣어서 처리해야 함
 */
class ILIBAPI iAria : public iObj
{
public:
	iAria();				///< 생성자.
	virtual ~iAria();		///< 소멸자.

	/**
	 * @brief  Instance 초기화 함수(/w 암호화 Seed)
	 * @return               초기화 성공 여부
	 * @param  szSeed        암호화 Seed
	 * @param  nKeyBits      암호화 Bit(*128,192,256)
	 */
	bool InitAria(NPCSTR szSeed, int32 nKeyBits = DEF_ARIA_KEYBITS);

	/**
	 * @brief  암호화 함수
	 * @return              암호화 Byte 수
	 * @param  pPlain       암호화 대상 Plain Data
	 * @param  nPlain       암호화 대상 Plain Data 크기
	 * @param  pCrypt       암호화 저장 버퍼(nPlain의 크기만큼 필요)
	 * @warning 주의 사항
	 * - nPlain은 반드시 LEN_ARIAUNIT의 배수이어야 함
	 * - pPlain이 LEN_ARIAUNIT의 배수가 아니면 Padding을 넣어서 처리해야 함
	 */
	uint32 Encrypt(const iByte* pPlain, uint32 nPlain, iByte* pCrypt);

	/**
	 * @brief  복호화 함수
	 * @return              복호화 Byte 수
	 * @param  pCrypt       복호화 대상 Crypted Data
	 * @param  nCrypt       복호화 대상 Crypted Data 크기
	 * @param  pPlain       복호화 저장 버퍼(nCrypto의 크기만큼 필요)
	 */
	uint32 Decrypt(const iByte* pCrypt, uint32 nCrypt, iByte* pPlain);

protected:
	int32 mnRound;
	uchar mszEncKey[LEN_ARIAUNIT*MAX_ARIAROUND];
	uchar mszDecKey[LEN_ARIAUNIT*MAX_ARIAROUND];
	bool Clear();
};
