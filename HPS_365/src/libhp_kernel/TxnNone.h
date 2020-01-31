
/* 
 * File:   TxnNone.h
 * Author: Tim.Ma
 *
 * Created on 2019.03.19
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNNONE_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNNONE_H__


#include "TxnContext.h"

class PUBLIC_EXPORT CTxnNone : public CTxnContext
{
public:
    CTxnNone(IN IParameter *pParamPtr, 
            IN IDataPack *pDataPackPtr, 
            IN IBaseCurl *pBaseCurlPtr, 
            IN IUserAction *pUserAction);

    virtual ~CTxnNone();

    virtual int PerformTxn(void);
    
protected:
    virtual WORD    PostProcessing_SAF(void);
    
};

#endif
