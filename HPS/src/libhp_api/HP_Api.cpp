
/*
 * File Name: HP_Api.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.14
 */

#include "Inc\BasicInc.h"

#include "Inc\HP_Api.h"
#include "Inc\DateMacro.h"
#include "Inc\IKernelCtrl.h"
#include "HPApiKernel.h"
#include "KernelLayerObjs.h"
#include "..\libhp_kernel\KernelVersion.h"

static IKernelCtrl *pKernelCtrlPtr = CHPApiKernel::GetKernelCtrlPtr();

#define API_VER_STR "1.2.0.006"

/*
 * HL_PostRequest
 * 
 * @param:
 *        [IN]pszJsonCmd, The request command string use json format.
 * 
 * @return:
 *        None.
 * 
 * @description:
 *        Public and release to user.
 */
void HL_PostRequest(const char *pszJsonCmd)
{
    if (NULL != pKernelCtrlPtr)
    {
        pKernelCtrlPtr->PostRequest(pszJsonCmd);
    }
}



/*
 * HL_GetResponse.
 * 
 * @param: 
 *        [IN]pszBuf, Store response json data.
 *        [IN]nBufSize, The buffer max length.
 * 
 * @return:
 *        None.
 * 
 * @description:
 *        Public and release to user.
 */
void HL_GetResponse(OUT char *pszBuffer, IN int nBufSize)
{
    if (NULL != pKernelCtrlPtr)
    {
        pKernelCtrlPtr->GetResponse(pszBuffer, nBufSize);
    }
}



/*
 * HL_GetVersion
 * 
 * @param:
 *        [IN]nLibVer, Only support LIB_API_VERSION, 
 *                                  LIB_HPKERNEL_VERSION.
 * 
 * @return:
 *        const char*
 * 
 * @description:
 *        Public and release to user.
 */
const char* HL_GetVersion(int nLibVer)
{
    if (LIB_API_VERSION == nLibVer)
    {
        static BOOL bInitApiVer = FALSE;
        static char szApiVerBuf[64] = {0};
        if (!bInitApiVer)
        {
            // Add compile date.
            sprintf(szApiVerBuf, "%s_%04d.%02d.%02d",
                    API_VER_STR, 
                    YEAR(),
                    MONTH() + 1, 
                    DAY());
            // sprintf(szApiVerBuf, "%s", API_VER_STR);
        }
        return szApiVerBuf;
    }
    else if (LIB_HPKERNEL_VERSION == nLibVer)
    {
        return GetHLKerVer();
    }
    return NULL;
}


/**
 * @brief HL_SafTransaction comment
 *
 * HL_SafTransaction()
 *
 * @param pszReqData      :[IN] the json request package, same with txnRequestData of the package from safTransaction command.
 *        pnRspDataSize   :[IN] the size of the buffer: pszRspData
 *        pnRspDataSize   :[OUT] the real length of the pszRspData
 *        pszRspData      :[OUT] the json response package, same with the normal transaction.
 * @return
 *        0: successful.
 *        other: failed.
**/
int HL_SafTransaction(IN char* pszReqData,  OUT char* pszRspData, INOUT int* pnRspDataSize)
{
    CTxnSAF cTxnSAF(HLKE_GetSoapKernelPtr(), HLKE_GetCurlObjPtr());
    return cTxnSAF.PerformTxn(pszReqData, pszRspData, pnRspDataSize);
}


/**
 * @brief HL_CheckConnectivity comment
 *
 * HL_CheckConnectivity()
 *
 * @param
 *        nTimeout       :[IN]in seconds, the timeout to check the connectivity with the host
 * @return
 *        0: the connectivity is available.
 *        other: the connectivity is unavailable.
**/
int HL_CheckConnectivity(int nTimeout)
{
    IParameter* pcParam = HLKE_GetParamPtr();

    CurlObject obj(pcParam);
    return obj.CurlCheckConnectivity(nTimeout);
}