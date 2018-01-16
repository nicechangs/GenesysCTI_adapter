//
//  iIniFile.h
//  iLib3
//
//  Created by Tony Ryu on 11. 4. 19..
//  Copyright 2011 TonyAppz. All rights reserved.
//

/**
 * @file    iIniFile.h
 * @brief   INI File 처리
 *
 * @version v0.1
 * @see     None
 * @date    2011.4.19
 */

#pragma once

#include "iFile.h"

#define MAXLEN_INIITEM			(128)	///< Section, Key String의 최대 길이.
#define MAXLEN_INIVALUE			(1024)	///< Value, Remark String의 최대 길이.
#define DEFLEN_HEXDIGIT			(8)		///< Hex Value 추가 시 기본 길이

/**
 * @class    iIniFile
 * @brief    INI File 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2011.4.19
 */
class ILIBAPI iIniFile : public iFile
{
public:
	iIniFile();				///< 생성자.
	virtual ~iIniFile();	///< 소멸자.

	/**
	 * @enum  eIniNode
	 * @brief INI Node의 Type을 지정 =
	 */
	enum eIniNode
	{
		ININODE_BLANK	= -1,	///< 공백 라인.
		ININODE_ROOT,			///< Root Node.
		ININODE_REMARK,			///< 주석 라인.
		ININODE_SECTION,		///< Section 라인.
		ININODE_CONTENTS		///< Key, Value 라인.
	};

	/**
	 * @struct  iIniNode
	 * @brief   INI Node 저장 Structure
	 */
	typedef struct iIniNode
	{
		eIniNode	nType;						///< eIniNode Type.
		char		szName[MAXLEN_INIITEM];		///< Section or Key 이름.
		char		szValue[MAXLEN_INIVALUE];	///< Key일 경우 Value String.
		char		szRemark[MAXLEN_INIVALUE];	///< 주석일 경우 주석 String.
		iIniNode*	pNextNode;					///< Next Node Pointer.
	} ININODE, *PININODE;

	/**
	 * @brief  INI File Open 함수
	 * @return              성공 여부
	 * @param  szIniFile    INI 파일 Name
	 * @param  szPath       INI 파일 Path
	 * @note szIniFile이 Full path를 지정할 경우 szPath를 NULL로 지정
	 */
	bool OpenIni(NPCSTR szIniFile, NPCSTR szPath = NULL);

	/**
	 * @brief  INF File Close 함수
	 * @return 항상 false를 반환
	 */
	bool CloseIni();

	/**
	 * @brief  OpenIni 호출 이후 해당 INI 파일이 수정 되었는지 체크 함수
	 * @return INI 파일 수정 여부
	 */
	bool NeedReload();

	/**
	 * @brief  Section이나 Key가 존재하는지 확인 함수
	 * @return              존재 시 true, 없을 경우 false를 반환
	 * @param  szSec        Section String
	 * @param  szKey        Key String(NULL일 경우 Section까지만 검색)
	 */
	bool IsExist(NPCSTR szSec, NPCSTR szKey = NULL);

	int64 ReadInt(NPCSTR szSec, NPCSTR szKey, int64 nDefault = 0);
	NPCSTR ReadStr(NPCSTR szSec, NPCSTR szKey, NPCSTR szDefault = "");
	bool ReadStr(NPCSTR szSec, NPCSTR szKey, NPSTR szKeyBuf, uint32 nKeyBuf, NPCSTR szDefault = "");
	bool ReadBool(NPCSTR szSec, NPCSTR szKey, bool bDefault = false);

	bool WriteInt(NPCSTR szSec, NPCSTR szKey, int64 nVal, NPCSTR szRem = NULL);
	bool WriteHex(NPCSTR szSec, NPCSTR szKey, int64 nVal, int32 nDgt = DEFLEN_HEXDIGIT, NPCSTR szRem = NULL);
	bool WriteStr(NPCSTR szSec, NPCSTR szKey, NPCSTR szVal, NPCSTR szRem = NULL);
	bool WriteCmt(NPCSTR szComment = NULL);

	bool DeleteKey(NPCSTR szSec, NPCSTR szKey);

	/**
	 * @brief  INI File을 순환하는 함수
	 * @return                   INI Node Pointer
	 * @param  szSec             원하는 Section String, NULL일 경우 Root부터 순환 
	 */
	PININODE GetFirstNode(NPCSTR szSec = NULL);

	/**
	 * @brief  INI File을 순환하는 함수
	 * @return                  INI Node Pointer
	 * @param  pPos             INI Node Pointer
	 * @warning 아래와 같은 방식으로 순환하여야 함
@code
PININODE pPos = stIni.GetFirstNode("Section");
while(pPos)
{
	printf("Section >> %s", pPos->szName);
	// pPos를 사용하여 INI 항목을 추출
	pPos = GetNextNode(pPos);
}
@endcode
	 */
	PININODE GetNextNode(PININODE& pPos);
protected:

	void ParseLine(PININODE pCurNode, NPSTR szLineBuf);

	bool ParseIniFile(NPCSTR szFullpath);
	bool WriteIniFile(NPCSTR szFullpath);

	PININODE InsertNode(eIniNode nType, NPCSTR szName, NPCSTR szValue, NPCSTR szRemark, PININODE pNextNode);
	void RemoveNode(PININODE pNode);

	PININODE FindLastNode();
	PININODE FindSec(NPCSTR szSec);
	PININODE FindKey(PININODE pSec, NPCSTR szKey, PININODE* ppPrv = NULL);

	NPSTR FindRemarkChar(NPSTR szValue);

private:
	bool mbDirty;
	time_t mtModified;
	PININODE mpRoot;
	ININODE mstRoot;
	bool mbSectSearch;
	char mszBuffer[MAXLEN_INIVALUE];
	char mszFullpath[MAXLEN_PATH];
};
