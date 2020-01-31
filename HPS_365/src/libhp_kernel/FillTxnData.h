
/*
 * File Name: FillTxnData.h
 * Author: Time.Ma 
 * 
 * Crate Date: 2019.3.13
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_FILLTXNDATA_H__
#define __HEARTLAND_PAYMENT_KERNEL_FILLTXNDATA_H__


#include "BasicTypeDef.h"
#include "TxnDef.h"
#include "VoltageObject.h"
#include "Inc/IParameter.h"
#include "libheartland.h"

class CFillTxnData
{
public:
    CFillTxnData(HeartlandSoap *pcSoapKernel, CVoltageObject* pcVoltageObj);
    virtual ~CFillTxnData();
    
    WORD SetTrackData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam, BOOL bCardType);
    WORD SetAmt(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetCashBackAmt(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetGratuityAmtInfo(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetAllowDupInfo(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    
    WORD SetAllowPartialAuth(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetEMVData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetTagData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetTransactionDescriptor(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_GENERAL_PARAM* pcGeneralParam);
    WORD SetUniqueDeviceId(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_GENERAL_PARAM* pcGeneralParam);
    WORD SetTokenRequest(ST_HSOAP_REQUEST* pcHsoapRequest, int nTokenFlag);
    
    WORD SetPINBlock(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetGatewayTxnId(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetTokenValue(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam, BOOL bCardType);
    WORD SetTokenValue(ST_HSOAP_REQUEST* pcHsoapRequest, char* pszTokenValue, BOOL bCardType);
    
    WORD SetInteracPOSSeqNbr(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);
    WORD SetInteracAccountType(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam);

    WORD SetReversalData(ST_HSOAP_REQUEST* pcHSoapRequest);
    
    WORD SetTxnResult(HEARTLAND_TRANS_PARAM* pTransParam, char* pszTag, char* pszValue);
    WORD SetTxnResult(HEARTLAND_TRANS_PARAM* pTransParam, char* pszTag, char* pszValue, int nLength);

protected:
    ULONG SetTagDataValue(ST_HSOAP_REQUEST *hsoapData, BYTE *baTagName, BYTE *baTlvListPtr, USHORT usTlvListLen, bool necessary);

protected:
    HeartlandSoap*          m_pcSoapKernel;
    CVoltageObject*         m_pcVoltageObj;
};

#endif /* FILLTXNDATA_H */

