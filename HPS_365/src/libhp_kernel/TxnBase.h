
/*
 * File Name: TxnBase.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#ifndef _H_H_TXN_BASE_H_H_
#define _H_H_TXN_BASE_H_H_

#include <ctosapi.h>
#include <pthread.h>

#include "BasicTypeDef.h"

class PUBLIC_EXPORT CTxnBase
{
public:
    CTxnBase();
    virtual int PerformTxn(void) = 0;
    virtual int InitLevel2Lib(void){ return 0; };
    virtual bool IsInteracOfEMVCL(void) { return false; };
    
public:
    static unsigned int EnableCancelTxn(const BOOL &bEnable);
    static BOOL IsEnableCancelTxn(void);
    static unsigned int GetState(void);
    static void SetState(const unsigned int &nState);
    
protected:
    virtual ~CTxnBase();

protected:
    static BOOL             bFallbackFlag_;
    
private:
    static BOOL             bCancelFlag_;
    static unsigned int     dwState_;
    static pthread_mutex_t  cMutex_;
};

#endif
