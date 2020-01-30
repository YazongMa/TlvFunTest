
#include <stdio.h>
#include <time.h>

#include "cURL.h"

#define d_HTTP_StatusSize               4
#define CURL_VERSION    		        "CURL_KERNEL_V1.2.0.20200102"

/* holder for curl fetch */
struct curl_fetch_st {
    char *payload;
    size_t size;
};

static __thread BYTE gCaPath[256];
static __thread BYTE gCaCert[256];
static __thread bool gDebugEnabled = false;
static __thread BYTE gHttpRet[d_HTTP_StatusSize];
static __thread struct curl_slist *gHeaders = NULL;
static __thread ULONG gHttpReqType = 0;
static __thread ULONG gulTimeout = 0;

#define CURL_DEBUG_MSG(...)                                                 \
    {                                                                       \
        if (gDebugEnabled)                                                  \
        {                                                                   \
            time_t t;                                                       \
            time (&t);                                                      \
            struct tm *lt = localtime (&t);                                 \
            printf("%02d-%02d %02d:%02d:%02d %s-%05d ",                     \
                lt->tm_mon, lt->tm_mday, lt->tm_hour,                       \
                lt->tm_min, lt->tm_sec, __FILE__, __LINE__);                \
            printf(__VA_ARGS__);                                            \
            printf("\n");                                                   \
        }                                                                   \
    }

#define CURL_INTERNAL_MSG(...)                                              \
    {                                                                       \
        time_t t;                                                           \
        time (&t);                                                          \
        struct tm *lt = localtime (&t);                                     \
        printf("%02d-%02d %02d:%02d:%02d %s-%05d ",                         \
            lt->tm_mon, lt->tm_mday, lt->tm_hour,                           \
            lt->tm_min, lt->tm_sec, __FILE__, __LINE__);                    \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
    }    

void CURL_DebugEnable(void) {
    gDebugEnabled = true;

#ifdef PLATFORM_ANDROID
#include <android/log.h>
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "----%lu----", pthread_self());
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gCaPath:%x", &gCaPath[0]);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gCaCert:%x", &gCaCert[0]);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gDebugEnabled:%x", &gDebugEnabled);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gHttpRet:%x", &gHttpRet[0]);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gHeaders:%x", &gHeaders);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "gHttpReqType:%x", &gHttpReqType);
    __android_log_print(ANDROID_LOG_INFO, "CURL_KERNEL", "----%lu----", pthread_self());
#else
    CURL_INTERNAL_MSG("CURL_KERNEL, ----%d----", pthread_self());
    CURL_INTERNAL_MSG("CURL_KERNEL, gCaPath:%x", &gCaPath[0]);
    CURL_INTERNAL_MSG("CURL_KERNEL, gCaCert:%x", &gCaCert[0]);
    CURL_INTERNAL_MSG("CURL_KERNEL, gDebugEnabled:%x", &gDebugEnabled);
    CURL_INTERNAL_MSG("CURL_KERNEL, gHttpRet:%x", &gHttpRet[0]);
    CURL_INTERNAL_MSG("CURL_KERNEL, gHeaders:%x", &gHeaders);
    CURL_INTERNAL_MSG("CURL_KERNEL, gHttpReqType:%x", &gHttpReqType);
    CURL_INTERNAL_MSG("CURL_KERNEL, ----%d----", pthread_self());
#endif
}
void CURL_DebugDisable(void) {
    gDebugEnabled = false;
}

ULONG CURL_GetVersion(char *pcVersion, int *piLength) {
    if (pcVersion == NULL || piLength == NULL) {
        CURL_INTERNAL_MSG("ERR: pcVersion == NULL || piLength == NULL");
        return d_CURL_ERR_INVALID_PARAMETER;
    }
    else {
        *piLength = strlen(CURL_VERSION);
        memcpy(pcVersion, CURL_VERSION, *piLength);
        pcVersion[*piLength] = '\0';
        return d_OK;
    }
}


size_t curl_cbHeader(void *ptr, size_t size, size_t nmemb, void  *userdata)
{
    int ret = 0;
    if ( strncmp((char *)ptr, "HTTP/1.1", 8) == 0 ) // get http response code
    {
            strtok((char *)ptr, " ");
            userdata = (strtok(NULL, " \n")); // http response code

            CURL_DEBUG_MSG("*userdata :%s \n", (char *)userdata);
            memcpy(gHttpRet, userdata, strlen(userdata));


            CURL_DEBUG_MSG("gHttpRet :%s\n", gHttpRet);
    }

    CURL_DEBUG_MSG("\n Get head Call pack*ptr :%s\n", (char *)ptr);
    return nmemb;
}


/* callback for curl fetch */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;                             /* calculate buffer size */
    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */
    
    /* expand buffer */
    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);
    
    /* check buffer */
    if (p->payload == NULL) {
        /* this isn't good */

        CURL_INTERNAL_MSG("ERROR: Failed to expand buffer in curl_callback");
        /* free buffer */
        free(p->payload);
        /* return */
        return -1;
    }
    
    /* copy contents to buffer */
    memcpy(&(p->payload[p->size]), contents, realsize);
    
    /* set new buffer size */
    p->size += realsize;
    
    /* ensure null termination */
    p->payload[p->size] = 0;
    
    /* return size */
    return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
    CURLcode rcode;                   /* curl result code */
    
    /* init payload */
    fetch->payload = (char *) calloc(1, sizeof(fetch->payload));
    
    /* check payload */
    if (fetch->payload == NULL) {
        /* log error */

        CURL_INTERNAL_MSG("ERROR: Failed to allocate payload in curl_fetch_url");
        /* return error */
        return CURLE_FAILED_INIT;
    }
    /* init size */
    fetch->size = 0;
    

    CURL_DEBUG_MSG("gCaPath %s\n", gCaPath);
    CURL_DEBUG_MSG("gCaPath %s\n", gCaCert);

    curl_easy_setopt(ch, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(ch, CURLOPT_CAPATH, gCaPath);
    curl_easy_setopt(ch, CURLOPT_CAINFO, gCaCert);
    /* set curl options */

    //curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, CURL_GetHttpReqType() ? HTTP_REQUEST_TYPE_POST : HTTP_REQUEST_TYPE_GET);
    curl_easy_setopt(ch, CURLOPT_HTTPHEADER, gHeaders);

    /* set url to fetch */
    curl_easy_setopt(ch, CURLOPT_URL, url);

    /* set cur TLS version, TLS version 1.2*/
    curl_easy_setopt(ch, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    
    /* set calback function */
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);
    
    curl_easy_setopt(ch, CURLOPT_HEADERFUNCTION, curl_cbHeader);  

    /* pass fetch struct pointer */
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);
    
    /* set default user agent */
    curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    /* set timeout */
    if (gulTimeout == 0)
    {
        curl_easy_setopt(ch, CURLOPT_TIMEOUT, 90);
    }
    else
    {
        curl_easy_setopt(ch, CURLOPT_TIMEOUT, gulTimeout);
    }

    /* enable location redirects */
    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
    
    /* set maximum allowed redirects */
    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

    // curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, false);
    // CURL_INTERNAL_MSG("Info Use CURLOPT_SSL_VERIFYPEER");

    /* fetch the url */
    rcode = curl_easy_perform(ch);
    
    /* return */
    return rcode;
}

ULONG CURL_SetCaPath(BYTE *caPath, USHORT pathlen)
{
    if(caPath == NULL || pathlen ==0)
    {

        CURL_INTERNAL_MSG("err CURL_SetCaCert");
        return d_CURL_ERR_INVALID_PARAMETER;
    }
    memset(gCaPath, 0x00, sizeof(gCaPath) );
    memcpy(gCaPath, caPath, pathlen);
    return d_OK;
}

ULONG CURL_SetCaCertFile(BYTE *caCert, USHORT pathlen)
{
    if(caCert == NULL || pathlen ==0)
    {
        CURL_INTERNAL_MSG("err CURL_SetCaCert");
        return d_CURL_ERR_INVALID_PARAMETER;
    }
    memset(gCaCert, 0x00, sizeof(gCaCert) );
    memcpy(gCaCert, caCert, pathlen);
    return d_OK;
}

ULONG CURL_SetHead(CHAR *headString)
{
    if(headString == NULL)
    {
        CURL_INTERNAL_MSG("err CURL_SetHead");
        return d_CURL_ERR_INVALID_PARAMETER;
    }
    gHeaders = curl_slist_append(gHeaders, headString);
    return d_OK;
}

ULONG CURL_Process(CurlInfo *stCurl)
{
    CURL *pCurlHead; /* curl handle */
    CURLcode rcode;
    ULONG ulRet = d_OK;

    struct curl_fetch_st curl_fetch;       /* curl fetch struct */
    memset(gHttpRet, 0x00, d_HTTP_StatusSize);

    /* init curl handle */
    if ( (pCurlHead = curl_easy_init()) == NULL) 
    {
        CURL_INTERNAL_MSG("pCurlHead init fail ");
        ulRet = d_CURL_ERR_INIT_FAIL;
        memcpy(stCurl->retValue, &ulRet, sizeof(unsigned long));
        return ulRet;
    }
    
    curl_easy_setopt(pCurlHead, CURLOPT_POSTFIELDS, stCurl->pcRequest);

    /* fetch page and capture return code */
    CURL_DEBUG_MSG("sstCurl->url  : %s\n", stCurl->url);
    rcode = curl_fetch_url(pCurlHead, stCurl->url, &curl_fetch);
 
    memcpy(stCurl->httpStatus, gHttpRet, d_HTTP_StatusSize);

    CURL_DEBUG_MSG("stCurl->httpStatus :%s\n" , stCurl->httpStatus);
    /* cleanup curl handle */
    curl_easy_cleanup(pCurlHead);
    
    /* free headers */
    curl_slist_free_all(gHeaders);
    gHeaders = NULL;

        /* check return code */
    if (rcode != CURLE_OK || curl_fetch.size < 1) {
        /* log error */

        CURL_INTERNAL_MSG("err rcode :%04X",rcode);
        CURL_INTERNAL_MSG("ERROR: Failed to fetch url (%s) - curl said: %s",
               stCurl->url, curl_easy_strerror(rcode));
        switch (rcode) {
            case CURLE_COULDNT_CONNECT:
            case CURLE_COULDNT_RESOLVE_HOST:
                ulRet = d_CURL_ERR_COULDNT_CONNECT;
                break;
            case CURLE_OPERATION_TIMEDOUT:
                ulRet = d_CURL_ERR_OPERATION_TIMEDOUT;
                break;
            case CURLE_SEND_ERROR:
                ulRet = d_CURL_ERR_SEND_ERROR;
                break;
            case CURLE_RECV_ERROR:
                ulRet = d_CURL_ERR_RECV_ERROR;
                break;
            default:
                ulRet = d_CURL_ERR_OTHER_ERROR;
                break;
        }
        memcpy(stCurl->retValue, &ulRet, sizeof(unsigned long));
        return ulRet;
    }//not ok finish

    /* check payload */
    if (curl_fetch.payload != NULL) {
        /* print result */

        CURL_DEBUG_MSG("curl_fetch.size :%08X", curl_fetch.size);
        CURL_DEBUG_MSG("CURL Returned: \n%s", curl_fetch.payload);
        stCurl->payload = (char *) malloc(curl_fetch.size +1);
        memcpy(stCurl->payload, curl_fetch.payload, curl_fetch.size);
        stCurl->payload[curl_fetch.size] = '\0';
        if (stCurl->payload == NULL) {

            CURL_INTERNAL_MSG("stCurl->payload == NULL");
            free(curl_fetch.payload);
            ulRet = d_CURL_ERR_INVALID_PARAMETER;
            memcpy(stCurl->retValue, &ulRet, sizeof(unsigned long));
            return ulRet;
        }

        CURL_DEBUG_MSG("CURL Returned: \n%s\n", stCurl->payload);
        /* free payload */
        free(curl_fetch.payload);
        ulRet = d_OK;
        memcpy(stCurl->retValue, &ulRet, sizeof(unsigned long));
        return ulRet;
    }// ok finish 
    else {
        /* error */

        CURL_INTERNAL_MSG("ERROR: Failed to populate payload");
        free(curl_fetch.payload);
        ulRet = d_CURL_ERR_FAIL_POPULATE_PAYLOAD;
        memcpy(stCurl->retValue, &ulRet, sizeof(unsigned long));
        return ulRet;
    }
}

//*************************************************************************
void CURL_SetHttpReqType(ULONG type)   {
    gHttpReqType = type;
}

ULONG CURL_GetHttpReqType(void)    {
    return gHttpReqType;
}
//*************************************************************************


void CURL_SetTimeout(ULONG ulTimeout)
{
    gulTimeout = ulTimeout;
}


ULONG CURL_GetTimeout(void)
{
    return gulTimeout;
}