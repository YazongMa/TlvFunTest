
/*
 * File BaseParam.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.8
 */

#ifndef _H_H_BASE_PARAM_H_H_ 
#define _H_H_BASE_PARAM_H_H_

#include "..\BasicTypeDef.h"
#include "..\Inc\IParameter.h"
#include "..\Inc\DefParamTypeId.h"

#include "..\TxnDef.h"
#include "..\Inc\TxnDataType.h"
#include "libstructdefined.h"
#include "libheartland.h"

class PUBLIC_EXPORT CBaseParam: public IParameter
{
public:
    CBaseParam();
    virtual ~CBaseParam();

    virtual int SetParameter(IN const tagParam &cParam);
    virtual int GetParameter(OUT tagParam* pcParam);

protected:
    // ST_HSOAP_REQUEST                    m_cSoapRequest;
    // ST_HSOAP_RESPONSE                   m_cSoapResponse;
    HEARTLAND_TRANS_PARAM               m_cTransParam;
    HEARTLAND_CURL_PARAM                m_cCurlParam;
    HEARTLAND_EMVCL_PARAM               m_cEmvclParam;
    HEARTLAND_EMV_PARAM                 m_cEmvParam;
    HEARTLAND_GENERAL_PARAM             m_cGeneralParam;
    HEARTLAND_PINENTRY_PARAM            m_cPinParam;
    HEARTLAND_AMOUNT_PARAM              m_cTxnAmountData;
    HEARTLAND_POLLINGCARD_CACHE         m_cPollingCardCache;
};

#endif