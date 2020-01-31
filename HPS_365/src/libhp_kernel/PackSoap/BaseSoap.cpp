
/*
 * File Name: AbsSoap.cpp
 * Author: Alan.Ren & Tim.Ma
 * 
 * Create Date: 2019.3.6
 */

#include "../TxnDef.h"
#include "BaseSoap.h"
#include "../dbghlp.h"

IDataPack* CBaseSoap::cTxnSoapBuffer_[kMaxSoapTypeCount] = { 0 };

/*
 * Public.
 */
int CBaseSoap::DoPack(OUT CByteStream *pcPackData)
{
    if (NULL == pcPackData)
    {
        TraceMsg("Error: Function parameter is NULL.");
        return -1;
    }

    IDataPack *pcDataPackPtr = NULL;
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    tagParam cParam = { 0 };
    int nTxnType = kInvalidType;

    InitTagParam(cParam, kTxnAmountData, cAmtParam);
    if (NULL != m_pcParamPtr)
    {
        m_pcParamPtr->GetParameter(&cParam);
        nTxnType = cAmtParam.nPackSoapType;
    }
    else 
    {
        TraceMsg("Error: invalid parameter");
        return -1;
    }

    TraceMsg("Info: SoapType:%d", nTxnType);
    pcDataPackPtr = CBaseSoap::cTxnSoapBuffer_[nTxnType];
    if (NULL == pcDataPackPtr)
    {
        TraceMsg("Error: invalid parameter nTxnType:%d", nTxnType);
        return -1;
    }
    return pcDataPackPtr->DoPack(pcPackData);
}

/*
 * Public.
 * 
 * Return -1, Error.
 */
int CBaseSoap::DoUnpack(IN const CByteStream &cData)
{
    IDataPack *pcDataPackPtr = NULL;
    HEARTLAND_AMOUNT_PARAM cAmtParam;
    int nTxnType = kInvalidType;
    tagParam cParam;

    bzero(&cAmtParam, sizeof(cAmtParam));
    InitTagParam(cParam, kTxnAmountData, cAmtParam);
    if (NULL != m_pcParamPtr)
    {
        m_pcParamPtr->GetParameter(&cParam);
        nTxnType = cAmtParam.nPackSoapType;
    }
    else 
    {
        TraceMsg("Error: m_pcParamPtr is NULL.");
        return -1;
    }
    pcDataPackPtr = CBaseSoap::cTxnSoapBuffer_[nTxnType];
    if (NULL == pcDataPackPtr)
    {
        TraceMsg("Error: m_pcParamPtr is NULL.");
        return -1;
    } 
    return pcDataPackPtr->DoUnpack(cData);
}

/*
 * Public.
 */
int CBaseSoap::SetTxnSoapPtr(IN const int &nTxnType,
                             IN CBaseSoap *pcSoap)
{
    if ((nTxnType < 0) || (nTxnType > kMaxSoapTypeCount))
    {
        return -1;
    }
    else if (NULL == pcSoap)
    {
        return -1;
    }
    cTxnSoapBuffer_[nTxnType] = pcSoap;
    TraceMsg("nTxnType:%d, Obj:%p", nTxnType, pcSoap);
    return 0;
}
