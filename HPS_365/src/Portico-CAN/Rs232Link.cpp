#include <ctosapi.h>
#include <unistd.h>

#include "Inc/StdafxHead.h"
#include "Inc/DbgMacro.h"
#include "Inc/ByteStream.h"
#include "Inc/cJSON.h"
#include "Inc/TxnInterface.h"
#include "Rs232Link.h"

#define Rs232_Port_Name                       "portid"
#define Rs232_Enable_Key                      "enable"
#define Rs232_Baud_Key                        "baud"
#define Rs232_Data_Bits                       "databits"
#define Rs232_Stop_Bits                       "stopbits"
#define Rs232_Parity_Mode                     "parity"
#define Enable_Value                          (1)
#define Default_RS232_Port                    (d_COM1)

typedef struct _tagRs232OpenConfig
{
    BOOL bEnableRs232;

    BYTE byPort;

    ULONG ulBaud;

    BYTE byParity;

    BYTE byDataBits;

    BYTE byStopBits;

    BOOL bContinue;

    BOOL bPortIsOpenSuccess;
} tagRs232OpenConfig;

static pthread_t cRs232Thread_ = -1; 
static const int Nop_ = 10 * 1000;
static void *rs232MonitorThread(void *pVoid);
static tagRs232OpenConfig cRs232Config_ = {FALSE, 0, 0, 0, 0, 0, FALSE};
#define Max_Rs232Rx_Error_Count (50)
#define Max_RS232Tx_Error_Count (20)


void getFileData(const char *pszFileName, CByteStream *pcByteStream);
static BOOL openRs232Port(const tagRs232OpenConfig *pcRs232Cfg);
static void closeRs232Port(const tagRs232OpenConfig *pcRs232Cfg);
static BOOL ParseDataAndPostRequest(CByteStream cData);
static void *rs232MonitorThread(void *pVoid);
static BOOL IsEnableRs232Module(void);


/*
 * InitRs232Module()
 * 
 * Author: Alan.Ren
 * 
 * Create Date: 2018.12.5
 * 
 * Description: Setup a thread to monitor RS232 port data.
 *
 *
 * Private.
 */
void InitRs232Module(const char* pcRs232CfgFile)
{
    if (NULL != pcRs232CfgFile && -1 == cRs232Thread_)
    {
        CByteStream cFileData;
        cJSON *pcJsonRoot = NULL;
        cJSON *pcJsonItem = NULL;

        getFileData(pcRs232CfgFile, &cFileData);
        do
        {
            pcJsonRoot = cJSON_Parse((char *)cFileData.PushZero());
            if (NULL == pcJsonRoot)
                break;
            
            printf("Info: Rs232Cfg:\r\n%s", (char *)cFileData.GetBuffer());
            pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Enable_Key);
            if (pcJsonItem != NULL && Enable_Value == atoi(pcJsonItem->valuestring))
            {
                bzero(&cRs232Config_, sizeof(tagRs232OpenConfig));

                // Enable RS232, Continue
                cRs232Config_.bEnableRs232 = TRUE;
                cRs232Config_.bContinue = TRUE;

                // Port id 
                pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Port_Name);
                if (NULL != pcJsonItem)
                {
                    cRs232Config_.byPort = Default_RS232_Port;
                    if (0 == strcmp("COM1", pcJsonItem->valuestring))
                    {
                        cRs232Config_.byPort = d_COM1;
                    }
                    if (0 == strcmp("COM2", pcJsonItem->valuestring))
                    {
                        cRs232Config_.byPort = d_COM2;
                    }
                    if (0 == strcmp("COM3", pcJsonItem->valuestring))
                    {
                        cRs232Config_.byPort = d_COM3;
                    }
                }

                // Baud
                pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Baud_Key);
                if (NULL != pcJsonItem)
                {
                    cRs232Config_.ulBaud = strtoul(pcJsonItem->valuestring, NULL, 10);
                }

                // Parity
                pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Parity_Mode);
                if (NULL != pcJsonItem && 0 != strlen(pcJsonItem->valuestring))
                {
                    cRs232Config_.byParity = pcJsonItem->valuestring[0];
                }

                // DataBits
                pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Data_Bits);
                if (NULL != pcJsonItem)
                {
                    cRs232Config_.byDataBits = atoi(pcJsonItem->valuestring);
                }

                // StopBits
                pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, Rs232_Stop_Bits);
                if (NULL != pcJsonItem)
                {
                    cRs232Config_.byStopBits = atoi(pcJsonItem->valuestring);
                }

                // Create a new thread to monitor RS232 port.
                int nRe = pthread_create(&cRs232Thread_, NULL, rs232MonitorThread, (void *)&cRs232Config_);
                if (-1 == nRe)
                {
                    DEBUG_INFO(DEBUGINFO_ERROR, "Error: pthread_create() failed.");
                }
            }

            // Free JSON object.
            cJSON_Delete(pcJsonRoot);
            pcJsonRoot = NULL;
        } while (0);
    }
}



/*
 * Public.
 * 
 * Author: Alan.Ren
 */
void BypassRsponseData(const char *pszJsonData)
{
    if (IsEnableRs232Module())
    {
        const char *pszTmp = pszJsonData;
        static CByteStream cLastData_;
        const USHORT usTxDataLen = strlen(pszJsonData);
        USHORT usRet;
        int nErrCount = 0;

        while (pszJsonData != NULL)
        {
            usRet = CTOS_RS232TxReady(cRs232Config_.byPort);
            if (usRet != d_OK && nErrCount < Max_RS232Tx_Error_Count)
            {
                usleep(Nop_);
                ++nErrCount;
                continue;
            }

            if (nErrCount > Max_Rs232Rx_Error_Count)
            {
                DEBUG_INFO(DEBUGINFO_ERROR, "Error: RS232 Error.");
                cLastData_.Empty();
                break;
            }
            cJSON *pcJsonRoot = NULL;

            if ((0 == cLastData_.GetLength() || 0 != strcmp(cLastData_.PushZero(), pszJsonData)) &&
                (usTxDataLen > 0) &&
                (NULL != (pcJsonRoot = cJSON_Parse(pszJsonData)))) 
            {
                usRet = CTOS_RS232TxData(cRs232Config_.byPort, (BYTE *)pszJsonData, usTxDataLen + 1);

                cLastData_ = CByteStream(pszJsonData);

                printf("Info: ByPassResponseData, Ret=0x%04X,\r\n%s\r\n", usRet, pszJsonData);
                cJSON_Delete(pcJsonRoot);
                pcJsonRoot = NULL;
            }
            break;
        }
    }
}



/*
 * Private.
 * 
 * Author: Alan.Ren
 */
void getFileData(const char *pszFileName, CByteStream *pcByteStream)
{
    if (NULL != pszFileName && NULL != pcByteStream)
    {
        FILE *pcFile = NULL;
        pcByteStream->Empty();

        do
        {
            pcFile = fopen(pszFileName, "rb");
            if (NULL == pcFile)
                break;
            while (!feof(pcFile))
            {
                BYTE byBuf[1024];
                int nRe;

                nRe = fread(byBuf, 1, 1024, pcFile);
                if (-1 != nRe)
                {
                    pcByteStream->Write(byBuf, nRe);
                }
            }

            fclose(pcFile);
            pcFile = NULL;
        } while (0);
    }
}


/*
 * Private.
 * 
 * Author: Alan.Ren
 */
static BOOL openRs232Port(const tagRs232OpenConfig *pcRs232Cfg)
{ 
    BOOL bRet = FALSE;
    WORD wRe = d_OK;

    wRe = CTOS_RS232Open(pcRs232Cfg->byPort, 
                         pcRs232Cfg->ulBaud,
                         pcRs232Cfg->byParity,
                         pcRs232Cfg->byDataBits,
                         pcRs232Cfg->byStopBits);
    if (wRe == d_OK)
    { 
        bRet = TRUE;
        CTOS_RS232FlushRxBuffer(pcRs232Cfg->byPort);
        CTOS_RS232FlushTxBuffer(pcRs232Cfg->byPort);
    }
    else
    {
        printf("Error: Open port failed, ErrorCode: 0x%04X\n\r", wRe);
    }
    return bRet;
}

/*
 * Private.
 * 
 * Author: Alan.Ren
 */
static void closeRs232Port(const tagRs232OpenConfig *pcRs232Cfg)
{
    CTOS_RS232Close(pcRs232Cfg->byPort);
}

/*
 * Private.
 * 
 * Author: Alan.Ren
 */
static BOOL ParseDataAndPostRequest(CByteStream cData)
{ 
    BOOL bParseRet = FALSE; 
    cJSON *pcJsonRoot = NULL;

    do
    {
        pcJsonRoot = cJSON_Parse((char *)cData.PushZero());
        if (NULL == pcJsonRoot)
        {
            break;
        }
        
        PostRequest((char *)cData.GetBuffer(), cData.GetLength());
        printf("Info: Request:\r\n%s\r\n", cData.PushZero());
        
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
        bParseRet = TRUE;
    } while (0);

    return bParseRet;
}

/*
 * Private.
 * 
 * Author: Alan.Ren
 */
static void *rs232MonitorThread(void *pVoid)
{
    pthread_detach(pthread_self());
    tagRs232OpenConfig *pRs232Config = reinterpret_cast<tagRs232OpenConfig *>(pVoid);
    BOOL bOpenRs232Ret = FALSE;

    bOpenRs232Ret = openRs232Port(pRs232Config);
    pRs232Config->bPortIsOpenSuccess = bOpenRs232Ret;
    while (bOpenRs232Ret && pRs232Config->bContinue)
    {
        BYTE byBuf[1024];
        USHORT usRxLen = 0;
        USHORT usRet;
        static CByteStream cRxJsonData_;
        static int nErrorCount_ = 0;
        cJSON *pcJsonRoot = NULL;

        if (nErrorCount_ > Max_Rs232Rx_Error_Count)
        {
            cRxJsonData_.Empty();
            nErrorCount_ = 0;
        }
        
        // Check data arrival.
        CTOS_RS232RxReady(pRs232Config->byPort, &usRxLen);
        if (0 == usRxLen)
        {
            usleep(Nop_);
            if (0 != cRxJsonData_.GetLength())
                ++nErrorCount_;
            continue;
        }

        usRxLen = sizeof(byBuf);
        usRet = CTOS_RS232RxData(pRs232Config->byPort, byBuf, &usRxLen);
        if (0 != usRxLen && d_OK == usRet)
        {
            cRxJsonData_.Write(byBuf, usRxLen);
        }

        // Parse command and send.
        if (ParseDataAndPostRequest(cRxJsonData_))
        {
            cRxJsonData_.Empty();
        }
        else
        {
            if (0 != cRxJsonData_.GetLength())
                nErrorCount_++;
        }
        usleep(Nop_);
    }

    if (bOpenRs232Ret)    // Close RS232 Port
        closeRs232Port(pRs232Config);
    
    // Clear thread ID.
    cRs232Thread_ = -1;
    pthread_exit(NULL);
    return NULL;
}

/*
 * Private.
 * 
 * Author: Alan.Ren
 */
static BOOL IsEnableRs232Module(void)
{
    return ((cRs232Config_.bEnableRs232) && (cRs232Config_.bContinue) && (-1 != cRs232Thread_) && (cRs232Config_.bPortIsOpenSuccess));
}

