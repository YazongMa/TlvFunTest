
/*
 * File Name: HPApiKernel.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.14
 */

#ifndef _H_H_HP_API_KERNEL_H_H_
#define _H_H_HP_API_KERNEL_H_H_

#include "Inc/BasicInc.h"
#include "Inc/IKernelCtrl.h"
#include "Inc/KernelState.h"
#include "HPApiContext.h"
#include "DetectCard.h"
#include "../libhp_kernel/Inc/IUserAction.h"
 
// Declare
class CTxnBase;
class IParameter;
class CTxnBase;
class CHPApiKernel : public CHPApiContext,
                     public IKernelCtrl,
                     public IUserAction
{
public:    
    virtual ~CHPApiKernel();

    virtual void PostRequest(IN const char *pszJsonCmd);

    virtual void GetResponse(OUT char *pszBuffer, 
                             IN const int nBufSize);

    void RequsetUserAction(IN const unsigned int &nActionType,
                           IN const void *pData,
                           IN const size_t &nDataSize);

    void WaitUserResponse(IN const unsigned int &nActionType,
                          OUT void *pBuffer,
                          IN const size_t &nBufferSize);

    static IKernelCtrl* GetKernelCtrlPtr(void);
    static IUserAction* GetUserActionPtr(void);

protected:
    CHPApiKernel();
    
    void SetupAPIThread(void);
    BOOL IsValidJsonCmd(IN const char *pszJsonCmd, OUT CJsonReqCmd *pcReqJsonCmd);
    void ExeProcessingJsonCmd(void);
    BOOL ExeUserActionJsonCmd(IN const CJsonReqCmd &cReqCmd, IN const char *pszSrcJson);
    
    int GetUiIndex(void);
    int GetUiIndexByTxnState(void);
    int GetUiIndexByTxnState(IN const int &nTxnState);
    int GetUiIndexByActType(void);
    void UpdateApiMode(void);

    virtual void ExeWriteDataCmd(void);
    virtual void ExeCurReqTxnCmd(void);
    virtual void ExeUpdateData(void);
    
    // Response
    virtual void DefResponse(OUT char *pszBuffer, IN const int nBufSize);
    virtual void UserInputResponse(OUT char *pszBuffer, IN const int nBufSize);

private:
    int       GetPackSoapType(void);
    int       ExeTxnOp(IN const int &nDectedCardType);
    void      SetTxnParam(IN const cJSON *pcJsonParam);
    BOOL      Init(IN const cJSON *pcJson);
    void      ShowResult(void);
    CTxnBase *GetTxnPtr(IN const int nCardType);
    BOOL      IsNeedDetectCard(void);
    BOOL      IsEnableCancelDetectCard(void);
    unsigned long     GetDetectCardTimeout(void);
    BOOL      ExeDetectCard(OUT int *pnCardType);
    void      ExeClearLastDetectInfo(void);
    void      WaitScCardRemove(void);
    BOOL      ExeDetectCard(OUT int *pnCardType, IN const int nCurKeState, IN const int nCardSet);
    void      DumpKernelInfo(void);
    void      ResetApiMode(void);

    void      NormalActionRequest(IN const unsigned int &nActionType,
                                  IN const void *pVoid,
                                  IN const size_t &nSize);
    void      NormalActionResponse(IN const unsigned int &nActionType, 
                                   OUT void *pVoid,
                                   IN const size_t &nSize);

    void      DefActionRequest(IN const unsigned int &nActType,
                               IN const void *pVoid,
                               IN const size_t &nSize);
    void      DefActionResponse(IN const unsigned int &nActType,
                                OUT void *pBuffer,
                                IN const size_t &nBufferSize);
    
    BOOL      IsActRspArrive(void);

    void      ProcCancelModeJsonCmd(IN const char *pszJsonCmd);

    void      UpdateTxnEntryMode(IN const int &nCardType);

    unsigned short IsTxnSeqNbrMatched(void);

    unsigned int   GetAIDSelectTimeout(void);

protected:
    // Static
    static void* APIThreadMainEntry(IN void *pVoid);
    static CHPApiKernel cHPApiKernelObj_;
};

#endif
