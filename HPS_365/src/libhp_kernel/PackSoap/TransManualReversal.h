/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TransManualReversal.h
 * Author: Tim.Ma
 *
 * Created on 2019/9/10, 13:42
 */


#ifndef __HEARTLAND_TXNLIB_TRANSMANUALREVERSAL_H__
#define __HEARTLAND_TXNLIB_TRANSMANUALREVERSAL_H__


#include "../Inc/IParameter.h"
#include "../FillTxnData.h"
#include "BaseSoap.h"



class PUBLIC_EXPORT TransManualReversal : public CBaseSoap
{
public:
    TransManualReversal(IN IParameter *pcTransParam, IN HeartlandSoap *pcSoapKernel, IN int nCardType);
    virtual ~TransManualReversal(void);
    
    virtual int DoPack(OUT CByteStream *pcPackData);

    virtual int DoUnpack(const CByteStream &cData);
    
protected:
    virtual void    FillRequestData(ST_HSOAP_REQUEST* pcSoapRequest);
    virtual WORD    GetSoapData(ST_HSOAP_RESPONSE* pcHSoapResponse, HEARTLAND_TRANS_PARAM* pcTransParam);
    
protected:
    IParameter*         m_pcParamPtr;
    HeartlandSoap*      m_pcSoapKernel;
    int                 m_nCardType;
    CFillTxnData        m_cFillTxnData;
};

#endif /* __HEARTLAND_TXNLIB_TRANSENTRY_H__ */

