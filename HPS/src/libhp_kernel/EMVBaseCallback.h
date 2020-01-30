
/*
 * File Name: EMVBaseCallback.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.12
 */

#ifndef _H_H_EMV_BASE_CALLBACK_H_H_ 
#define _H_H_EMV_BASE_CALLBACK_H_H_

#include "BasicTypeDef.h"
#include <emvaplib.h>

#include "Inc/IParameter.h"
#include "Inc/IDataPack.h"
#include "Inc/IBaseCurl.h"
#include "Inc/IPinAction.h"
#include "Inc/IUserAction.h"
#include "Inc/IEmvCallback.h"
#include "Inc/IEmvSpecialEvt.h"
#include "dbghlp.h"
#include "CallUserAction.h"

#define d_INVALID_ACTION_CODE        (0x00)
class PUBLIC_EXPORT CEMVBaseCallback: public IEmvCallback
{
public:
        CEMVBaseCallback(IN IParameter *pParamPtr,
                         IN IPinAction *pPinAction,
                         IN IUserAction *pUserAction);
        virtual ~CEMVBaseCallback();

        virtual BYTE GetVersion(void);

        virtual void DisplayShow(IN char *pszMsg);

        virtual void ErrorMsg(IN char *pszErrMsg);

        virtual void EMVConfigActive(INOUT BYTE *pbyActiveIndex);

        virtual BOOL HashVerify(IN BYTE *pbyRID, IN BYTE byKeyIndex,
                                IN BYTE *pbyModule, IN WORD wModuleLen,
                                IN BYTE *pbyExponent, IN WORD wExponentLen,
                                IN BYTE *pbyHash, IN WORD wHashLen);

        virtual WORD TxnDataGet(OUT EMV_TXNDATA *pcTxnData);

        virtual WORD AppList(IN BYTE byAppNum,
                             IN char szAppLabel[][d_LABEL_STR_SIZE + 1],
                             OUT BYTE *pbySelIndex);

        virtual WORD AppSelectedConfirm(IN BOOL bRequiredByCard,
                                        IN BYTE *pbyLabel,
                                        IN BYTE byLabelLen);

        virtual BOOL TerminalDataGet(IN WORD wTag,
                                     INOUT WORD *pwValueLen,
                                     OUT BYTE *pbyValue);

        virtual BOOL CAPKGet(IN BYTE *pRID,
                             IN BYTE bKeyIndex,
                             OUT BYTE *pModulus,
                             OUT WORD *pModulusLen,
                             OUT BYTE *pExponent,
                             OUT WORD *pExponentLen);

        virtual void GetPINNotify(IN BYTE bPINType,
                                  IN WORD bRemainingCounter,
                                  OUT BOOL *pIsInternalPINPad,
                                  OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara);

        virtual WORD OnlinePINBlockGet(OUT ONLINE_PIN_DATA *pOnlinePINData);

        virtual WORD OfflinePINBlockGet(void);

        virtual void OfflinePINVerifyResult(IN WORD wResult);

        virtual void TxnOnline(IN ONLINE_PIN_DATA *pOnlinePINData,
                               OUT EMV_ONLINE_RESPONSE_DATA *pOnlineResponseData);

        virtual void TxnIssueScriptResult(IN BYTE *pScriptResult,
                                          IN WORD pScriptResultLen);

        virtual void TxnResult(IN BYTE bResult,
                               IN BOOL IsSignatureRequired);

        virtual void TotalAmountGet(IN BYTE *pPAN,
                                    IN BYTE bPANLen,
                                    OUT ULONG *pAmount);

        virtual void ExptionFileCheck(IN BYTE *pPAN,
                                      IN BYTE bPANLen,
                                      OUT BOOL *isException);

        virtual BOOL CAPKRevocationChk(IN BYTE *pbRID,
                                       IN BYTE bCAPKIdx,
                                       IN BYTE *pbSerialNumuber);
        virtual unsigned short OnAppListEx(IN BYTE bAppNum,
                                    IN EMV_APP_LIST_EX_DATA *pstAppListExData,
                                    OUT BYTE *pbAppSelectedIndex);

protected:
        IParameter *m_pParamPtr;
        IPinAction *m_pPinAction;
        CCallUserAction m_cCallUserAction;
};

#endif
