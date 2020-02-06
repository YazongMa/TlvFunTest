
/*
 * File Name: TxnEmvcl.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNEMVECL_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNEMVECL_H__

#include "TxnContext.h"

#include <emv_cl.h>

class PUBLIC_EXPORT CTxnEmvcl : public CTxnContext
{
public:
    CTxnEmvcl(IN IParameter *pParamPtr,
              IN IDataPack *pDataPackPtr,
              IN IBaseCurl *pBaseCurlPtr,
              IN IPinAction *pPinEntryPtr,
              IN IUserAction *pUserAction);

    virtual ~CTxnEmvcl();
    
    virtual int     PerformTxn(void);

    virtual int     InitLevel2Lib(void);
    
    virtual bool    IsInteracOfEMVCL(void);

protected:
    int             InitContactless(void);

    void            InitTransaction(void);

    ULONG           PerformTransactionEx(void);

    WORD            AnalysisTransactionEx(void);

    WORD            RetrieveCardData(void);

    void            CommWithHeartlandHost(void);
    
    void            ResetHostRspData(void);

    WORD            CompleteTransaction(void);
    
    int             ParseHostResponse(void);

    void            FillRspIccData_AuthData(IN BYTE *pbySrcData, IN WORD wSrcDataLength);

    void            FillRspIccData_IssuerScript(IN BYTE *pbySrcData, IN WORD wSrcDataLength);

    virtual WORD    ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen);
    
    void            SendReversal(void);

    void            ProcessMatchedTxnSeqNbr(void);
    
    void            GetOfflineTxnResult(void);

    virtual void    GetCardHolderNameFromICC(void);

private:
    static void     OnShowMsg(unsigned char bKernel, EMVCL_USER_INTERFACE_REQ_DATA* pstUserInterfaceRequestData);

protected:
    EMVCL_RC_DATA_EX    m_cRCData;

    CByteStream         m_cTlvList;

    BYTE                m_bCompleteFunRequired;
    BYTE                m_byActionCode;
    BYTE                m_byAuthCode[2 + 1];    // ARC
    BYTE                m_byIADBuf[16 + 1];     // IAD
    WORD                m_wRecvIADLen;          // IAD LEN
    BYTE                m_byIssuerScript[4096]; // Issue script
    WORD                m_wRecvIssuerScriptLen; // Issue script Length.
};

#endif
