
/* 
 * Copyright (c) Castles Technology Co., Ltd. 
 * 
 * File:   operatexml.h
 * Author: 
 *
 * Created: 2018/07/21
 */
#include "operatexml.h"
#include "tinyxml.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_PATH (256)




/////////////////////////////////////////////////////////////////////////////////
// CProfile
// 
// Function:
//          Construct function.
//
// @param:
//          None
//
// @return:
//          None
/////////////////////////////////////////////////////////////////////////////////
CProfile::CProfile() : m_bIsInitXmlDoc(FALSE),
m_pszProfilePath(NULL),
m_pXmlDoc(NULL)
{
    m_pConfigEle = NULL;
    m_NextGroup = false;
}

/////////////////////////////////////////////////////////////////////////////////
// ~CProfile
//
// Function:
//          Destruct function, close the open file.
//
// @param:
//          None
//
// @return:
//          None
/////////////////////////////////////////////////////////////////////////////////
CProfile::~CProfile()
{
    if (m_bIsInitXmlDoc)
    {
        CloseConfigFile();
        m_bIsInitXmlDoc = FALSE;
    }

    m_pszProfilePath = NULL;
    
}



/////////////////////////////////////////////////////////////////////////////////
// OpenConfigFile
//
// Function:
//          Open the configuration file.
//
// @param:
//          None
//
// @return:
//          OK - Successfully open the configuration file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::OpenConfigFile(_IN_ PCSTR pszProfilePath,
        _IN_ PCSTR pszXmlNodeName)
{
    if (NULL == pszProfilePath || NULL == pszXmlNodeName)
    {
//        DEBUG_INFO(0, "Error: Profile path or XML config node name is NULL");
//        return DEBUG_RETCODE("OpenConfigFile", ERROR_XML);
        return ERROR_XML;
    }

    m_pXmlDoc = new TiXmlDocument(pszProfilePath);
    if (NULL == m_pXmlDoc)
    {
//        DEBUG_INFO(0, "Error: New TiXmlDocument object failed.");
//        return DEBUG_RETCODE("OpenConfigFile", ERROR_XML);
        return ERROR_XML;
    }

    bool bLoadXml = m_pXmlDoc->LoadFile();
    if (!bLoadXml)
    {
//        DEBUG_INFO(0, "Error: XML file load failed, error info: %s", m_pXmlDoc->ErrorDesc());
//        return DEBUG_RETCODE("OpenConfigFile", ERROR_XML);
        return ERROR_XML;
    }
//    m_pXmlDoc->Print();
    
    TiXmlElement *pRootEle = NULL;

    pRootEle = m_pXmlDoc->RootElement(); //CLConfig
    if (NULL == pRootEle)
    {
        return ERROR_INVALID_NODE;
    }
    m_pConfigEle = pRootEle->FirstChildElement(pszXmlNodeName);//CLConfig
    if (NULL == m_pConfigEle)
    {
        return ERROR_INVALID_NODE;
    }
    
    m_bIsInitXmlDoc = TRUE;
    return OK;
}



/////////////////////////////////////////////////////////////////////////////////
// CloseConfigFile
//
// Function:
//          Close the configuration file.
//
// @param:
//          None
//
// @return:
//          OK - Successfully close the configuration file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::CloseConfigFile()
{
    if (!m_bIsInitXmlDoc)
    {
        return OK;
    }
//    m_pXmlDoc->Print();
    m_pXmlDoc->Clear();
    
    delete m_pXmlDoc;
    m_pXmlDoc = NULL;

    m_bIsInitXmlDoc = FALSE;
    return OK;
}



/////////////////////////////////////////////////////////////////////////////////
// GetKeyValue
//
// Function:
//          Read data from the specified node from the XML file.
//
// @param:
//          [IN]  pszAppName, The parent node name.
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszDefVal,  If the pszKeyName node not exist, copy
//                the default string to the pszValBuf.
//          [OUT] pszValBuf, Pointer to the buffer that receives the 
//                the retrieved string.
//          [IN]  Specifies the size of the pszValBuf.
// @return:
//          OK - Successfully get data from XML file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::GetKeyValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszDefVal,
        _OUT_ char *pszValBuf,
        _IN_ WORD uBufLen)
{
//    DEBUG_ENTRY();
    
    WORD wRet;
    wRet = examGetKeyFuncParam(pszAppName, pszKeyName, pszDefVal,
            pszValBuf, uBufLen);
    if (OK != wRet)
    {
//        DEBUG_INFO(0, "Error: Examine GetKeyValue function parameter failed.");
//        return DEBUG_RETCODE("GetKeyValue", wRet);
        return wRet;
    }

    strcpy(pszValBuf, pszDefVal);
    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    
    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
//        return DEBUG_RETCODE("GetKeyValue", ERROR_XML);
        return ERROR_XML;
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);
    if (NULL != pKeyEle && NULL != pKeyEle->GetText())
    {
        if ((strlen(pKeyEle->GetText()) + 1) <= uBufLen)
        {
//            DEBUG_INFO(0, "The size of key %ld, bufsize:%ld", pKeyEle->GetText(), uBufLen);
            strcpy(pszValBuf, pKeyEle->GetText());
        }
        else
        {
//            DEBUG_INFO(0, "Error: The fourth argument size too small");
//            return DEBUG_RETCODE("GetKeyValue", ERROR_INSUFFICIENT_BUFFERR);
        }   
    }
    else
        return  FALSE;
//       return DEBUG_RETCODE("GetKeyValue", false);
    
//    return DEBUG_EXIT(OK);
    return OK;
}

WORD CProfile::GetKeyValue(     
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszDefVal,
        _OUT_ char *pszValBuf)
 {
//    DEBUG_ENTRY();

    if (NULL == pszKeyName)
//        return DEBUG_RETCODE("GetKeyValue", ERROR_XML);

        strcpy(pszValBuf, pszDefVal);
    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;

    pAppNameEle = m_pCurrentEle; //Group or Item

    if (NULL == pAppNameEle) {
//        DEBUG_INFO(0, "Error: the last node not exist.");
//        return DEBUG_RETCODE("GetKeyValue", ERROR_XML);
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);

    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);
    if (NULL != pKeyEle && NULL != pKeyEle->GetText()) {
//        DEBUG_INFO(0, "key value%s", pKeyEle->GetText());
        strcpy(pszValBuf, pKeyEle->GetText());
    } else
        return FALSE;
//       return DEBUG_RETCODE("GetKeyValue", false);

//    return DEBUG_EXIT(OK);
    return OK;
}


WORD CProfile::GetKeyValue(_IN_ PCSTR pszDefVal, _OUT_ char *pszValBuf)
{
//    DEBUG_ENTRY();
    
    
    strcpy(pszValBuf, pszDefVal);
    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    
    pAppNameEle = m_pCurrentEle; //Group or Item
    
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: the last node not exist.");
//        return DEBUG_RETCODE("GetKeyValue", ERROR_XML);
        return ERROR_XML;
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);

//    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);
    pKeyEle = pAppNameEle;
    if (NULL != pKeyEle && NULL != pKeyEle->GetText())
    {
//        DEBUG_INFO(0, "key value%s", pKeyEle->GetText());
        strcpy(pszValBuf, pKeyEle->GetText());
    }
    else 
	    return FALSE;
//       return DEBUG_RETCODE("GetKeyValue", false);
        
//    return DEBUG_EXIT(OK);
    return OK;
}


/////////////////////////////////////////////////////////////////////////////////
// GetKeyAttributeValue
//
// Function:
//          Read data from the specified node attribute from the XML file.
//
// @param:
//          [IN]  pszAppName, The parent node name.
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszAttrName, The need read attribute name.
//          [OUT] pszValBuf, Pointer to the buffer that receives the 
//                the retrieved string.
//          [IN]  Specifies the size of the pszValBuf.
// @return:
//          OK - Successfully get data from XML file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::GetKeyAttributeValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _OUT_ char *pszValBuf,
        _IN_ USHORT uBufLen)
{
    if (!m_bIsInitXmlDoc)
    {
        printf("Error: XML config file is not open.");
        return (ERROR_XML);
    }

    if (pszAppName == NULL || pszKeyName == NULL ||
            pszAttrName == NULL || uBufLen == 0)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_INVALID_PARAM);
        return (ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;

    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
        return (ERROR_XML);
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);//Group
    
    static int Count = 0;
    if (m_NextGroup)
        Count++;
    for (int i=0; i < Count; i++)
    {
        pKeyEle = pKeyEle->NextSiblingElement();
    }
    
   if (NULL == pKeyEle)
    {
        Count = 0;
        m_NextGroup = FALSE;
//        DEBUG_INFO(0, "Error: %s node not exist.", pszKeyName);
        return (ERROR_INVALID_GROUP);
    }
    
    if ((NULL != pKeyEle->Attribute(pszAttrName)))
    {
        if ((strlen(pKeyEle->Attribute(pszAttrName)) + 1) <= uBufLen)
        {
            memcpy(pszValBuf, pKeyEle->Attribute(pszAttrName),
                    strlen(pKeyEle->Attribute(pszAttrName)));
        }
        else
        {
//            DEBUG_INFO(0, "Error: The fourth argument size too small");
            return (ERROR_INSUFFICIENT_BUFFERR);
        }
    }
    else
    {
//        DEBUG_INFO(0, "Error: %s attribute not exist.", pszAttrName);
        return (ERROR_XML);
    }
    m_NextGroup = TRUE;
    m_pCurrentEle = pKeyEle;
    return OK;
}

WORD CProfile::GetKeyAttributeValue(_IN_ PCSTR pszAppName, _IN_ PCSTR pszKeyName,                            
        _IN_ PCSTR AIDPtr,
        _OUT_ char *AIDValuePtr,
        _IN_ USHORT usAIDLen,
        _IN_ PCSTR KernelIDPtr,
        _OUT_ char *KernelIDValuePtr,
        _IN_ USHORT usKernelIDLen,
        _IN_ PCSTR TxnTypePtr,
        _OUT_ char *TxnTypeValuePtr,
        _IN_ USHORT usTxnTypeLen)
{
//    DEBUG_ENTRY();
    
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
        return ERROR_XML;
    }

    if (pszAppName == NULL || pszKeyName == NULL || AIDPtr == NULL ||
            KernelIDPtr == NULL || TxnTypePtr == NULL)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
        return (ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;

    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
        return (ERROR_XML);
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);//Group
    
    static int Count = 0;
    if (m_NextGroup)
        Count++;
    for (int i=0; i < Count; i++)
    {
        pKeyEle = pKeyEle->NextSiblingElement();
    }
    
   if (NULL == pKeyEle)
    {
        Count = 0;
        m_NextGroup = FALSE;
//        DEBUG_INFO(0, "Error: %s node not exist.", pszKeyName);
        return (ERROR_INVALID_GROUP);
    }
    
    if ((NULL != pKeyEle->Attribute(AIDPtr)))
    {
        if ((strlen(pKeyEle->Attribute(AIDPtr)) + 1) <= usAIDLen)
        {
            memcpy(AIDValuePtr, pKeyEle->Attribute(AIDPtr),
                    strlen(pKeyEle->Attribute(AIDPtr)));
        
            if ((strlen(pKeyEle->Attribute(KernelIDPtr)) + 1) <= usKernelIDLen)
            {
                memcpy(KernelIDValuePtr, pKeyEle->Attribute(KernelIDPtr),
                        strlen(pKeyEle->Attribute(KernelIDPtr)));
            
                if ((strlen(pKeyEle->Attribute(TxnTypePtr)) + 1) <= usTxnTypeLen)
                {
                    memcpy(TxnTypeValuePtr, pKeyEle->Attribute(TxnTypePtr),
                            strlen(pKeyEle->Attribute(TxnTypePtr)));
                }
                else
                {
//                    DEBUG_INFO(0, "Error: The fourth argument size too small");
                    return (ERROR_INSUFFICIENT_BUFFERR);
                }
            }
            else
            {
//                DEBUG_INFO(0, "Error: The fourth argument size too small");
                return (ERROR_INSUFFICIENT_BUFFERR);
            }
        }
        else
        {
//            DEBUG_INFO(0, "Error: The fourth argument size too small");
            return (ERROR_INSUFFICIENT_BUFFERR);
        }
    }
    else
    {
//        DEBUG_INFO(0, "Error: %s attribute not exist.", AIDPtr);
        return (ERROR_XML);
    }
    m_NextGroup = true;
    m_pCurrentEle = pKeyEle;
    return OK;
    
    
//    return DEBUG_EXIT(OK);
}

WORD CProfile::FixedGroupAttrValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,                            
        _IN_ PCSTR AIDPtr,
        _IN_ char *AIDValuePtr,              
        _IN_ PCSTR KernelIDPtr,
        _IN_ char *KernelIDValuePtr,
        _IN_ PCSTR TxnTypePtr,
        _IN_ char *TxnTypeValuePtr)
{
//    DEBUG_ENTRY();
    
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }

    if (pszAppName == NULL || pszKeyName == NULL || AIDPtr == NULL ||
            KernelIDPtr == NULL || TxnTypePtr == NULL)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    m_pCurrentEle = NULL;
    
    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName); //Group
    
    for (; NULL != pKeyEle; pKeyEle = pKeyEle->NextSiblingElement())
    {
        if (0 == strcmp(AIDValuePtr, pKeyEle->Attribute(AIDPtr)))
        {
            if (0 == strcmp(KernelIDValuePtr, pKeyEle->Attribute(KernelIDPtr)))
                if (0 == strcmp(TxnTypeValuePtr, pKeyEle->Attribute(TxnTypePtr)))
                {
//                    DEBUG_INFO(0, "Find the Group");
                    break;
                }
        }   
    }
    if (NULL == pKeyEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszKeyName);
//        return DEBUG_RETCODE("GetKeyAttributeValue", MSG_NO_DATA);
        return ERROR_MSG_NO_DATA;
    }
            
    m_pCurrentEle = pKeyEle;
//    return DEBUG_EXIT(OK);
    return OK;
}

WORD CProfile::GetKeyAttributeValue(_IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _OUT_ char *pszValBuf,
        _IN_ USHORT uBufLen)
{
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
        return (ERROR_XML);
    }

    if (pszKeyName == NULL || pszAttrName == NULL || uBufLen == 0)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
        return (ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    m_NextGroup = false;
            
    pAppNameEle = m_pCurrentEle;
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pAppNameEle);
        return (ERROR_XML);
    }
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);
    
    static int KeyCount = 0;
    for (int i=0;i<KeyCount;i++ )
    {
        pKeyEle = pKeyEle->NextSiblingElement();
    }
    KeyCount ++;
    if (NULL == pKeyEle)
    {
        KeyCount = 0;
        m_NextGroup = true;
//        DEBUG_INFO(0, "Error: %s node not exist.", pszKeyName);
        return (ERROR_INVALID_NODE);
    }

    if ((NULL != pKeyEle->Attribute(pszAttrName)))
    {
        if ((strlen(pKeyEle->Attribute(pszAttrName))) <= uBufLen)
        {
            memcpy(pszValBuf, pKeyEle->Attribute(pszAttrName),
                    strlen(pKeyEle->Attribute(pszAttrName)));
        }
        else
        {
//            DEBUG_INFO(0, "Error: The fourth argument size too small");
            return (ERROR_INSUFFICIENT_BUFFERR);
        }
    }
    else
    {
//        DEBUG_INFO(0, "Error: %s attribute not exist.", pszAttrName);
        return (ERROR_XML);
    }
    m_pCurrentEle = pKeyEle;
    return OK;
}

/////////////////////////////////////////////////////////////////////////////////
// SetKeyValue
//
// Function:
//          Write data to the XML file.
//
// @param:
//          [IN]  pszAppName, The parent node name.
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszKeyValue, Pointer to a string to be written to 
//                the XML file.
//
// @return:
//           OK - Successfully write date to the XML file.
//           Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetKeyValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszKeyValue)
{
    BOOL bSaveRet;
    WORD wRet;

    wRet = examSetKeyFuncParam(pszAppName, pszKeyName, pszKeyValue);
    if (OK != wRet)
    {
//        return DEBUG_RETCODE("SetKeyValue", wRet);
        return ERROR_XML;
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;

    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //Group or Item
//    DEBUG_INFO(0, "pAppNameEle%08x", pAppNameEle);
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: App Name not exist.");
//        return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        return ERROR_XML;
    }

    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName);
    if (NULL == pKeyEle)
    {
        pKeyEle = new TiXmlElement(pszKeyName);
        if (NULL == pKeyEle)
        {
//            DEBUG_INFO(0, "Error: New TiXmlElement failed.");
//            return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
            return ERROR_XML;
        }
        pAppNameEle->LinkEndChild(pKeyEle);
    }

    TiXmlText *pEleText = new TiXmlText(pszKeyValue);
    if (NULL == pEleText)
    {
//        DEBUG_INFO(0, "Error: New TiXmlText object failed.");
//        return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        return ERROR_XML;
    }

    pKeyEle->Clear();
    pKeyEle->LinkEndChild(pEleText);
    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument Save File failed, error %s",
//                m_pXmlDoc->ErrorDesc());
//        return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        return ERROR_XML;
    }
    return OK;
}

/////////////////////////////////////////////////////////////////////////////////
// SetNextKeyValue
//
// Function:
//          Creat a lower level node and write node data.
//
// @param:
//          [IN]  pszNextKey, The sub node name.
//          [IN]  pszNextKeyValue, Pointer to a string of pszNextKey.
//
// @return:
//           OK - Successfully write date to the XML file.
//           Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetNextKeyValue(_IN_ PCSTR pszNextKey, _IN_ PCSTR pszNextKeyValue)
{
//    DEBUG_ENTRY();
    
    BOOL bSaveRet;
    TiXmlElement *pKeyEle = NULL;

    if (NULL == pszNextKey || NULL == pszNextKeyValue)
        return ERROR_XML;
//        return DEBUG_RETCODE("Invaild parameter", ERROR_XML);
    
//    DEBUG_INFO(0, "Current Ele %06x", m_pCurrentEle);
    
    pKeyEle = m_pCurrentEle->FirstChildElement(pszNextKey);
    if (NULL == pKeyEle)
    {
        pKeyEle = new TiXmlElement(pszNextKey);
        if (NULL == pKeyEle)
        {
//            DEBUG_INFO(0, "Error: Creat Node failed.");
//            return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
            return ERROR_XML;
        }
        m_pCurrentEle->LinkEndChild(pKeyEle);
    }
    
    TiXmlText *pEleText = new TiXmlText(pszNextKeyValue);
    if (NULL == pEleText)
    {
//        DEBUG_INFO(0, "Error: New TiXmlText object failed.");
//        return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        return ERROR_XML;
    }

    pKeyEle->Clear();
    pKeyEle->LinkEndChild(pEleText);   
    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument File failed, error %s",m_pXmlDoc->ErrorDesc());
        return ERROR_XML;
    }
    return (OK);
}

/////////////////////////////////////////////////////////////////////////////////
// SetNextValue
//
// Function:
//          Write data to the current node.
//
// @param:
//          [IN]  pszNextKeyValue, Pointer to a string to be written to 
//                the current node.
//
// @return:
//           OK - Successfully write date to the XML file.
//           Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetNextValue(_IN_ PCSTR pszNextKeyValue) {
//    DEBUG_ENTRY();

    BOOL bSaveRet;
    TiXmlElement *pKeyEle = NULL;

    if (NULL == pszNextKeyValue)
    {
        return ERROR_XML;
    }
//    DEBUG_INFO(0, "CurrentEle %06x", m_pCurrentEle);
    
    pKeyEle = m_pCurrentEle;

    TiXmlText *pEleText = new TiXmlText(pszNextKeyValue);
    if (NULL == pEleText)
    {
//        DEBUG_INFO(0, "Error: New TiXmlText object failed.");
//        return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
    }

    pKeyEle->Clear();
    pKeyEle->LinkEndChild(pEleText);
    
    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument File failed, error %s",
//                m_pXmlDoc->ErrorDesc());
        return  ERROR_XML;
    }
    return (OK);
}

/////////////////////////////////////////////////////////////////////////////////
// SetKeyAttributeValue
//
// Function:
//          set specified node attribute for the XML file.
//
// @param:        
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszAttrName, The need set attribute name.
//          [IN]  pszKeyValue, Pointer to the value of pszAttrName
// @return:
//          OK - Successfully set data for XML file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetKeyAttributeValue(_IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _IN_ PCSTR pszKeyValue)
{
//    DEBUG_ENTRY();
    
    BOOL bFlag = false;
    BOOL bSaveRet;
    
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }

    if (pszKeyName == NULL || pszAttrName == NULL)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_INVALID_PARAM);
    }
    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    
//    DEBUG_INFO(0, "pCurrentEle %012x", m_pCurrentEle);
    pAppNameEle = m_pCurrentEle;  //Group
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName); //Item
    
    for (; NULL != pKeyEle; pKeyEle = pKeyEle->NextSiblingElement())
    {
        if (0 == strcmp(pszKeyValue, pKeyEle->Attribute(pszAttrName)))
        {
//            DEBUG_INFO(0, "Find same group");
            bFlag = true;
            break;
        }   
    }
    
    if (!bFlag)
    {
        pKeyEle = new TiXmlElement(pszKeyName);
        if (NULL == pKeyEle)
        {
//            DEBUG_INFO(0, "Error: New TiXmlElement failed.");
//            return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        }
        pAppNameEle->LinkEndChild(pKeyEle);
        pKeyEle->SetAttribute(pszAttrName, pszKeyValue);
    }
    
    m_pCurrentEle = pKeyEle;
    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument Save File failed, error %s",
//                m_pXmlDoc->ErrorDesc());
        return  ERROR_XML;
    }
    
    return (OK);
}

/////////////////////////////////////////////////////////////////////////////////
// SetKeyAttributeValue
//
// Function:
//          set specified node attribute for the XML file.
//
// @param:     
//          [IN]  pszAppName, The parent node name.
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszAttrName, The need set attribute name.
//          [IN]  pszKeyValue, Pointer to the value of pszAttrName
// @return:
//          OK - Successfully set data for XML file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetKeyAttributeValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _IN_ PCSTR pszKeyValue)
{
//    DEBUG_ENTRY();
    
    BOOL bFlag = false;
    BOOL bSaveRet;
    
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }

    if (pszAppName == NULL || pszKeyName == NULL ||
            pszAttrName == NULL)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    m_pCurrentEle = NULL;
    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }
//    DEBUG_INFO(0, "pAppNameEle %08x", pAppNameEle);
//    DEBUG_INFO(0, "pCurrentEle %08x", m_pCurrentEle);
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName); //Group
    
    for (; NULL != pKeyEle; pKeyEle = pKeyEle->NextSiblingElement())
    {
        if (0 == strcmp(pszKeyValue, pKeyEle->Attribute(pszAttrName)))
        {
//            DEBUG_INFO(0, "Find same group");
            bFlag = true;
            break;
        }   
    }

    if (!bFlag)
    {
        pKeyEle = new TiXmlElement(pszKeyName);
        if (NULL == pKeyEle)
        {
//            DEBUG_INFO(0, "Error: New TiXmlElement failed.");
//            return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        }
        pAppNameEle->LinkEndChild(pKeyEle);
        (*pKeyEle).SetAttribute(pszAttrName, pszKeyValue);
    }
    
    //pKeyEle->Clear();
    m_pCurrentEle = pKeyEle;
    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument Save File failed, error %s",
//                m_pXmlDoc->ErrorDesc());
        return ERROR_XML;
    }
    
    return (OK);
}

/////////////////////////////////////////////////////////////////////////////////
// SetKeyAttributeValue
//
// Function:
//          set specified node attribute for the XML file.
//
// @param:     
//          [IN]  pszAppName, The parent node name.
//          [IN]  pszKeyName, The sub node name.
//          [IN]  pszAID, The need set attribute name.
//          [IN]  pszAIDValue, Pointer to the value of pszAID
//          [IN]  pszKernelID, The need set attribute name.
//          [IN]  pszKernelIDValue, Pointer to the value of pszKernelID
//          [IN]  pszTxnType, The need set attribute name.
//          [IN]  pszTxnTypeValue, Pointer to the value of pszTxnType
// @return:
//          OK - Successfully set data for XML file.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::SetKeyAttributeValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,                   
        _IN_ PCSTR pszAID,
        _IN_ PCSTR pszAIDValue,
        _IN_ PCSTR pszKernelID,
        _IN_ PCSTR pszKernelIDValue,
        _IN_ PCSTR pszTxnType,
        _IN_ PCSTR pszTxnTypeValue)
{
//    DEBUG_ENTRY();
    
    BOOL bFlag = false;
    BOOL bSaveRet;
    
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }

    if (pszAppName == NULL || pszKeyName == NULL 
        || pszAID == NULL || pszAIDValue == NULL 
        || pszKernelID == NULL || pszKernelIDValue ==NULL 
        || pszTxnType == NULL || pszTxnTypeValue == NULL)
    {
//        DEBUG_INFO(0, "Error: Invalid parameter.");
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_INVALID_PARAM);
    }

    TiXmlElement *pAppNameEle = NULL;
    TiXmlElement *pKeyEle = NULL;
    m_pCurrentEle = NULL;
    pAppNameEle = m_pConfigEle->FirstChildElement(pszAppName); //CAPK
    if (NULL == pAppNameEle)
    {
//        DEBUG_INFO(0, "Error: %s node not exist.", pszAppName);
//        return DEBUG_RETCODE("GetKeyAttributeValue", ERROR_XML);
    }
    
    pKeyEle = pAppNameEle->FirstChildElement(pszKeyName); //Group
    
    for (; NULL != pKeyEle; pKeyEle = pKeyEle->NextSiblingElement())
    {
        if (0 == strcmp(pszAIDValue, pKeyEle->Attribute(pszAID)))
        {
            if (0 == strcmp(pszKernelIDValue, pKeyEle->Attribute(pszKernelID)))
                if (0 == strcmp(pszTxnTypeValue, pKeyEle->Attribute(pszTxnType)))
                {
//                    DEBUG_INFO(0, "Find same group");
                    bFlag = true;
                    break;
                }
        }   
    }

    if (!bFlag)
    {
        pKeyEle = new TiXmlElement(pszKeyName);
        if (NULL == pKeyEle)
        {
//            DEBUG_INFO(0, "Error: New TiXmlElement failed.");
//            return DEBUG_RETCODE("SetKeyValue", ERROR_XML);
        }
        pAppNameEle->LinkEndChild(pKeyEle);
        
        pKeyEle->SetAttribute(pszAID, pszAIDValue);
        pKeyEle->SetAttribute(pszKernelID, pszKernelIDValue);
        pKeyEle->SetAttribute(pszTxnType, pszTxnTypeValue);
    }

    bSaveRet = m_pXmlDoc->SaveFile();
    if (!bSaveRet)
    {
//        DEBUG_INFO(0, "Error: Save TinyXmlDocument Save File failed, error %s",
//                m_pXmlDoc->ErrorDesc());
        return  ERROR_XML;
    }
    m_pCurrentEle = pKeyEle;
    return (OK);
}

/////////////////////////////////////////////////////////////////////////////////
// examSetKeyFuncParam
//
// Function:
//          Check whether the SetKeyValue function parameter is valid.
//
// @param:
//          Consistent with SetKeyValue function.
//
// @return:
//          OK - The parameter is valid.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::examSetKeyFuncParam(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszKeyValue)
{
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("GetKeyValue", ERROR_XML);
    }

    if (pszAppName == NULL || pszKeyName == NULL || pszKeyValue == NULL)
    {
//        DEBUG_INFO(0, "Error: Function parameter invalid.");
//        return DEBUG_RETCODE("examSetKeyFuncParam", ERROR_INVALID_PARAM);
    }
    return OK;
}



/////////////////////////////////////////////////////////////////////////////////
// examGetKeyFuncParam
//
// Function:
//          Check whether the GetKeyValue function parameter is valid.
//
// @param:
//          Consistent with GetKeyValue function.
//
// @return:
//          OK - The parameter is valid.
//          Other values - Please refer to the ErrorCode.h file.
/////////////////////////////////////////////////////////////////////////////////
WORD CProfile::examGetKeyFuncParam(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszDefVal,
        _OUT_ char *pszValBuf,
        _IN_ USHORT uBufLen)
{
    if (!m_bIsInitXmlDoc)
    {
//        DEBUG_INFO(0, "Error: XML config file is not open.");
//        return DEBUG_RETCODE("examGetKeyFuncParam", ERROR_XML);
        return ERROR_XML;
    }

    if (NULL == pszAppName || NULL == pszKeyName ||
            NULL == pszDefVal || NULL == pszValBuf ||
            uBufLen == 0)
    {
//        DEBUG_INFO(0, "Error: Function parameter invalid.");
//        return DEBUG_RETCODE("examGetKeyFuncParam", ERROR_INVALID_PARAM);
        return ERROR_INVALID_PARAM;
    }

    pszValBuf[0] = '\0';
    if ((strlen(pszDefVal) + 1) >= uBufLen)
    {
//        DEBUG_INFO(0, "Error: The fourth argument size too small");
//        return DEBUG_RETCODE("examGetKeyFuncParam", ERROR_INSUFFICIENT_BUFFERR);
        return ERROR_INSUFFICIENT_BUFFERR;
    }

    return OK;
}