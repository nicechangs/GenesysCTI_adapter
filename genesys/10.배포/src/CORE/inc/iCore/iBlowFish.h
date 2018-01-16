//
//  iBlowFish.h
//  
//
//  Created by Tony Ryu on 2013.2.26.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iBlowFish.h
 * @brief   Blowfish 암호화 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.26
 */

#pragma once

#include "iObj.h"

#define DEFCNT_SUBLOOP		(16)
#define DEFCNT_SUBKEYS		(DEFCNT_SUBLOOP+2)
#define DEFCNT_SBOXES		(4)
#define DEFCNT_ENTRIES		(256)
#define DEFSIZE_PROCUNIT	(8)				///< En/Decoding 시 데이터 크기는 반드시 8의 배수.

/**
 * @class    iBlowFish
 * @brief    Blowfish 암호화 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.2.27
 * @warning 주의 사항
 * - 암호화 및 복호화 데이터의 크기는 반드시 8의 배수이어야 함
 * - 만약 크기가 8 배수가 아니면 Padding을 넣어서 처리해야 함
 */
class ILIBAPI iBlowFish : public iObj
{
public:
	iBlowFish();			///< 생성자.
	virtual ~iBlowFish();	///< 소멸자.

	/**
	 * @brief  Instance 초기화 함수(/w 암호화 Seed)
	 * @return               초기화 성공 여부
	 * @param  szSeed        암호화 Seed
	 * @param  nKeyBits      암호화 Bit(*128,192,256)
	 */
	bool Init(NPCSTR szSeed);

	/**
	 * @brief  암호화 함수
	 * @return              암호화 성공 여부
	 * @param(inout) pData  암호화 대상 Plain Data / 암호화 결과
	 * @param(in)    nSize  암호화 대상 Plain Data 크기
	 * @warning 주의 사항
	 * - pData는 In/Out Parameter이다.
	 * - nSize 반드시 8의 배수이어야 함
	 * - nSize 8의 배수가 아니면 Padding을 넣어서 처리해야 함
	 */
	bool Encrypt(iByte* pData, uint32 nSize);

	/**
	 * @brief  암호화 함수
	 * @return              암호화 실패 시 0 반환 / 암호화 결과 크기
	 * @param(in)    pSrc   암호화 대상 Plain Data
	 * @param(in)    nSrc   암호화 대상 Plain Data 크기
	 * @param(out)   pDst   암호화 결과 Buffer
	 * @param(in)    nDst   암호화 결과 Buffer 크기
	 *  / 
	 * @warning 주의 사항
	 * - nDst는 (((nSrc+7)/8)*8)보다 크거나 같아야 한다.
	 */
	uint32 Encrypt(const iByte* pSrc, uint32 nSrc, iByte* pDst, uint32 nDst);

	/**
	 * @brief  복호화 함수
	 * @return              복호화 성공 여부
	 * @param(inout) pData  복호화 대상 Crypted Data / 복호화 결과
	 * @param(in)    nSize  복호화 대상 Crypted Data 크기
	 * - pData는 In/Out Parameter이다.
	 * - nSize 반드시 8의 배수이어야 함
	 * - nSize 8의 배수가 아니면 Padding을 넣어서 처리해야 함
	 */
	bool Decrypt(iByte* pData, uint32 nSize);

	/**
	 * @brief  복호화 함수
	 * @return              복호화 실패 시 0 반환 / 복호화 결과 크기
	 * @param(in)    pSrc   복호화 대상 Crypted Data
	 * @param(in)    nSrc   복호화 대상 Crypted Data 크기
	 * @param(out)   pDst   복호화 결과 Buffer
	 * @param(in)    nDst   복호화 결과 Buffer 크기
	 * - nSrc는 반드시 8의 배수이어야 함
	 * - nDst는 nSrc보다 크거나 같아야 함
	 */
	uint32 Decrypt(const iByte* pSrc, uint32 nSrc, iByte* pDst, uint32 nDst);

protected:
	void Encrypt(uint32& nDataL, uint32& nDataR);
	void Decrypt(uint32& nDataL, uint32& nDataR);
	uint32 Trsl(uint32 nData);
	uint32 mCtxP[DEFCNT_SUBKEYS];
	uint32 mCtxS[DEFCNT_SBOXES][DEFCNT_ENTRIES];
};
















