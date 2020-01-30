
/*
 * File Name: HP_Api.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.14
 */

#ifndef _H_H_HP_API_H_H_
#define _H_H_HP_API_H_H_

#define LIB_API_VERSION            0x01 
#define LIB_HPKERNEL_VERSION       0x02

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef	__cplusplus
extern "C" {
#endif

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
void HL_PostRequest(IN const char *pszJsonCmd);



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
void HL_GetResponse(OUT char *pszBuffer, IN int nBufSize);



/*
 * HL_GetVersion
 * 
 * @param:
 *        [IN]nLibVer, Only support LIB_API_VERSION, 
 *                              LIB_HPKERNEL_VERSION.
 * 
 * @return:
 *        const char*
 * 
 * @description:
 *        Public and release to user.
 */
const char* HL_GetVersion(IN int nLibVer);


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
int HL_SafTransaction(IN char* pszReqData,  OUT char* pszRspData, INOUT int* pnRspDataSize);


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
int HL_CheckConnectivity(int nTimeout);

#ifdef	__cplusplus
}
#endif

#endif
