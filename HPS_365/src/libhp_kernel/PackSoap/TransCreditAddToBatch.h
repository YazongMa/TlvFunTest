
/* 
 * File:   TransCreditAddToBatch.h
 * Author: Tim.Ma
 *
 * Created on 2019.03.14
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TRANSAUTHCOMPLETE_H__
#define __HEARTLAND_PAYMENT_KERNEL_TRANSAUTHCOMPLETE_H__

#include "../BasicTypeDef.h"
#include "../Inc/IParameter.h"
#include "../Inc/DefParamTypeId.h"
#include "../TxnTag.h"
#include "../VoltageObject.h"
#include "../FillTxnData.h"


#include "TransCreditVoid.h"


class PUBLIC_EXPORT TransCreditAddToBatch : public TransCreditVoid
{
public:
    TransCreditAddToBatch(IN IParameter *pcTransParam,
                    IN HeartlandSoap *pcSoapKernel,
                    IN CVoltageObject *pcVoltageObj);
    virtual ~TransCreditAddToBatch(void);
    
protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_TransCreditAddToBatch_H__ */

