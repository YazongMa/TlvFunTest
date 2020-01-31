
/* 
 * File:   TransBatchClose.h
 * Author: Tim.Ma 
 *
 * Created Date:
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_BATCHCLOSE_H__
#define __HEARTLAND_PAYMENT_KERNEL_BATCHCLOSE_H__

#include "../FillTxnData.h"
#include "../BasicTypeDef.h"
#include "../Inc/IParameter.h"


#include "BaseSoap.h"

class PUBLIC_EXPORT TransBatchClose : public CBaseSoap
{
public:
    TransBatchClose(IN IParameter* pcTransParam, IN HeartlandSoap* pcSoapKernel);
    virtual ~TransBatchClose();
    
    virtual int DoPack(OUT CByteStream *pcPackData);
    virtual int DoUnpack(const CByteStream &cData);
    
protected:
      WORD GetSoapData(ST_HSOAP_RESPONSE* pcHsoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);

private:
    HeartlandSoap*          m_pcSoapKernel;
    CFillTxnData            m_cFillTxnData;
    int                     m_nSoapTxnType;
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_BATCHCLOSE_H__ */

