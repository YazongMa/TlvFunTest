
/*
 * File Name: TxnInitConfig.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.16
 */

#include <ctosapi.h> 

#include "AppMain.h"
#include "TxnJsonKey.h"
#include "dbghlp.h"
#include "TxnRequest.h"
#include "DateMacro.h"

/*
 * Public.
 */
CTxnRequest::CTxnRequest() : 
m_bEnableVoltage(FALSE)
{ }

/*
 * Public.
 */
CTxnRequest::~CTxnRequest()
{ }

/*
 * Initialize JOSN data.
 */
typedef struct 
{
    const char *pszKey;
    const char *pszValue;
} JsonTag;

/*
 * Private.
 */
static const JsonTag cKeysBuf_[] = 
{
    { CFG_ARQC_TIMEOUT, "30"},
    { CFG_CertFile, Cfg_Cert_File_Path },
    { CFG_Cert_Path, Cfg_Cert_Folder_Path },
    { CFG_Emv_Config, Cfg_Emv_Config_Xml_Path },
    { CFG_EMVCL_Config, Cfg_EmvCL_Config_Xml_Path },
    { CFG_Developer_ID, "002914" },
    { CFG_Device_ID, "6215047" },
    { CFG_Host_Url, "https://cert.api2.heartlandportico.com/Hps.Exchange.PosGateway/PosGatewayService.asmx" },
    { CFG_Http_Method, "POST" },
    { CFG_License_ID, "122212" },
    { CFG_PIN_Entry_Timeout, "30" },
    { CFG_PWD, "$Test1234" },
    { CFG_Poll_Card_Timeout, "30" },
    { CFG_POS_Gateway_Url, "http://Hps.Exchange.PosGateway" },
    { CFG_Site_ID, "122240" },
    { CFG_User_Name, "777700959212" },
    { CFG_Version_Nbr, "2721" },

    { Txn_Type, "writeData"}
};

static const JsonTag cVoltageCfgFlags[] = 
{
    {"cfgVoltageFlag", "TRUE"},

//  {"cfgBBParamFile", "Config/hps_bbparams_test.xml"},
};

/*
 * Private.
 */
static const JsonTag cGprsDefParamBuf_[] = {
    { CFG_Communicate_Type, "gprs" },
    { CFG_GSM_RegTimeout, "120" },
    { CFG_GPRS_InitTimeout, "120" }
};

/*
 * Private.
 */
CByteStream CTxnRequest::GenTxnInitJsonConfigWithGPRSConfig(cJSON *pcJsonRoot,
                                                            const char *pszCommCfg)
{
    CByteStream cData;

    if (NULL != pcJsonRoot)
    {
        const char *pszAPN = NULL;
        const char *pszID = NULL;
        const char *pszPwd = NULL;
        int nSimSlot = -1;
        cJSON *pcJsonItem = NULL;

        pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, CFG_APN_Name);
        if (NULL != pcJsonItem)
        {
            pszAPN = pcJsonItem->valuestring;
        }

        pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, CFG_APN_ID);
        if (NULL != pcJsonItem)
        {
            pszID = pcJsonItem->valuestring;
        }

        pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, CFG_APN_PWD);
        if (NULL != pcJsonItem)
        {
            pszPwd = pcJsonItem->valuestring;
        }

        pcJsonItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Sim_Slot);
        if (NULL != pcJsonItem)
        {
            nSimSlot = atoi(pcJsonItem->valuestring);
        }
        cData = GenTxnInitJsonConfig(TRUE, pszAPN, pszID, pszPwd, nSimSlot, pszCommCfg);
    }
    return cData;
}

/*
 * Public.
 * 
 * Description: Generate a initialize configuration JSON data.
 * 
 * Create Date: 2018.12.04
 */
CByteStream CTxnRequest::GenTxnInitJsonConfig(const char *pszCommCfg, 
                                              const char *pszHostCfg,
                                              const BOOL &bEnableVoltage)
{
    CByteStream cData;
    CByteStream cFileData;
    cJSON *pcJsonRoot = NULL;
#if 0
    cJSON *pcJsonItem = NULL;
#endif 

    EnableVoltage(bEnableVoltage);

    TraceMsg("Info: CommCfg=%s, HostCfg=%s", pszCommCfg, pszHostCfg);
    cFileData = getFileData(pszHostCfg);

    do
    {
        TraceMsg("Info: Filedata:\n%s", cFileData.PushZero());
        pcJsonRoot = cJSON_Parse((char *)cFileData.PushZero());
        if (NULL == pcJsonRoot)
            break;

#if 0   // GPRS is not support now. 
        pcJsonItem = cJSON_GetObjectItem(pcJsonItem, CFG_Link_Type);
        if (NULL != pcJsonItem &&
            0 == strcmp(Link_Gprs, pcJsonItem->valuestring))
        { 
            cData = GenTxnInitJsonConfigWithGPRSConfig(pcJsonRoot, pszCommCfg);
        } 
        else
#endif
        {
            cData = GenTxnInitJsonConfig(FALSE, NULL, NULL, NULL, 0, pszCommCfg);
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Private.
 */
CByteStream CTxnRequest::getFileData(const char *pszFileName)
{
    CByteStream cFileData;
    FILE *pcFile = NULL;

    do
    {
        pcFile = fopen(pszFileName, "rb");
        if (NULL == pcFile)
            break;

        while (!feof(pcFile))
        {
            BYTE byBuf[1024];
            int nLen;

            nLen = fread(byBuf, 1, 1024, pcFile);
            if (nLen > 0)
                cFileData.Write(byBuf, nLen);
        }

        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    return cFileData;
}

/*
 * Private.
 */
CByteStream CTxnRequest::GenTxnInitJsonConfig(BOOL bUseGprs,
                                              const char *pszAPN,
                                              const char *pszID,
                                              const char *pszPwd,
                                              const int &nSimSlot,
                                              const char *pszRs232CfgFile)
{
    CByteStream cConfig;
    cJSON *pcJsonRoot = NULL;
    cJSON *pcJsonComm = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        const int nBufSize = sizeof(cKeysBuf_)/sizeof(cKeysBuf_[0]);
        for (int nIndex = 0; nIndex < nBufSize; ++nIndex)
        {
            cJSON_AddStringToObject(pcJsonRoot, cKeysBuf_[nIndex].pszKey,
                                    cKeysBuf_[nIndex].pszValue);
        }

        if (IsEnableVoltage())
        {
            const int nVolFlagsCount = sizeof(cVoltageCfgFlags)/sizeof(cVoltageCfgFlags[0]);
            for (int nIndex=0; nIndex < nVolFlagsCount; ++nIndex)
            {
                cJSON_AddStringToObject(pcJsonRoot, cVoltageCfgFlags[nIndex].pszKey,
                                        cVoltageCfgFlags[nIndex].pszValue);
            }
        }

        // If use GPRS link, add GPRS configuration.
        if (bUseGprs)
        {
            TraceMsg("APN=%s", pszAPN);
            TraceMsg("ID=%s", pszID);
            TraceMsg("Pwd=%s", pszPwd);
            TraceMsg("SimSlot=%d", nSimSlot);
            cJSON_AddItemToObject(pcJsonRoot, CFG_Communication,
                                  GenGPRSConfigJsonData(pszAPN, pszID, pszPwd, nSimSlot));
        }

        if (NULL != pszRs232CfgFile && 0 != strlen(pszRs232CfgFile))
        {
            cJSON_AddStringToObject(pcJsonRoot, CFG_Rs232_Cfg, pszRs232CfgFile);
        }

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            TraceMsg("Info: Init Request:\n%s", pszJsonStr);
            cConfig << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error:Init Request, cJSON_Print() failed.");
        }
        
        cJSON_Delete(pcJsonRoot);
    } while (0);
    return cConfig;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenSaleJsonData(const unsigned long long &nAmount, const kCardType &cardType, const double &dbSequenceId)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    char szAmtBuf[64] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: Call cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_Sale);
        if (cardType == kCredit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Credit_Card_Type);
        }
        else if (cardType == kDebit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Debit_Card_Type);
        }
        // sprintf(szAmtBuf, "%I64u", nAmount);
        sprintf(szAmtBuf, "%llu", nAmount);
        cJSON_AddStringToObject(pcJsonRoot, Txn_Amount, szAmtBuf);

        cJSON_AddNumberToObject(pcJsonRoot, Txn_SequenceId, dbSequenceId);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            TraceMsg("Info: Sale Request:\n%s", pszJsonStr);
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error: Sale Request, cJSON_Print() failed.");
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenPreAuthJsonData(const unsigned long long &nAmount)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    char szAmtBuf[64] = {0};
    
    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_PreAuth);
        // sprintf(szAmtBuf, "%I64u", nAmount);
        sprintf(szAmtBuf, "%llu", nAmount);
        cJSON_AddStringToObject(pcJsonRoot, Txn_Amount, szAmtBuf);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            TraceMsg("Info: Pre Auth:\n%s", pszJsonStr);
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error: Pre Auth Request, cJSON_Print() failed.");
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenAuthCompleteJsonData(const unsigned long long &nAmount, const char *pszTxnId)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;
    char szAmtBuf[64] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_AuthComplete);
        // sprintf(szAmtBuf, "%I64u", nAmount);
        // sprintf(szAmtBuf, "%llu", nAmount);
        // cJSON_AddStringToObject(pcJsonRoot, Txn_Amount, szAmtBuf);

        // Transaction Id
        cJSON_AddStringToObject(pcJsonRoot, Txn_Gateway_Id,
                                (NULL == pszTxnId) ? "" : pszTxnId);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            TraceMsg("Info: Auto Complete:\n%s", pszJsonStr);
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error: Auto Complete Request, cJSON_Print() failed.");
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenSettlementJsonData(void)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_Settlement);
        cJSON_AddStringToObject(pcJsonRoot, Txn_Amount, "");
        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error: Settlement Request, cJSON_Print() failed.");
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenCancelJsonData(void)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL ==  pcJsonRoot)
        {
            TraceMsg("Error: cJSON_CreateObject() failed.");
            break;
        }

        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_Cancel);

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }
        else
        {
            TraceMsg("Error: cJSON_Print() failed.");
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
    return cData;
}

/*
 * Private.
 */
cJSON *CTxnRequest::GenGPRSConfigJsonData(const char *pszAPN, const char *pszID, const char *pszPwd, const int &nSimSlot)
{
    cJSON *pGPRSJsonCfg = NULL;
    char szTmpBuf[64] = {0};

    do
    {
        pGPRSJsonCfg = cJSON_CreateObject();
        if (NULL == pGPRSJsonCfg)
            break;

        // Add Default
        const int nParamCount = sizeof(cGprsDefParamBuf_)/sizeof(cGprsDefParamBuf_[0]);
        for (int i = 0; i < nParamCount; ++i)
        {
            cJSON_AddStringToObject(pGPRSJsonCfg, cGprsDefParamBuf_[i].pszKey,
                                    cGprsDefParamBuf_[i].pszValue);
        }

        // Add user input configuration.
        cJSON_AddStringToObject(pGPRSJsonCfg, CFG_APN_Name, (pszAPN == NULL) ? "" : pszAPN);
        cJSON_AddStringToObject(pGPRSJsonCfg, CFG_APN_ID, (pszID == NULL) ? "" : pszID);
        cJSON_AddStringToObject(pGPRSJsonCfg, CFG_APN_PWD, (pszPwd == NULL) ? "" : pszPwd);
        sprintf(szTmpBuf, "%d", nSimSlot);
        cJSON_AddStringToObject(pGPRSJsonCfg, CFG_Sim_Slot, szTmpBuf);

    } while (0);
    return pGPRSJsonCfg;
}

/*
 * Public.
 *
 * Create Date: 2018.12.12.
 */
CByteStream CTxnRequest::GenVoidJsonData(const char *pszTxnId)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;
    char szAmt[64] = {0};

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_Void);
        
        // Transaction Id
        cJSON_AddStringToObject(pcJsonRoot, Txn_Gateway_Id,
                                (NULL == pszTxnId) ? "" : pszTxnId);

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            TraceMsg("Info: Void Json Data:\r\n%s", pszJsonData);
            cData << pszJsonData;
            cJSON_free((void *)pszJsonData);
            pszJsonData = NULL;
        }
        
        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
    return cData;
}

/*
 * Public.
 * 
 * Create Date: 2018.12.12.
 */
CByteStream CTxnRequest::GenReturnJsonData(const unsigned long long &nAmt, const kCardType &cardType)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;
    char szAmt[64] = { 0 };

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_Return);

        // sprintf(szAmt, "%I64u", nAmt);
        sprintf(szAmt, "%llu", nAmt);
        cJSON_AddStringToObject(pcJsonRoot, Txn_Amount, szAmt);

        if (cardType == kCredit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Credit_Card_Type);
        }
        else if (cardType == kDebit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Debit_Card_Type);
        }

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cData << pszJsonData;
            cJSON_free((void *)pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenPollingCardJsonData(const kCardType &cardType)
{
    CByteStream cData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_PollingCard);

        if (cardType == kCredit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Credit_Card_Type);
        }
        else if (cardType == kDebit)
        {
            cJSON_AddStringToObject(pcJsonRoot, Txn_Card_Type, Debit_Card_Type);
        }

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData << pszJsonStr;
            cJSON_free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenTxnInitJsonData(const char *pszJsonCfgFile)
{
    CByteStream cFileData = getFileData(pszJsonCfgFile);
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;
    CByteStream cInitJson;

    do
    {
        pcJsonRoot = cJSON_Parse((char *)cFileData.GetBuffer());
        if (NULL == pcJsonRoot)
            break;
        
        if (NULL == cJSON_GetObjectItem(pcJsonRoot, CFG_App_Ver))
        {
            static BOOL bGenAppVersion = FALSE;
            static char szAppVer[64] = {0};

            if (!bGenAppVersion)
            {
                sprintf(szAppVer, "%s_%04d.%02d.%02d",
                        TestAp_Version,
                        YEAR(),
                        MONTH() + 1,
                        DAY());
                bGenAppVersion = TRUE;
            }
            cJSON_AddStringToObject(pcJsonRoot, CFG_App_Ver, szAppVer);
        }

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cInitJson << pszJsonData;
            cJSON_free((void *)pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
    
    return cInitJson;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenReadDataJsonData(void)
{
    CByteStream cData;
    cJSON *pcJson = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJson = cJSON_CreateObject();
        if (NULL == pcJson)
            break;
        cJSON_AddStringToObject(pcJson, Txn_Type, Txn_ReadData);

        pszJsonStr = cJSON_Print(pcJson);
        if (NULL != pszJsonStr)
        {
            cData.Write(pszJsonStr, strlen(pszJsonStr));
            free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJson);
    } while (0); 

    return cData;
}

/*
 * Public
 */
CByteStream CTxnRequest::GenSetVoltageFlagJsonData(const BOOL &bEnable)
{
    CByteStream cData; 
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_UpdateData);
        cJSON_AddStringToObject(pcJsonRoot, CFG_Voltage_Flag, bEnable ? "true" : "false");

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData.Write(pszJsonStr, strlen(pszJsonStr));
            free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */ 
CByteStream CTxnRequest::GenSetTokenFlagJsonData(const BOOL &bEnable)
{
    CByteStream cData; 
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_UpdateData);
        cJSON_AddStringToObject(pcJsonRoot, CFG_Token_Flag, bEnable ? "true" : "false");

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData.Write(pszJsonStr, strlen(pszJsonStr));
            free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenSetEmvDebugFlagJsonData(const BOOL &bEnable)
{
    CByteStream cData; 
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_UpdateData);
        cJSON_AddStringToObject(pcJsonRoot, CFG_EMV_Debug_Flag, bEnable ? "true" : "false");

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData.Write(pszJsonStr, strlen(pszJsonStr));
            free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenSetEmvCLDebugFlagJsonData(const BOOL &bEnable)
{
    CByteStream cData; 
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonStr = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_UpdateData);
        cJSON_AddStringToObject(pcJsonRoot, CFG_EMVCL_Debug_Flag, bEnable ? "true" : "false");

        pszJsonStr = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonStr)
        {
            cData.Write(pszJsonStr, strlen(pszJsonStr));
            free((void *)pszJsonStr);
            pszJsonStr = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenGetKernelVersionJsonData(void)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_GetKernelVer);

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenGetAppVersionJsonData(void)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Txn_Type, Txn_GetAppVer);

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenAIDConfirmJsonData(const BOOL &bConfirm)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_AidConfim);
        if (bConfirm)
        {
            cJSON_AddStringToObject(pcJsonRoot, Act_Value, "TRUE");
        }
        else
        {
            cJSON_AddStringToObject(pcJsonRoot, Act_Value, "FALSE");
        }
        

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}


/*
 * Public.
 */
CByteStream CTxnRequest::GenAIDSelectJsonData(const int &nSelector)
{
 
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;
    char szActValue[9] = { 0 };

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        sprintf(szActValue, "##%d", nSelector);
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_AidList);
        cJSON_AddStringToObject(pcJsonRoot, Act_Value, szActValue);
        

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;   
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenInteracPOSSeqNbr(const BYTE *pbyPosSeqNbr)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_InteracPosSeqNbr);
        cJSON_AddStringToObject(pcJsonRoot, Act_Value, (char *)pbyPosSeqNbr);
        

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}

/*
 * Public.
 */
CByteStream CTxnRequest::GenInteracAccountType(const BYTE *pbyAccountType)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_InteracAccountType);
        cJSON_AddStringToObject(pcJsonRoot, Act_Value, (char *)pbyAccountType);
        

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}

CByteStream CTxnRequest::GenLanguageSelectJsonData(const BYTE *pbyLanguage)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_LanguageSelect);
        cJSON_AddStringToObject(pcJsonRoot, Act_Value, (char *)pbyLanguage);

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}

CByteStream CTxnRequest::GenLanguageSwitchJsonData(const BYTE *pbyLanguage)
{
    CByteStream cJsonData;
    cJSON *pcJsonRoot = NULL;
    const char *pszJsonData = NULL;

    do
    {
        pcJsonRoot = cJSON_CreateObject();
        if (NULL == pcJsonRoot)
            break;
        
        cJSON_AddStringToObject(pcJsonRoot, Act_Type, Act_LanguageSwitch);
        cJSON_AddStringToObject(pcJsonRoot, Act_Value, (char *)pbyLanguage);

        pszJsonData = cJSON_Print(pcJsonRoot);
        if (NULL != pszJsonData)
        {
            cJsonData << pszJsonData;

            cJSON_free((void *) pszJsonData);
            pszJsonData = NULL;
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    return cJsonData;
}