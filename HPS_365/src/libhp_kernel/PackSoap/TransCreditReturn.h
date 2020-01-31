/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TransCreditReturn.h
 * Author: Administrator
 *
 * Created on 2019.03.14
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TRANSCREDITRETURN_H__
#define __HEARTLAND_PAYMENT_KERNEL_TRANSCREDITRETURN_H__


#include "TransCreditSale.h"

class PUBLIC_EXPORT TransCreditReturn : public TransCreditSale
{
public:
    TransCreditReturn(IN IParameter *pcTransParam,
                  IN HeartlandSoap *pcSoapKernel,
                  IN CVoltageObject *pcVoltageObj);
    virtual ~TransCreditReturn();
    
protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
};

#endif /* TRANSCREDITRETURN_H */

