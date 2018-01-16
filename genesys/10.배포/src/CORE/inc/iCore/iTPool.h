//
//  iTPool.h
//  iCore
//
//  Created by Tony Ryu on 2012. 11. 12..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    iTPool.h
 * @brief   Threading Pool 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012. 11. 12.
 */

#pragma once

#include "iQue.h"
#include "iMutex.h"
#include "iThread.h"

#define DEFMAX_TPOOL        (128)
#define DEFTIME_MANAGE      (5*ONESEC)

typedef struct
{
    size_t                  nSize;
} TPOOLJOB, *PTPOOLJOB;

class ILIBAPI iTPool : public iThread
{
public:
    iTPool();
    virtual ~iTPool();

    uint32 GetMaxThreads() { return mnThreads; };
    uint32 GetActThreads() { return (mnThreads-mqTPool.GetCnt()); };
    uint32 GetIdlThreads() { return mqTPool.GetCnt(); };
    uint32 GetManageTime() { return mtMngTime; };

    bool TPoolStart(uint32 nMaxThreads = DEFMAX_TPOOL, uint32 nMngTime = DEFTIME_MANAGE);
    bool TPoolStop(bool bNow = true);

    int32 DeligateJob(PTPOOLJOB pJob);

protected:
    typedef struct
    {
        int32               nIdx;
        iTPool*             pMain;
#ifdef IWINDOWS
        HANDLE              hThread;
        HANDLE              hEvent;
        CRITICAL_SECTION    hMutex;
#else
        pthread_t           hThread;
        pthread_cond_t      hEvent;
        pthread_mutex_t     hMutex;
        pthread_attr_t      hAttr;
#endif
        PTPOOLJOB           pJob;
    } TPOOLVAR, *PTPOOLVAR;

    bool InitTPool(uint32 nMaxThreads);
    void FreeTPool();

    PTPOOLJOB GetJobs(uint32 nTIdx) { return mpThread[nTIdx].pJob; };

    uint32 RegistThread(PTPOOLVAR pVar);
    int32 WaitEventPool(PTPOOLVAR pVar);
    friend NPVOID iTPoolFunc(NPVOID pArg);

    // You can override below functions.
    virtual NPVOID ThreadRun(NPVOID pArg);
    virtual NPVOID TPoolMain(PTPOOLVAR pVar);
    virtual void TPoolThread(int32 nIdx, PTPOOLJOB pJob) {};
    virtual bool TPoolManage() { return true; };
    virtual void OnCloseThread(uint32 nIdx) { };

private:
    uint32 mnThreads;
    uint32 mtMngTime;
    iMutex mstCs;
    typedef iQue<PTPOOLVAR> iTPoolQue;
    iTPoolQue mqTPool;
    PTPOOLVAR mpThread;
};
