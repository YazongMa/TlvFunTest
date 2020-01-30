
/* 
 * File:   CTxnNone.h
 * Author: Tim.Ma
 *
 * Created on 2019.03.19
 */

#include "ByteStream.h"
#include "FunBase.h"
#include "Inc/TxnDataType.h"
#include "Inc/DefParamTypeId.h"

#include "dbghlp.h"
#include "TxnNone.h"
#include "TlvFun.h"
#include "TxnDef.h"
#include "TxnTag.h"
#include "Inc/TxnState.h"
#include "Inc/UserActFlags.h"
#include "TxnJsonKey.h"
#include "cJSON.h"

/**
* Function:
*      CTxnNone's Construct Function:
*
* @param
*      pParamPtr*       : the parameters of the pool
*      pDataPackPtr*    : point of the object to pack and unpack
*      IBaseCurl*       : point of the object to communicate with host
*      IUserAction*     : point of the object to chat with user
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
CTxnNone::CTxnNone(IN IParameter *pParamPtr, 
        IN IDataPack *pDataPackPtr, 
        IN IBaseCurl *pBaseCurlPtr,
        IN IUserAction *pUserAction)
:
CTxnContext("None",
        pParamPtr,
        pDataPackPtr,
        pBaseCurlPtr,
        NULL,
        pUserAction)
{
    m_pszPanEntryLegend = "None";
}


/**
* Function:
*      Destruct Function:
*
* @param
*      none
*
* @return
*      none.
*
* Author: Tim.Ma
*/
CTxnNone::~CTxnNone() 
{ 
}


/**
* Function:
*      PerformTxn, transaction for none card's txn
*
* @param
*      none
*
* @return
*      0: no practical meaning 
*
* Author: Tim.Ma
*/
int CTxnNone::PerformTxn(void)
{
    TraceMsg("INFO: %s Entry", __FUNCTION__);
    // Clear cache
    CTxnContext::InitParam();

    HEARTLAND_AMOUNT_PARAM cAmtParam;
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kTxnAmountData, cAmtParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet)
    {
        TraceMsg("GetParameter Failed:%d",  nRet);
        return nRet;
    }
    
    if (cAmtParam.nPackSoapType == kSAF_SoapType)
    {
        TraceMsg("-------------SAF TRANSACTION...-------------");
        m_nPackSoapType = cAmtParam.nPackSoapType;
        CTxnContext::CommWithHost();
        PostProcessing_SAF();
        TraceMsg("-------------SAF TRANSACTION DONE-----------");
    }
    else if(cAmtParam.nPackSoapType == kCheckConnectivity_SoapType)
    {
        SetState(TXN_STATE_CONNECTING);
        TraceMsg("----------CHECK CONNECTIVITY...-------------");
        //TestConnect();
        nRet = m_pBaseCurlPtr->CurlCheckConnectivity();
        if (nRet == 0)
        {
            SetState(TXN_STATE_CONNECTIVITY_AVAILABLE);
        }
        else
        {
            SetState(TXN_STATE_CONNECTIVITY_NOT_AVAILABLE);
        }
        TraceMsg("Info: CurlCheckConnectivity nRet:%08X", nRet);
        TraceMsg("----------CHECK CONNECTIVITY DONE-----------");
    }
    else
    {
        TraceMsg("------------NORMAL TRANSACTION...-----------");
        SetState(TXN_STATE_PREPARING_TXNDATA);
        CTxnContext::PreProcessing();
        SetState(TXN_STATE_PREPARING_TXNDATA_SUCCESS);

        CTxnContext::CommWithHost();
        if (CTxnContext::m_bNeedCheckSAF)
        {
            ExeSAF();
        }
        CTxnContext::PostProcessing();
        TraceMsg("------------NORMAL TRANSACTION DONE---------");
    }
    
    TraceMsg("INFO: %s Exit", __FUNCTION__);
    return 0;
}


WORD CTxnNone::PostProcessing_SAF(void)
{
    TraceMsg("Info: PostProcessing_SAF entry");

    tagParam cTagParam = {0};
    HEARTLAND_TRANS_PARAM transParam = {0};

    InitTagParam(cTagParam, kHLTransParam, transParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        TraceMsg("GetParameter <transParam> failed wRet:%d", nRet);
        return 1;
    }

    WORD wLen = 0;
    BYTE* pbyData = NULL;
    nRet = CTlvFun::TLVListReadTagValue(d_TAG_TXN_RESULT, transParam.bResult, transParam.bResultLen, &pbyData, &wLen);
    if (nRet || NULL == pbyData)
    {
        SetState(TXN_STATE_DECLINED);
        TraceMsg("Invalid Parameter");
    }
    else if(*pbyData == d_TXN_APPROVED)
    {
        SetState(TXN_STATE_APPROVAL);
        TraceMsg("Transaction successful");
    }
    else
    {
        SetState(TXN_STATE_DECLINED);
        TraceMsg("Transaction fail");
    }

    TraceMsg("Info: PostProcessing_SAF exit");
    return 0;    
}
