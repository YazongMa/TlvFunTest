
/*
 * File Name: TxnBase.cpp
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#include "TxnBase.h"

unsigned int CTxnBase::dwState_ = -1;
pthread_mutex_t CTxnBase::cMutex_ = PTHREAD_MUTEX_INITIALIZER;
BOOL CTxnBase::bCancelFlag_ = FALSE;
BOOL CTxnBase::bFallbackFlag_ = FALSE;


#define Lock() { \
    pthread_mutex_lock(&CTxnBase::cMutex_); \
}

#define Unlock() { \
    pthread_mutex_unlock(&CTxnBase::cMutex_); \
}


/**
* Function:
*      CTxnBase's Construct Function:
*
* @param
*      [none]
*
* @return
*      [none]
*
* Author: Tim.Ma
*/
CTxnBase::CTxnBase()
{ }


/**
* Function:
*      CTxnBase's Destruct Function:
*
* @param
*      [none]
*
* @return
*      [none]
*
* Author: Tim.Ma
*/
CTxnBase::~CTxnBase()
{ }


/**
* Function:
*      GetState, get kernel's state
*
* @param
*      [none]
*
* @return
*      unsigned int: the state of kernel
*
* Author: Tim.Ma
*/
unsigned int CTxnBase::GetState(void)
{
    Lock();
    const unsigned int nState = dwState_;
    Unlock();
    return nState;
}


/**
* Function:
*      EnableCancelTxn, set a flag to txn flow to cancel the current transaction
*
* @param
*      [in]bEnable: 
*
* @return
*      0: no practical meaning 
*
* Author: Tim.Ma
*/
unsigned int CTxnBase::EnableCancelTxn(IN const BOOL &bEnable)
{
    Lock();
    bCancelFlag_ = bEnable;
    Unlock();
    return 0;
}

/**
* Function:
*      IsEnableCancelTxn, get the flag cancel the current txn flow or not
*
* @param
*      [none] 
*
* @return
*      BOOL: TRUE: cancel the current txn flow
*             FALSE: don't cancel the current txn flow
*
* Author: Tim.Ma
*/
BOOL CTxnBase::IsEnableCancelTxn(void)
{
    Lock();
    const BOOL bEnable = bCancelFlag_;
    Unlock();

    return bEnable;
}

/**
* Function:
*      SetState, set kernel's state
*
* @param
*      [out]nState: the state of kernel
*
* @return
*      [none]
*
* Author: Tim.Ma
*/
void CTxnBase::SetState(const unsigned int &nState)
{
    Lock();
    dwState_ = nState;
    Unlock();
}
