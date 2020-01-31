
/* 
 * Copyright (c) Castles Technology Co., Ltd. 
 * 
 * File:   Profile.h
 * Author: Alan.Ren
 *
 * Created: 2017/06/21
 */

#ifndef _H_H_PROFILE_H_
#define _H_H_PROFILE_H_


#include "deftype.h"
#include "tinyxml.h"
#include "export.h"

#define _IN_
#define _OUT_
#define _IN_OUT_


#define OK                              0
#define ERROR_XML                       1
#define ERROR_INVALID_NODE              2
#define ERROR_INVALID_PARAM             3
#define ERROR_INSUFFICIENT_BUFFERR      4
#define ERROR_INVALID_GROUP             5
#define ERROR_MSG_NO_DATA               6
#define ERROR_INVALID_TAG               7


class PUBLIC_EXPORT CProfile
{
public:
    CProfile();
    virtual ~CProfile();

public:
    TiXmlElement *m_pCurrentEle;
    
    WORD OpenConfigFile(_IN_ PCSTR pszProfilePath,
        _IN_ PCSTR pszXmlNodeName);

    WORD CloseConfigFile();

    WORD GetKeyValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszDefVal,
        _OUT_ char *pszValBuf,
        _IN_ WORD uBufLen);
    
    WORD GetKeyValue( 
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszDefVal,
        _OUT_ char *pszValBuf);
    
    WORD GetKeyValue(_IN_ PCSTR pszDefVal, _OUT_ char *pszValBuf);

    WORD SetKeyValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszKeyValue);
    
    WORD SetNextKeyValue(
        _IN_ PCSTR pszNextKey,
        _IN_ PCSTR pszNextKeyValue);
    
    WORD SetNextValue(_IN_ PCSTR pszNextKeyValue);
    
    
    //========================================================
    // GetKeyAttributeValue 
    WORD GetKeyAttributeValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _OUT_ char *pszValBuf,
        _IN_ USHORT uBufLen);
     
    WORD GetKeyAttributeValue(_IN_ PCSTR pszAppName, _IN_ PCSTR pszKeyName,                            
        _IN_ PCSTR AIDPtr, _OUT_ char *AIDValuePtr, _IN_ USHORT usAIDLen,
        _IN_ PCSTR KernelIDPtr, _OUT_ char *KernelIDValuePtr, _IN_ USHORT usKernelIDLen,
        _IN_ PCSTR TxnTypePtr, _OUT_ char *TxnTypeValuePtr, _IN_ USHORT usTxnTypeLen);
     
    WORD GetKeyAttributeValue(_IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _OUT_ char *pszValBuf,
        _IN_ USHORT uBufLen);
     
    //======================================================== 
     
    WORD FixedGroupAttrValue(_IN_ PCSTR pszAppName,
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR AIDPtr,
        _IN_ char *AIDValuePtr,             
        _IN_ PCSTR KernelIDPtr,
        _IN_ char *KernelIDValuePtr,
        _IN_ PCSTR TxnTypePtr,
        _IN_ char *TxnTypeValuePtr);
     
    WORD SetKeyAttributeValue(_IN_ PCSTR pszKeyName, //<Item index="03">
        _IN_ PCSTR pszAttrName,
        _IN_ PCSTR pszKeyValue);
     
    WORD SetKeyAttributeValue(_IN_ PCSTR pszAppName, //<Group RID="A000000025">
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAttrName,
        _IN_ PCSTR pszKeyValue);
     
    WORD SetKeyAttributeValue(_IN_ PCSTR pszAppName, //<Group AID="A000000333" KernelID="07" TxnType="00">
        _IN_ PCSTR pszKeyName,
        _IN_ PCSTR pszAID,
        _IN_ PCSTR pszAIDValue,
        _IN_ PCSTR pszKernelID,
        _IN_ PCSTR pszKernelIDValue,
        _IN_ PCSTR pszTxnType,
        _IN_ PCSTR pszTxnTypeValue);

private:
    WORD examSetKeyFuncParam(_IN_ PCSTR pszAppName,
    _IN_ PCSTR pszKeyName,
    _IN_ PCSTR pszKeyValue);

    WORD examGetKeyFuncParam(_IN_ PCSTR pszAppName,
    _IN_ PCSTR pszKeyName,
    _IN_ PCSTR pszDefVal,
    _OUT_ char *pszValBuf,
    _IN_ USHORT uBufLen);
    
private:
    TiXmlDocument *m_pXmlDoc;
    TiXmlElement *m_pConfigEle;
    char *m_pszProfilePath;

    BOOL m_bIsInitXmlDoc;
    BOOL m_NextGroup;
};

#endif /* PROFILE_H */

