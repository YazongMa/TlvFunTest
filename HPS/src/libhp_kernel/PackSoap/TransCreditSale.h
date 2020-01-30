
/* 
 * File:   TransCreditSale.h
 * Author: Tim.Ma
 *
 * Created on 2018/8/2日, 14:05
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TRANSCREDITSALE_H__
#define __HEARTLAND_PAYMENT_KERNEL_TRANSCREDITSALE_H__

#include "../BasicTypeDef.h"
#include "../Inc/IParameter.h"
#include "../Inc/DefParamTypeId.h"
#include "../TxnTag.h"
#include "../VoltageObject.h"
#include "../FillTxnData.h"


#include "BaseSoap.h"


class PUBLIC_EXPORT TransCreditSale : public CBaseSoap
{
public:
    TransCreditSale(IN IParameter *pcTransParam,
                    IN HeartlandSoap *pcSoapKernel,
                    IN CVoltageObject *pcVoltageObj);
    virtual ~TransCreditSale(void);

    virtual int DoPack(OUT CByteStream *pcPackData);

    virtual int DoUnpack(const CByteStream &cData);
    
protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHSoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
    virtual int  GetTokenFlag(void);

protected:
    HeartlandSoap*          m_pcSoapKernel;
    CFillTxnData            m_cFillTxnData;
    int                     m_nSoapTxnType;
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_TRANSCREDITSALE_H__ */

