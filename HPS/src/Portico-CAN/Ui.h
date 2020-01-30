
/*
 * File Name: Ui.h
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.19
 */

#ifndef _H_UI_Module_20181119_H_
#define _H_UI_Module_20181119_H_

#include <pthread.h>

#include "TxnRequest.h"
#include "dbghlp.h"

enum TxnType
{
    kTxnInvalid = -1,
    kTxnSale,
    kTxnPreAuth,
    kTxnAuthComplete,
    kTxnSettlement,
    kTxnVoid,
    kTxnReturn,
    kTxnPollingCard
};

enum UiMode
{
    kNormal=0,
    kRs232Mode
};

enum Language
{
    kEnglish = 1,
    kFrench
};

class CUi
{
public:
    CUi();
    virtual ~CUi();

    BOOL InitUiModule(int Mode=0);
    void SwitchUi(const int &uiIndex);
    void SetTxnType(const int &cType);
    void SetCardType(const int &nType);
    const int GetCurUiIndex(void);
    void SetSimSlot(const int &nSlot);
    int GetUiMode(void);
    void SetSequenceId(const double &dbId)
    {
        m_dbSequenceId = dbId;
    }

    void SetGatewayId(const char *pszId);
    const char *GetGatewayId(void) const 
    {
        return m_pszGatewayId;
    }

    void SetAidList(IN char szAppLabel[][17], IN int nAppNum);
    const int GetAidList(OUT char szAppLabel[][17]) const 
    {
        TraceMsg("GetAidList Entry, m_nAppNum:%d", m_nAppNum);   
        memcpy(szAppLabel, m_szAppLabel, sizeof(char) * 17 * m_nAppNum);
        TraceMsg("GetAidList Exit");
        return m_nAppNum;
    }
    
    void SetAidConfirm(IN char *pszLabel);
    
    int GetLanguage(void);
    
    void SetLanguage(int nLanguage);

    void SetPOSSequenceNbr(const char* pszPOSSequenceNbr)
    {
        memset(m_pszPOSSequenceNbr, 0, sizeof(m_pszPOSSequenceNbr));
        strcpy(m_pszPOSSequenceNbr, pszPOSSequenceNbr);
    }

    const char* GetPOSSequenceNbr()
    {
        return m_pszPOSSequenceNbr;
    }

private:
    int GetCardType(void);

    int GetCurTxnType(void);

    void ShowUi(const int &nUiIndex);

    void ShowBmp(const char *pszBmpFile);

    void ShowUi(const char *pszMultiBmpPath, const int &nOffset = 0);

    const char *GetBmpFilePath(const int &nUiIndex);

    USHORT ShowInputAmt(int *pnAmt);

    USHORT ShowSelectCardType(void);
    
    USHORT ShowInputGatewayId(char *pszGatewayId, BYTE* pbySize);

    USHORT GetInputAmount(unsigned long long *ullAmount, BYTE *pbyLable, IN BYTE byStartY);

    void ClearLines(BYTE byFirstLine, BYTE byLastLine);

    int GetSimSlot(void);

    WORD GetInputString(BYTE *pbyLable, CByteStream &cData);

    void SaveAPNName(const char *pszAPN);

    void SaveID(const char *pszID);

    void SavePwd(const char *pszPwd);

    void SetContinue(const BOOL bContinue);

    BOOL IsContinue(void);

    int UiIndexMap(const int &nUiIndex);

    void InitEthLink(void);
    void InitGprsLink(void);
    void LoadDefLinkType(void);

    void waitUserInputCancel(void);

    BOOL IsEnableUseVoltage(void);

    void PrintVoltageInfo(const BOOL &bEnable);

    void ResetSequenceId(void)
    {
        m_dbSequenceId = 0.0f;
    }

    void ExecuteGetResponse(void);


private:
    int m_nSimSlot;
    int m_nCurUiIndex;
    pthread_t m_cUiThreadId;
    pthread_t m_cInitEthTrdId;
    pthread_t m_cInitGprsTrdId;
    pthread_mutex_t m_cMutex;
    int m_nTxnType;
    int m_nCardType;
    int m_nUiMode;
    BOOL m_bContinue;
    const char *m_pszAPN;
    const char *m_pszPwd;
    const char *m_pszID;
    const char *m_pszGatewayId;
    char m_pszPOSSequenceNbr[16 + 1];
    double m_dbSequenceId;

    int m_nAppNum;
    char m_szAppLabel[10][17];
    char m_szLabel[17];
    
    int m_nLanguage;

    static void *uiModuleThread(void *pVoid);
    static void *uiModuleTrd_Rs232Mode(void *pVoid);
    static void *uiInitEthThread(void *pVoid);
    static void *uiInitGprsThread(void *pVoid);
};

#endif 