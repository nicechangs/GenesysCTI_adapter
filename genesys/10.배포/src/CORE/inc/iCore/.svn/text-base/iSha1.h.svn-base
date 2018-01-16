//
//  iSha1.h
//
//
//  Created by Tony Ryu on 2013.2.27.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iSha1.h
 * @brief   SHA1 Hash 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.27
 */

#pragma once

#include "iObj.h"

#define DEFLEN_SHA1_STATE		(5)
#define DEFLEN_SHA1_COUNT		(2)
#define DEFLEN_SHA1_BLK			(64)
#define DEFLEN_SHA1_DIGEST		(20)	///< SHA1 Hash 결과 크기(Binary) 형태.

/**
 * @class    iSha1
 * @brief    SHA1 Hash 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.2.27
 */
class ILIBAPI iSha1 : public iObj
{
public:
	iSha1();			///< 생성자.
	virtual ~iSha1();	///< 소멸자.

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
	 * @param  nBuf               Hex String 저장 버퍼 크기(20*2+1 이상이어야 함)
	 */
	NPSTR GetHashResult(NPSTR szBuf, size_t nBuf);

protected:
	uint32 mnTotal[DEFLEN_SHA1_COUNT];
	uint32 mnState[DEFLEN_SHA1_STATE];
	iByte mszBuffer[DEFLEN_SHA1_BLK];
	iByte mszInnPad[DEFLEN_SHA1_BLK];
	iByte mszOutPad[DEFLEN_SHA1_BLK];

	iByte mszDigest[DEFLEN_SHA1_DIGEST];

	void Clear();
	void Transform(iByte* szBuf);
};
