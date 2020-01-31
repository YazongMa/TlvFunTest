
/*
 * File Name: HPApiContext.cpp
 * Author: Alan.Ren
 * 
 * Creat Date: 2019.4.1
 */

#include "HPApiContext.h"

#include "Inc/KernelState.h"
#include "KernelLayerObjs.h"
#include "JsonReqCmd.h"

/*
 * Public.
 */
CHPApiContext::CHPApiContext(): 
m_nCurState(kInvalid),
m_ulStateTimeout(ULONG_MAX),
m_bEnableCancel(FALSE),
m_nCurApiMode(kWaitingNewJsonCmd),
m_nLastApiMode(kWaitingNewJsonCmd),
m_pParamPtr(HLKE_GetParamPtr()),
m_bEnableResponse(TRUE),
cApiThreadId_(-1),
m_nDetectedCardType(kUnknown)
{
    pthread_mutex_init(&cMutex_, NULL);
    pthread_mutex_init(&cApiCallMutex_, NULL);
}

/*
 * public.
 */
CHPApiContext::~CHPApiContext()
{
    pthread_mutex_destroy(&cMutex_);
    pthread_mutex_destroy(&cApiCallMutex_);
}
