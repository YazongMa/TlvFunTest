
/*
 * File Name: KernelLayerObjs.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.20
 */

#include "KernelLayerObjs.h"
#include "HPApiKernel.h"

// Static Parameter.
static CBaseParam cParam;
static CDetectCard cDetectCard;
static HeartlandSoap cHeartlandSoap;
static CPinObject cPinObj(&cParam);
static CVoltageObject cVoltage(&cParam);
static CurlObject cCurlObj(&cParam);

// Soap package objects. 
static CBaseSoap cBaseSoap(&cParam);
static TransCreditSale cCreditSaleSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransCreditAuth cCreditAuthSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransCreditAddToBatch cCreditAddToBatchSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransCreditVoid cCreditVoidSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransCreditReturn cCreditReturnSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransDebitSale cDebitSaleSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransDebitReturn cDebitReturnSoap(&cParam, &cHeartlandSoap, &cVoltage);
static TransBatchClose cBatchCloseSoap(&cParam, &cHeartlandSoap);
static TransReversal cTransReversal(&cParam, &cHeartlandSoap);
static TransManualReversal cTransManualCreditReversal(&cParam, &cHeartlandSoap, kCmdSetCredit);
static TransManualReversal cTransManualDebitReversal(&cParam, &cHeartlandSoap, kCmdSetDebit);
static TransSAF cTransSAF(&cParam, &cHeartlandSoap, &cVoltage);

// DataPack
static IDataPack *GetDataPack_(void)
{
    static BOOL bDefSet = FALSE;

    if (!bDefSet)
    {
        bDefSet = TRUE;
        cBaseSoap.SetTxnSoapPtr(kCreditSale_SoapType, &cCreditSaleSoap);
        cBaseSoap.SetTxnSoapPtr(kPreAuth_SoapType, &cCreditAuthSoap);
        cBaseSoap.SetTxnSoapPtr(kAuthComplete_SoapType, &cCreditAddToBatchSoap);
        cBaseSoap.SetTxnSoapPtr(kCreditVoid_SoapType, &cCreditVoidSoap);
        cBaseSoap.SetTxnSoapPtr(kCreditReturn_SoapType, &cCreditReturnSoap);
        cBaseSoap.SetTxnSoapPtr(kDebitSale_SoapType, &cDebitSaleSoap);
        cBaseSoap.SetTxnSoapPtr(kDebitReturn_SoapType, &cDebitReturnSoap);
        cBaseSoap.SetTxnSoapPtr(kBatchClose_SoapType, &cBatchCloseSoap);
        cBaseSoap.SetTxnSoapPtr(kReversal_SoapType, &cTransReversal);
        cBaseSoap.SetTxnSoapPtr(kManualCreditReversal_SoapType, &cTransManualCreditReversal);
        cBaseSoap.SetTxnSoapPtr(kManualDebitReversal_SoapType, &cTransManualDebitReversal);
        cBaseSoap.SetTxnSoapPtr(kSAF_SoapType, &cTransSAF);
    }
    return dynamic_cast<IDataPack *>(&cBaseSoap);
}

static IDataPack *pDataPack = GetDataPack_();

// emv callback & api
static CEMVBaseCallback cEmvCallback(&cParam, &cPinObj, CHPApiKernel::GetUserActionPtr());
static CEMVApi cEmvApi(&cEmvCallback, &cParam);

// Transaction
static CTxnMsr cTxnMsr(&cParam, pDataPack, &cCurlObj, &cPinObj, CHPApiKernel::GetUserActionPtr());
static CTxnEmv cTxnEmv(&cParam, pDataPack, &cCurlObj, &cEmvApi, CHPApiKernel::GetUserActionPtr());
static CTxnEmvQC cTxnEmvQC(&cParam, pDataPack, &cCurlObj, &cEmvApi, CHPApiKernel::GetUserActionPtr());
static CTxnEmvcl cTxnEmvCl(&cParam, pDataPack, &cCurlObj, &cPinObj, CHPApiKernel::GetUserActionPtr());
static CTxnNone cTxnNoneCard(&cParam, pDataPack, &cCurlObj, CHPApiKernel::GetUserActionPtr());

// Export functions.
IParameter* HLKE_GetParamPtr(void)
{
    return dynamic_cast<IParameter *>(&cParam);
}

/*
 */
CTxnBase* HLKE_GetMSRTxnPtr(void)
{
    return dynamic_cast<CTxnBase *>(&cTxnMsr);
}

/*
 */
CTxnBase* HLKE_GetEMVTxnPtr(void)
{
    tagParam cTagParam = { 0 };
    HEARTLAND_GENERAL_PARAM cGeneralParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, cGeneralParam);
    cParam.GetParameter(&cTagParam);
    
    if (0 == strcasecmp(cGeneralParam.szCertName, "port0890"))
    {
        return dynamic_cast<CTxnBase *>(&cTxnEmvQC);
    }
    else if(0 == strcasecmp(cGeneralParam.szCertName, "port0841") ||
            0 == strcasecmp(cGeneralParam.szCertName, "hpscanada0725"))
    {
        return dynamic_cast<CTxnBase *>(&cTxnEmv);
    }
    
    return dynamic_cast<CTxnBase *>(&cTxnEmv);
}

/*
 */
CTxnBase* HLKE_GetEMVCLTxnPtr(void)
{
    return dynamic_cast<CTxnBase *>(&cTxnEmvCl);
}


/*
 */
CTxnBase* HLKE_GetNoneCardTxnPtr(void)
{
    return dynamic_cast<CTxnBase *>(&cTxnNoneCard);
}


/*
 */
CDetectCard* HLKE_GetDetectedCardPtr(void)
{
    return (&cDetectCard);
}

/*
 */
HeartlandSoap* HLKE_GetSoapKernelPtr(void)
{
    return (&cHeartlandSoap);
}


/*
 */
IBaseCurl* HLKE_GetCurlObjPtr(void)
{
    return (&cCurlObj);
}