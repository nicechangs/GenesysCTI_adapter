//
//  iLib.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iLib.h
 * @brief   iCore에서 사용되는 전역 함수 모음
 *
 * @version v0.1
 * @see     없음
 * @date    2012.5.14.
 */

#pragma once

#include "iType.h"

/**
 * @enum  eVersion
 * @brief iVersion 함수 사용 시 Argument로 사용
 * @see   int32 iVersion(eVersion eIdx)
 */
enum eVersion
{
	eVerStart = 0,
	eVerMajor = 0,	/// Major 버전
	eVerMinor ,		/// Minor 버전
	eVerPatch ,		/// Patch 버전
	eVerBuild ,		/// Build 버전
	eVerHotfx ,		/// HotFix 버전
	eVerEnd
};

/**
 * @brief   iCORE Library 버전 확인
 * @return  iCORE Library 버전 정보를 문자열로 반환
 */
ILIBAPI NPCSTR iVersion();

/**
 * @brief   iCORE Library 버전 확인
 * @return  iCORE Library 버전 정보를 숫자 형태로 반환
 */
ILIBAPI int32 iVersion(eVersion eIdx);

//////////////////////////////////////////////////////////////////////////////
// Error 관련 함수
/**
 * @brief           마지막 오류코드 반환 함수
 * @return          오류코드
 */
ILIBAPI int32 iGetLastErr();

/**
 * @brief           오류메시지 반환 함수
 * @return          오류메시지
 * @param[in]       nErr 오류코드\n -1 입력 시 마지막 오류코드로 대체
 */
ILIBAPI NPCSTR iGetLastErrMsg(int32 nErr = -1);


//////////////////////////////////////////////////////////////////////////////
// Math 관련 함수
/**
 * @brief           난수 반환 함수
 * @return          난수
 * @param[in]       nModular 난수 영역 지정\n 0 입력 시 난수 원본 반환
 */
ILIBAPI uint32 iGetRandom(uint32 nModular = 0);



//////////////////////////////////////////////////////////////////////////////
// Time 관련 함수

/**
 * 현재 시각을 tv에 저장하여 반환
 * @brief           unix gettimeofday 대체 함수
 * @return          성공 여부(0:성공 else 실패)
 * @param[out]      tv NULL이 아닐 경우 현재 시각에 대한 timeval 셋팅
 * @note            Unix 계열 시각 함수들과의 호환성을 위해 개발됨
 *					참조 http://alones.kr/tag/gettimeofday
 */
ILIBAPI int32 iGetTimeOfDay(struct timeval *tv);

/**
 * unix localtime 대체 함수
 * @brief      pTime의 시각 정보를 struct tm 형식으로 변환
 * @return     성공 여부(true:성공, false:실패)
 * @param[in]  pTime   time_t
 * @param[in]  pstTime struct tm* 
 */
ILIBAPI bool iLocalTime(time_t* pTime, struct tm* pstTime);

/**
 * @brief           Sleep 함수
 * @return          없음
 * @param[in]       nMilisec Sleep할 시간(Milisec 단위)
 * @note            내부적으로 usleep(Unix계열), Sleep(Windows계열) 함수 사용
 */
ILIBAPI void iSleep(time_t nMilisec);

/**
 * @brief           프로세스 시작 기준 소요 시간 반환 함수(단위:Milisecond/Microsec)
 * @return          소요 시간(Milisecond/Microsec)
 * @note            프로세스 시작 기준 시간(초)부터 Milisecond/Microsec 단위
 */
ILIBAPI time_t iGetMilisec();

/**
 * @brief           프로세스 시작 기준 소요 시간 반환 함수(단위:Milisecond/Microsec)
 * @return          소요 시간(Milisecond/Microsec)
 * @note            프로세스 시작 기준 시간(초)부터 Milisecond/Microsec 단위
 */
ILIBAPI uint64 iGetMicrosec();

/**
 * @brief           시스템 시간을 Integer Type으로 반환 함수
 * @return          시스템 시간(Integer)
 * @param[in]       szFmt 요청 시각 정보\n'Y':Year(4Digits), 'M':Month(2Digits), 'D':Day(2Digits)\n'h':Hour(2Digits), 'm':Minute(2Digits), 's':Second(2Digits)\n'$':Milisec(3Digits), 'u':Microsec(3Digits)
 * @param[in,out]   pLen 반환값 길이 정보(optional)
 * @note            에제
@code
iGetCurTime("YMD")   => 20120518(2012. 5. 18)
iGetCurTime("hms$u") => 130001123456(13:00:01.123456)
@endcode
 */
ILIBAPI uint32 iGetCurTime(NPCSTR szFmt, uint32* pLen = NULL);

/**
 * @brief          시스템 시간을 Integer Type으로 반환 함수
 * @return         시스템 시간(Integer)
 * @param[in]      szFmt 요청 시각 정보\n'Y':Year(4Digits), 'M':Month(2Digits), 'D':Day(2Digits)\n'h':Hour(2Digits), 'm':Minute(2Digits), 's':Second(2Digits)\n'$':Milisec(3Digits), 'u':Microsec(3Digits)
 * @param[in,out]  pLen 반환값 길이 정보(optional)
 * @note           에제
@code
iGetCurTime64("YMD")   => 20120518(2012. 5. 18)
iGetCurTime64("hms$u") => 130001123456(13:00:01.123456)
@endcode
 */
ILIBAPI uint64 iGetCurTime64(NPCSTR szFmt, uint32* pLen = NULL);

/**
 * @brief  tTime 시각을 년월일 형식인 Integer Type으로 반환 함수
 * @return               날자 정보
 * @param  tTime         시각 정보(0 일 경우 현재 시각)
 */
ILIBAPI uint32 iGetDate(time_t tTime = 0);

/**
 * @brief  tTime 시각을 시분초 형식인 Integer Type으로 반환 함수
 * @return               날자 정보
 * @param  tTime         시각 정보(0 일 경우 현재 시각)
 */
ILIBAPI uint32 iGetTime(time_t tTime = 0);

/**
 * @brief  년,월,일,시,분,초 정보로 time_t 값을 계산하는 함수
 * @return              시각 정보
 * @param  nYear        년
 * @param  nMonth       월
 * @param  nDay         일
 * @param  nHour        시
 * @param  nMin         분
 * @param  nSec         초
 */
ILIBAPI time_t iMkTime(uint32 nYear = 0, uint32 nMonth = 0, uint32 nDay = 0, uint32 nHour = 0, uint32 nMin = 0, uint32 nSec = 0);

/**
 * @brief           시스템 시간을 지정된 형식으로 변환 후 반환 함수
 * @return          시스템 시간 문자열(szBuf 주소값)
 * @param[in]   	szFmt 시각 형식 문자열(#Y,#M,#D,#h,#m,#s)\n Milisec 이하의 시간은 추출할 수 없음
 * @param[out]  	szBuf 반환 문자열 버퍼
 * @param[in]       nBuf szBuf 문자열 버퍼의 크기
 * @warning         szBuf 크기가 부족할 경우 변환 도중 반환 되므로 주의할 것
 * @note            에제
@code
iGetStrTime("#Y년 #M월 #D일", szBuf, sizeof(szBuf))   => "2012년 05월 01일"
iGetStrTime("#Y년 #MM월 #DD일", szBuf, sizeof(szBuf)) => "2012년 5월 1일"
@endcode
 */
ILIBAPI NPSTR iGetStrTime(NPCSTR szFmt, NPSTR szBuf, size_t nBuf);

/**
 * @brief           지정된 시간을 지정된 형식으로 변환 후 반환 함수
 * @return          시간 문자열(szBuf 주소값)
 * @param[in]       tTime 시간 정보
 * @param[in]   	szFmt 시각 형식 문자열(#Y,#M,#D,#h,#m,#s)\n Milisec 이하의 시간은 추출할 수 없음
 * @param[out]  	szBuf 반환 문자열 버퍼
 * @param[in]       nBuf szBuf 문자열 버퍼의 크기
 * @warning         szBuf 크기가 부족할 경우 변환 도중 반환 되므로 주의할 것
 * @note            에제
@code
iGetTime2Str(tTime, "#Y년 #M월 #D일", szBuf, sizeof(szBuf))      => "2012년 05월 01일"
iGetTime2Str(tTime, "#Y년 #MM월 #DD일", szBuf, sizeof(szBuf))    => "2012년 5월 1일"
@endcode
 */
ILIBAPI NPSTR iGetTime2Str(time_t tTime, NPCSTR szFmt, NPSTR szBuf, size_t nBuf);


//////////////////////////////////////////////////////////////////////////////
// String 관련 함수
/**
 * @brief           문자열 길이 반환 함수
 * @return          문자열 길이
 * @param[in]       szStr 문자열 포인터
 * @param[in]       nMax 문자열 길이>nMax>0 일 경우 nMax 반환
 * @note            szStr이 NULL일 경우 0 반환
 */
ILIBAPI size_t iStrLen(NPCSTR szStr, size_t nMax = 0);

/**
 * @brief           문자열 복사 함수
 * @return          복사된 문자열(szDst) 반환
 * @param[out]      szDst 복사 될 문자열 버퍼 포인터
 * @param[in]       szSrc 복사 할 문자열 포인터
 * @param[in]       nLen 복사 문자열 길이 지정\n 0일 경우 전체 복사
 * @note            szSrc가 NULL일 경우 strcpy(szDst, "") 수행
 * @warning         szDst가 NULL일 경우 NULL 반환
 */
ILIBAPI NPSTR iStrCpy(NPSTR szDst, NPCSTR szSrc, size_t nLen = 0);

/**
 * @brief           문자열 추가 함수
 * @return          추가된 문자열(szDst) 반환
 * @param[in,out]   szDst 추가 될 문자열 버퍼 포인터
 * @param[in]       szSrc 추가 할 문자열 포인터
 * @param[in]       nLen 추가 할 문자열 길이 지정\n 0일 경우 전체 추가
 * @note            szSrc가 NULL일 경우 szDst 반환
 * @warning         szDst가 NULL일 경우 NULL 반환
 */
ILIBAPI NPSTR iStrCat(NPSTR szDst, NPCSTR szSrc, size_t nLen = 0);

/**
 * @brief           문자열 추가 함수
 * @return          추가된 문자열(szDst) 반환
 * @param[in,out]   szDst 추가 될 문자열 버퍼 포인터
 * @param[in]       szFmt 추가 할 문자열의 형식
 * @param[in]       ... szFmt 형식에 맞는 Argument List
 * @warning         szDst의 Buffer 크기 확인이 필요함
 */
ILIBAPI NPSTR iStrAdd(NPSTR szDst, NPCSTR szFmt, ...);

/**
 * @brief           문자열 추가 함수
 * @return          추가된 문자열(szDst) 반환
 * @param[in,out]   szDst 추가 될 문자열 버퍼 포인터
 * @param[in]       nDst szDst 버퍼의 크기
 * @param[in]       szFmt 추가 할 문자열의 형식
 * @param[in]       ... szFmt 형식에 맞는 Argument List
 */
ILIBAPI NPSTR iStrAdd(NPSTR szDst, size_t nDst, NPCSTR szFmt, ...);

/**
 * @brief           문자열 비교 함수
 * @return          문자열 비교 결과 반환(0:동일 else 다름)
 * @param[in]       szStr1 비교할 문자열1 포인터
 * @param[in]       szStr2 비교할 문자열2 포인터
 * @param[in]       nLen 비교 할 문자열 길이 지정\n 0일 경우 전체 비교
 * @note            szStr1, szStr2중 하나라도 NULL일 경우 -1 반환
 */
ILIBAPI int32 iStrCmp(NPCSTR szStr1, NPCSTR szStr2, size_t nLen = 0);

/**
 * @brief           알파벹 대소문자 구분없이 문자열 비교 함수
 * @return          문자열 비교 결과 반환(0:동일 else 다름)
 * @param[in]       szStr1 비교할 문자열1 포인터
 * @param[in]       szStr2 비교할 문자열2 포인터
 * @param[in]       nLen 비교 할 문자열 길이 지정\n 0일 경우 전체 비교
 * @note            szStr1, szStr2중 하나라도 NULL일 경우 -1 반환
 */
ILIBAPI int32 iStrCaseCmp(NPCSTR szStr1, NPCSTR szStr2, size_t nLen = 0);

/**
 * @brief           문자열 비교 함수
 * @return          문자열 비교 결과 반환(true:동일, false:다름)
 * @param[in]       szStr 비교 대상 문자열 포인터
 * @param[in]       szMatch 비교할 문자열 포인터
 * @param[in]       bCase 비교 시 대소문자 비교 여부
 * @note            szMatch 문자열의 길이까지만 비교함\n szMatch가 szStr보다 문자열 길이가 길 경우 false 반환
 */
ILIBAPI bool iStrMatch(NPCSTR szStr, NPCSTR szMatch, bool bCase = true);

/**
 * @brief           snprintf 대체 함수
 * @return          추가된 문자열 길이를 반환
 * @param[in,out]   szBuf 문자열 버퍼 포인터
 * @param[in]       nBuf szBuf 버퍼 크기
 * @param[in]       szFmt 추가 할 문자열의 형식
 * @param[in]       ... szFmt 형식에 맞는 Argument List
 * @note            szFmt와 Argument List를 조합하여 nBuf가 넘을 경우\n
 * szBuf[0]에 NULL로 초기화 하고 0을 반환함
 */
ILIBAPI int32 iStrPrint(NPSTR szBuf, int32 nBuf, NPCSTR szFmt, ...);

/**
 * @brief  정규식을 통한 문자열 확인 함수
 * @return                확인 결과(true:정규식에 적합, false:정규식에 어긋남)
 * @param  szStr          확인 대상 문자열
 * @param  szRegex        정규식 문자열
 * @param  bIgnoreCase    영문자 대소문자 차이를 무시할지 여부
 */
ILIBAPI bool iStrRegex(NPCSTR szStr, NPCSTR szRegex, bool bIgnoreCase = false);

/**
 * @brief  순방향 문자열 검색 함수 
 * @return               검색 성공 시 검색 위치 반환, 실패 시 NULL 반환 
 * @param  szStr         검색 대상 문자열 
 * @param  szKey         검색 키
 */
ILIBAPI NPCSTR iFindStr(NPCSTR szStr, NPCSTR szKey);

/**
 * @brief  순방향 문자열 검색 함수 
 * @return               검색 성공 시 검색 위치 반환, 실패 시 NULL 반환 
 * @param  szStr         검색 대상 문자열 
 * @param  pFirst        검색 대상 시작 위치
 * @param  szKey         검색 키
 */
ILIBAPI NPCSTR iFindStr(NPCSTR szStr, NPCSTR pFirst, NPCSTR szKey);

/**
 * @brief  역방향 문자열 검색 함수
 * @return                검색 성공 시 검색 위치 반환, 실패 시 NULL 반환 
 * @param  szStr         검색 대상 문자열 
 * @param  szKey         검색 키
 */
ILIBAPI NPCSTR iRFindStr(NPCSTR szStr, NPCSTR szKey);

/**
 * @brief  역방향 문자열 검색 함수 
 * @return               검색 성공 시 검색 위치 반환, 실패 시 NULL 반환 
 * @param  szStr         검색 대상 문자열 
 * @param  pLast         검색 대상 시작 위치
 * @param  szKey         검색 키
 */
ILIBAPI NPCSTR iRFindStr(NPCSTR szStr, NPCSTR pLast, NPCSTR szKey);

/**
 * @brief  문자열에서 주어진 문자들을 찾는 함수
 * @return               szDelim내 문자들 중 하나와 일치하는 szStr 문자열의 위치를 반환한다
 * @param  szStr         검색 대상 문자열 
 * @param  szDelim       검색 키가 되는 문자들의 집합
 */
ILIBAPI NPCSTR iFindTok(NPCSTR szStr, NPCSTR szDelim);

/**
 * @brief  문자열에서 주어진 문자들을 찾는 함수
 * @return               szDelim내 문자들 중 하나와 일치하는 szStr 문자열의 위치를 반환한다
 * @param  szStr         검색 대상 문자열 
 * @param  pFirst        검색 대상 시작 위치
 * @param  szDelim       검색 키가 되는 문자들의 집합
 */
ILIBAPI NPCSTR iFindTok(NPCSTR szStr, NPCSTR pFirst, NPCSTR szDelim);

/**
 * @brief  문자열에서 주어진 문자들을 역으로 찾는 함수
 * @return               szDelim내 문자들 중 하나와 일치하는 szStr 문자열의 위치를 반환한다
 * @param  szStr         검색 대상 문자열 
 * @param  szDelim       검색 키가 되는 문자들의 집합
 */
ILIBAPI NPCSTR iRFindTok(NPCSTR szStr, NPCSTR szDelim);

/**
 * @brief  문자열에서 주어진 문자들을 역으로 찾는 함수
 * @return               szDelim내 문자들 중 하나와 일치하는 szStr 문자열의 위치를 반환한다
 * @param  szStr         검색 대상 문자열 
 * @param  pLast         검색 대상 시작 위치
 * @param  szDelim       검색 키가 되는 문자들의 집합
 */
ILIBAPI NPCSTR iRFindTok(NPCSTR szStr, NPCSTR pLast, NPCSTR szDelim);

/**
 * @brief  strtok 함수와 동일
 * @return              다음 토큰에 대한 포인터를 반환하거나 만일 더 이상 토큰이 없다면 NULL을 반환한다.
 * @param  szStr        검색 대상 문자열 
 * @param  szDeli       검색 키가 되는 문자들의 집합
 * @param  ppBuf        다음 검색 위치
 */
ILIBAPI NPSTR iStrTok(NPSTR szStr, NPCSTR szDeli, NPSTR* ppBuf);

/**
 * @brief  문자열의 왼쪽 일부분을 복사하는 함수
 * @return               szDst를 반환
 * @param  szDst         복사될 문자열 버퍼(nCnt+1 보다 크기가 커야함)
 * @param  szSrc         복사 원본
 * @param  nCnt          복사할 문자열 길이(szSrc 길이보다 클 경우 처음 NULL까지 복사)
 */
ILIBAPI NPSTR iLeftStr(NPSTR szDst, NPCSTR szSrc, size_t nCnt);

/**
 * @brief  문자열의 중간 일부분을 복사하는 함수
 * @return               szDst를 반환
 * @param  szDst         복사될 문자열 버퍼(nCnt 보다 크거나 같아야 한다.)
 * @param  szSrc         복사 원본
 * @param  nPos          복사 시작이 되는 szSrc상에서의 위치
 * @param  nCnt          복사할 문자열 길이(szSrc+nPos에서의 길이보다 클 경우 처음 NULL까지 복사)
 */
ILIBAPI NPSTR iMidStr(NPSTR szDst, NPCSTR szSrc, size_t nPos, size_t nCnt = 0);

/**
 * @brief  문자열의 오른쪽 일부분을 복사하는 함수
 * @return               szDst를 반환
 * @param  szDst         복사될 문자열 버퍼(nCnt 보다 크거나 같아야 한다.)
 * @param  szSrc         복사 원본
 * @param  nCnt          왼쪽 기준으로 복사할 문자열 길이(szSrc 길이보다 클 경우 szSrc와 동일한 문자열을 복사함)
 */
ILIBAPI NPSTR iRightStr(NPSTR szDst, NPCSTR szSrc, size_t nCnt);

/**
 * @brief  문자열 중 특정 문자를 주어진 문자로 치환하는 함수
 * @return                   치환한 Count
 * @param  szStr             대상 문자열
 * @param  cFrom             검색 대상 문자
 * @param  cTo               치환할 문자
 */
ILIBAPI uint32 iReplaceChar(NPSTR szStr, char cFrom, char cTo);

/**
 * @brief  문자열 중 특정 문자열을 주어진 문자열로 치환하는 함수
 * @return                  치환한 Count
 * @param  szStr            대상 문자열
 * @param  nStr             szStr 문자열이 아닌 버퍼의 크기
 * @param  szFrom           검색 대상 문자열
 * @param  szTo             치환할 문자열
 */
ILIBAPI uint32 iReplaceStr(NPSTR szStr, size_t nStr, NPCSTR szFrom, NPCSTR szTo);

/**
 * @brief  문자열에서 특정 문자를 제거하는 함수
 * @return                  제거된 Count
 * @param  szStr            대상 문자열 
 * @param  cChar            삭제할 문자
 */
ILIBAPI uint32 iRemoveChar(NPSTR szStr, char cChar);

/**
 * @brief  문자열의 영문자를 모두 대문자화 하는 함수
 * @return                szStr을 반환
 * @param  szStr          대상 문자열
 */
ILIBAPI NPSTR iStrUpper(NPSTR szStr);

/**
 * @brief  문자열의 영문자를 모두 소문자화 하는 함수
 * @return                szStr을 반환
 * @param  szStr          대상 문자열
 */
ILIBAPI NPSTR iStrLower(NPSTR szStr);

/**
 * @brief  문자열의 앞 뒤에 있는 White Space를 제거하는 함수
 * @return            szStr을 반환 
 * @param  szStr      대상 문자열
 * @warning           원본을 변경함
 */
ILIBAPI NPSTR iTrim(NPSTR szStr);

/**
 * @brief  문자열의 앞쪽에 있는 White Space를 제거하는 함수
 * @return            szStr을 반환 
 * @param  szStr      대상 문자열
 * @warning           원본을 변경함
 */
ILIBAPI NPSTR iLTrim(NPSTR szStr);

/**
 * @brief  문자열의 뒷쪽에 있는 White Space를 제거하는 함수
 * @return            szStr을 반환 
 * @param  szStr      대상 문자열
 * @warning           원본을 변경함
 */
ILIBAPI NPSTR iRTrim(NPSTR szStr);

/**
 * @brief  대상 문자열에서 szMask에 주어진 문자 집합 이외의 문자를 제거
 * @return               szSrc를 반환
 * @param  szSrc         대상 문자열
 * @param  szMask        Masking 할 문자 집합
 * @warning              원본을 변경함
@code
// Digit만 추출
char szMyTelNo[] = "Tel No : 010-1234-5678";
iStrMask(szMyTelNo, "0123456789"); // => "01012345678"
@endcode
 */
ILIBAPI NPSTR iStrMask(NPSTR szSrc, NPCSTR szMask);

/**
 * @brief  대상 문자열에서 szMask에 주어진 문자 집합 이외의 문자를 제거
 * @return               szDst 반환
 * @param  szSrc         대상 문자열
 * @param  szDst         제거된 문자열을 저장할 버퍼(szSrc의 길이와 동일한 크기)
 * @param  szMask        Masking 할 문자 집합
 * @note                 원본을 변경하지 않고 szDst에 Masking된 문자열을 저장
 */
ILIBAPI NPSTR iStrMask(NPCSTR szSrc, NPSTR szDst, NPCSTR szMask);

/**
 * @brief  문자열을 Integer화 하는 함수
 * @return            변환한 Integer 값 반환
 * @param  szVal      대상 문자열
 * @param  nLen       숫자화 할 대상 문자열에서 길이를 지정(optional)
 */
ILIBAPI int64 iAtoI(NPCSTR szVal, size_t nLen = 0);

/**
 * @brief  문자열을 Unsigned Integer화 하는 함수
 * @return            변환한 Unsigned Integer 값 반환
 * @param  szVal      대상 문자열
 * @param  nLen       숫자화 할 대상 문자열에서 길이를 지정(optional)
 */
ILIBAPI uint64 iAtoU(NPCSTR szVal, size_t nLen = 0);

/**
 * @brief  문자열을 분석하여 Integer화 하는 함수
 * @return               변환한 Integer 값 반환
 * @param  szVal         대상 문자열
 * @param  nLen          숫자화 할 대상 문자열에서 길이를 지정(optional)
 * @note                 문자열의 형태를 분석하여 숫자화를 진행함
@code
iCvt2Num("0x10");	// => 16(16진법)
iCvt2Num("x10");	// => 16(16진법)
iCvt2Num("010");	// => 8(8진법)
iCvt2Num("B10");	// => 2(2진법)
iCvt2Num("b10");	// => 2(2진법)
iCvt2Num("123456");	// => 1234562(10진법)
@endcode
 */
ILIBAPI int64 iCvt2Num(NPCSTR szVal, size_t nLen = 0);

/**
 * @brief  Binary Data를 Hex화 하여 문자열로 변환하는 함수
 * @return               Hex 문자열 길이(Null 미포함)
 * @param  szSrc         원본 Binary Data
 * @param  nSrc          szSrc Data의 크기
 * @param  szDst         Hex 문자열 저장 버퍼
 * @param  nDst          szDst 버퍼의 크기
 * @param  cDeli         1Byte를 Hexa 변환 후 추가해줄 Delimeter
@code
iBin2Hex(szMac, sizeof(szMac), szMacAddr, sizeof(szMacAddr));		// => 123456789abc
iBin2Hex(szMac, sizeof(szMac), szMacAddr, sizeof(szMacAddr), ':');	// => 12:34:56:78:9a:bc
@endcode
 */
ILIBAPI size_t iBin2Hex(const iByte* szSrc, size_t nSrc, NPSTR szDst, size_t nDst, char cDeli = ILIB_NULLCHAR);

/**
 * @brief  Hex 문자열을 Binary Data로 변환하는 함수
 * @return               저장된 Binary Data의 크기 반환
 * @param  szSrc         변환할 문자열
 * @param  nSrc          변환 대상 문자열의 길이
 * @param  szDst         Binary Data를 저장할 버퍼
 * @param  nDst          szDst 버퍼의 크기
@code
char szMacAddr[] = "12:34:56:78:9a:bc"
iHex2Bin(szMacAddr, sizeof(szMacAddr), szMac, sizeof(szMac));
@endcode
 */
ILIBAPI size_t iHex2Bin(NPCSTR szSrc, size_t nSrc, iByte* szDst, size_t nDst);

//////////////////////////////////////////////////////////////////////////////
// File 관련 함수
/**
 * @brief  Directory 생성 함수
 * @return             성공 여부
 * @param  szPathname  생성할 Directory Path
 * @note               부모 Directory가 존재하지 않을 경우 부모 Directory도 생성함
 * @note               Unix 계열의 경우 생성 Directory의 권한은 0777로 생성됨
 */
ILIBAPI bool iMkDir(NPCSTR szPathname);

/**
 * @brief  현재 Directory를 변경하는 함수
 * @return             성공 여부
 * @param  szPathname  변경하고 싶은 Directory Path
 * @see iSetCurDir
 */
ILIBAPI bool iChDir(NPCSTR szPathname);

/**
 * @brief  Directory 삭제 함수 
 * @return             성공 여부
 * @param  szPathname  삭제하고 싶은 대상 Directory Path
 */
ILIBAPI bool iRmDir(NPCSTR szPathname);

/**
 * @brief  현재 Directory를 가져오는 함수 
 * @return                 성공 여부
 * @param  szPathname      현재 Directory를 저장할 버퍼
 * @param  nPathname       szPathname 버퍼의 크기
 */
ILIBAPI bool iGetCurDir(NPSTR szPathname, int32 nPathname);

/**
 * @brief  현재 Directory를 지정하는 함수 
 * @return                 성공 여부
 * @param  szPathname      변경하고 싶은 Directory Path
 * @see iChDir
 */
ILIBAPI bool iSetCurDir(NPCSTR szPathname);

/**
 * @brief  Directory 존재 여부를 반환하는 함수 
 * @return                  Directory 존재 여부를 반환
 * @param  szPathname       대상 Directory Path
 */
ILIBAPI bool iIsExistDir(NPCSTR szPathname);

/**
 * @brief  File 존재 여부를 반환하는 함수 
 * @return                  File 존재 여부를 반환
 * @param  szFilename       대상 File Path
 */
ILIBAPI bool iIsExistFile(NPCSTR szFilename);

/**
 * @brief  File 이동 함수
 * @return                성공 여부
 * @param  szSrc          원본 파일
 * @param  szDst          대상 파일
 */
ILIBAPI bool iMoveFile(NPCSTR szSrc, NPCSTR szDst);

/**
 * @brief  File 복사 함수
 * @return                성공 여부
 * @param  szSrc          원본 파일
 * @param  szDst          대상 파일
 */
ILIBAPI bool iCopyFile(NPCSTR szSrc, NPCSTR szDst);

/**
 * @brief  File 삭제 함수
 * @return                  성공 여부
 * @param  szFilename       대상 파일
 */
ILIBAPI bool iRemoveFile(NPCSTR szFilename);

/**
 * @brief  Symbolic Link 생성 함수
 * @return                성공 여부
 * @param  szFilename     대상 파일
 * @param  szLinkname     Link 파일
 * @warning               Windows에서는 동작하지 않음(무조건 true를 반환)
 */
ILIBAPI bool iLinkFile(NPCSTR szFilename, NPCSTR szLinkname);

/**
 * @brief  파일 크기를 반환 하는 함수
 * @return                   파일 크기를 반환
 * @param  szFilename        대상 파일
 */
ILIBAPI size_t iGetFileSize(NPCSTR szFilename);

/**
 * @brief  파일의 마지막 수정 시각을 반환하는 함수
 * @return                    마지막 수정 시각을 반환 
 * @param  szFilename         대상 파일
 */
ILIBAPI time_t iGetFileMTime(NPCSTR szFilename);


ILIBAPI NPCSTR iBasename(NPCSTR szPath);


//////////////////////////////////////////////////////////////////////////////
// Process 관련 함수
/**
 * @brief  프로세스 실행중인지 확인하는 함수
 * @return                 프로세스 실행 여부
 * @param  nPid            프로세스 id
 */
ILIBAPI bool iIsRunProc(pid_t nPid);

/**
 * @brief  자신 프로세스의 ID 값을 반환
 * @return 프로세스 id
 */
ILIBAPI pid_t iGetProcId();

/**
 * @brief  자신 쓰레드의 ID 값을 반환
 * @return 쓰레드 id
 */
ILIBAPI pthread_t iGetThreadId();

/**
 * @brief  프로세스의 실행 파일 위치 반환(실행 파일 포함)
 * @return                   반환되는 경로의 길이(실패 시 -1 반환)
 * @param  nPid              대상 프로세스 id
 * @param  szBuf             경로 저장 버퍼
 * @param  nBuf              버퍼의 크기
 */
ILIBAPI int32 iGetProcPath(pid_t nPid, NPSTR szBuf, size_t nBuf);

/**
 * @brief  현재 사용자 ID를 반환
 * @return 사용자 ID
 * @warning Windows에서는 "" 반환
 */
ILIBAPI NPCSTR iGetUserId();

/**
 * @brief  현재 사용자의 Home Path를 반환
 * @return Home Path
 * @warning Windows에서는 "" 반환
 */
ILIBAPI NPCSTR iGetUserHome();

/**
 * @brief  프로세스 중복 방지 여부 반환
 * @param  szProcName 프로세스명(사용자 지정 가능함)
 * @param  szPidPath  Locking 파일 저장할 Path(ex: /var/run)
 * @return            해당 프로세스가 이미 실행중(true)인지 여부 반환
 * @warning Windows에서는 szPidPath가 큰 의미는 없음
 */
ILIBAPI bool IsRunningProc(NPCSTR szProcName, NPCSTR szPidPath);

//////////////////////////////////////////////////////////////////////////////
// Memory Debuging 관련 함수
ILIBAPI NPVOID iMalloc(size_t nSize);
ILIBAPI void iFree(NPVOID pPtr);
ILIBAPI void iMemSts(uint32 nLine, NPCSTR szFmt, ...);

//////////////////////////////////////////////////////////////////////////////
// Debuging 관련 함수
ILIBAPI void iMemDump(NPCSTR szName, NPCSTR pDump, size_t nBytes);
ILIBAPI void iAssert(bool bAssert);
ILIBAPI void iDebug(NPCSTR szMsg);
ILIBAPI void iDebug(NPCSTR szMsg1, NPCSTR szMsg2);
ILIBAPI void iTrace(NPCSTR szFmt, ...);
ILIBAPI void iTraceTm(NPCSTR szFmt, ...);
ILIBAPI void iPrint(NPCSTR szFmt, ...);
ILIBAPI void iPrintTm(NPCSTR szFmt, ...);
ILIBAPI void iPrintLn(NPCSTR szFmt, ...);
ILIBAPI void iDispLn(NPCSTR szDisp);
ILIBAPI void iHexDump(NPCSTR szBuf, size_t nLen, NPCSTR szFmt, ...);

#define ITRACE(fmt,...)	iTraceTm(" %s +%-4d |%s|" fmt,__FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#define DBGTRC			iTraceTm(" %s +%-4d |%s|", __FILE__, __LINE__, __FUNCTION__)
#define DBGSTR(s)		iTraceTm(" %s +%-4d |%s|%s|", __FILE__, __LINE__, __FUNCTION__, s?s:"$NULL")
#define DBGINT(x)		iTraceTm(" %s +%-4d |%s|%d|", __FILE__, __LINE__, __FUNCTION__, x)
#define DBGINT64(x)		iTraceTm(" %s +%-4d |%s|%lld|", __FILE__, __LINE__, __FUNCTION__, x)
#define DBGPTR(x)		iTraceTm(" %s +%-4d |%s|%p|", __FILE__, __LINE__, __FUNCTION__, x)

