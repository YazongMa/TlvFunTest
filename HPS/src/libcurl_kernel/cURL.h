#ifndef __HEARTLAND_CURL_H__
#define	__HEARTLAND_CURL_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <curl/curl.h>
#include <stdbool.h>
    
#define HTTP_REQUEST_TYPE_GET                 "GET"
#define HTTP_REQUEST_TYPE_POST                "POST"    

#define d_CURL_ERR_OFSET                       0xEC00
#define d_CURL_ERR_INIT_FAIL                   d_CURL_ERR_OFSET + 0x01
#define d_CURL_ERR_INVALID_PARAMETER           d_CURL_ERR_OFSET + 0x02
#define d_CURL_ERR_FAILED_FETCH_URL            d_CURL_ERR_OFSET + 0x03
#define d_CURL_ERR_FAIL_POPULATE_PAYLOAD       d_CURL_ERR_OFSET + 0x04
#define d_CURL_ERR_HTTP_RET_CODE_FAILE         d_CURL_ERR_OFSET + 0x05
#define d_CURL_ERR_COULDNT_CONNECT             d_CURL_ERR_OFSET + 0x06
#define d_CURL_ERR_OPERATION_TIMEDOUT          d_CURL_ERR_OFSET + 0x07 //the timeout time was reached
#define d_CURL_ERR_SEND_ERROR                  d_CURL_ERR_OFSET + 0x08
#define d_CURL_ERR_RECV_ERROR                  d_CURL_ERR_OFSET + 0x09
#define d_CURL_ERR_OTHER_ERROR                 d_CURL_ERR_OFSET + 0x0A

    
typedef struct
{
    IN char *url; 
    IN char *pcRequest;
    OUT char *payload;
    OUT char *httpStatus;
    OUT unsigned long *retValue;
}CurlInfo;

typedef enum  {
    E_HTTP_GET = 0,
    E_HTTP_POST
} enHttpReqType;

ULONG CURL_Process(CurlInfo *stCurl);
ULONG CURL_SetCaPath(BYTE *caPath, USHORT pathlen);
ULONG CURL_SetCaCertFile(BYTE *caCert, USHORT pathlen);
ULONG CURL_SetHead(CHAR *headString);
void  CURL_DebugEnable(void);
void  CURL_DebugDisable(void);
ULONG CURL_GetVersion(char *pcVersion, int *piLength);

void CURL_SetHttpReqType(ULONG reqType);
ULONG CURL_GetHttpReqType(void);

void CURL_SetTimeout(ULONG ulTimeout);
ULONG CURL_GetTimeout(void);

#ifdef	__cplusplus
}
#endif

#endif	/* __HEARTLAND_CURL_H__ */

