//
//  iPoll.h
//
//
//  Created by Tony Ryu on 2012.12.7.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iPoll.h
 * @brief   Polling 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012.12.7
 */

#pragma once

#include <map>
#include "iMutex.h"
#include "iThread.h"

#ifdef ILINUX
#include <sys/epoll.h>
typedef struct epoll_event		iPollEvt;
enum
{
	IPOLLIN				= EPOLLIN,
	IPOLLOUT			= EPOLLOUT,
	IPOLLERR			= EPOLLERR,
	IPOLLHUP			= EPOLLHUP,
};
#else
enum
{
	IPOLLIN				= 0,
	IPOLLOUT			,
	IPOLLERR			,
	IPOLLHUP			,
};
#endif

#define IPOLL_INFINITE			(ILIB_INFINITE)
#define IPOLL_DEFOPT			(IPOLLIN)
#define MAXCNT_IPOLLEVT			(32)

/**
 * @class    iPoll
 * @brief    epoll을 사용한 비동기 Event 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2012.12.7
 * @note     iPoll은 상속을 통해서 사용하고 아래 함수들을 직접 구현
- virtual bool OnRead(iFd hFd, NPVOID pArg);
- virtual bool OnWrite(iFd hFd, NPVOID pArg);
- virtual bool OnError(iFd hFd, NPVOID pArg);
- virtual bool OnHangUp(iFd hFd, NPVOID pArg);
- virtual void OnDelete(iFd hFd, NPVOID pArg);
- virtual void OnTimeout();
 * @warning  해당 Class는 Linux OS에서만 동작
 */
class ILIBAPI iPoll : public iThread
{
public:
	iPoll();
	virtual ~iPoll();

	// for Debugging
	uint32 GetFdCnt() { return mapArgs.size(); };

	/**
	 * @brief  epoll 초기화 함수
	 * @return               성공 여부
	 * @param  nEvtSize      동시 처리 Event 수
	 * @param  nWaitTime     OnTimeout Callback 처리 Time(해당 Time 동안 아무 Evnet도 발생하지 않는다면 OnTimeout Callback을 호출함)
	 */
	bool InitPoll(uint32 nEvtSize, uint32 nWaitTime = ONESEC);

	/**
	 * @brief  epoll 해제 함수
	 * @return 항상 false를 반환함
	 */
	bool FreePoll();

	/**
	 * @brief  epoll의 Open 여부를 반환
	 * @return epoll의 Open 여부
	 */
	inline bool IsOpened() { return bool(mhPoll>=0); };

	/**
	 * @brief  Event를 받을 File Descriptor를 추가
	 * @return             성공 여부
	 * @param  hFd         대상 File Descriptor
	 * @param  pArg        Callback에서 수신할 User Pointer
	 * @param  nEvent      수신 Event Masking(기본으로 IN Event만 수행)
	 */
	bool Insert(iFd hFd, NPVOID pArg, int32 nEvent = IPOLL_DEFOPT);

	/**
	 * @brief  File Descriptor Event 변경
	 * @return             성공 여부
	 * @param  hFd         대상 File Descriptor
	 * @param  pArg        Callback에서 수신할 User Pointer
	 * @param  nEvent      수신 Event Masking(기본으로 IN Event만 수행)
	 */
	bool Update(iFd hFd, NPVOID pArg, int32 nEvent = IPOLL_DEFOPT);

	/**
	 * @brief  File Descriptor 삭제
	 * @return             성공 여부
	 * @param  hFd         대상 File Descriptor
	 */
	bool Delete(iFd hFd);

	/**
	 * @brief OnTimeout Callback 함수의 Time 설정 함수
	 * @param nWaitTime        OnTimeout Callback 처리 Time(해당 Time 동안 아무 Evnet도 발생하지 않는다면 OnTimeout Callback을 호출함)
	 */
	void SetWaitTime(int32 nWaitTime);
protected:
	iFd mhPoll;
	uint32 mnEvtSize;
	int32 mnWaitTime;
	iMutex mstCs;
	typedef std::map<iFd, NPVOID> iPollArg;
	iPollArg mapArgs;

	virtual NPVOID ThreadRun(NPVOID pArg);
	virtual bool EvtProc(int32 nEvent, iFd hFd, NPVOID pArg);
	virtual bool OnRead(iFd hFd, NPVOID pArg) { return false; };
	virtual bool OnWrite(iFd hFd, NPVOID pArg) { return false; };
	virtual bool OnError(iFd hFd, NPVOID pArg) { return false; };
	virtual bool OnHangUp(iFd hFd, NPVOID pArg) { return false; };
	virtual void OnDelete(iFd hFd, NPVOID pArg) {};
	virtual void OnTimeout() {};
};








