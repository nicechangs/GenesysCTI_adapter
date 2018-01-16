/*
***************************************************************************
        Bridgetec.cop 
        CTI Bridge Adapter for Genesys IVR Server.

        TITLE   : TcpSessionBase
        WRITER  : KIMCG
        DATE    : 2014.09.16 
        
        Copyright (c) 2014-2014 ALL RIGHTS RESERVED.
***************************************************************************
*/


#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "QueueBase.h"

using namespace std;

// classes
class CQueueOfResponse: public CQueueBase
{
    public:
        // constructor && destructor
        CQueueOfResponse(void);
        virtual ~CQueueOfResponse(void);

    public:
        // sigleton ..
        static CQueueOfResponse* Inst(); 
        static void FreeInst();

    private:
        static CQueueOfResponse  *m_pInstance;

};

