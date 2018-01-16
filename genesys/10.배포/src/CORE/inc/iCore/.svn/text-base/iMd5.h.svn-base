//
//  iMd5.h
//
//
//  Created by Tony Ryu on 2013.2.26.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iMd5.h
 * @brief   MD5 Hash 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.26
 */

#pragma once

#include "iObj.h"

#define DEFLEN_MD5_STATE			(4)
#define DEFLEN_MD5_COUNT			(2)
#define DEFLEN_MD5_BLK				(64)
#define DEFLEN_MD5_DIGEST			(16)	///< MD5 Hash 결과 크기(Binary) 형태.

/**
 * @class    iMd5
 * @brief    MD5 Hash 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.2.26
 */
class ILIBAPI iMd5 : public iObj
{
public:
	iMd5();				///< 생성자.
	virtual ~iMd5();	///< 소멸자.

	/**
	 * @brief Hash 시작(내부 변수 Clear)
	 */
	void Start();

	/**
	 * @brief 데이터 입력 함수
	 * @param pData       입력 데이터 
	 * @param nData       입력 데이터 크기
	 * @note 해당 함수를 반복 수행 가능
	 */
	void Insert(iByte* pData, size_t nData);

	/**
	 * @brief Hash 종료 처리(Hash Result 생성)
	 */
	void End();

	/**
	 * @brief  Hash 결과값 크기 반환
	 * @return                    결과값 크기
	 */
	inline size_t GetResultSize() { return sizeof(mszDigest); };

	/**
	 * @brief  Hash 결과값 반환
	 * @return iByte(unsigned char) 포인터
	 */
	inline iByte* GetHashResult() { return mszDigest; };

	/**
	 * @brief  Hash 결과값을 Hex String형으로 반환
	 * @return                    szBuf 포인터를 반환 
	 * @param  szBuf              Hex String 저장 버퍼
	 * @param  nBuf               Hex String 저장 버퍼 크기(16*2+1 이상이어야 함)
	 */
	NPSTR GetHashResult(NPSTR szBuf, size_t nBuf);

protected:
	uint32 mnState[DEFLEN_MD5_STATE];
	uint32 mnCount[DEFLEN_MD5_COUNT];
	iByte mszBuffer[DEFLEN_MD5_BLK];

	iByte mszDigest[DEFLEN_MD5_DIGEST];

	void Clear();
	void Transform(iByte* pBlock);

	void Encode(iByte* pDst, uint32* pSrc, size_t nLen);
	void Decode(uint32* pDst, iByte* pSrc, size_t nLen);
};
