
/* 
 * File:   TransSAF.h
 * Author: Charlie.Zhao
 *
 * Created on 2019年11月11日, 上午9:30
 */

#ifndef TRANSSAF_H
#define TRANSSAF_H

#include "../BasicTypeDef.h"
#include "../Inc/IParameter.h"
#include "../Inc/DefParamTypeId.h"
#include "../TxnTag.h"
#include "../VoltageObject.h"
#include "../FillTxnData.h"


#include "BaseSoap.h"

class PUBLIC_EXPORT TransSAF : public CBaseSoap
{
public:
    TransSAF(IN IParameter *pcTransParam,
                    IN HeartlandSoap *pcSoapKernel,
                    IN CVoltageObject *pcVoltageObj);
    virtual ~TransSAF(void);

    virtual int DoPack(OUT CByteStream *pcPackData);

    virtual int DoUnpack(const CByteStream &cData);

protected:
    virtual WORD GetSoapData(ST_HSOAP_RESPONSE* pcHSoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);

protected:
    HeartlandSoap*          m_pcSoapKernel;
    CFillTxnData            m_cFillTxnData;
    int                     m_nSoapTxnType;
    
    char m_szTxnType[8];
    char m_szTxnAmount[32];
    char m_szTxnSafSequenceID[32];

};

#endif /* TRANSSAF_H */

