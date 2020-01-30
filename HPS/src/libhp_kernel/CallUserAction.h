
/*
 * File Name: CallUserAction.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.05.21
 */

#ifndef _H_H_Call_User_Action_H_H_
#define _H_H_Call_User_Action_H_H_

#include "BasicTypeDef.h"

#include "Inc/IParameter.h"
#include "Inc/DefParamTypeId.h"
#include "Inc/IUserAction.h"
#include "Inc/UserActFlags.h"
#include "export.h"

#include "cJSON.h"

class PUBLIC_EXPORT CCallUserAction
{
public:
    CCallUserAction(IN IUserAction *pUserActionPtr,
                    IN IParameter *pParamPtr);
    virtual ~CCallUserAction();

    virtual WORD ReqUserSelectAID(IN BYTE byAppNum,
                                  IN char szAppLabel[][17],
                                  OUT BYTE *pbySelIndexu);

    virtual WORD ReqUserConfirmSelectedAID(IN BYTE *pbyLabel,
                                           IN BYTE byLen);

    virtual void ReqUserOutTotalAmt(IN BYTE *pPAN,
                                    IN BYTE bPANLen,
                                    OUT unsigned long *pAmount);

    virtual void ReqUserChkExpFile(IN BYTE *pPAN,
                                   IN BYTE bPANLen,
                                   OUT BOOL *isException);

    virtual BOOL ReqUserConfirmConnWithHost(void);

    virtual void ReqUserPromptLastPINInput(void);

    virtual void ReqUserPrintSignature(void);

    virtual void ReqUserCallBank(OUT WORD *pwTxnResult);

    virtual void ReqUserInputEMVPIN(IN BOOL bStart);

    virtual void ReqUserInputPosSeqNbr(OUT BYTE* pbyPosSeqNbr);

    virtual void ReqUserInputAccountType(OUT BYTE* pbyAccountType);

    virtual BOOL ReqUserInteracReceiptWarning(void);
    
    virtual void LanguageProcessing(void);
    
protected:
    virtual BOOL IsEnableUserAction(IN const DWORD &dwFlag);

    virtual void ReqUserAction(IN const unsigned int &nActType,
                               IN const void *pVoidData,
                               IN const size_t &nDataLen);

    virtual void WaitUserFinishAction(IN const unsigned int &nActType,
                                      OUT void *pBuffer,
                                      IN const size_t &nBufSize);
            
    virtual WORD MatchLanguage(OUT BOOL *pbNeedSelect, 
                               OUT BYTE *pbyLanguage);

private:
    IParameter *m_pParamPtr;
    IUserAction *m_pUserAction;
};

#endif
