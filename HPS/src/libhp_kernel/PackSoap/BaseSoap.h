
/*
 * File Name: BaseSoap.h
 * Author: Alan.Ren & Tim.Ma
 * 
 * Create Date: 2019.3.6
 */


#ifndef _H_H_PACK_SOPE_H_H_
#define _H_H_PACK_SOPE_H_H_

#include "../BasicTypeDef.h"
#include "../Inc/IDataPack.h"
#include "../Inc/IParameter.h"
#include "../Inc/DefParamTypeId.h"
#include "../Inc/TxnDataType.h"
#include "../dbghlp.h"

#include "libstructdefined.h"
#include "libheartland.h"

class PUBLIC_EXPORT CBaseSoap: public IDataPack
{
public:
    CBaseSoap(IN IParameter *pcParam): m_pcParamPtr(pcParam)
    { }
    
    virtual ~CBaseSoap() { }

    int SetTxnSoapPtr(IN const int &nTxnType,
                      IN CBaseSoap *pcSope);
    
    virtual int DoPack(OUT CByteStream *pcPackData);

    virtual int DoUnpack(const CByteStream &cData);

protected:
    IParameter *m_pcParamPtr;

    static IDataPack *cTxnSoapBuffer_[kMaxSoapTypeCount];
};

#endif 
