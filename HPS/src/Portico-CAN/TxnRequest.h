
/*
 * FileName: TxnInitConfig.h
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.16
 */

#ifndef _H_TXN_Request_20181115_CONFIG_H_
#define _H_TXN_Request_20181115_CONFIG_H_

#include "./Inc/cJSON.h"
#include "./Inc/deftype.h"
#include "./Inc/ByteStream.h"

enum kCardType
{
    kUnknownType = -1,
    kCredit,
    kDebit
};

class CTxnRequest
{
public:
    CTxnRequest();
    virtual ~CTxnRequest();

public:
    CByteStream GenTxnInitJsonData(const char *pszJsonCfgFile);
    CByteStream GenSaleJsonData(const unsigned long long &nAmount, const kCardType &cardType, const double &dbSequenceId=0.0f);
    CByteStream GenPreAuthJsonData(const unsigned long long &nAmount);
    CByteStream GenAuthCompleteJsonData(const unsigned long long &nAmount, const char *pszTxnId = NULL);
    CByteStream GenSettlementJsonData(void);
    CByteStream GenCancelJsonData(void);
    CByteStream GenVoidJsonData(const char *pszTxnId = NULL);
    CByteStream GenReturnJsonData(const unsigned long long &nAmt, const kCardType &cardType);
    CByteStream GenPollingCardJsonData(const kCardType &cardType);
    CByteStream GenReadDataJsonData(void);
    CByteStream GenSetVoltageFlagJsonData(const BOOL &bEnable);
    CByteStream GenSetTokenFlagJsonData(const BOOL &bEnable);
    CByteStream GenSetEmvDebugFlagJsonData(const BOOL &bEnable);
    CByteStream GenSetEmvCLDebugFlagJsonData(const BOOL &bEnable);
    CByteStream GenGetKernelVersionJsonData(void);
    CByteStream GenGetAppVersionJsonData(void);
    CByteStream GenAIDConfirmJsonData(const BOOL &bConfirm);
    CByteStream GenAIDSelectJsonData(const int &nSelector);
    CByteStream GenInteracPOSSeqNbr(const BYTE *pbyPosSeqNbr);
    CByteStream GenInteracAccountType(const BYTE *pbyAccountType);
    CByteStream GenLanguageSelectJsonData(const BYTE *pbyLanguage);
    CByteStream GenLanguageSwitchJsonData(const BYTE *pbyLanguage);

  private:
    CByteStream GenTxnInitJsonConfig(BOOL bUseGprs = FALSE,
                                     const char *pszAPN = NULL,
                                     const char *pszID = NULL,
                                     const char *pszPwd = NULL,
                                     const int &nSimSlot = 0,
                                     const char *pszRs232CfgFile = NULL);
    CByteStream GenTxnInitJsonConfig(const char *pszCommCfg, 
                                     const char *pszHostCfg, 
                                     const BOOL &bEnableVoltage = FALSE);
    cJSON *GenGPRSConfigJsonData(const char *pszAPN, 
                                 const char *pszID, 
                                 const char *pszPwd, 
                                 const int &nSimSlot);
    CByteStream GenTxnInitJsonConfigWithGPRSConfig(cJSON *pcJsonRoot, const char *pszCommCfg);
    CByteStream getFileData(const char *pszFilePath);
    void EnableVoltage(const BOOL &bEnable)
    {
        m_bEnableVoltage = bEnable;
    }

    BOOL IsEnableVoltage(void) const
    {
        return m_bEnableVoltage;
    }

private:
    BOOL m_bEnableVoltage;
};

#endif 