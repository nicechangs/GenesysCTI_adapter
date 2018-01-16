//
//  iMsgQue.h
//
//
//  Created by Tony Ryu on 2012.12.7.
//  Copyright (c) 2012 Tony Ryu. All rights reserved.
//

/**
 * @file    iMsgQue.h
 * @brief   Double Locked Queue를 이용한 Message Queue 처리.
 * @warning Unix 계열에서는 Double Lock을 사용, Windows에서는 Event 사용.
 *
 * @version v0.1
 * @see     None
 * @date    2012.12.7
 */

#pragma once

#include "iQue.h"
#include "iMutex.h"
#include "iThread.h"

#define DEFMAXCNT_MSG				(1024)

// pMsg를 수동으로 삭제할 경우 return false; Otherwise MUST return true;
typedef bool (*MSGCBFUNC)(NPSTR pMsg, int32 nMsg);

class ILIBAPI iMsgQue : public iThread
{
	typedef iQue<NPSTR>				iMsgDeq;
public:
	iMsgQue(uint32 nMaxMsg = DEFMAXCNT_MSG);
	virtual ~iMsgQue();

	bool StartMsgQue(MSGCBFUNC pCbFunc = NULL);
	void StopMsgQue();

	void Clear();
	uint32 GetMsgCnt() { return mqMsg.GetCnt(); };
	void SetMaxMsg(uint32 nMaxMsg) { mnMaxMsg = nMaxMsg; };

	bool PshMsg(NPCSTR pMsg, int32 nMsg);
	NPSTR PopMsg(int32& nMsg);

	void DelMsg(NPSTR pMsg);

	virtual bool SetEvent();
protected:
	NPSTR NewMsg(NPCSTR pMsg, int32 nMsg);

	virtual NPVOID ThreadRun(NPVOID pArg);
	virtual void OnStart(NPVOID pArg);
	virtual void OnStop(NPVOID pArg);

	virtual bool OnPopMsg(NPSTR pMsg, int32 nMsg) { return true; };
private:
	bool mbQuing;
	uint32 mnMaxMsg;
	iMutex mcsQue;
#ifdef IWINDOWS
	iEvent mEvtWait;
#else
	iMutex mcsWait;
#endif
	iMsgDeq mqMsg;
	MSGCBFUNC mpCallback;
};

