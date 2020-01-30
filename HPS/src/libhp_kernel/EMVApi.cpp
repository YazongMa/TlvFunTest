
/*
 * File Name: BaseEmvCallback.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.11
 */

#include "EMVApi.h"
#include "dbghlp.h"
#include "TxnDef.h"
#include "Inc/DefParamTypeId.h"

#include "cJSON.h"
#include <emvaplib.h>

#ifdef PLATFORM_ANDROID
#include "IccSetting/emv_setting.h"
#include <kmshelper.h>
#endif

// static
static IParameter *pcParam_;

// Static.
EMV_EVENT CEMVApi::cEMVEvt_ = { 0 };

// Static.
IEmvCallback* CEMVApi::pImpEmvCallback_ = NULL;

// static Special Event.
IEMVSpecialEvt* CEMVApi::pImpEmvSpecEvtBuf_[64] = { 0 };

// AID, APP Name.
char CEMVApi::szAID_[32] = {0};
char CEMVApi::szAppName_[32] = {0};

// Spec Event Callback function.
static void EVT_OUTPUTCARDAPDU_(IN BYTE *pTxAPDU, IN USHORT TxAPDULen,
                                IN BYTE *pRxAPDU, IN USHORT RxAPDULen);
static void EVT_ApplicationSelectionMethod_(OUT BYTE *pbMethod);
static void EVT_InteracASFMode_(OUT BYTE *pbMode);
static void EVT_DisablePINNull_(OUT BOOL *IsDisablePINNull);
static unsigned short Evt_OnGetPINDone_(void);

/*
 * Txn Result.
 */
BYTE CEMVApi::byTxnResult_ = 0x00;

/* 
 * Construct function.
 */
CEMVApi::CEMVApi(IN IEmvCallback *pCbObj, IN IParameter *pcParam)
{
    pcParam_ = pcParam;
    pImpEmvCallback_ = pCbObj;
    bzero(pImpEmvSpecEvtBuf_, sizeof(pImpEmvSpecEvtBuf_));
}

/* 
 * Desctruct function.
 */
CEMVApi::~CEMVApi()
{ }

/*
 * Private.
 */
void CEMVApi::InitEmvEvt(void)
{
    static BOOL bRegCbFunc_ = FALSE;
    if (!bRegCbFunc_)
    {
        bRegCbFunc_ = TRUE;

        bzero(&cEMVEvt_, sizeof(cEMVEvt_));
        cEMVEvt_.Version = GetVersion();
        cEMVEvt_.OnDisplayShow = NULL;
        cEMVEvt_.OnErrorMsg = OnErrorMsg;
        cEMVEvt_.OnEMVConfigActive = NULL;
        cEMVEvt_.OnHashVerify = NULL;
        cEMVEvt_.OnTxnDataGet = OnTxnDataGet;
        cEMVEvt_.OnAppList = OnAppList;
        cEMVEvt_.OnAppSelectedConfirm = OnAppSelectedConfirm;
        cEMVEvt_.OnTerminalDataGet = NULL;
        cEMVEvt_.OnCAPKGet = NULL;
        cEMVEvt_.OnGetPINNotify = OnGetPINNotify;
        cEMVEvt_.OnOnlinePINBlockGet = OnOnlinePINBlockGet;
        cEMVEvt_.OnOfflinePINBlockGet = NULL;
        cEMVEvt_.OnOfflinePINVerifyResult = OnOfflinePINVerifyResult;
        cEMVEvt_.OnTxnOnline = NULL; //OnTxnOnline;
        cEMVEvt_.OnTxnIssuerScriptResult = OnTxnIssuerScriptResult;
        cEMVEvt_.OnTxnResult = OnTxnResult;
        cEMVEvt_.OnTotalAmountGet = OnTotalAmountGet;
        cEMVEvt_.OnExceptionFileCheck = OnExceptionFileCheck;
        cEMVEvt_.OnCAPKRevocationCheck = OnCAPKRevocationCheck;
    }
}
/*
 * Private.
 */
void CEMVApi::InitSpeFunc(void)
{
    static BOOL bRegSpeEvtFunc_ = FALSE;
    if (!bRegSpeEvtFunc_)
    {
        bRegSpeEvtFunc_ = TRUE;
        EMV_SpecialEventRegister(d_EVENTID_INTERAC_ASF_MODE, (void *)EVT_InteracASFMode_);
        // EMV_SpecialEventRegister(d_EVENTID_OUTPUT_CARDAPDU, (void *)EVT_OUTPUTCARDAPDU_);
        EMV_SpecialEventRegister(d_EVENTID_DISABLE_PINNULL, (void *)EVT_DisablePINNull_);
		
#ifdef PLATFORM_ANDROID
        EMV_SpecialEventRegister(d_EVENTID_GET_PIN_DONE, (void *)Evt_OnGetPINDone_);
        EMV_SpecialEventRegister(d_EVENTID_APP_LIST_EX, (void *)EVT_APP_LIST_EX_5);
#endif
//        PP_SpecialEventRegister(d_PP_EVENTID_APPLICATION_SELECTION_METHOD,
//                                (void *)EVT_ApplicationSelectionMethod_);
        WORD wErr;
        wErr = EMV_CDAModeSet(d_CDA_MODE_1);
        if (d_OK != wErr)
        {
            TraceMsg("Error: Call PP_iSetCDAMode() failed, Error=0x%04X", wErr);
        }
    }
}

/*
 * public.
 */
WORD CEMVApi::InitEMV(IN char *pszEmvCfgPath, IN const BYTE &bDebufEnable)
{
    static BOOL bInitEMVKe_ = FALSE;
    WORD wErr = d_EMVAPLIB_OK;
    
    InitEmvEvt();
    if (!bInitEMVKe_)
    {
#ifdef PLATFORM_ANDROID
        BYTE byVerStr[32] = { 0 };
        wErr = EMV_TxnKernelVersionGet(byVerStr);
        TraceMsg("Info: Call EMV_TxnKernelVersionGet(), wErr=%d, byVerStr=%s", wErr, byVerStr);
        TraceMsg("Info: Call InitEMV(), pszEmvCfgPath=%s", pszEmvCfgPath);

        wErr = EMV_Initialize(&cEMVEvt_,  NULL);
        if (bDebufEnable)
        {
            SetDebug(TRUE, 0xF4);
        }
        else
        {
            SetDebug(FALSE, 0xF4);
        }

        EMVManage cEMVManage;
        cEMVManage.EMVInit(TRUE, pszEmvCfgPath);
#else
        wErr = EMV_Initialize(&cEMVEvt_, pszEmvCfgPath);
#endif
        bInitEMVKe_ = ((wErr == d_EMVAPLIB_OK) ? TRUE : FALSE);
    }
    InitSpeFunc();
    return wErr;
}


/*
 * Public.
 */
WORD CEMVApi::SetDebug(IN const BYTE &bDebufEnable,
                       IN const BYTE &bDebugPort)
{
    EMV_SetDebug(bDebufEnable, bDebugPort);
    return 0;
}

/*
 * Public.
 */
WORD CEMVApi::SelectTxnApp(void)
{
    bzero(szAID_, sizeof(szAID_));
    bzero(szAppName_, sizeof(szAppName_));
    BYTE byAIDBufferLen = sizeof(szAID_)-1;
    BYTE byAppNameBufferLen = sizeof(szAppName_) - 1;
    return EMV_TxnAppSelect((BYTE *)szAID_,
                            &byAIDBufferLen,
                            (BYTE *)szAppName_,
                            &byAppNameBufferLen);
}

/*
 * Public.
 */
static const struct 
{
    const char * const pszTagName;
    const WORD wTagVal;
} s_cBDCTagBuf[] = 
{
    {"Application Interchange Profile",                      0x82},
    {"Application Transaction Counter",                      0x9F36},
    {"Application Usage Control",                            0x9F07},
    {"CID",                                                  0x9F27},
    {"CVM List",                                             0x8E},
    {"CVM Results",                                          0x9F34},
    {"IFD Serial Number",                                    0x9F1E},
    {"Issuer Action Code - Default",                         0x9F0D},
    {"Issuer Action Code - Denial",                          0x9F0E},
    {"Issuer Action Code - Online",                          0x9F0F},
    {"Issuer Script Results",                                0x9F5B}, // Castles define.
    {"Terminal Capabilities",                                0x9F33},
    {"Terminal Type",                                        0x9F35},
    {"TVR",                                                  0x95},
    {"TSI",                                                  0x9B}, // Add normal
    
    // {"TC/ARQC or AAC", },

    {"Application Cryptogram",                               0x9F26},
    {"Unpredictable Number",                                 0x9F37},
    {"Acquirer Identifier",                                  0x9F01},
    {"Amount, Authorised (Numberic)",                        0x9F02}, 
    {"Amount, Other (Numberic)",                             0x9F03},
    {"Application Effective Date",                           0x5F25},
    {"Application Expiration Date",                          0x5F24},
    {"Application PAN",                                      0x5A},
    {"Application PAN Sequence Number",                      0x5F34},
    {"Authorisation Code",                                   0x89},
    {"Authorisation Response Code",                          0x8A},
    {"Issuer Country Code",                                  0x5F28},
    {"Merchant Category Code",                               0x9F15}, 
    {"Merchant Identifier",                                  0x9F16},

    // {"Message Type", }, 

    {"POS Entry Mode",                                       0x9F39},
    {"Terminal Country Code",                                0x9F1A},
    {"Terminal Identifier",                                  0x9F1C},
    {"Transaction Amount",                                   0x0081}, // Castles define.
    {"Transaction Currency Code",                            0x5F2A},
    {"Transaction Date",                                     0x9A},
    {"Transaction Time",                                     0x9F21}, 
    {"Transaction Type",                                     0x9C}
};

#define array_count_of(array) (sizeof(array)/(sizeof(array[0])))
void GetBDCMessage(OUT CByteStream *pcBDCObj) 
{
    pcBDCObj->Empty();
    cJSON *pcJsonRoot = NULL;
    
    do
    {
        // Create JSON object
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        // Set message type
        cJSON_AddStringToObject(pcJsonRoot, "Message Type", "BDC Message");

        // Get all tags data.
        const size_t nMaxSize = array_count_of(s_cBDCTagBuf);
        for (size_t nIndex=0; nIndex < nMaxSize; ++nIndex)
        {
            BYTE byBuf[512];
            char szHexBuf[1026] = {0};
            char szFmtTagName[64] = {0};
            WORD wBufLen = sizeof(byBuf);
            WORD wErr = d_EMVAPLIB_OK;

            wErr = EMV_DataGet(s_cBDCTagBuf[nIndex].wTagVal, 
                        &wBufLen, byBuf);
            if (d_EMVAPLIB_OK == wErr)
            {
                bzero(szHexBuf, sizeof(szHexBuf));
                for (WORD wIndex=0; wIndex < wBufLen; ++wIndex)
                {
                    sprintf(szHexBuf + (wIndex*2), "%02X", byBuf[wIndex]);
                }
                sprintf(szFmtTagName, "%04X---%.50s", s_cBDCTagBuf[nIndex].wTagVal,
                         s_cBDCTagBuf[nIndex].pszTagName);
                cJSON_AddStringToObject(pcJsonRoot, szFmtTagName,
                                        szHexBuf);
            }
            else
            {
                TraceMsg("Error: Get tag=%04X, tagName=%s failed.", 
                           s_cBDCTagBuf[nIndex].wTagVal,
                           s_cBDCTagBuf[nIndex].pszTagName);
            }
        }
    } while (0);

    // Add data to pcBCDObj
    if (pcJsonRoot != NULL)
    {
        const char *pszJsonString = cJSON_Print(pcJsonRoot);
        (*pcBDCObj) << pszJsonString;
        free((void *)pszJsonString);
    }

    // Delete JSON object.
    if (NULL != pcJsonRoot)
    {
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    }
}

/*
 * Private.
 */
void CEMVApi::DumpBDCMsg(void)
{
    CByteStream cBdc;
    GetBDCMessage(&cBdc);
    TraceMsg("Info: BDC=%s\r\n", cBdc.PushZero());
}

/*
 * Public.
 */
WORD CEMVApi::PerformEMVTxn(void)
{
    TraceMsg("PerformEMVTxn--Entry");
    ResetTxnResult();
    const WORD wErr = EMV_TxnPerform();
    DumpBDCMsg();
    TraceMsg("PerformEMVTxn, Ret=0x%04X--Exit", wErr);
    return wErr;
}

/*
 * Public.
 */
WORD CEMVApi::CompleteEMVTxn(IN EMV_ONLINE_RESPONSE_DATA *pOnlineRspData)
{
    return EMV_TxnCompletion(pOnlineRspData);
}

#define RegFuncPtr(flag, inputMask, funcId, funcPtr, srcObjPtr, dstObjPtr) {\
    if (flag & inputMask) { \
        EMV_SpecialEventRegister((funcId), (void *)(funcPtr)); \
    } \
}

/*
 * Public.
 */
void CEMVApi::RegisterEMVSpecEvt(IN IEMVSpecialEvt *pcEvt, IN unsigned int &uEvtMask)
{
    if (NULL != pcEvt)
    {
        RegFuncPtr(kOutCardApdu, uEvtMask, d_EVENTID_OUTPUT_CARDAPDU, EVT_OUTPUTCARDAPDU_0,
                   pcEvt, pImpEmvSpecEvtBuf_[0]);
        RegFuncPtr(kDisablePINNull, uEvtMask, d_EVENTID_DISABLE_PINNULL, EVT_DISABLE_PINNULL_1,
                   pcEvt, pImpEmvSpecEvtBuf_[1]);
        RegFuncPtr(kFstGACTermdecision, uEvtMask, d_EVENTID_FIRSTGENAC_TERMDECISION, EVT_FIRSTGENAC_TERMDECISION_2,
                   pcEvtr, pImpEmvSpecEvtBuf_[2]);
        RegFuncPtr(kDisAppList, uEvtMask, d_EVENTID_DISPLAY_APPLICATION_LIST, EVT_DISPLAY_APPLICATION_LIST_3,
                   pcEvt, pImpEmvSpecEvtBuf_[3]);
        RegFuncPtr(kEnableInternalGetPINCancelKeyBypass, uEvtMask, d_EVENTID_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS,
                   EVT_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS_4, pcEvt, pImpEmvSpecEvtBuf_[4]);
        RegFuncPtr(kAppListEx, uEvtMask, d_EVENTID_APP_LIST_EX, EVT_APP_LIST_EX_5,
                   pcEvt, pImpEmvSpecEvtBuf_[5]);
        RegFuncPtr(kNonEmvCard, uEvtMask, d_EVENTID_NON_EMV_CARD, EVT_NON_EMV_CARD_6,
                   pcEvt, pImpEmvSpecEvtBuf_[6]);
        RegFuncPtr(kMCCSDynamicTerminalSupAid, uEvtMask, d_EVENTID_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID,
                   EVT_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID_7, pcEvt, pImpEmvSpecEvtBuf_[7]);
        RegFuncPtr(kNsiccPayment, uEvtMask, d_EVENTID_NSICC_PAYMENT, EVT_NSICCPayment_8,
                   pcEvt, pImpEmvSpecEvtBuf_[8]);
        RegFuncPtr(kTxnForceOnline, uEvtMask, d_EVENTID_TXN_FORCED_ONLINE, EVT_TxnForcedOnline_9,
                   pcEvt, pImpEmvSpecEvtBuf_[9]);
        RegFuncPtr(kInterAcAsfMode, uEvtMask, d_EVENTID_INTERAC_ASF_MODE, EVT_InteracASFMode_10,
                   pcEvt, pImpEmvSpecEvtBuf_[10]);
        RegFuncPtr(kGetPinDone, uEvtMask, d_EVENTID_GET_PIN_DONE, EVT_GET_PIN_DONE_11,
                   pcEvt, pImpEmvSpecEvtBuf_[11]);
    }
}

/*
 * Public.
 */
BYTE CEMVApi::GetVersion(void)
{
    return pImpEmvCallback_->GetVersion();
}

void CEMVApi::OnDisplayShow(IN char *pStrMsg)
{
    pImpEmvCallback_->DisplayShow(pStrMsg);
}

void CEMVApi::OnErrorMsg(IN char *pStrMsg)
{
    pImpEmvCallback_->ErrorMsg(pStrMsg);
}

void CEMVApi::OnEMVConfigActive(INOUT BYTE* pActiveIndex)
{
    pImpEmvCallback_->EMVConfigActive(pActiveIndex);
}

BOOL CEMVApi::OnHashVerify(IN BYTE *pRID,
                           IN BYTE bKeyIndex,
                           IN BYTE *pModulus,
                           IN USHORT ModulusLen,
                           IN BYTE *pExponent,
                           IN USHORT ExponentLen,
                           IN BYTE *pHash,
                           IN USHORT HashLen)
{
    return pImpEmvCallback_->HashVerify(pRID,
                                        bKeyIndex,
                                        pModulus,
                                        ModulusLen,
                                        pExponent,
                                        ExponentLen,
                                        pHash,
                                        HashLen);
}

WORD CEMVApi::OnTxnDataGet(OUT EMV_TXNDATA *pTxnData)
{
    return pImpEmvCallback_->TxnDataGet(pTxnData);
}

WORD CEMVApi::OnAppList(IN BYTE AppNum,
                        IN char AppLabel[][d_LABEL_STR_SIZE + 1],
                        OUT BYTE *pAppSelectedIndex)
{
    return pImpEmvCallback_->AppList(AppNum, AppLabel, pAppSelectedIndex);
}

USHORT CEMVApi::OnAppSelectedConfirm(IN BOOL IsRequiredbyCard,
                                     IN BYTE *pLabel,
                                     IN BYTE bLabelLen)
{
    return pImpEmvCallback_->AppSelectedConfirm(IsRequiredbyCard, pLabel, bLabelLen);
}

BOOL CEMVApi::OnTerminalDataGet(IN USHORT usTag,
                                INOUT USHORT *pLen,
                                OUT BYTE *pValue)
{
    return pImpEmvCallback_->TerminalDataGet(usTag, pLen, pValue);
}

BOOL CEMVApi::OnCAPKGet(IN BYTE *pRID,
                        IN BYTE bKeyIndex,
                        OUT BYTE *pModulus,
                        OUT USHORT *pModulusLen,
                        OUT BYTE *pExponent,
                        OUT USHORT *pExponentLen)
{
    return pImpEmvCallback_->CAPKGet(pRID,
                                     bKeyIndex,
                                     pModulus,
                                     pModulusLen,
                                     pExponent,
                                     pExponentLen);
}

void CEMVApi::OnGetPINNotify(IN BYTE bPINType,
                             IN USHORT bRemainingCounter,
                             OUT BOOL *pIsInternalPINPad,
                             OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara)
{
    pImpEmvCallback_->GetPINNotify(bPINType,
                                   bRemainingCounter,
                                   pIsInternalPINPad,
                                   pGetPINFuncPara);
}

USHORT CEMVApi::OnOnlinePINBlockGet(OUT ONLINE_PIN_DATA *pOnlinePINData)
{
    return pImpEmvCallback_->OnlinePINBlockGet(pOnlinePINData);
}

USHORT CEMVApi::OnOfflinePINBlockGet(void)
{
    return pImpEmvCallback_->OfflinePINBlockGet();
}

void CEMVApi::OnOfflinePINVerifyResult(IN USHORT usResult)
{
    pImpEmvCallback_->OfflinePINVerifyResult(usResult);
}

void CEMVApi::OnTxnOnline(IN ONLINE_PIN_DATA *pOnlinePINData,
                          OUT EMV_ONLINE_RESPONSE_DATA *pOnlineResponseData)
{
    pImpEmvCallback_->TxnOnline(pOnlinePINData, pOnlineResponseData);
}

void CEMVApi::OnTxnIssuerScriptResult(IN BYTE *pScriptResult,
                                      IN USHORT pScriptResultLen)
{
    pImpEmvCallback_->TxnIssueScriptResult(pScriptResult, pScriptResultLen);
}

void CEMVApi::OnTxnResult(IN BYTE bResult,
                          IN BOOL IsSignatureRequired)
{
    SetTxnResult(bResult);
    pImpEmvCallback_->TxnResult(bResult, IsSignatureRequired);
}

void CEMVApi::OnTotalAmountGet(IN BYTE *pPAN,
                               IN BYTE bPANLen,
                               OUT ULONG *pAmount)
{
    pImpEmvCallback_->TotalAmountGet(pPAN, bPANLen, pAmount);
}

void CEMVApi::OnExceptionFileCheck(IN BYTE *pPAN,
                                   IN BYTE bPANLen,
                                   OUT BOOL *isException)
{
    pImpEmvCallback_->ExptionFileCheck(pPAN, bPANLen, isException);
}

BOOL CEMVApi::OnCAPKRevocationCheck(IN BYTE *pbRID,
                                    IN BYTE bCAPKIdx,
                                    IN BYTE *pbSerialNumuber)
{
    return pImpEmvCallback_->CAPKRevocationChk(pbRID, bCAPKIdx, pbSerialNumuber);
}

//
// EMV Special Event.
//
void CEMVApi::EVT_OUTPUTCARDAPDU_0(IN BYTE *pTxAPDU,
                                   IN USHORT TxAPDULen,
                                   IN BYTE *pRxAPDU,
                                   IN USHORT RxAPDULen)
{
    if (NULL != pImpEmvSpecEvtBuf_[0])
    {
        pImpEmvSpecEvtBuf_[0]->OUTPUTCARDAPDU(pTxAPDU,
                                              TxAPDULen,
                                              pRxAPDU,
                                              RxAPDULen);
    }
}

void CEMVApi::EVT_DISABLE_PINNULL_1(OUT BOOL *IsDisablePINNull)
{
    if (NULL != pImpEmvSpecEvtBuf_[1])
    {
        pImpEmvSpecEvtBuf_[1]->DISABLE_PINNULL(IsDisablePINNull);
    }
}

void CEMVApi::EVT_FIRSTGENAC_TERMDECISION_2(INOUT BYTE *TermDecision)
{
    if (NULL != pImpEmvSpecEvtBuf_[2])
    {
        pImpEmvSpecEvtBuf_[2]->FIRSTGENAC_TERMDECISION(TermDecision);
    }
}

BOOL CEMVApi::EVT_DISPLAY_APPLICATION_LIST_3(void)
{
    if (NULL != pImpEmvSpecEvtBuf_[3])
    {
        return pImpEmvSpecEvtBuf_[3]->DISPLAY_APPLICATION_LIST();
    }
    return FALSE;
}

void CEMVApi::EVT_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS_4(OUT BYTE *isEnableGetPINBypassByCancelKey)
{
    if (NULL != pImpEmvSpecEvtBuf_[4])
    {
        pImpEmvSpecEvtBuf_[4]->ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS(isEnableGetPINBypassByCancelKey);
    }
}


unsigned short CEMVApi::EVT_APP_LIST_EX_5(IN BYTE bAppNum,
                                IN EMV_APP_LIST_EX_DATA *pstAppListExData,
                                OUT BYTE *pbAppSelectedIndex)
{
    return pImpEmvCallback_->OnAppListEx(bAppNum, pstAppListExData, pbAppSelectedIndex);
}

void CEMVApi::EVT_NON_EMV_CARD_6(OUT BOOL *IsNonEMVCard)
{
    if (NULL != pImpEmvSpecEvtBuf_[6])
    {
        pImpEmvSpecEvtBuf_[6]->NON_EMV_CARD(IsNonEMVCard);
    }
}

void CEMVApi::EVT_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID_7(OUT BYTE *pbAID,
                                                        OUT BYTE *pbAIDLen,
                                                        OUT BYTE *pbAIDNoMatchOption)
{
    if (NULL != pImpEmvSpecEvtBuf_[7])
    {
        pImpEmvSpecEvtBuf_[7]->MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID(pbAID, pbAIDLen, pbAIDNoMatchOption);
    }
}

void CEMVApi::EVT_NSICCPayment_8(IN BYTE *pbAID, IN BYTE bAIDLen, OUT BYTE *pbNSICCPayment)
{
    if (NULL != pImpEmvSpecEvtBuf_[8])
    {
        pImpEmvSpecEvtBuf_[8]->NSICCPayment(pbAID, bAIDLen, pbNSICCPayment);
    }
}

void CEMVApi::EVT_TxnForcedOnline_9(OUT BYTE *pbForcedONL)
{
    if (NULL != pImpEmvSpecEvtBuf_[9])
    {
        pImpEmvSpecEvtBuf_[9]->TxnForcedOnline(pbForcedONL);
    }
}

void CEMVApi::EVT_InteracASFMode_10(OUT BYTE *pbMode)
{
    if (NULL != pImpEmvSpecEvtBuf_[10])
    {
        pImpEmvSpecEvtBuf_[10]->InteracASFMode(pbMode);
    }
}

WORD CEMVApi::EVT_GET_PIN_DONE_11(void)
{
    if (NULL != pImpEmvSpecEvtBuf_[11])
    {
        return pImpEmvSpecEvtBuf_[11]->GET_PIN_DONE();
    }
    return 0;
}

/*
 * Private.
 */
void EVT_OUTPUTCARDAPDU_(IN BYTE *pTxAPDU, IN USHORT TxAPDULen, IN BYTE *pRxAPDU, IN USHORT RxAPDULen)
{
    TraceBytes(pRxAPDU, RxAPDULen, "Info: RxAPDU=");
    TraceBytes(pTxAPDU, TxAPDULen, "Info: TxAPDU=");
}


/*
 * Private.
 */
void EVT_ApplicationSelectionMethod_(OUT BYTE *pbMethod)
{
    // DEBUG_ENTRY();
    TraceMsg("Info: Select PSE");
    (*pbMethod) = 0x01; //CEmvContext::GetAppSelMethod();
    TraceMsg("Info: Select PSE");
    // DEBUG_EXIT(OK);
}


/*
 * Private.
 */
void EVT_InteracASFMode_(OUT BYTE *pbMode)
{
    TraceMsg("Info: Interac ASF Mode");

    tagParam cTagParam = { 0 };
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    pcParam_->GetParameter(&cTagParam);

    // "port0890", "port0841" are USA certification
    (*pbMode) = 0x02; 
    if (0 == strcasecmp(cGeneralParam.szCertName, "port0890") || 
        0 == strcasecmp(cGeneralParam.szCertName, "port0841"))
    {
        (*pbMode) = 0x02; 
    }
    else if(0 == strcasecmp(cGeneralParam.szCertName, "hpscanada0725"))
    {
        //pbMode :
        // 0x00 : Normal ASF logic (default)
        // 0x01 : No ASF logic
        // 0x02 : No ASF logic and drop interac AID (for VISA requirement in US terminal)
        (*pbMode) = 0x00; 
    }

    TraceMsg("Info: Interac ASF Mode: 0x%02x, CertName: %s", *pbMode, cGeneralParam.szCertName);
}


/*
 * Public.
 */
void EVT_DisablePINNull_(OUT BOOL *IsDisablePINNull)
{
    TraceMsg("Info: Disable PIN NULL");

    *IsDisablePINNull = FALSE;
    TraceMsg("Info: Disable PIN NULL");
}


unsigned short Evt_OnGetPINDone_(void)
{
  //__DebugAddSTR((BYTE*)"OnGetPINDone() is triggered -->");
  CTOS_LCDTClearDisplay();
#ifdef PLATFORM_ANDROID
  CTJNI_KMS_DisableLCD();
#endif

  return 0;
}
