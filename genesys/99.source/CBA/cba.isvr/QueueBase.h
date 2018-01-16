/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : QueueBase
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/

#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <queue>
#include <list>
#include <iMutex.h>
#include <iostream>
#include "CBInterface.h"


using namespace std;

typedef struct _QUEUE_ITEM
{
    char szCBSessionName[32];
    char szCBSessionIp[32];
    int32 nCBSessionPort;

    char szIServerSessionName[32];
    char szIServerSessionIp[32];
    int32 nIServerSessionPort;

    int nRequestPacketLen;
    int nResponsePacketLen;

    char szRequestPacket[SOCKET_BUFFER_SIZE];
    char szRequestPacketBody[4096];

    char szResponsePacket[SOCKET_BUFFER_SIZE];
    char szResponsePacketBody[4096];

    pktXCCPHeader  stXCCPHeader;

    int nTeNo;
    int nChannelNo;
    
}QUEUE_ITEM;

class CQueueBase
{
    // constructor && destructor
    public:        
        CQueueBase(void);
        virtual ~CQueueBase(void);

    // implementation
    public:
        int Enqueue(QUEUE_ITEM stItem);
        QUEUE_ITEM Dequeue();
        int GetCount();

    // fields..
    public:
        string m_strName;
        std::queue<QUEUE_ITEM> m_queue;

    protected:
    	iMutex m_mutexLock;
};


