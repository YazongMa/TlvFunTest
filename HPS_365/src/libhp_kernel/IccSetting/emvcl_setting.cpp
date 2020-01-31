/*
 * Copyright(c) Castles Technology Co,. Ltd
 *
 * File Name: EMVCLManage.cpp
 * Author: Amber Sun
 *
 * Create Date: 5/17/2019
 */



#include "../dbghlp.h"
#include "../ByteStream.h"
#include "../FunBase.h"
#include "operatexml.h"
#include "xmlnodeNets.h"
#include "emvcl_setting.h"


// Specify the identity of the logs outputted in this file.
#define TAG "HEARTLAND_CERTIFICATION_SATURN1000"


static CProfile s_cProfile;

void LOGPointer(BYTE * pbyPointer,BYTE byLen, char *pbyStr);



EMVCLManage::EMVCLManage()
{
    pbyConfigFile = (BYTE *)"/sdcard/emvcl_config.xml";
}


EMVCLManage::~EMVCLManage() {
    CloseFile();
}


/**
 * @Function
 *  EMVCLInit:
 *         This method is used to init emvcl environment.
 * @param
 *        void
 *
 * @return
 *       TRUE - Init success
 *       FALSE - Init false
 *
 * @Author: Amber
 */
BOOL EMVCLManage::EMVCLInit(BOOL isBReInit, char *pszEmvclCfgPath)
{
    DWORD dwRet = 0;
    BYTE ListData[d_EMVCL_BUF_LEN] = {0};

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    if (isBReInit)
    {
        pbyConfigFile = (BYTE *)pszEmvclCfgPath;
    }
    TraceMsg("Open CTLS xml file :%s", pbyConfigFile);
    dwRet = OpenFile((char *)pbyConfigFile, (BYTE *)CONFIG_XML_CLConfig);
    if (d_EMVCL_NO_ERROR != dwRet)
    {
        return FALSE;
    }

    EMVCLKernelClear();
    
    while (TRUE) //For CAPK
    {
//        static int i = 0;
//        TraceMsg("\n CAPK Num :%d \n", i);
//        i++;
        
        dwRet = EMVGetCAPK();
        if (d_EMVCL_NO_ERROR != dwRet)
        {
            if (ERROR_INVALID_NODE == dwRet)
            {
                continue;  
            }
            break;
        }
        
        dwRet = EMVCLSetCAPK();
        if (d_EMVCL_NO_ERROR != dwRet)
        {
            if (d_EMVCL_RC_INVALID_VISA_CA_KEY == dwRet)
            {
                continue;
            }
            break;
        }
    }
    
    while (TRUE) //For TagData
    {
//        static int j = 0;
//        TraceMsg("\n TagData Num :%d \n", j);
//        j++;
        
        dwRet = EMVCLGetTagData();
        if (d_EMVCL_NO_ERROR != dwRet)
        {
            break;
        }
        
        dwRet = EMVCLSetTagData();
    }

    //Set white list
    memcpy(ListData, d_EMVCL_LIST_DATA, d_EMVCL_LIST_DATA_SIZE);
    dwRet = EMVCL_SecureDataWhitelistSet(0x00, ListData, d_EMVCL_LIST_DATA_SIZE);

    EMVCLSetPara();
    CloseFile();

    TraceMsg(" Line:%d, Func:%s Exit \n", __LINE__, __func__);
    return TRUE;
}


/**
 * @Function
 *  IsCTLSCardExist:
 *      This function is use to detect CTLS card is exist or remove.
 * @param
 *      None
 * @return
 *      OK - CTLS Card is exist
 *      Other values - CTLS Card is remove.
 *
 * @Author: Wendy
 */
WORD EMVCLManage::IsCTLSCardExist(void)
{
    WORD wRet;
    BYTE byATQA[10] = {0};
    BYTE bySAK[10] = {0};
    BYTE byCSN[10] = {0};
    BYTE byCSNLen;
    BYTE byATQB[10] = {0};
    BYTE byATQBLen = 0;
    BYTE byAFI = 0;
    BYTE byszPUPI[100] = {0};
    BYTE byszRep[100] = {0};

    wRet = CTOS_CLTypeAActiveFromIdle(0, byATQA, bySAK, byCSN, &byCSNLen);
    if (wRet != d_OK)
    {
        wRet = CTOS_WUPB(byAFI, byATQB, &byATQBLen);
        if (wRet != d_OK)
        {
            CTOS_CLPowerOff();
        }
        else
        {
            CTOS_ATTRIB(byszPUPI, byszRep);
        }
    }

    return wRet;
}




/* ================================================ EMVCL PART ================================================ */
/**
 * @Function
 *  EMVCLKernelClear:
 *      Clear the environment of EMVCL Kernel
 *
 * @param
 *          void
 * @return
 *          void
 *
 * @Author: Amber
 */
void EMVCLManage::EMVCLKernelClear(void)
{
    EMVCL_AID_SET_TAG_DATA DeTagData = {0};
    EMVCL_CA_PUBLIC_KEY CAPubKey = {0};

    memset(&DeTagData, 0x00, sizeof(EMVCL_AID_SET_TAG_DATA));
    memset(&CAPubKey, 0x00, sizeof(EMVCL_CA_PUBLIC_KEY));

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    //Clear Tag Data
    EMVCL_AIDSetTagData(0x02, &DeTagData);
    //Clear CAPK
    CAPubKey.bAction = 0x02;
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x00\x25", &CAPubKey);
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x00\x03", &CAPubKey);
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x00\x04", &CAPubKey);
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x00\x65", &CAPubKey);
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x01\x52", &CAPubKey);
    EMVCL_SetCAPK((BYTE*)"\xA0\x00\x00\x03\x33", &CAPubKey);
}

/**
 * @Function
 *  EMVCLSetCAPK:
 *      Set CAPK configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVCL_NO_ERROR - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVCLManage::EMVCLSetCAPK(void)
{
    BYTE baRID[RID_SIZE] = {0};
    DWORD dwRet = 0;
    
    
    EMVCL_CA_PUBLIC_KEY CAPubKey = {0};

    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);

    memset(CAPubKey.baModulus, 0, sizeof(CAPubKey.baModulus));
    
    CAPubKey.bAction = 0x00; //Add one CAPK 
    CFunBase::Str2Hex((const char*)m_byRID, baRID);
    CFunBase::Str2Hex((const char*)m_byIndex, &CAPubKey.bIndex);
    CFunBase::Str2Hex((const char*)m_byExponent, CAPubKey.baExponent);
    CFunBase::Str2Hex((const char*)m_byHash, CAPubKey.baHash);
    CFunBase::Str2Hex((const char*)m_byModule, CAPubKey.baModulus);

    CAPubKey.uiExponentLen = strlen((const char*)CAPubKey.baExponent);
    CAPubKey.uiModulusLen = strlen((char *)m_byModule) / 2;

    dwRet = EMVCL_SetCAPK(baRID, &CAPubKey);
    if (d_EMVCL_NO_ERROR != dwRet)
    {
        TraceMsg("Line:%d, Func:%s Error:SetCAPK retcode %08x \n", __LINE__, __func__, dwRet);
        return dwRet;
    }

    TraceMsg("Line:%d, Func:%s Fun:EMVCL_SetCAPK OK \n", __LINE__, __func__);
    return d_EMVCL_NO_ERROR;
}

/**
 * @Function
 *  EMVCLSetTagData:
 *      Set Tag data configuration
 *
 * @param
 *          void
 * @return
 *          d_EMVCL_NO_ERROR - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVCLManage::EMVCLSetTagData(void)
{
    BYTE byAddAction = 0x00;
    DWORD dwRet = 0;
    BYTE byszTagData[TLV_DATA_SIZE] = {0};
    BYTE byszTagDataHex[TLV_DATA_SIZE] = {0};
    USHORT usTagDataLen = 0;
    

    EMVCL_AID_SET_TAG_DATA SetTagData = {0};

    TraceMsg("File:%s, Line:%d, Func:%s Entry \n", __FILE__, __LINE__, __func__);

    memset(&SetTagData, 0x00, sizeof(EMVCL_AID_SET_TAG_DATA));

    CFunBase::Str2Hex((const char*)m_byAID, SetTagData.baAID);
    SetTagData.bAIDLen = strlen((char *)m_byAID) / 2;
    CFunBase::Str2Hex((const char*)m_byKernelID, &SetTagData.bKernelID);
    CFunBase::Str2Hex((const char*)m_byTxnType, &SetTagData.bTransactionType);
    usTagDataLen = strlen((char *)m_byTagData);
    memcpy(byszTagData, m_byTagData, usTagDataLen);
    if (!memcmp(m_byAID, d_AID_NETS, strlen(d_AID_NETS)) || !memcmp(m_byAID, d_AID_VISA, strlen(d_AID_VISA))
    || !memcmp(m_byAID, d_AID_UNIONPAY, strlen(d_AID_UNIONPAY)) )
    {
        TraceMsg("Add DF8F49");
        memcpy(&byszTagData[usTagDataLen], d_UDOL_DF8F49, d_UDOL_DF8F49_SIZE);
    }
    else if (!memcmp(m_byAID, d_AID_AMEX, strlen(d_AID_AMEX)))
    {
        TraceMsg("Add DF5A");
        memcpy(&byszTagData[usTagDataLen], d_UDOL_DF5A, d_UDOL_DF5A_SIZE);
    }


    CFunBase::Str2Hex((const char*)byszTagData, byszTagDataHex);
    SetTagData.pbaTAGData = byszTagDataHex;
    SetTagData.usTAGDataLen = strlen((char *)byszTagData) / 2;
//    LOGPointer(SetTagData.pbaTAGData, SetTagData.usTAGDataLen, "Tag Data ");
    
    TraceMsg("\n Line:%d, Func:%s Fun:EMVCL_AIDSetTagData \n", __LINE__, __func__);

    dwRet = EMVCL_AIDSetTagData(byAddAction, &SetTagData);
    if (d_EMVCL_NO_ERROR != dwRet)
    {
        TraceMsg("\n Line:%d, Func:%s Error:SetTag retcode %08x \n", __LINE__, __func__, dwRet);
        return dwRet;
    }

    TraceMsg(" Line:%d, Func:%s Exit ok \n", __LINE__, __func__);
    return dwRet;
}

/**
 * @Function
 *  EMVCLSetPara:
 *      Set Parameter to kernel.
 *
 * @param
 *          void
 * @return
 *          d_EMVCL_NO_ERROR - Set success
 *          Other Values - Please refer to emv_cl.h
 *
 * @Author: Amber
 */
DWORD EMVCLManage::EMVCLSetPara(void)
{
    DWORD dwRet = 0;
    BYTE byIndexLen = 0;
    BYTE byszParaIndex[PARA_INDEX_SIZE] = {0};
    BYTE byszParaHexIndex[PARA_INDEX_SIZE] = {0};
    BYTE byszParaIndexValue[PARA_INDEX_SIZE] = {0};
    BYTE byszParaIndexHexValue[PARA_INDEX_SIZE] = {0};
    EMVCL_PARAMETER_DATA stParaData;
    memset(&stParaData, 0x00, sizeof(EMVCL_PARAMETER_DATA));
    
    INT nCount = 0;

    while(TRUE)
    {
        memset(byszParaIndex, 0x00, sizeof(byszParaIndex));
        memset(byszParaIndexValue, 0x00, sizeof(byszParaIndexValue));
        memset(byszParaIndexHexValue, 0x00, sizeof(byszParaIndexHexValue));
        dwRet = s_cProfile.GetKeyAttributeValue(CONFIG_XML_PARAMETER, ITEM, PARAINDEX, (char *)byszParaIndex, PARA_INDEX_SIZE);

        if (d_EMVCL_NO_ERROR == dwRet)
        {
            s_cProfile.GetKeyValue("", (char *)byszParaIndexValue);

            byIndexLen = strlen((char*)byszParaIndex)/2;
            CFunBase::Str2Hex((char*)byszParaIndex, byszParaHexIndex);
            if (byIndexLen == 1) {
                stParaData.uiIndex[nCount] = byszParaHexIndex[0];
            }
            else if (byIndexLen == 2)
            {
                stParaData.uiIndex[nCount] = byszParaHexIndex[0] *256 + byszParaHexIndex[1];
            }

            stParaData.uiLen[nCount] = strlen((char*)byszParaIndexValue)/2;
            CFunBase::Str2Hex((char*)byszParaIndexValue, byszParaIndexHexValue);
            memcpy(stParaData.baData[nCount], byszParaIndexHexValue, strlen((char*)byszParaIndexValue)/2);

            TraceMsg("stParaData.uiIndex[%d] is %04x", nCount, stParaData.uiIndex[nCount]);
            TraceMsg("stParaData.uiLen[%d] is %04x", nCount, stParaData.uiLen[nCount]);
//            LOGPointer(stParaData.baData[nCount], stParaData.uiLen[nCount], "stParaData.baData");

            nCount ++;
            continue;
        }
        break;
    }
    stParaData.uiNoP = nCount;

    dwRet = EMVCL_SetParameter(&stParaData);

    TraceMsg(" Line:%d, Func:%s Exit ret %08x \n", __LINE__, __func__, dwRet);
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
 *           OK - If successful
 *           OtherValue - Otherwise
 **/
WORD EMVCLManage::OpenFile(char *pszFileName, BYTE *pbyXmlNode)
{
    TraceMsg(" Line:%d, Func:%s Entry \n", __LINE__, __func__);
    
    WORD wRet = 0;
    
    if (NULL == pszFileName || NULL == pbyXmlNode)
    {
        TraceMsg("Line:%d, Func:%s ERROR:No xml file find \n", __LINE__, __func__);
        return ERROR_INVALID_PARAM;
    }    
    
    wRet = s_cProfile.OpenConfigFile(pszFileName, (char*)pbyXmlNode);
    if(wRet != d_OK)
    {
        TraceMsg("Line:%d, Func:%s ERROR: Not open File ret: %04x \n", __LINE__, __func__, wRet);
        return (wRet);
    }

    TraceMsg("Line:%d, Func:%s Exit ok \n", __LINE__, __func__);
    return(d_OK);
}

/**
 * CloseFile
 *
 * Function:
 *           Close the xml file opened current.
 *
 * @param:
 *           void
 *
 * @return:
 *           OK - If successful
 *           OtherValue - Otherwise
 **/
void EMVCLManage::CloseFile(void)
{
    TraceMsg("Line:%d, Func:%s  \n", __LINE__, __func__);
    s_cProfile.CloseConfigFile();
}


/**
 * EMVGetCAPK
 *
 * Function:
 *           Get CAPK from xml for EMV & EMVCL.
 *
 * @param:
 *           None.
 *
 * @return:
 *           OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVCLManage::EMVGetCAPK(void)
{
    DWORD dwRet = 0;

    memset(m_byRID, 0x00, RID_SIZE);
    memset(m_byIndex, 0x00, INDEX_SIZE);
    memset(m_byModule, 0x00, MODULE_SIZE);
    memset(m_byExponent, 0x00, EXPONENT_SIZE);
    memset(m_byHash, 0x00, HASH_SIZE);

    TraceMsg("Function: CardManage::EMVGetCAPK Entry");

    // <Group RID="A000000152">
    dwRet = s_cProfile.GetKeyAttributeValue(CONFIG_XML_CAPK_NODE, CAPK_GROUP, CRID, (char *)m_byRID, RID_SIZE);
    if (dwRet != d_OK)
    {
        TraceMsg("GetKeyAttributeValue ret: %08x", dwRet);
        return dwRet;
    }
    // <Item index="5A">
    dwRet = s_cProfile.GetKeyAttributeValue(ITEM, INDEX, (char *)m_byIndex, INDEX_SIZE);
    if (dwRet != d_OK)
    {
        TraceMsg("GetKeyAttributeValue ret: %08x", dwRet);
        return dwRet;
    }

    s_cProfile.GetKeyValue(MODULES, "", (char *)m_byModule);
    s_cProfile.GetKeyValue(EXPONENT, "", (char *)m_byExponent);
    s_cProfile.GetKeyValue(HASH, "", (char *)m_byHash);

    TraceMsg("Function: CardManage::EMVGetCAPK Exit OK");
    return d_EMVCL_NO_ERROR;
}

/**
 * EMVCLGetTagData
 *
 * Function:
 *           Get tag date from xml for EMVCL.
 *
 * @param:
 *           None.
 *
 * @return:
 *           OK - If successful
 *           OtherValue - Otherwise
 **/
DWORD EMVCLManage::EMVCLGetTagData(void)
{
    DWORD dwRet = 0;

    memset(m_byAID, 0x00, AID_SIZE);
    memset(m_byKernelID, 0x00, KERNEL_ID_SIZE);
    memset(m_byTxnType, 0x00, TXN_TYPE_SIZE);
    memset(m_byTagData, 0x00, TLV_DATA_SIZE);

    TraceMsg("Function: CardManage::EMVCLGetTagData Entry");
    dwRet = s_cProfile.GetKeyAttributeValue(CONFIG_XML_TAG_NODE, TERM_GROUP,
                                          DAID, (char *)m_byAID, AID_SIZE,
                                          KERNEL_ID, (char *)m_byKernelID, KERNEL_ID_SIZE,
                                          TXN_TYPE, (char *)m_byTxnType, TXN_TYPE_SIZE);
    if (dwRet != d_OK)
    {
        TraceMsg("GetKeyAttributeValue ret: %08x", dwRet);
        return dwRet;
    }

    dwRet = s_cProfile.GetKeyValue(ITEM, "", (char *)m_byTagData);
    if (dwRet != d_OK)
    {
        TraceMsg("GetKeyValue ret: %08x", dwRet);
        return dwRet;
    }

    TraceMsg("Function: CardManage::EMVCLGetTagData Exit OK");
    return d_EMVCL_NO_ERROR;
}

///* ======================================= Other Functions ================================================= */

//void EMVCLManage::LOGPointer(BYTE * pbyPointer,BYTE byLen, char *pbyStr)
//{
//    if (NULL == pbyPointer || NULL == pbyStr)
//    {
//        TraceMsg("NULL String: Nothing to print\n");
//        return;
//    }
//
//    TraceMsg(" %s ", pbyStr);
//    for(int i=0; i<byLen; i++)
//    {
//        TraceMsg("%02x", *(pbyPointer + i));
//    }
//}

