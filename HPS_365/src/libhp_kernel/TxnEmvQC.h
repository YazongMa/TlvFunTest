/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxnEmvQC.h
 * Author: Charlie.Zhao&Peter.Yuan
 *
 * Created on 2019年10月29日, 上午11:20
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNEMVQC_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNEMVQC_H__

#include "TxnEmv.h"


class PUBLIC_EXPORT CTxnEmvQC : public CTxnEmv
{
public:
    CTxnEmvQC(IN IParameter *pParamPtr, 
            IN IDataPack *pDataPackPtr,
            IN IBaseCurl *pBaseCurlPtr,
            IN CEMVApi *pEmvApi,
            IN IUserAction *pUserAction);
    
    virtual ~CTxnEmvQC();

    virtual int PerformTxn(void);
    
protected:
    void TxnOnline(void);
    void GetTxnResult(const bool bOnline);

};

#endif