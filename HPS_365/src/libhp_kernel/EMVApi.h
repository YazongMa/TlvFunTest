
/*
 * File Name: BaseEmvCallback.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.11
 */

#ifndef _H_H_BASE_EMV_CALLBACK_H_H_
#define _H_H_BASE_EMV_CALLBACK_H_H_

#include "BasicTypeDef.h"
#include <emvaplib.h>

#include "Inc/IParameter.h"
#include "Inc/IEmvCallback.h"
#include "Inc/IEmvSpecialEvt.h"

class PUBLIC_EXPORT CEMVApi
{
public:
    CEMVApi(IN IEmvCallback *pCbObj, IN IParameter *pcParam);
    virtual ~CEMVApi();

    virtual WORD InitEMV(IN char *pszEmvCfgPath, IN const BYTE &bDebufEnable = FALSE);
    virtual WORD SetDebug(IN const BYTE &bDebufEnable, IN const BYTE &bDebugPort);
    virtual WORD SelectTxnApp(void);
    virtual WORD PerformEMVTxn(void);
    virtual WORD CompleteEMVTxn(IN EMV_ONLINE_RESPONSE_DATA* pOnlineRspData);
    virtual void RegisterEMVSpecEvt(IN IEMVSpecialEvt *pcEvt, IN unsigned int &uEvtMask);
    virtual BYTE GetTxnResult(void) const
    {
        return byTxnResult_;
    }

private:
    static void InitEmvEvt(void);
    static void InitSpeFunc(void);
    static void DumpBDCMsg(void);

protected:
    static BYTE GetVersion(void);
    static void OnDisplayShow(IN char *pStrMsg);
    static void OnErrorMsg(IN char *pStrMsg);
    static void OnEMVConfigActive(INOUT BYTE* pActiveIndex);
    static BOOL OnHashVerify(IN BYTE *pRID, IN BYTE bKeyIndex, IN BYTE *pModulus, IN USHORT ModulusLen, IN BYTE *pExponent, IN USHORT ExponentLen, IN BYTE *pHash, IN USHORT HashLen);
    static USHORT OnTxnDataGet(OUT EMV_TXNDATA *pTxnData);
    static USHORT OnAppList(IN BYTE AppNum, IN char AppLabel[][d_LABEL_STR_SIZE+1], OUT BYTE *pAppSelectedIndex);
    static USHORT OnAppSelectedConfirm(IN BOOL IsRequiredbyCard, IN BYTE *pLabel, IN BYTE bLabelLen);
    static BOOL OnTerminalDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
    static BOOL OnCAPKGet(IN BYTE *pRID, IN BYTE bKeyIndex, OUT BYTE *pModulus, OUT USHORT *pModulusLen, OUT BYTE *pExponent, OUT USHORT *pExponentLen);
    static void OnGetPINNotify(IN BYTE bPINType, IN USHORT bRemainingCounter, OUT BOOL* pIsInternalPINPad, OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara);
    static USHORT OnOnlinePINBlockGet(OUT ONLINE_PIN_DATA *pOnlinePINData);
    static USHORT OnOfflinePINBlockGet(void);
    static void OnOfflinePINVerifyResult(IN USHORT usResult);
    static void OnTxnOnline(IN ONLINE_PIN_DATA *pOnlinePINData, OUT EMV_ONLINE_RESPONSE_DATA* pOnlineResponseData);
    static void OnTxnIssuerScriptResult(IN BYTE* pScriptResult, IN USHORT pScriptResultLen);
    static void OnTxnResult(IN BYTE bResult, IN BOOL IsSignatureRequired);
    static void OnTotalAmountGet(IN BYTE *pPAN, IN BYTE bPANLen, OUT ULONG *pAmount);
    static void OnExceptionFileCheck(IN BYTE *pPAN, IN BYTE bPANLen, OUT BOOL *isException);       
    static BOOL OnCAPKRevocationCheck(IN BYTE *pbRID, IN BYTE bCAPKIdx, IN BYTE *pbSerialNumuber);

    // Special Eventtypedef 
    static void EVT_OUTPUTCARDAPDU_0(IN BYTE *pTxAPDU, IN USHORT TxAPUDLen, IN BYTE *pRxAPDU, IN USHORT RxAPDULen);
    static void EVT_DISABLE_PINNULL_1(OUT BOOL *IsDisablePINNull);
    static void EVT_FIRSTGENAC_TERMDECISION_2(INOUT BYTE *TermDecision);
    static BOOL EVT_DISPLAY_APPLICATION_LIST_3(void);
    static void EVT_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS_4(OUT BYTE *isEnableGetPINBypassByCancelKey);
    static unsigned short EVT_APP_LIST_EX_5(IN BYTE bAppNum, IN EMV_APP_LIST_EX_DATA *pstAppListExData, OUT BYTE *pbAppSelectedIndex);
    static void EVT_NON_EMV_CARD_6(OUT BOOL *IsNonEMVCard);
    static void EVT_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID_7(OUT BYTE *pbAID, OUT BYTE *pbAIDLen, OUT BYTE *pbAIDNoMatchOption);
    static void EVT_NSICCPayment_8(IN BYTE *pbAID, IN BYTE bAIDLen, OUT BYTE *pbNSICCPayment);
    static void EVT_TxnForcedOnline_9(OUT BYTE *pbForcedONL);
    static void EVT_InteracASFMode_10(OUT BYTE *pbMode);
    static WORD EVT_GET_PIN_DONE_11(void);

protected:
    static void ResetTxnResult(void)
    {
        byTxnResult_ = 0x00;
    }

    static void SetTxnResult(IN const BYTE &byTxnResult)
    {
        byTxnResult_ = byTxnResult;
    }

protected:
    static EMV_EVENT cEMVEvt_;
    static IEmvCallback *pImpEmvCallback_;
    static char szAID_[32];
    static char szAppName_[32];
    static BYTE byTxnResult_;

    // Special Event
    static IEMVSpecialEvt *pImpEmvSpecEvtBuf_[64];
};

#endif
