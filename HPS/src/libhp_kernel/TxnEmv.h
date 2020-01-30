
/*
 * File Name: TxnEmv.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */


#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNEMV_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNEMV_H__

#include "TxnContext.h"
#include "EMVApi.h"

class PUBLIC_EXPORT CTxnEmv : public CTxnContext
{
public:
    CTxnEmv(IN IParameter *pParamPtr, 
            IN IDataPack *pDataPackPtr,
            IN IBaseCurl *pBaseCurlPtr,
            IN CEMVApi *pEmvApi,
            IN IUserAction *pUserAction);

    virtual ~CTxnEmv();

    virtual int PerformTxn(void);

    virtual void RegisterEMVSpecEvt(IN IEMVSpecialEvt *pcEvt, unsigned int uEvtMask);

    virtual int InitLevel2Lib(void);

protected:
    WORD InitEMV(void);
    WORD SelectTxnApp(void);
    WORD CompleteEMVTxn(void);
    
    void SendReversal(void);

    void TxnOnline(void);
    void CommWithHeartlandHost(void);
    void ResetHostRspData(void);
    int  ParseHostResponse(void);
    void DumpHostRsp(void);
    
    void FillReqIccData(void);
    void FillRspIccData_ARC(IN BYTE *pbySrcData, IN WORD wSrcDataLength);
    void FillRspIccData_AuthData(IN BYTE *pbySrcData, IN WORD wSrcDataLength);
    void FillRspIccData_IssuerScript(IN BYTE *pbySrcData, IN WORD wSrcDataLength);
    void GetTxnResult(const bool bOnline);
    void SetCardInformation(void);
    
    WORD CheckAPPInCard(BYTE *pbyRID);
    void ReActivateEMVKernel();

    virtual WORD    ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen);

protected:
    CEMVApi*            m_pEmvApi;

    // Online Host response
    BYTE                m_byActionCode;
    BYTE                m_byAuthCode[2 + 1];    // ARC
    BYTE                m_byIADBuf[16 + 1];     // IAD
    WORD                m_wRecvIADLen;          // IAD LEN
    BYTE                m_byIssuerScript[4096]; // Issue script
    WORD                m_wRecvIssuerScriptLen; // Issue script Length.

    BYTE                m_byAppLabel[64 + 1];
    WORD                m_wEMVCompletionResponse;
};
#endif