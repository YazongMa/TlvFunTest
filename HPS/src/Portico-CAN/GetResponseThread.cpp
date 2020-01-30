
/*
 * File Name: GetResponseThread.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.19
 */

#include <ctosapi.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "Ui.h"
#include "dbghlp.h"
#include "cJSON.h"
#include "TxnJsonKey.h"
#include "AppMain.h"
#include "./Inc/TxnInterface.h"
#include "Rs232Link.h"
#include "Receipt.h"

#define Nop (30 * 1000)
extern CUi *getUiModuleObj(void);

static pthread_t cGetResponseThreadId_ = -1;

static void Interac_POSSequenceNbr(const char* pszBankRspCode, const char* pszTraceNumber, CUi *pcUi)
{
    TraceMsg("%s Entry", __FUNCTION__);
    char szTraceNumber[16 + 1] = { 0 };	

    if (NULL==pszBankRspCode || NULL==pszTraceNumber)
    {
        TraceMsg("ERROR: pszBankRspCode:%x, pszTraceNumber:%x Invalid", pszBankRspCode, pszTraceNumber);
        return ;
    }

    TraceMsg("INFO: pszBankRspCode:%s, pszTraceNumber:%s OK", pszBankRspCode, pszTraceNumber);
    if (strlen(pszTraceNumber)==12)
    {
        if (strcmp("899", pszBankRspCode)==0)
        {
            pcUi->SetPOSSequenceNbr(pszTraceNumber);
        }
        else if(strlen(pszBankRspCode)!=0)
        {
            strcpy(szTraceNumber, pszTraceNumber);
            if (memcmp(&szTraceNumber[8], "999", 3) == 0)
            {
                strcpy(&szTraceNumber[8], "0010");
            }
            else
            {
                sprintf(&szTraceNumber[8], "%04d", atoi(&pszTraceNumber[8]) + 10);
            }
            pcUi->SetPOSSequenceNbr(szTraceNumber);
        }
        
    }
    TraceMsg("INFO: pszBankRspCode:%s, pszTraceNumber:%s", pszBankRspCode, pcUi->GetPOSSequenceNbr());

    TraceMsg("%s Exit", __FUNCTION__);
}


/*
 * Write to com port.
 */
static void writeDataToComPort(const char *pszStr)
{
    WORD wErr;
    do
    {
        if (NULL == pszStr)
        {
            wErr = d_SYSTEM_INVALID_PARA;
            break;
        }

        wErr = CTOS_RS232Open(d_COM1, 115200, 'N', 8, 1);
        if (d_OK != wErr)
            break;
        
        wErr = CTOS_RS232TxData(d_COM1, (BYTE *)pszStr, strlen(pszStr));
        if (d_OK != wErr)
            break;
        
        CTOS_RS232Close(d_COM1);
    } while (0);

    if (d_OK != wErr)
    {
        TraceMsg("Error: writeDataToComPort, ErrorCode=0x%04X", wErr);
    }
}


/*
 * Transmit receipt.
 * 
 * nApproval:
 *  0: APPROVAL
 *  1: PARTIAL APPROVAL
 *  2: DECLINE
 */
static void printTxnReceipt(IN const cJSON *pcJsonRoot, IN int nApproval, IN int nLanguage)
{
    CReceipt receipt(nApproval, nLanguage);
    const char* pszJsonString = receipt.PrintReceipt(pcJsonRoot);
    
    if (pszJsonString)
    {
        printf("receipt\n%s\n", pszJsonString);
    }
    else
    {
        printf("receipt error\n");
        return;
    }
    
    cJSON* pcJson = cJSON_Parse((char*)pszJsonString);
    cJSON* pcSubJson = NULL;

    int nLine = 0;
    if (pcJson)
    {
        pcSubJson = cJSON_GetObjectItem(pcJson, "Line Count");
        if (pcSubJson)
        {
            if(pcSubJson->valuestring)
            {
                nLine = atoi(pcSubJson->valuestring);
            }
        }
    }

    BYTE byTemp[384*600] = { 0 };	  //The width of paper is 384 dot.
    CTOS_FONT_ATTRIB FONT_ATTRIB = { 0 };
    FONT_ATTRIB.X_Zoom = 1; //The width magnifies X_Zoom diameters //
    FONT_ATTRIB.Y_Zoom = 1; //The height magnifies Y_Zoom diameters //
    FONT_ATTRIB.X_Space = 0;  //The width of the space between the font with next font //
    FONT_ATTRIB.Y_Space = 0;  //The Height of the space between the font with next font //
    FONT_ATTRIB.FontSize = d_FONT_12x24;

    for (int i = 0; i < nLine; i++)
    {
        char chBuf[32] = {0};
        sprintf(chBuf, "Line%02d", i);
        
        pcSubJson = cJSON_GetObjectItem(pcJson, chBuf);
        if (pcSubJson)
        {
            if(pcSubJson->valuestring)
            {
                //CTOS_PrinterPutString((BYTE*)pcSubJson->valuestring);
                CTOS_PrinterBufferPutString(byTemp, 1, 24 * i, (BYTE *)pcSubJson->valuestring, &FONT_ATTRIB);
            }
        }
    }
    CTOS_PrinterBufferOutput((BYTE *)byTemp, 24 * nLine / 8 + 1); //Print the "CASTLES" Graphic & Text Logo //
    CTOS_PrinterFline(100);  
    cJSON_Delete(pcJson);
}

/*
 * Private.
 */
static void parseResponse(const char *pszJsonData, CUi *pcUi)
{
    cJSON *pcJsonRoot = NULL;
    cJSON *pcJsonSubItem = NULL;
    cJSON *pcJsonAryItem = NULL;
    cJSON *pcJsonRspCode = NULL;
    cJSON *pcJsonTxnType = NULL;
    cJSON *pcJsonBankRspCode = NULL;
    cJSON *pcJsonTraceNumber = NULL;

    do
    {
        pcJsonRoot = cJSON_Parse(pszJsonData);
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: Error Json Data:\n%s", pszJsonData);
            break;
        }

        // Interac special json tags
        pcJsonBankRspCode = cJSON_GetObjectItem(pcJsonRoot, "txnBankRespCode");
        pcJsonTraceNumber = cJSON_GetObjectItem(pcJsonRoot, "txnTraceNumber");

        // normal json tags
        pcJsonTxnType = cJSON_GetObjectItem(pcJsonRoot, "txnType");
        pcJsonRspCode = cJSON_GetObjectItem(pcJsonRoot, "txnStatusCode");
        pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Txn_Ui_Index);
        if (NULL != pcJsonSubItem)
        {
            static int nLastUiIndex = Ui_Invalid_Index;
            if (nLastUiIndex != pcJsonSubItem->valueint)
            {
                pcUi->SwitchUi(pcJsonSubItem->valueint);
                nLastUiIndex = pcJsonSubItem->valueint;
                TraceMsg("Info: Ui Index=%x", pcJsonSubItem->valueint);
                if (Ui_Polling_Card_Success == nLastUiIndex)
                {
                    pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Txn_SequenceId);
                    if (NULL != pcJsonSubItem)
                    {
                        pcUi->SetSequenceId(pcJsonSubItem->valuedouble);
                        TraceMsg("Info: Ui Index=%d, %f", pcJsonSubItem->valueint, pcJsonSubItem->valuedouble);
                    }
                }
                else if (Ui_Select_Aid == nLastUiIndex || Ui_Confirm_Select_Aid == nLastUiIndex)
                {
                    // AID Type
                    pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Act_Type);
                    if (NULL != pcJsonSubItem)
                    {
                        if (strcmp(pcJsonSubItem->valuestring, Act_AidList) == 0)
                        {
                            pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Act_Value);
                            int nArySize = cJSON_GetArraySize(pcJsonSubItem);

                            char (*pszAppLabel)[17] = new char[nArySize][17];
                            for (int i=0; i<nArySize; ++i)
                            {
                                pcJsonAryItem = cJSON_GetArrayItem(pcJsonSubItem, i);
                                strcpy(pszAppLabel[i], pcJsonAryItem->valuestring);
                            }
                            
                            pcUi->SetAidList(pszAppLabel, nArySize);
                            delete[] pszAppLabel;
                        }
                        else if(strcmp(pcJsonSubItem->valuestring, Act_AidConfim) == 0)
                        {
                            pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Act_Value);
                            pcUi->SetAidConfirm(pcJsonSubItem->valuestring);
                        }
                    }
                }
                else if (Ui_Txn_Approval == nLastUiIndex || Ui_Txn_Offline_Approval == nLastUiIndex) // APPROVAL
                {
                    if (pcJsonRspCode != NULL && pcJsonTxnType != NULL && strcmp("00", pcJsonRspCode->valuestring) == 0) 
                    {
                        TraceMsg("Info: pcJsonTxnType: %s", pcJsonTxnType->valuestring);
 
                        if (strcmp("authComplete", pcJsonTxnType->valuestring) == 0) 
                        {
                            // AuthAmt
                            printTxnReceipt(pcJsonRoot, 1, pcUi->GetLanguage());
                        }  
                        else
                        {
                            // Amt 
                            printTxnReceipt(pcJsonRoot, 0, pcUi->GetLanguage());
                        }
                    }
                    else if(Ui_Txn_Offline_Approval == nLastUiIndex)
                    {
                        // Amt 
                        printTxnReceipt(pcJsonRoot, 0, pcUi->GetLanguage());
                    }
                    if(pcUi->GetLanguage() == kFrench)
                    {
                        pcUi->SetLanguage(kEnglish);
                    }

                    if (NULL != pcJsonBankRspCode && NULL != pcJsonTraceNumber && Ui_Txn_Approval == nLastUiIndex)
                    {
                        Interac_POSSequenceNbr(pcJsonBankRspCode->valuestring, pcJsonTraceNumber->valuestring, pcUi);
                    }
                }
                else if (Ui_Txn_Decline == nLastUiIndex || Ui_Txn_Offline_Decline == nLastUiIndex)
                {
                    if (pcJsonRspCode != NULL) 
                    {
                        TraceMsg("Info: txnStatusCode: %s", pcJsonRspCode->valuestring);
                        if (memcmp(pcJsonRspCode->valuestring, "10", 2) == 0)
                        {   
                            printTxnReceipt(pcJsonRoot, 1, pcUi->GetLanguage()); // PARTIAL APPROVAL
                        }   
                        else
                        {
                            printTxnReceipt(pcJsonRoot, 2, pcUi->GetLanguage()); // DECLLINE
                        }                 
                    }
                    else if(Ui_Txn_Offline_Decline == nLastUiIndex)
                    {
                        printTxnReceipt(pcJsonRoot, 2, pcUi->GetLanguage()); // DECLLINE
                    }
                    if(pcUi->GetLanguage() == kFrench)
                    {
                        pcUi->SetLanguage(kEnglish);
                    }

                    if (NULL != pcJsonBankRspCode && NULL != pcJsonTraceNumber && Ui_Txn_Decline == nLastUiIndex)
                    {
                        Interac_POSSequenceNbr(pcJsonBankRspCode->valuestring, pcJsonTraceNumber->valuestring, pcUi);
                    }
                }
                
                TraceMsg("Info: Response JsonData=\r\n%s", pszJsonData);
            }
        }

        // GatewayId
        pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Txn_Gateway_Id);
        if (NULL != pcJsonSubItem)
        {
            // TraceMsg("Info: GatewayId=%s", pcJsonSubItem->valuestring);
            pcUi->SetGatewayId(pcJsonSubItem->valuestring);
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
}

/*
 * Private.
 */
static void* getResponseThread_(void *pVoid)
{
    pthread_detach(pthread_self());
    CUi *pUiModule = getUiModuleObj();

    if (NULL == pUiModule)
    {
        TraceMsg("Error: getUiModuleObj() return NULL");
        return NULL;
    }

    while (NULL != pUiModule && Ui_Exit_App != pUiModule->GetCurUiIndex())
    {
        char szJsonRsp[4096] = {0};
        int nBufSize = sizeof(szJsonRsp);

        GetResponse(szJsonRsp, &nBufSize);
        if (nBufSize != 0)
        {
            // TraceMsg("Info: Response Json Data:\n%s", szJsonRsp);
            parseResponse(szJsonRsp, pUiModule);
        }
        else
        {
            if (0 != nBufSize)
            {
                TraceMsg("Info: Response length=%d", nBufSize);
            }
        }
        BypassRsponseData(szJsonRsp);
        usleep(Nop);
    }
    return NULL;
}

/*
 * Public.
 */
extern void InitRspThread(void)
{
    if (cGetResponseThreadId_ = -1)
    {
        int re;

        re = pthread_create(&cGetResponseThreadId_, NULL,
                            getResponseThread_, NULL);
        if (re == -1)
        {
            TraceMsg("Error: pthread_create() failed.");
            return;
        }
    }
}
