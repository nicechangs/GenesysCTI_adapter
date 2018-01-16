//
//  iFile.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iFile.h
 * @brief   File 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.14
 */

#pragma once

#include "iObj.h"

/**
 * @class    iFile
 * @brief    File 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.5.14
 * @note 상속 후 사용할 수 있는 기능
 * - 가상 함수인 OnFileOpen은 fopen이 성공한 후 호출 됨
 * - 가상 함수인 OnFileClose는 fclose 직전에 호출 됨 
 */
class ILIBAPI iFile : public iObj
{
public:
	iFile();
	virtual ~iFile();

	/**
	 * @enum     eFileOrig
	 * @brief    File Seek 함수에서 사용
	 */
	enum eFileOrig
	{
		IFILE_BEGIN = 0,	///< File 시작 지점 부터.
		IFILE_CUR,			///< File의 현재 위치로 부터.
		IFILE_END			///< File의 마지막 지점으로부터.
	};

	inline bool IsOpened() { return bool(mpFile!=NULL); };
	inline NPCSTR GetFilename() { return mszFile; };

	/**
	 * @brief  파일 Open 처리
	 * @return               성공 여부
	 * @param  szFile        파일명
	 * @param  szOpt         Open Option(fopen 함수와 동일)
	 */
	bool OpenFile(NPCSTR szFile, NPCSTR szOpt);
	void CloseFile();

	size_t Read(NPSTR szBuf, size_t nSize);
	size_t Write(NPCSTR szBuf, size_t nSize);

	/**
	 * @brief  파일의 한 라인(EOL)을 읽어들이는 함수
	 * @return           szLine의 포인터, 실패 시 NULL 반환
	 * @param  szLine    저장 버퍼
	 * @param  nSize     저장 버퍼 크기
	 */
	NPSTR GetS(NPSTR szLine, int32 nSize);

	/**
	 * @brief  파일에 한 라인(EOL)단위로 저장하는 함수
	 * @return           성공 여부
	 * @param  szLine    저장할 버퍼
	 */
	bool PutS(NPCSTR szLine);

	/**
	 * @brief  파일의 한 문자 단위로 읽어들이는 함수
	 * @return 읽어들인 문자 반환
	 */
	int32 GetC();

	/**
	 * @brief  파일의 한 문자 단위로 저장하는 함수
	 * @return           성공 여부
	 * @param  nChar     저장할 문자
	 */
	bool PutC(int32 nChar);

	/**
	 * @brief  fprintf와 동일 함수
	 * @return             저장 된 Byte 수
	 * @param  szFormat    Variable Argument의 Format String
	 * @param  ...         Variable Arguments
	 */
	int32 Printf(NPCSTR szFormat, ...);

	int32 Flush();

	int32 Seek(int64 nOffset, eFileOrig nOrg = IFILE_CUR);
	int64 Tell();

	size_t GetSize();

protected:
	virtual void OnFileOpen(FILE* pFile, NPCSTR szFilename) {};
	virtual void OnFileClose(FILE* pFile, NPCSTR szFilename) {};

private:
	FILE* mpFile;
	char mszFile[MAXLEN_PATH];
};



