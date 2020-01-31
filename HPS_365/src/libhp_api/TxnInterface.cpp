
/*
 * File Name: TxnInterface.cpp
 * Author: Alan.Ren & Tim.Ma
 * 
 * Create Date: 2019.03.29
 */

#include "Inc/BasicInc.h"
#include "Inc/HP_Api.h"
#include "../libhp_kernel/dbghlp.h"
#include "TxnInterface.h"

/**
* @brief PostRequest comment
*
* PostRequest()
*
* @param pszReqData     : [IN] Json data passed by user
* @param nReqDataLength : [IN] Length of Json data passed by user
* @return
 *      OK : success,
 *      other: fail
*/
int PostRequest(IN char* pszReqData, IN int nReqDataLength)
{
    static bool bVersionFlag = false;
    if (bVersionFlag == false)
    {
        TraceMsg("************************************************\n");
        TraceMsg("*     API    Version:%s      *\n", GetVersion(LIB_API_VERSION));
        TraceMsg("*     KERNEL Version:%s      *\n", GetVersion(LIB_HPKERNEL_VERSION));
        TraceMsg("************************************************\n");
        bVersionFlag = true;
    }
    HL_PostRequest(pszReqData);
    return 0;
}



/**
* @brief GetResponse comment
*
* GetResponse()
*
* @param pszReqData     : [OUT] Json data output for user
* @param nReqDataLength : [IN]  length of pszRspData buffer
* @param nReqDataLength : [OUT] real length of response Json data output for user
* @return
*      OK : success,
*      other: fail
*/
int GetResponse(IN char* pszRspData, IN_OUT int* pnRspDataLength)
{
    HL_GetResponse(pszRspData, (*pnRspDataLength) - 1);
    *pnRspDataLength = strlen(pszRspData);
    return 0;
}


/*
 * @brief GetVersion comment
 * 
 * @param:
 *        [IN]nLibVer, Only support LIB_API_VERSION, 
 *                              LIB_HPKERNEL_VERSION.
 * 
 * @return
 *      OK : success,
 *      other: fail
 */
const char *GetVersion(IN int nVerType)
{
    return HL_GetVersion(nVerType);
}


/**
 * @brief SafTransaction comment
 *
 * SafTransaction()
 *
 * @param pszReqData      :[IN] the json request package, same with txnRequestData of the package from safTransaction command.
 *        pnRspDataSize   :[IN] the size of the buffer: pszRspData
 *        pnRspDataSize   :[OUT] the real length of the pszRspData
 *        pszRspData      :[OUT] the json response package, same with the normal transaction.
 * @return
 *        0: successful.
 *        other: failed.
**/
int SafTransaction(IN char* pszReqData,  OUT char* pszRspData, INOUT int* pnRspDataSize)
{
    return HL_SafTransaction(pszReqData, pszRspData, pnRspDataSize);
}


/**
 * @brief CheckConnectivity comment
 *
 * CheckConnectivity()
 *
 * @param
 *        nTimeout       :[IN]in seconds, the timeout to check the connectivity with the host
 * @return
 *        0: the connectivity is available.
 *        other: the connectivity is unavailable.
**/
int CheckConnectivity(int nTimeout)
{
    return HL_CheckConnectivity(nTimeout);
}