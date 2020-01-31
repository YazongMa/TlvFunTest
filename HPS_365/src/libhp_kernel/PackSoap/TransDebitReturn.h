
/* 
 * File:   TransDebitReturn.h
 * Author: Tim.Ma
 *
 * Created on 2018/8/2日, 14:05
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TRANSDEBITRETURN_H__
#define __HEARTLAND_PAYMENT_KERNEL_TRANSDEBITRETURN_H__

#include "../BasicTypeDef.h"
#include "../Inc/IParameter.h"
#include "../Inc/DefParamTypeId.h"
#include "../TxnTag.h"
#include "../VoltageObject.h"
#include "../FillTxnData.h"



#include "TransDebitSale.h"


class PUBLIC_EXPORT TransDebitReturn : public TransDebitSale
{
public:
    TransDebitReturn(IN IParameter *pcTransParam,
                    IN HeartlandSoap *pcSoapKernel,
                    IN CVoltageObject *pcVoltageObj);
    virtual ~TransDebitReturn(void);
    
protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_TRANSDEBITRETURN_H__ */

