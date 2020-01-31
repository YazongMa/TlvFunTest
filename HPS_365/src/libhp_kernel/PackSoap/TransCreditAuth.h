
/* 
 * File:   TransCreditAuth.h
 * Author: Tim.Ma
 *
 * Created on 2019.03.14
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TransCreditAuth_H__
#define __HEARTLAND_PAYMENT_KERNEL_TransCreditAuth_H__


#include "TransCreditSale.h"

class PUBLIC_EXPORT TransCreditAuth : public TransCreditSale
{
public:
    TransCreditAuth(IN IParameter *pcTransParam,
                  IN HeartlandSoap *pcSoapKernel,
                  IN CVoltageObject *pcVoltageObj);
    virtual ~TransCreditAuth();
    
protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_TransCreditAuth_H__ */

