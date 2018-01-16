//
//  iAes.h
//
//  Created by Tony Ryu on 2014.1.23.
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iAes.h
 * @brief   AES 대칭키 암호화 처리
 * @author  Tony Ryu(bridgetec@gmail.com)
 *
 * @version v0.1
 * @see     None
 * @date    2014.1.23
 */

#pragma once

#include "iObj.h"

#define DEF_AES_KEYBITS			(128)		///< 기본 암호화 Key Bit Count(128, 192 or 256).
#define MAXLEN_AES_BUFFER		(128)
#define DEFCNT_AES_SBOXES		(256)
#define MAXCNT_AES_ROUNDCONST	(10)
#define DEFSIZE_AES_UNIT		(16)

/**
 * @class    iAes
 * @brief    AES 대칭키 암호화 처리 Class
 * @note     AES ECB 방식만 제공(+Padding Mode)
 * @note     InitEnc 수행 후 Decrypt 수행 시 실패 처리됨
 * @note     InitDec 수행 후 Encrypt 수행 시 실패 처리됨
 * @warning  Thread Safe/Non Thread Safe 함수를 구분하여 사용할 것
 *           Non Thread Safe 함수 사용 시 Encrypt/Decrypt 수행 후 
 *           Result 반환까지 Critical Section으로 처리
 */
class iAes
{
public:
	iAes();
	virtual ~iAes();
	
	/**
	 * @brief  Encryption Instance 초기화 함수(/w 암호화 Seed)
	 * @return               초기화 성공 여부
	 * @param  szKey         암호화 Key
	 * @param  nKeyBits      암호화 Bit(*128,192,256)
	 * @param  bPadding      Padding 처리 여부(기본:true)
	 */
	bool InitEnc(NPCSTR szKey, int32 nKeyBits = DEF_AES_KEYBITS, bool bPadding = true);


	/**
	 * @brief  암호화 함수(Thread Safe)
	 * @return             암호화 결과 Crypted Data 크기(실패 시 0 반환)
	 * @param(in)    pSrc  암호화 대상 Plain Data
	 * @param(in)    nSrc  암호화 대상 Plain Data 크기
	 * @param(out)   pDst  암호화 결과 Crypted Data
	 * @param(in)    nDst  암호화 결과 Crypted Data Buffer 크기
	 * @note               nDst는 nSrc보다 큰 DEFSIZE_AES_UNIT(16)의 배수보다 크거나 같아야 함
	 * @note               Thread Safe 함수
	 */
	size_t Encrypt(const iByte* pSrc, size_t nSrc, iByte* pDst, size_t nDst);
	/**
	 * @brief  암호화 함수(Non Thread Safe)
	 * @return              암호화 성공 여부
	 * @param(in)    pData  암호화 대상 Plain Data
	 * @param(in)    nSize  암호화 대상 Plain Data 크기
	 * @note                결과 값은 Result, Length 함수를 사용
	 * @see                 Result, Length
	 */
	bool Encrypt(const iByte* pData, size_t nSize);
	/**
	 * @brief  암호화 함수(Non Thread Safe)
	 * @return              암호화 성공 여부
	 * @param(in)    szStr  암호화 대상 Plain String
	 * @note                결과 값은 Result, Length 함수를 사용
	 * @see                 Result, Length
	 */
	bool Encrypt(NPCSTR szStr);

	/**
	 * @brief  Decryption Instance 초기화 함수(/w 암호화 Seed)
	 * @return               초기화 성공 여부
	 * @param  szKey         암호화 Key
	 * @param  nKeyBits      암호화 Bit(*128,192,256)
	 * @param  bPadding      Padding 처리 여부(기본:true)
	 * @note   Encryption 시 Padding 처리를 하지 않았다면 bPadding은 false로 지정
	 */
	bool InitDec(NPCSTR szKey, int32 nKeyBits = DEF_AES_KEYBITS, bool bPadding = true);

	/**
	 * @brief  복호화 함수(Thread Safe)
	 * @return             복호화 결과 Plain Data 크기(실패 시 0 반환)
	 * @param(in)    pSrc  복호화 대상 Crypted Data
	 * @param(in)    nSrc  복호화 대상 Crypted Data 크기
	 * @param(out)   pDst  복호화 결과 Plain Data
	 * @param(in)    nDst  복호화 결과 Plain Data Buffer 크기
	 * @note               nSrc는 반드시 DEFSIZE_AES_UNIT(16)의 배수이어야 함
	 * @note               nDst는 nSrc보다 크거나 같아야 함
	 * @note               Thread Safe 함수
	 */
	size_t Decrypt(const iByte* pSrc, size_t nSrc, iByte* pDst, size_t nDst);
	/**
	 * @brief  복호화 함수(Non Thread Safe)
	 * @return              복호화 성공 여부
	 * @param(in)    pData  복호화 대상 Crypted Data
	 * @param(in)    nSize  복호화 대상 Crypted Data 크기
	 * @note                nSize는 반드시 DEFSIZE_AES_UNIT(16)의 배수이어야 함
	 * @note                결과 값은 Result, Length 함수를 사용
	 * @see                 Result, Length
	 */
	bool Decrypt(const iByte* pData, size_t nSize);

	/**
	 * @brief  암호화/복호화 결과
	 * @return 결과 Data의 Pointer 반환
	 */
	inline const iByte* Result() { return mpResult; };
	/**
	 * @brief  암호화/복호화 결과 크기
	 * @return 결과 Data의 크기를 반환
	 */
	inline size_t Length() { return mnLength; };
	/**
	 * @brief  Instance 초기화 시 설정한 Padding 여부
	 * @return Padding 여부를 반환
	 */
	inline bool IsPadding() { return mbPadding; };

protected:
	int32 mnRound;
	int32 mnMode;
	uint32 *mpRKeys;
	uint32 mnRKeys[MAXLEN_AES_BUFFER];
	iByte* mpResult;
	size_t mnLength;
	bool mbPadding;

	void Encrypt(iByte* pIData, iByte* pOData = NULL);
	void Decrypt(iByte* pIData, iByte* pOData = NULL);
	void ClearAll();
	bool ClearResult(size_t nSize = 0);
};





























