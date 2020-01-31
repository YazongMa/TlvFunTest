
/*
 * File Name: TxnContext.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.5.28
 */

#ifndef _H_H_TXN_CONTEXT_H_H_
#define _H_H_TXN_CONTEXT_H_H_

#include "Inc/IParameter.h"
#include "Inc/IDataPack.h"
#include "Inc/IBaseCurl.h"
#include "Inc/IPinAction.h"
#include "Inc/IUserAction.h"


#include "TxnBase.h"
#include "CallUserAction.h"



#define d_TAG_MAX_LENGTH      8 + 1

typedef enum
{
    CARD_BRAND_UNKNOWN,
    CARD_BRAND_MASTERCARD,
    CARD_BRAND_VISA,
    CARD_BRAND_AMEX,
    CARD_BRAND_JCB,
    CARD_BRAND_DISCOVER
}HL_CARD_BRAND;


class PUBLIC_EXPORT CTxnContext : public CTxnBase
{
public:
    CTxnContext(IN const char* pszPanEntryMode,
            IN IParameter *pParamPtr, 
            IN IDataPack *pDataPackPtr,
            IN IBaseCurl *pBaseCurlPtr,
            IN IPinAction *pPinEntryPtr,
            IN IUserAction *pUserAction);
    virtual ~CTxnContext();

    virtual void    SendReversal(void);

protected:
    virtual WORD    InitParam(void);

    virtual WORD    GetAcctNbr(unsigned char* pszTrack2, const int nTrack2Len);

    virtual WORD    GetPinBolck(void);

    virtual WORD    PreProcessing(void);

    virtual WORD    PostProcessing(void);

    virtual WORD    ReadSpecialTags(WORD wTagName, unsigned char *pszTlvValue, WORD *pwTlvValueLen);
    
    virtual WORD    CommWithHost(void);

    virtual WORD    SetPinParam(void);

    virtual WORD    GetCardBrandFromBinRange(void);

    virtual BOOL    IsInteracAID(const BYTE& bARQCType);

    virtual WORD    PollingCardProcess(unsigned short usCardEntryMode);

    virtual BOOL    IsTxnSeqNbrMatched(void);

    virtual void    CopyDataFromCache(char* pAryTag[], int nTagCount, 
                        unsigned short *pusTlvListDstLen,  unsigned char ucaTlvListDst[],
                        unsigned short usTlvListSrcLen,  unsigned char ucaTlvListSrc[]);

    virtual void    GetCardHolderNameFromICC(void);

    virtual void    UpdateCardHolderInfo(void);

    virtual void    InteracSpecialProcess(bool bNeedAccount);

    virtual void    Set_SAF_Results(unsigned long long ullAmount, int nState);

    virtual WORD    ExeSAF(void);

    virtual void    ResetInteracPosSeqNbr();

protected:
    char                m_cPinParamFlag;
    const char*         m_pszPanEntryMode;
    const char*         m_pszPanEntryLegend;
    char                m_szAcctNbr[20];
    char                m_szTransAmount[20];
    char                m_szCardExpireDate[20];
    char                m_szCardHolderName[32];
    char                m_szTransAmountOther[20];
    int                 m_nPackSoapType;
    int                 m_nFirstPlainDigits;

    IParameter*         m_pParamPtr;
    IPinAction*         m_pPinEntryPtr;
    IDataPack*          m_pDataPackPtr;
    IBaseCurl*          m_pBaseCurlPtr;
    CCallUserAction     m_cCallUserAction;
    CByteStream         m_SAFPackage;
    BOOL                m_bNeedCheckSAF;
};

#endif
