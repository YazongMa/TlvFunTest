
/*
 * File Name: IEmvCallback.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.11
 */

#ifndef _H_H_I_EMV_CALLBACK_H_H_
#define _H_H_I_EMV_CALLBACK_H_H_

#ifndef IN
#define IN
#endif 

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


#include <emvaplib.h>


class IEmvCallback
{
public:
    virtual BYTE GetVersion() = 0;

    virtual void DisplayShow(IN char *pszMsg)
    { }

    virtual void ErrorMsg(IN char *pszErrMsg)
    { }

    virtual void EMVConfigActive(INOUT BYTE *pbyActiveIndex)
    { }

    virtual BOOL HashVerify(IN BYTE *pbyRID, IN BYTE byKeyIndex, 
                            IN BYTE *pbyModule, IN WORD wModuleLen,
                            IN BYTE *pbyExponent, IN WORD wExponentLen,
                            IN BYTE *pbyHash, IN WORD wHashLen)
                            { }

    virtual WORD TxnDataGet(OUT EMV_TXNDATA *pcTxnData) = 0;

    virtual WORD AppList(IN BYTE byAppNum,
                         IN char szAppLabel[][d_LABEL_STR_SIZE + 1],
                         OUT BYTE *pbySelIndex) = 0;

    virtual WORD AppSelectedConfirm(IN BOOL bRequiredByCard,
                                    IN BYTE *pbyLabel, 
                                    IN BYTE byLabelLen) = 0;

    virtual BOOL TerminalDataGet(IN WORD wTag,
                                 INOUT WORD *pwValueLen,
                                 OUT BYTE *pbyValue)
    {
        return FALSE;
    }

    virtual BOOL CAPKGet(IN BYTE *pRID,
                         IN BYTE bKeyIndex,
                         OUT BYTE *pModulus,
                         OUT WORD *pModulusLen, 
                         OUT BYTE *pExponent,
                         OUT WORD *pExponentLen)
    {
        return FALSE;
    }

    virtual void GetPINNotify(IN BYTE bPINType,
                              IN WORD bRemainingCounter,
                              OUT BOOL *pIsInternalPINPad,
                              OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara) = 0;

    virtual WORD OnlinePINBlockGet(OUT ONLINE_PIN_DATA *pOnlinePINData) = 0;

    virtual WORD OfflinePINBlockGet(void) = 0;

    virtual void OfflinePINVerifyResult(IN WORD wResult)
    { }
    
    virtual void TxnOnline(IN ONLINE_PIN_DATA *pOnlinePINData,
                           OUT EMV_ONLINE_RESPONSE_DATA *pOnlineResponseData) = 0;

    virtual void TxnIssueScriptResult(IN BYTE *pScriptResult,
                                      IN WORD pScriptResultLen)
    { }

    virtual void TxnResult(IN BYTE bResult,
                           IN BOOL IsSignatureRequired) = 0;

    virtual void TotalAmountGet(IN BYTE *pPAN,
                                IN BYTE bPANLen,
                                OUT ULONG *pAmount)
    { } 

    virtual void ExptionFileCheck(IN BYTE *pPAN,
                                  IN BYTE bPANLen,
                                  OUT BOOL *isException)
    { }

    virtual BOOL CAPKRevocationChk(IN BYTE *pbRID,
                                   IN BYTE bCAPKIdx,
                                   IN BYTE *pbSerialNumuber)
    {
        return FALSE;
    }

    virtual unsigned short OnAppListEx(IN BYTE bAppNum,
                                IN EMV_APP_LIST_EX_DATA *pstAppListExData,
                                OUT BYTE *pbAppSelectedIndex)
    {
        *pbAppSelectedIndex = 0;
        return 0;
    }
};

#endif 