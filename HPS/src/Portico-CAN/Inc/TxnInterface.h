
/**
* @file TxnInterface.h
*
* interface declaration file
*/

#ifndef __HEARTLAND_TXNINTERFACE_H__
#define __HEARTLAND_TXNINTERFACE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef IN_OUT
#define IN_OUT
#endif

#define LIB_API_VERSION                    0x01 
#define LIB_HPKERNEL_VERSION               0x02

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
 **/
int PostRequest(IN char* pszReqData, IN int nReqDataLength);


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
 **/
int GetResponse(IN char* pszRspData,  IN_OUT int* pnRspDataLength);


/**
 * @brief GetVersion comment
 *
 * GetVersion()
 *
 * @param piLength      : [IN] 0x01, API layer version.
 *                             0x02, HL kernel version.
 * @return
 *        char*: Version string.
**/
const char *GetVersion(IN int nVerType);

#ifdef __cplusplus
};
#endif

#endif //__HEARTLAND_TXNINTERFACE_H__
