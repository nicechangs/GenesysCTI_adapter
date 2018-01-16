//
//  iJson.h
//
//
//  Created by Tony Ryu on 2013.2.27.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iJson.h
 * @brief   JSON Parser & Generator
 *
 * @version v0.1
 * @see     None
 * @date    2013.2.27
 */

#pragma once

#include <iType.h>
#include "jansson.h"

/**
 * @enum  eJsType
 * @brief Json Node Type
 * @brief iJson 사용 시 사용할 JSON Node의 Type
 */
enum eJsType	// jansson defined order
{
	eJsObj,		/// Json Object
	eJsArr,		/// Json Array
	eJsStr,		/// Json String(Text)
	eJsInt,		/// Json Integer Value(int64)
	eJsReal,	/// Json Float Value(double)
	eJsTrue,	/// Json Boolean Value(true)
	eJsFalse,	/// Json Boolean Value(false)
	eJsNull,	/// Json Null Value(null)
};

/**
 * @class     iJson
 * @brief     JSON DOM 형식의 Parser & Generator
 * @author    Tony Ryu(bridgetec@gmail.com)
 * @date      2013.2.27
 * @note      Non Thread Safe Class
 */
class ILIBAPI iJson
{
public:
	iJson();									///< 생성자(기본).
	iJson(const iJson& rItem);					///< 생성자(복사).
	iJson(NPCSTR szStr);						///< 생성자(Text).
	iJson(int64 nInt);							///< 생성자(Integer).
	iJson(bool bBool);							///< 생성자(Boolean).
	iJson(double fDouble);						///< 생성자(Float).
	iJson(NPCSTR szKey, const iJson& rItem);	///< 생성자(Object).
	virtual ~iJson();							///< 소멸자.

	size_t RefCnt() { return mpData?mpData->refcount:0; };

	iJson& operator=(const iJson& rItem);		///< 복사연산자 Overide(Reference Count 증가).

	bool Reset(eJsType nType = eJsNull);			///< 초기화 함수.
	bool CopyFrom(const iJson& rItem);			///< 복사 함수(Referenc Count는 놔두고 새로운 Instance로 복사).

	/**
	 * @brief   Json Type을 반환
	 * @return  Json Type을 반환
	 * @see     eJsType
	 */
	inline eJsType Type() { return mpData?(eJsType)json_typeof(mpData):eJsNull; };
	inline bool IsNull() { return json_is_null(mpData); };
	inline bool IsBool() { return json_is_boolean(mpData); };
	inline bool IsReal() { return json_is_real(mpData); };
	inline bool IsInt() { return json_is_integer(mpData); };
	inline bool IsStr() { return json_is_string(mpData); };
	inline bool IsObj() { return json_is_object(mpData); };
	inline bool IsArr() { return json_is_array(mpData); };

	/**
	 * @brief  Child Node의 갯수를 반환
	 * @return Child Node Count(Array or Object 만 가능)
	 */
	size_t GetCnt();

	bool GetBool();		///< Json 값을 반환(Type이 상이할 경우 false 반환).
	double GetReal();	///< Json 값을 반환(Type이 상이할 경우 .0을 반환).
	int64 GetInt();		///< Json 값을 반환(Type이 상이할 경우 0을 반환).
	NPCSTR GetStr();	///< Json 값을 반환(Type이 상이할 경우 ""을 반환).

	bool SetBool(bool bVal);			///< Json Boolean 값을 설정(기존 Type을 삭제 후 값을 설정함).
	bool SetNull();						///< Json Null 값을 설정(기존 Type을 삭제 후 값을 설정함).
	bool SetReal(double fVal);			///< Json Float 값을 설정(기존 Type을 삭제 후 값을 설정함).
	bool SetInt(int64 nVal);			///< Json Integer 값을 설정(기존 Type을 삭제 후 값을 설정함).
	bool SetStr(NPCSTR szVal);			///< Json Text 값을 설정(기존 Type을 삭제 후 값을 설정함).
	bool SetStrs(NPCSTR szFmt, ...);	///< Json Argumented Text 값을 설정(기존 Type을 삭제 후 값을 설정함).

	// for Object
	/**
	 * @brief  Child에 szKey가 존재하는지 확인 함수
	 * @return              szKey 존재 여부
	 * @param  szKey        Child Node Key
	 */
	bool IsExist(NPCSTR szKey);

	/**
	 * @brief  Child Node를 iJson으로 반환하는 함수
	 * @return              성공 여부
	 * @param(in)   szKey   Child Node Key
	 * @param(out)  rItem   Child Node 반환
	 * @note 주의 사항
	 * - Child Node에 대해 새로운 Instance가 생성되는것이 아니라 Reference Count만 증가함
	 */
	bool GetItem(NPCSTR szKey, iJson& rItem);

	/**
	 * @brief  iJson Instance를 Child Node로 설정
	 * @return              성공 여부
	 * @param  szKey        Child Node Key
	 * @param  rItem        Child Node
	 * @note 주의 사항
	 * - Child Node에 대해 새로운 Instance가 생성되는것이 아니라 Reference Count만 증가함
	 * - 값은 Key가 존재 시 이전 Key Reference Count를 줄이고 새 Key Reference Count를 증가시킴
	 */
	bool SetItem(NPCSTR szKey, const iJson& rItem);

	/**
	 * @brief  Object Node에서 Child Node를 삭제
	 * @return              성공 여부
	 * @param  szKey        Child Node Key
	 */
	bool DelItem(NPCSTR szKey);

	/**
	 * @brief  Child Node 순회
	 * @return 최초 Child Node에 대한 Position 값
	 */
	NPos First();

	/**
	 * @brief  Child Node 순회 함수
	 * @return           Next 노드의 존재 유무를 반환
	 * @param(inout)  pos       First 함수에서 받은 NPos 값
	 * @param(out)    szKey     Key
	 * @param(out)    stVal     Value
@code
NPCSTR pKey;
iJson stVal;
NPos pos = stJson.First();
while(stJson.Next(pos, pKey, stVal))
{
	iTrace("Child %s/%d", pKey, stVal.Type());
}
@endcode
	 */
	bool Next(NPos& pos, NPCSTR& szKey, iJson& stVal);

	// for Object Helper
	bool GetItemBool(NPCSTR szKey);		///< Child Node(szKey)의 Boolean 값을 반환(Type이 다를경우 false를 반환).
	double GetItemReal(NPCSTR szKey);	///< Child Node(szKey)의 Float 값을 반환(Type이 다를경우 .0 을 반환).
	int64 GetItemInt(NPCSTR szKey);		///< Child Node(szKey)의 Integer 값을 반환(Type이 다를경우 0 을 반환).
	NPCSTR GetItemStr(NPCSTR szKey);	///< Child Node(szKey)의 String(Text)값을 반환(Type이 다를경우 "" 을 반환).

	bool SetItemBool(NPCSTR szKey, bool bVal);			///< Child Node(szKey)에 Boolean 값을 설정.
	bool SetItemReal(NPCSTR szKey, double fVal);		///< Child Node(szKey)에 Float 값을 설정.
	bool SetItemInt(NPCSTR szKey, int64 nVal);			///< Child Node(szKey)에 Integer 값을 설정.
	bool SetItemStr(NPCSTR szKey, NPCSTR szVal);		///< Child Node(szKey)에 String(Text)값을 설정.
	bool SetItemStrs(NPCSTR szKey, NPCSTR szFmt, ...);	///< Child Node(szKey)에 Argumented String(Text)값을 설정.
	bool SetItemNull(NPCSTR szKey);						///< Child Node(szKey)에 Null 값을 설정.

	// for Array
	/**
	 * @brief  Child Node에 특정 Index 존재 여부를 반환
	 * @return              존재 여부
	 * @param  nIdx         Array의 Index 번호
	 */
	bool IsExist(uint32 nIdx);

	/**
	 * @brief  현재 Node(Array)에 Item 추가
	 * @return               성공 여부
	 * @param  rItem         추가 될 iJson Item
	 * @note 주의 사항
	 * - 현재 Node가 Array가 아닐 경우 Reset 후 Arrary로 설정
	 * - Array의 맨 뒤의 Item으로 추가됨
	 * - Array의 중간에 추가하는 함수는 지원하지 않음
	 */
	bool PushItem(const iJson& rItem);
	bool PushItemBool(bool bVal);			///< 현재 Node(Array)에 Boolean 값 추가.
	bool PushItemReal(double fVal);			///< 현재 Node(Array)에 Float 값 추가.
	bool PushItemInt(int64 nVal);			///< 현재 Node(Array)에 Integer 값 추가.
	bool PushItemStr(NPCSTR szVal);			///< 현재 Node(Array)에 String(Text) 값 추가.
	bool PushItemStrs(NPCSTR szFmt, ...);	///< 현재 Node(Array)에 Argumented String(Text) 값 추가.
	bool PushItemNull();					///< 현재 Node(Array)에 Null 값 추가.

	/**
	 * @brief  Array Node에서 Index로 Child Item을 가져오는 함수
	 * @return                  성공 여부
	 * @param(in)  nIdx         Index 값
	 * @param(out) rItem        반환 될 iJson
	 */
	bool GetItem(uint32 nIdx, iJson& rItem);

	/**
	 * @brief  Array Node에 특정 Index의 Item을 설정하는 함수
	 * @return                 성공 여부
	 * @param(in) nIdx         Index 값(현재 Node의 GetCnt()보다 작아야 함)
	 * @param(in) rItem        설정 할 iJson
	 */
	bool SetItem(uint32 nIdx, const iJson& rItem);

	/**
	 * @brief  Array Node에서 특정 Index의 Item을 삭제하는 함수
	 * @return              성공 여부
	 * @param  nIdx         삭제 대상 Item Index 값
	 * @note                해당 Index가 삭제되면 Array상에 이후 Index값은 1씩 줄어듬
	 */
	bool DelItem(uint32 nIdx);

	// for Pack/Unpack
	bool JsPack(NPCSTR szFmt, ...);
	int32 JsUnpack(NPCSTR szFmt, ...);
	bool Merge(iJson& rItem);

	/**
	 * @brief  Json String을 Parsing 후 내부에 저장하는 함수
	 * @return                    성공 여부
	 * @param(in)    szJson       Json String
	 * @param(inout) pErr         실패 시 실패 사유
	 */
	bool SetJson(NPCSTR szJson, json_error_t* pErr = NULL);

	/**
	 * @brief  Json File을 Parsing 후 내부에 저장하는 함수
	 * @return               성공 여부
	 * @param  szFile        파일 이름
	 * @param  pErr          실패 시 실패 사유
	 */
	bool LoadFile(NPCSTR szFile, json_error_t* pErr = NULL);

	/**
	 * @brief  Json String을 반환 하는 함수
	 * @return              반환된 Json String의 길이(Null 제외)
	 * @param  szJson       Json 저장용 버퍼
	 * @param  nJson        szJson의 크기
	 * @param  nFlag        반환할 Json 형식
	 * @note nFlag 설명
	 * - JSON_COMPACT(Default) : 최소의 길이로 Json을 반환
	 * - JSON_INDENT(x) : x 만큼의 Space를 Indent로 하는 정리된 Json을 반환
	 */
	size_t GetJson(NPSTR szJson, size_t nJson, size_t nFlag = JSON_COMPACT);

	// MUST RelJson(ppJson) after using it.
	/**
	 * @brief  Json String을 반환 하는 함수(내부 버퍼를 할당)
	 * @return               반환된 Json String의 길이(Null 제외한 실제 Json의 길이)
	 * @param  nOffset       내부 버퍼의 앞 부분에 건너 띄울 크기(아래 참조)
	 * @param  ppJson        내부 버퍼를 할당 받을 포인터
	 * @param  nFlag         반환할 Json 형식
	 * @warning              할당 받은 ppJson을 반드시 FreeJson 함수로 버퍼를 해제하여야 한다.
	 * @note nFlag 설명
	 * - JSON_COMPACT(Default) : 최소의 길이로 Json을 반환
	 * - JSON_INDENT(x) : x 만큼의 Space를 Indent로 하는 정리된 Json을 반환
	 * @note nOffset 설명
	 * - nOffset은 패킷 송수신 시 Header부가 있다면 해당 Header부 만큼 공란을 비워놓기 위함이다.
	 * - 반환(return) 값의 길이 정보에는 Offset 길이는 제외되어 있다.
	 * @see FreeJson
	 */
	size_t AlocJson(size_t nOffset, NPSTR* ppJson, size_t nFlag = JSON_COMPACT);

	/**
	 * @brief AlocJson으로 할당 받은 내부 버퍼를 해제 하는 함수
	 * @param pJson         AlocJson으로 할당받은 버퍼 포인터
	 * @see AlocJson
	 */
	static void FreeJson(NPSTR pJson);

	/**
	 * @brief  Json을 파일로 저장하는 함수
	 * @return               성공 여부
	 * @param  szFile        파일 이름 
	 * @param  nFlag         반환할 Json 형식
	 * @note nFlag 설명
	 * - JSON_COMPACT(Default) : 최소의 길이로 Json을 반환
	 * - JSON_INDENT(x) : x 만큼의 Space를 Indent로 하는 정리된 Json을 반환
	 */
	bool SaveFile(NPCSTR szFile, size_t nFlag = JSON_COMPACT);

	void Dump(NPCSTR szFmt, ...);
	void DbgDump(NPCSTR szFunc, uint32 nLine);

	static void MemTrace();
	static void MemTotStat(int64& nCurMem, int64& nCurCnt, int64& nTotMem, int64& nTotCnt);
	static int64 MemAloc(bool bTotal = false);
	static int64 MemCnt(bool bTotal = false);
protected:
	json_t* GetPtr(bool bAddRef = true) const;
	void SetPtr(json_t* pData);
	json_t* mpData;

};



