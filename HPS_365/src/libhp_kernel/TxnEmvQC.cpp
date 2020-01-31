/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxnEmvQC.cpp
 * Author: Charlie.Zhao&Peter.Yuan
 * 
 * Created on 2019年10月29日, 上午11:20
 */

#include "TxnEmvQC.h"
#include "TxnDef.h"
#include "TxnTag.h"
#include "TlvFun.h"
#include "dbghlp.h"
#include "Inc/TxnState.h"

CTxnEmvQC::CTxnEmvQC(IN IParameter *pParamPtr, 
        IN IDataPack *pDataPackPtr,
        IN IBaseCurl *pBaseCurlPtr,
        IN CEMVApi *pEmvApi,
        IN IUserAction *pUserAction)
:CTxnEmv(pParamPtr, pDataPackPtr, pBaseCurlPtr, pEmvApi, pUserAction)
{

}

CTxnEmvQC::~CTxnEmvQC()
{
    
}

int CTxnEmvQC::PerformTxn(void)
{
    int nRet;
    
    nRet = InitEMV();
    if (nRet != 0)
    {
        TraceMsg("InitEMV Failed:%d",  nRet);
        return nRet;
    }
    
    nRet = CTxnContext::PreProcessing();
    if (nRet != 0)
    {
        TraceMsg("PreProcessing Failed:%d",  nRet);
        return nRet;
    }
    
    nRet = SelectTxnApp();
    if (nRet != 0)
    {
        TraceMsg("SelectTxnApp Failed:%d",  nRet);
        return nRet;
    }
    
    nRet = m_pEmvApi->PerformEMVTxn();
    if (nRet == d_EMVAPLIB_OK)
    {
        GetTxnResult(false);
    }
    else if(nRet == d_EMVAPLIB_ERR_EVENT_ONLINE)
    {
        FillReqIccData();
        ResetHostRspData();
        m_byActionCode = d_ONLINE_ACTION_UNABLE;
        memcpy(m_byAuthCode, "Z3", 2);
        nRet = CompleteEMVTxn();
        if (nRet == d_EMVAPLIB_OK)
        {
            TraceMsg("Please Remove Card");
            SetState(TXN_STATE_PLEASE_REMOVE_CARD);
            while (TRUE)
            {
                USHORT usErr = d_OK;
                BYTE byStatus = 0;

                usErr = CTOS_SCStatus(d_SC_USER, &byStatus);
                if ((d_OK == usErr) && (d_MK_SC_PRESENT == byStatus))
                {
                    continue;
                }
                TraceMsg("Card Removed");
                break;
            }
            TxnOnline();
            if (CTxnContext::m_bNeedCheckSAF)
            {
                ExeSAF();
            }
        }
        else
        {
            TraceMsg("CompleteEMVTxn Failed:0x%04X",  nRet);
//            return nRet;
        }
    }
    else if (nRet != 0)
    {
        TraceMsg("PerformEMVTxn Failed:0x%04X",  nRet);
//        return nRet;
    }
    
    nRet = CTxnContext::PostProcessing();
//    if (d_TXN_APPROVAL_WITH_KERNEL_DECLINE == nRet)
//    {
//        SendReversal();
//    }
    
    return nRet;
}

void CTxnEmvQC::TxnOnline(void)
{
    TraceMsg("TxnOnline entry");
    if (m_cCallUserAction.ReqUserConfirmConnWithHost())
    {
        CommWithHeartlandHost();
        DumpHostRsp();
//        CompleteEMVTxn();
    }
    else
    {
        m_byActionCode = d_ONLINE_ACTION_UNABLE;
        TraceMsg("Info: User cancel communicate with host.");
    }
    
    BYTE byTxnResult = d_TXN_DECLINED;
    if (m_byActionCode == d_ONLINE_ACTION_APPROVAL)
    {
        byTxnResult = d_TXN_APPROVED;
    }

    TraceMsg("TxnResult byTxnResult:%d", byTxnResult);

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("FillReqIccData GetParameter failed: 0x%04X", nRet);
    }
    else
    {
        CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_RESULT, 1, &byTxnResult, &transParam.bResultLen, transParam.bResult);
        nRet = m_pParamPtr->SetParameter(cTagParam);
        if (nRet != 0)
        {
            TraceMsg("FillReqIccData SetParameter failed: 0x%04X", nRet);
        }
    }
    
    TraceMsg("TxnOnline exit");
}


void CTxnEmvQC::GetTxnResult(const bool bOnline)
{
    BYTE bResult = m_pEmvApi->GetTxnResult();

    TraceMsg("Info : TxnResult: %d, bOnline:%d", bResult, bOnline?1:0);

    HEARTLAND_TRANS_PARAM transParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("FillReqIccData GetParameter failed: 0x%04X", nRet);
        return;
    }

//    WORD    wLen = 0;
//    BYTE*   pbyData = NULL;
    BYTE    byTxnResult = d_TXN_DECLINED;

    if (bResult == d_TXN_RESULT_APPROVAL) 
    {
        TraceMsg("Offline Approved");
        byTxnResult = d_TXN_DECLINED;// or d_TXN_FAIL ??  Need to confirm
    }

//    if (bOnline == true)    
//    {
//        nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_RESULT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
//        if (nRet != 0)
//        {
//            TraceMsg("FillReqIccData TLVListReadTagValue failed: 0x%04X", nRet);
//        }
//        else if(d_TXN_APPROVED == *pbyData && d_TXN_RESULT_APPROVAL == bResult)
//        {
//            byTxnResult = d_TXN_APPROVED;
//            TraceMsg("Info : Host APPROVAL, and EMV Kernel APPROVAL");
//        }
//        else if(d_TXN_APPROVED == *pbyData && d_TXN_RESULT_DECLINE == bResult)
//        {
//            byTxnResult = d_TXN_APPROVAL_WITH_KERNEL_DECLINE;
//            TraceMsg("Info : Host APPROVAL, but EMV Kernel DECLINE");
//        }
//        else
//        {
//            byTxnResult = d_TXN_DECLINED;
//            TraceMsg("Info : Host DECLINED");
//        }
//    }
    
    CTlvFun::TLVFormatAndUpdateList(d_TAG_TXN_RESULT, 1, &byTxnResult, &transParam.bResultLen, transParam.bResult);
    nRet = m_pParamPtr->SetParameter(cTagParam);
    TraceMsg("TxnResult byTxnResult:%d, nRet:%d", byTxnResult, nRet);
    
    TraceMsg("TxnResult exit");

    return ;
}