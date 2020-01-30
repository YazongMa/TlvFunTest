/*
 * Copyright(c) Castles Technology Co,. Ltd
 *
 * File Name: EMVManage.cpp
 * Author: Amber Sun
 *
 * Create Date: 6/03/2019
 */

#include "emv_setting.h"

#include "../dbghlp.h"
#include "../ByteStream.h"
#include "../FunBase.h"
#include "operatexml.h"
#include "xmlnodeNets.h"


// Specify the identity of the logs outputted in this file.
#define TAG "HEARTLAND_CERTIFICATION_SATURN1000"


static int CheckTagNumber(PBYTE pbyTags);
static int CheckLengthLength(PBYTE pByLen, USHORT *usValueLen);


static CProfile s_cTxmlfile;


EMVManage::EMVManage()
{
    pbyConfigFile = (BYTE *)"/sdcard/emv_config.xml";
}

EMVManage::~EMVManage() {
    CloseFile();
}

/**
 * @Function
 *  EMVInit:
 *         This method is used to init emv environment.
 * @param
 *        void
 *
 * @return
 *       TRUE - Init success
 *       FALSE - Init false
 *
 * @Author: Amber
 */
BOOL EMVManage::EMVInit(BOOL isBReInit, char *pszEmvCfgPath)
{
    DWORD dwRet = 0;
    BYTE Action = 0;
    BYTE ListData[d_EMV_BUF_SIZE] ={0};

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    if (isBReInit)
    {
        pbyConfigFile = (BYTE *)pszEmvCfgPath;
    }
    TraceMsg("Open emv xml :%s", pbyConfigFile);
    dwRet = OpenFile((char *)pbyConfigFile, (BYTE *)CONFIG_XML_Config);
    if (d_EMVAPLIB_OK != dwRet)
    {
        return FALSE;
    }

    EMVKernelClear();

#if 1
    while (TRUE) //For CAPK
    {
//        static int i = 0;
//        TraceMsg("\n CAPK Num :%d \n", i);
//        i++;

        dwRet = EMVGetCAPK();
        if (d_EMVAPLIB_OK != dwRet)
        {
            if (ERROR_INVALID_NODE == dwRet)
            {
                continue;
            }
            break;
        }

        dwRet = EMVSetCAPK();
        if (d_EMVAPLIB_OK != dwRet)
        {
//            if (d_EMV_RC_INVALID_VISA_CA_KEY == dwRet)
//            {
                continue;
//            }
//            break;
        }
    }
#endif

#if 1
    //Set config
    dwRet = EMVSetAPPList();
//    if (d_OK != dwRet)
//    {
////        return FALSE;
//    }
#endif
    TraceMsg("EMV Init - EMVSetAPPList end");
#if 1
    dwRet = EMVSetTerminalConfig();
//    if (d_OK != dwRet)
//    {
//        return FALSE;
//    }
    TraceMsg("EMV Init - EMVSetTerminalConfig end");
#endif
#if 1
    dwRet = EMVSetAppConfig();
//    if (d_OK != dwRet && (ERROR_INVALID_GROUP != dwRet))
//    {
//        return FALSE;
//    }
#endif

    CloseFile();

    //Set Secure Data White list
    Action = 0;
    memcpy(ListData,d_EMV_LIST_DATA, d_EMV_LIST_DATA_SIZE);
    dwRet = EMV_SecureDataWhitelistSet(Action, ListData, d_EMV_LIST_DATA_SIZE);
    if(dwRet != 0)
    {
        TraceMsg(" Line:%d, Func:%s. EMV_SecureDataWhitelistSet fail! ReturnCode is:%04X!\n", __LINE__,__func__,dwRet);
        return FALSE;
    }

//    EMV_SetDebug(FALSE, d_EMV_DEBUG_PORT_USB);
    TraceMsg(" Line:%d, Func:%s Exit OK \n", __LINE__, __func__);
    return TRUE;
}

/**
 * @Function
 *  EMVKernelClear:
 *         This method is used to clear emv environment.
 * @param
 *        void
 *
 * @return
 *       TRUE - Init success
 *       FALSE - Init false
 *
 * @Author: Amber
 */
DWORD EMVManage::EMVKernelClear(void)
{
    DWORD dwRet = 0;
    BYTE pRID[RID_SIZE] = {0};

    EMV_CA_PUBLIC_KEY stCAPubKey;
    memset(&stCAPubKey, 0, sizeof(EMV_CA_PUBLIC_KEY));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    //delete CAPK in the flash
    memcpy(pRID, d_CLEAR_CAPK_RID, d_CLEAR_CAPK_RID_SIZE);
    stCAPubKey.bVersion = 0x01;  // Should be 0x01;
    stCAPubKey.bAction = 0x02;
    stCAPubKey.bIndex = 0xE1;
    stCAPubKey.uiModulusLen = d_CLEAR_CAPK_MODULE_SIZE;
    memcpy(stCAPubKey.baModulus, d_CLEAR_CAPK_MODULE, d_CLEAR_CAPK_MODULE_SIZE);
    stCAPubKey.uiExponentLen = 1;  // Length of Extension
    memset(stCAPubKey.baExponent, 0x00, EXPONENT_SIZE);      // Extension
    memcpy(stCAPubKey.baExponent, (BYTE *) "\x03", 1);
    memset(stCAPubKey.baHash, 0x00, d_CLEAR_CAPK_HASH_SIZE);         // Key Hash (SHA-1) Result
    memcpy(stCAPubKey.baHash, d_CLEAR_CAPK_HASH, d_CLEAR_CAPK_HASH_SIZE);

    dwRet = EMV_CAPKSet(pRID, &stCAPubKey);
    if(d_OK != dwRet)
    {
        TraceMsg("EMV_CAPKSet ret: %08x", dwRet);
        return FALSE;
    }

    //delete terminal tag data
    dwRet = EMV_TerminalTagsSet(0x02, d_TERM_CONFIG_NEW, d_TERM_CONFIG_LEN_NEW);
    if(d_OK != dwRet)
    {
        TraceMsg("EMV_TerminalTagsSet ret: %08x", dwRet);
        return dwRet;
    }
    //delete app tag data
    dwRet = EMV_AppTagsSet(0x02, d_CLEAR_APP_AID, d_CLEAR_APP_AID_SIZE, d_APP_VISA_CONFIG_NEW, d_APP_VISA_CONFIG_LEN_NEW);
    if(d_OK != dwRet)
    {
        TraceMsg("EMV_AppTagsSet ret: %08x", dwRet);
        return dwRet;
    }

    TraceMsg("Line:%d, Func:%s Exit OK \n", __LINE__, __func__);
    return dwRet;
}

/**
 * @Function
 *  EMVSetAPPList:
 *      Set App list configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVAPLIB_OK - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVManage::EMVSetAPPList(void)
{
    DWORD dwRet = 0;
    DWORD dwAIDNum = 0;
    BYTE byAIDLen = 0;
    BYTE byszAID[AID_SIZE] = {0};
    BYTE byzIndex[INDEX_SIZE] = {0};
    BYTE byszASI[ASI_SIZE] = {0};
    BYTE byszAIDHex[D_AID_NUM][AID_SIZE] = {0};
    BYTE byASI = 0;

    EMV_APPLICATION_PARA stpAppData[D_AID_NUM] = {0};
    memset(stpAppData, 0x00, sizeof(EMV_APPLICATION_PARA));
    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    while(TRUE)
    {
        memset(byszAID, 0x00, sizeof(byszAID));
        memset(byzIndex, 0x00, sizeof(byzIndex));
        memset(byszASI, 0x00, sizeof(byszASI));
//        TraceMsg(" dwAIDNum is %d", dwAIDNum);
//        dwRet = EMVGetAppList(stpAppData[dwAIDNum].pbAID, stpAppData[dwAIDNum].bAIDLen, &stpAppData[dwAIDNum].bApplication_Selection_Indicator);
//        dwRet = EMVGetAppList(byszAID, byAIDLen, &byszASI);

#if 1
        dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_APPLIST_NODE, ITEM, INDEX, (char *)byzIndex, INDEX_SIZE);
        if (d_EMVAPLIB_OK != dwRet)
        {
            TraceMsg("GetKeyAttributeValue ret:%08x", dwRet);
            break;
        }

        s_cTxmlfile.GetKeyValue(DAID, "", (char *)byszAID);
        s_cTxmlfile.GetKeyValue(ASI, "", (char *)byszASI);
        byAIDLen = strlen((char *)byszAID) / 2;

        CFunBase::Str2Hex((char *)byszAID, byszAIDHex[dwAIDNum]);
        CFunBase::Str2Hex((char *)byszASI, &byASI);
//        LOGPointer(byszAIDHex, byAIDLen, (char *)"AID Hex data:");
#endif

//        memcpy(stpAppData[dwAIDNum].pbAID, byszAIDHex[dwAIDNum], byAIDLen);
        stpAppData[dwAIDNum].pbAID = byszAIDHex[dwAIDNum];
        stpAppData[dwAIDNum].bAIDLen = byAIDLen;
        stpAppData[dwAIDNum].bApplication_Selection_Indicator = byASI;
//        LOGPointer(stpAppData[dwAIDNum].pbAID, byAIDLen, (char *)"m_byAID data:");

        dwAIDNum ++;
    } //end of get app list

    dwRet = EMV_ApplicationSet(dwAIDNum, stpAppData);

    TraceMsg(" Line:%d, Func:%s Exit ret:%08x \n", __LINE__, __func__, dwRet);
    return dwRet;
}

/**
 * @Function
 *  EMVSetTerminalConfig:
 *      Set Terminal configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVAPLIB_OK - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVManage::EMVSetTerminalConfig(void)
{
    BYTE byAction = 0x00;
    DWORD dwRet = 0;
    BYTE byTagLen = 0;
    BYTE byTagDateLen = 0;

    BYTE byszTag[d_EMV_TAG_SIZE] = {0};
    BYTE byszTagData[d_EMV_BUF_SIZE] = {0};

    CByteStream cTermData;
    cTermData.Empty();

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    while(TRUE)
    {
        memset(byszTag, 0x00, d_EMV_TAG_SIZE);
        memset(byszTagData, 0x00, d_EMV_BUF_SIZE);
        dwRet = EMVGetTerminalConfig(byszTag, byTagLen, byszTagData, byTagDateLen);
        if (d_EMVAPLIB_OK != dwRet)
        {
            break;
        }

        TraceMsg("Tag Len %d, TagDateLen is %d", byTagLen, byTagDateLen);
        //Tag
        cTermData.Write(byszTag, byTagLen);
        //Length of Value, 1 byte is enough
        cTermData << byTagDateLen;
        //Value
        cTermData.Write(byszTagData, byTagDateLen);
    }
//    LOGPointer(cTermData.GetBuffer(), cTermData.GetLength(), (char *)"Term data:");

    dwRet = EMV_TerminalTagsSet(byAction, cTermData.GetBuffer(), cTermData.GetLength());

    cTermData.Empty();
    TraceMsg(" Line:%d, Func:%s Exit ret: %08x \n", __LINE__, __func__, dwRet);
    return dwRet;
}

/**
 * @Function
 *  EMVSetCAPK:
 *      Set CAPK configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVAPLIB_OK - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVManage::EMVSetCAPK(void)
{
    DWORD dwRet = 0;
    BYTE byszRID[RID_SIZE] = {0};

    EMV_CA_PUBLIC_KEY stCAPubKey;
    memset(&stCAPubKey, 0, sizeof(EMV_CA_PUBLIC_KEY));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    memset(byszRID, 0x00, RID_SIZE);
    memset(&stCAPubKey, 0x00, sizeof(EMV_CA_PUBLIC_KEY));

    stCAPubKey.bVersion = 0x01; //Default Value
    stCAPubKey.bAction = 0x00; //Add one CAPK
    CFunBase::Str2Hex((const char*)m_byRID, byszRID);
    CFunBase::Str2Hex((const char*)m_byIndex, &stCAPubKey.bIndex);
    CFunBase::Str2Hex((const char*)m_byExponent, stCAPubKey.baExponent);
    CFunBase::Str2Hex((const char*)m_byHash, stCAPubKey.baHash);
    CFunBase::Str2Hex((const char*)m_byModule, stCAPubKey.baModulus);

    stCAPubKey.uiExponentLen = strlen((const char*)stCAPubKey.baExponent);
    stCAPubKey.uiModulusLen = strlen((char *)m_byModule) / 2;

    dwRet = EMV_CAPKSet((BYTE *)byszRID, (EMV_CA_PUBLIC_KEY*)&stCAPubKey);
    if(d_EMVAPLIB_OK != dwRet)
    {
        TraceMsg("Line:%d  Fail to CAPKSet:%d\n", __LINE__, dwRet);
        return dwRet;
    }

    TraceMsg(" Line:%d, Func:%s Exit OK \n", __LINE__, __func__);
    return dwRet;
}

/**
 * @Function
 *  EMVSetAppConfig:
 *      Set App configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVAPLIB_OK - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVManage::EMVSetAppConfig(void)
{
    BYTE Action = 0x00;
    BYTE byAIDLen = 0;
    BYTE byTagLen = 0;
    BYTE byTagDateLen = 0;
    DWORD dwRet = 0;

    BYTE byszAID[AID_SIZE] = {0};
    BYTE byszAIDHex[AID_SIZE] = {0};
    BYTE byszTag[d_EMV_TAG_SIZE] = {0};
    BYTE byszTagData[d_EMV_BUF_SIZE] = {0};

    BYTE byszBuf[d_EMV_BUF_SIZE] = {0};
    CByteStream cAppConfigData;
    cAppConfigData.Empty();

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    while (TRUE)
    {
        memset(byszAID, 0x00, sizeof(byszAID));
        memset(byszTag, 0x00, sizeof(byszTag));
        memset(byszTagData, 0x00, sizeof(byszTagData));

//        dwRet = EMVGetAppConfig(byszAID, byAIDLen, byszTag, byTagLen, byszTagData, byTagDateLen);
#if 1
        dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_APPCONFIG_NODE, APPCONFIG_GROUP, DAID, (char *)byszAID, AID_SIZE);
        if (dwRet != d_EMVAPLIB_OK)
        {
            return dwRet;
        }

        dwRet = s_cTxmlfile.GetKeyAttributeValue(ITEM, EMV_TAG, (char *)byszTag, 8);
        if (dwRet == d_EMVAPLIB_OK)
        {
            s_cTxmlfile.GetKeyValue("", (char *)byszTagData);
        }
#endif

        if (d_EMVAPLIB_OK != dwRet)
        {
            if (ERROR_INVALID_NODE == dwRet)
            {
                byAIDLen = strlen((char*)byszAID) / 2;
                memset(byszAIDHex, 0x00, sizeof(byszAIDHex));
                CFunBase::Str2Hex((char *)byszAID, byszAIDHex);
//                LOGPointer(byszAID, byAIDLen, (char *)"AID data:");
//                LOGPointer(cAppConfigData.GetBuffer(), cAppConfigData.GetLength(), (char *)"App Config data:");
//                TraceMsg("Length: %d", cAppConfigData.GetLength());

                dwRet = EMV_AppTagsSet(Action, byszAIDHex, byAIDLen, cAppConfigData.GetBuffer(), cAppConfigData.GetLength());

                TraceMsg(" Line:%d, Func:%s EMV_AppTagsSet ret: %08x \n", __LINE__, __func__, dwRet);

                cAppConfigData.Empty();
                continue;
            }
            break;
        }

        byTagLen = 2; //strlen((char*)byszTag) / 2;
        byTagDateLen = strlen((char*)byszTagData) / 2;
        memset(byszBuf, 0x00, sizeof(byszBuf));
        CFunBase::Str2Hex((char*)byszTag, byszBuf);
        cAppConfigData.Write(byszBuf, byTagLen);  //Write Tag
        cAppConfigData << byTagDateLen;           //Write TagValueLength
        memset(byszBuf, 0x00, sizeof(byszBuf));
        CFunBase::Str2Hex((char*)byszTagData, byszBuf);
        cAppConfigData.Write(byszBuf, byTagDateLen); //Write TagValue
    } //end while

    TraceMsg("Line:%d, Func:%s Exit ret: %08x \n", __LINE__, __func__, dwRet);
    return (dwRet);
}

/**
 * @Function
 *  IsEMVCardExist:
 *      This function is use to detect emv card is exist or remove.
 * @param
 *      None
 * @return
 *      d_SC_Present - EMV Card is exist
 *      Other values - EMV Card is remove.
 *
 * @Author: Amber
 */
DWORD EMVManage::IsEMVCardExist(void)
{
    DWORD dwRet = 0;
    BYTE byStatus = d_MK_SC_PRESENT;
    //Insert
    dwRet = CTOS_SCStatus(d_SC_USER, &byStatus);
    if (d_EMVAPLIB_OK != dwRet) {
        TraceMsg("\n CTOS_SCStatus ret: %04x", dwRet);
        return dwRet;
    }

    if (byStatus & d_MK_SC_PRESENT) //Card present
    {
        TraceMsg("EMV card present");
        return d_SC_Present;
    }

    return d_SC_Remove;
}

DWORD EMVManage::EMVSetTerminalData(BYTE* pbyAID, BYTE byAIDLen, BYTE* pbyTlvData, INT nTlvDataLen)
{
    USHORT usTag;
    DWORD dwRet = 0;
    INT nCurrentLen = 0;
    if (NULL == pbyAID || NULL == pbyTlvData || byAIDLen < 1 || nTlvDataLen < 1)
    {
        return ERROR_INVALID_PARAM;
    }
    while (TRUE)
    {
        int nTagLen = CheckTagNumber(pbyTlvData);
        if (nTagLen == 0 || nTagLen == ERROR_INVALID_PARAM)
        {
            TraceMsg("Tag length is %02x - invalid", nTagLen);
            return ERROR_INVALID_TAG;
        }
        else if (nTagLen == 1)
        {
            usTag = pbyTlvData[nCurrentLen];
        }
        else if (nTagLen == 2)
        {
            usTag = pbyTlvData[nCurrentLen] * 256 + pbyTlvData[nCurrentLen + 1];
        }
        else
        {
            return ERROR_INVALID_TAG;
        }

        USHORT usTagValueLen = 0;
        nCurrentLen += nTagLen;
        int nTagValueLenLen = CheckLengthLength(pbyTlvData + nCurrentLen, &usTagValueLen);
        if (nTagValueLenLen == 0 || nTagValueLenLen == ERROR_INVALID_PARAM)
        {
            TraceMsg("Tag length is %02x - invalid", nTagValueLenLen);
            return ERROR_INVALID_TAG;
        }
        BYTE byszTagValue[d_EMV_BUF_SIZE] = {0};
        nCurrentLen += nTagValueLenLen;
        if (nTlvDataLen < nCurrentLen + usTagValueLen)
        {
            TraceMsg("Exit nOutLen is %04x, nCurrentLen is %04x, usTagValueLen is %04x", nTlvDataLen, nCurrentLen, usTagValueLen);
            break;
        }
        memset(byszTagValue, 0x00, sizeof(byszTagValue));
        memcpy(byszTagValue, pbyTlvData + nCurrentLen, usTagValueLen);
        nCurrentLen += usTagValueLen;
        dwRet = EMV_TerminalDataSet(pbyAID, byAIDLen, usTag, usTagValueLen, byszTagValue);
        TraceMsg("EMV_TerminalDataSet ret %04x", dwRet);
        TraceMsg("nOutLen is %04x, nCurrentLen is %04x, usTagValueLen is %04x", nTlvDataLen, nCurrentLen, usTagValueLen);
    }
    return dwRet;
}


/* ======================================= Get Data from xml ================================================= */
/**
 * CreatFile
 *
 * Function:
 *           Load the xml file.
 *
 * @param:
 *           [IN]  pszFileName  The name of xml file.
 *
 * @return:
 *           d_EMVAPLIB_OK - If successful
 *           OtherValue - Otherwise
 **/
WORD EMVManage::OpenFile(char *pszFileName, BYTE *pbyXmlNode)
{
    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    WORD wRet = 0;

    if (NULL == pszFileName || NULL == pbyXmlNode)
    {
        TraceMsg("Line:%d, Func:%s ERROR:No xml file find \n", __LINE__, __func__);
        return ERROR_INVALID_PARAM;
    }

    wRet = s_cTxmlfile.OpenConfigFile(pszFileName, (char*)pbyXmlNode);
    if(wRet != d_EMVAPLIB_OK)
    {
        TraceMsg("Line:%d, Func:%s ERROR: Not open File ret: %04x \n", __LINE__, __func__, wRet);
        return (wRet);
    }

    TraceMsg("Line:%d, Func:%s Exit ok \n", __LINE__, __func__);
    return(d_EMVAPLIB_OK);
}

void EMVManage::CloseFile(void)
{
    TraceMsg("Line:%d, Func:%s  \n", __LINE__, __func__);
    s_cTxmlfile.CloseConfigFile();
}

/**
 * EMVGetCAPK
 *
 * Function:
 *           Get CAPK from xml for EMV & EMVCL.
 *
 * @param:
 *           None.
 * @return:
 *           d_EMVAPLIB_OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVManage::EMVGetCAPK(void)
{
    DWORD dwRet = 0;

    memset(m_byRID, 0x00, RID_SIZE);
    memset(m_byIndex, 0x00, INDEX_SIZE);
    memset(m_byModule, 0x00, MODULE_SIZE);
    memset(m_byExponent, 0x00, EXPONENT_SIZE);
    memset(m_byHash, 0x00, HASH_SIZE);

    TraceMsg("Function: CardManage::EMVGetCAPK Entry");

    // <Group RID="A000000152">
    dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_CAPK_NODE, CAPK_GROUP, CRID, (char *)m_byRID, RID_SIZE);
    if (dwRet != d_EMVAPLIB_OK)
    {
        TraceMsg("GetKeyAttributeValue ret: %08x", dwRet);
        return dwRet;
    }
    // <Item index="5A">
    dwRet = s_cTxmlfile.GetKeyAttributeValue(ITEM, INDEX, (char *)m_byIndex, INDEX_SIZE);
    if (dwRet != d_EMVAPLIB_OK)
    {
        TraceMsg("GetKeyAttributeValue ret: %08x", dwRet);
        TraceBytes(m_byRID, RID_SIZE, "RID");
        return dwRet;
    }

    s_cTxmlfile.GetKeyValue(MODULES, "", (char *)m_byModule);
    s_cTxmlfile.GetKeyValue(EXPONENT, "", (char *)m_byExponent);
    s_cTxmlfile.GetKeyValue(HASH, "", (char *)m_byHash);

    TraceMsg("Function: CardManage::EMVGetCAPK Exit OK");
    return d_EMVAPLIB_OK;
}

/**
 * EMVGetAppList
 *
 * Function:
 *           Get EMV App List from xml for EMV.
 *
 * @param:
 *           .
 *
 * @return:
 *           d_EMVAPLIB_OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVManage::EMVGetAppList(BYTE *pbyAID, BYTE &byAIDLen, BYTE *pbyASI)
{
    DWORD dwRet = 0;

    BYTE byszAID[AID_SIZE] = {0};
    BYTE byszASI[ASI_SIZE] = {0};
    BYTE byzIndex[INDEX_SIZE] = {0};

    memset(byszAID, 0x00, sizeof(byszAID));
    memset(byszASI, 0x00, sizeof(byszASI));
    memset(byzIndex, 0x00, sizeof(byzIndex));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_APPLIST_NODE, ITEM, INDEX, (char *)byzIndex, INDEX_SIZE);
    if (dwRet != d_OK)
    {
        TraceMsg("GetKeyAttributeValue ret:%08x", dwRet);
        return dwRet;
    }

    s_cTxmlfile.GetKeyValue(DAID, "", (char *)byszAID);
    s_cTxmlfile.GetKeyValue(ASI, "", (char *)byszASI);

    byAIDLen = strlen((char *)byszAID) / 2;

    CFunBase::Str2Hex((char *)byszAID, pbyAID);
    CFunBase::Str2Hex((char *)byszASI, pbyASI);

    TraceMsg(" Line:%d, Func:%s Exit OK \n",__LINE__, __func__);
    return d_EMVAPLIB_OK;
}

/**
 * EMVGetTerminalConfig
 * Function:
 *           Get Terminal Config from xml for EMV.
 *
 * @param:
 *           None.
 *
 * @return:
 *           d_EMVAPLIB_OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVManage::EMVGetTerminalConfig(BYTE *pbyTag, BYTE &byTagLen, BYTE* pbyTagValue, BYTE &byTagValueLen)
{
    DWORD dwRet = 0;
    BYTE byszTag[d_EMV_TAG_SIZE] = {0};
    BYTE byszTagValue[d_EMV_BUF_SIZE] = {0};

    memset(byszTag, 0x00, sizeof(byszTag));
    memset(byszTagValue, 0x00, sizeof(byszTagValue));
    memset(pbyTag, 0x00, strlen((char *)pbyTag));
    memset(pbyTagValue, 0x00, strlen((char *)pbyTagValue));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_TERM_NODE, ITEM, EMV_TAG, (char *)byszTag, d_EMV_TAG_SIZE*2);
    if (dwRet != d_OK)
    {
        return dwRet;
    }

    s_cTxmlfile.GetKeyValue("", (char *)byszTagValue);

    byTagLen = 2; //strlen((char *)byszTag) / 2;
    byTagValueLen = strlen((char *)byszTagValue) / 2;

    CFunBase::Str2Hex((char *)byszTag, pbyTag);
    CFunBase::Str2Hex((char *)byszTagValue, pbyTagValue);

//    memcpy(pbyTag, byszTag, byTagLen);
//    memcpy(pbyTagValue, byszTagValue, byTagValueLen);



//    printf("\n Tag is: ");
//    cFun.PrintPointer(pbyTagValue, 10);
//    printf("\n Value is: ");
//    cFun.PrintPointer(byszTagValue, 16);
//
//    printf("\n Tag is: ");
//    cFun.PrintPointer(byszTag, strlen((char *)byszTag));
//    printf("\n Value is: ");
//    cFun.PrintPointer(byszTagValue, strlen((char *)byszTagValue));
//
//    printf("\n Tag is: ");
//    cFun.PrintPointer(pbyTag, strlen((char *)pbyTag));
//    printf("\n Value is: ");
//    cFun.PrintPointer(pbyTagValue, strlen((char *)pbyTagValue));

    TraceMsg(" Line:%d, Func:%s Exit OK \n", __LINE__, __func__);
    return d_EMVAPLIB_OK;
}

/**
 * EMVGetAppConfig
 * Function:
 *           Get App from xml for EMV.
 *
 * @param:
 *           .
 *
 * @return:
 *           d_EMVAPLIB_OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVManage::EMVGetAppConfig(BYTE *pbyAID, BYTE &byAIDLen, BYTE *pbyTag, BYTE &byTagLen, BYTE *pbyTagValue, BYTE &byTagValueLen)
{
    DWORD dwRet = 0;
    BYTE byszAID[AID_SIZE] = {0};
    BYTE byszTag[4] = {0};
    BYTE byszTagValue[60] = {0};
    BYTE byszAIDHex[AID_SIZE] = {0};

//    memset(m_byAID, 0x00, AID_SIZE);
//    memset(m_byTag, 0x00, 4);
//    memset(m_byTagValue, 0x00, 16);
//    memset(byszAIDHex, 0x00, sizeof(byszAIDHex));

    memset(byszAID, 0x00, sizeof(byszAID));
    memset(byszTag, 0x00, sizeof(byszTag));
    memset(byszTagValue, 0x00, sizeof(byszTagValue));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    dwRet = s_cTxmlfile.GetKeyAttributeValue(CONFIG_XML_APPCONFIG_NODE, APPCONFIG_GROUP, DAID, (char *)byszAID, AID_SIZE);
    if (dwRet != d_OK)
    {
        TraceMsg("Line:%d, Func:%s Get AID Attribute Value ret: %08x", __LINE__, __func__, dwRet);
        return dwRet;
    }

    dwRet = s_cTxmlfile.GetKeyAttributeValue(ITEM, EMV_TAG, (char *)byszTag, 8);
    if (dwRet != d_OK)
    {
        TraceMsg("Line:%d, Func:%s Get Tag Attribute Value ret: %08x", __LINE__, __func__, dwRet);
        return dwRet;
    }

    s_cTxmlfile.GetKeyValue("", (char *)byszTagValue);

    byAIDLen = strlen((char *)byszAID) / 2;
    byTagLen = strlen((char *)byszTag) / 2;
    byTagValueLen = strlen((char *)byszTagValue) / 2;
    if (byTagLen > 3)
    {
        byTagLen = 2;
    }

//    Str2Hex((char *)byszAID, pbyAID);
//    Str2Hex((char *)byszAID, m_byAID);
    CFunBase::Str2Hex((char *)byszAID, byszAIDHex);
    memcpy(pbyAID, byszAID, byAIDLen * 2);
    CFunBase::Str2Hex((char *)byszTag, pbyTag);
    CFunBase::Str2Hex((char *)byszTagValue, pbyTagValue);

    LOGPointer(pbyAID, byAIDLen, (char *)"AID is");
//    LOGPointer(m_byAID, byAIDLen, (char *)"AID is");
    LOGPointer(pbyTag, byTagLen, (char *)"Tag is");
    LOGPointer(pbyTagValue, byTagValueLen, (char *)"Value is");

    TraceMsg("File:%s, Line:%d, Func:%s Exit OK \n", __FILE__, __LINE__, __func__);
    return d_EMVAPLIB_OK;
}

/* ======================================= end of Get Data from xml ================================================= */
void EMVManage::LOGPointer(BYTE * pbyPointer,BYTE byLen, char *pbyStr)
{
    if (NULL == pbyPointer || NULL == pbyStr)
    {
        TraceMsg("NULL String: Nothing to print\n");
        return;
    }

    TraceMsg(" %s ", pbyStr);
    for(int i=0; i<byLen; i++)
    {
        TraceMsg("%02x", *(pbyPointer + i));
    }
}



int CheckTagNumber(PBYTE pbyTags)
{
    USHORT usTagLen = 1;

    if(pbyTags == NULL)
        return 0;

    //check tag
    int iCnt = 0;
    if((pbyTags[iCnt] & 0x1F) == 0x1F)
    {
        usTagLen ++;
        iCnt ++;
        while(pbyTags[iCnt] & 0x80)
        {
            usTagLen ++;
            iCnt ++;
            if(usTagLen > 4)
                return ERROR_INVALID_PARAM;
        }
    }

    return usTagLen;	// Tag Length
}


int CheckLengthLength(PBYTE pbyLen, unsigned short* pusValueLen)
{
    if(pbyLen == NULL && pusValueLen == NULL)
    {
        return ERROR_INVALID_PARAM;
    }

    USHORT usLengthLen;

    if (*pbyLen < 0x80)
    {
        usLengthLen = 1;
        *pusValueLen = *pbyLen;
    }
    else if (*pbyLen == 0x81)
    {
        usLengthLen = 2;
        *pusValueLen = pbyLen[1];
    }
    else if (*pbyLen == 0x82)
    {
        usLengthLen = 3;
        *pusValueLen = ((pbyLen[1] << 8) + pbyLen[2]);
    }
    else if (*pbyLen == 0x83)
    {
        usLengthLen = 4;
        *pusValueLen = ((pbyLen[1] << 16) + (pbyLen[2] << 8) + pbyLen[3]);
    }
    else
    {
        usLengthLen = 0;
    }

    return usLengthLen;
}
