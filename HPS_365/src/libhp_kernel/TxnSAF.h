
/* 
 * File:   TxnSAF.h
 * Author: Tim Ma
 *
 * Created on 2019年12月10日, 上午9:30
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNSAF_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNSAF_H__

#include "TxnBase.h"
#include "Inc/IDataPack.h"
#include "Inc/IBaseCurl.h"

#include "libstructdefined.h"
#include "libheartland.h"

class PUBLIC_EXPORT CTxnSAF : public CTxnBase
{
public:
    CTxnSAF(IN HeartlandSoap *pcSoapKernel,
            IN IBaseCurl *pBaseCurlPtr);
    virtual ~CTxnSAF(void);

    virtual int PerformTxn(void);

    virtual int PerformTxn(IN char* pcJsonString,  OUT char* pszRspData, INOUT int* pnRspDataSize);

protected:

    virtual int DoPack(IN char* pcJsonString, OUT CByteStream *pcPackData);

    virtual int DoUnpack(IN char* pszHostString, OUT CByteStream *pcRespData);

    virtual void UpdateKMSUserData(void);

protected:
    HeartlandSoap*          m_pcSoapKernel;
    IBaseCurl*              m_pcBaseCurlPtr;
    int                     m_nSoapTxnType;
    
    char m_szTxnType[8];
    char m_szTxnAmount[32];
    char m_szTxnSafSequenceID[32];
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_TXNSAF_H__ */

