
/*
 * File Name: CurlProcess.cpp
 * Author: Tim.Ma
 * 
 * Create Date: 2019.3.12
 */

#include "CurlObject.h"

#include "TxnDef.h"
#include "dbghlp.h"
#include "../libcurl_kernel/cURL.h"


/**
* Function:
*      CurlObject's Construct Function:
*
* @param
*      IParameter*   :  the parameters of the pool
*
* @return
*      Void.
*
* Author: Tim.Ma
*/
CurlObject::CurlObject(IN IParameter *pParamPtr)
:
m_pParamPtr(pParamPtr)
{
}


/**
* Function:
*      CurlObject's Destruct Function:
*
* @param
*      [none]
*
* @return
*      [none]
*
* Author: Tim.Ma
*/
CurlObject::~CurlObject()
{
}


/**
* Function:
*      CurlProcess, send the packed data to host, and receive data from host
*
* @param
*      pcInData: data that neede to be sent to host
*      cOutData: data that receive from host
*
* @return
*      0:       success
*      other:   fail, please refer to cURL.h
* Author: Tim.Ma
*/
int CurlObject::CurlProcess(IN CByteStream *pcInData, OUT CByteStream& cOutData)
{
    TraceMsg("Info: requestData:\r\n%s\r\n", pcInData->GetBuffer());
    cOutData.Empty();
    tagParam cTagParam = {0};
    HEARTLAND_CURL_PARAM curlParam = {0};
    char szHttpStatus[4] = {0};
    ULONG ulRetValue;
    CurlInfo stCurl = {0};

    InitTagParam(cTagParam, kCurlParam, curlParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        return nRet;
    }

    stCurl.httpStatus = szHttpStatus;
    stCurl.retValue = &ulRetValue;
    stCurl.pcRequest = (char *)pcInData->GetBuffer();
    stCurl.url = (char *)curlParam.szHostUrl;
    CURL_SetHead((char *)"Accept-Encoding: gzip,deflate");
    CURL_SetHead((char *)"Content-Type: text/xml;charset=UTF-8");
    CURL_SetHead((char *)"SOAPAction: DoTransaction");
    CURL_SetCaPath((BYTE *)curlParam.szCertPath, strlen((char *)curlParam.szCertPath));
    CURL_SetCaCertFile((BYTE *)curlParam.szCertFile, strlen((char *)curlParam.szCertFile));
    
    if (0 == strcasecmp(curlParam.szHttpMethod, "POST"))
    {
        CURL_SetHttpReqType(E_HTTP_POST);
    }
    else
    {
        CURL_SetHttpReqType(E_HTTP_GET);
    }

    const ULONG ulRet = CURL_Process(&stCurl);
    if (0 == ulRet)
    {
        cOutData.Write((void *)stCurl.payload, strlen(stCurl.payload));
        cOutData.PushZero();
        if (stCurl.payload)
        {
            free(stCurl.payload);
        }
    } 

    TraceMsg("Info: CURL_Process:%lX\r\n", ulRet);
    TraceMsg("Info: httpStatus:%s\r\n", szHttpStatus);
    TraceMsg("Info: retValue:%lX\r\n", ulRetValue);
    TraceMsg("Info: responseData:\r\n%s\r\n", cOutData.GetBuffer());
    return ulRet;
}


int CurlObject::CurlCheckConnectivity(IN unsigned long ulTimeout)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);

    tagParam cTagParam = {0};
    HEARTLAND_CURL_PARAM curlParam = {0};
    char szHttpStatus[4] = {0};
    ULONG ulRetValue;
    CurlInfo stCurl = {0};

    ULONG ulOrigTimeout = CURL_GetTimeout();

    InitTagParam(cTagParam, kCurlParam, curlParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        return nRet;
    }

    stCurl.httpStatus = szHttpStatus;
    stCurl.retValue = &ulRetValue;
    stCurl.url = (char *)curlParam.szHostUrl;
    CURL_SetHead((char *)"Accept-Encoding: gzip,deflate");
    CURL_SetHead((char *)"Content-Type: text/xml;charset=UTF-8");
    CURL_SetHead((char *)"SOAPAction: DoTransaction");
    CURL_SetCaPath((BYTE *)curlParam.szCertPath, strlen((char *)curlParam.szCertPath));
    CURL_SetCaCertFile((BYTE *)curlParam.szCertFile, strlen((char *)curlParam.szCertFile));

    if (0 == strcasecmp(curlParam.szHttpMethod, "POST"))
    {
        CURL_SetHttpReqType(E_HTTP_POST);
    }
    else
    {
        CURL_SetHttpReqType(E_HTTP_GET);
    }

    // special the timeout for checking connectivity
    CURL_SetTimeout(ulTimeout);

    CByteStream cOutData;
    const ULONG ulRet = CURL_Process(&stCurl);
    if (0 == ulRet)
    {
        cOutData.Write((void *)stCurl.payload, strlen(stCurl.payload));
        cOutData.PushZero();
        if (stCurl.payload)
        {
            free(stCurl.payload);
        }
    }

    // set back the original timeout for curl kernel
    CURL_SetTimeout(ulOrigTimeout);
    TraceMsg("Info: CURL_Process:%lX, %s\r\n", ulRet, szHttpStatus);
    TraceMsg("Info: libcurl_kernel retValue:%lX\r\n", ulRetValue);
    TraceMsg("Info: responseData:\r\n%s\r\n", cOutData.GetBuffer());
    return ulRet;
}