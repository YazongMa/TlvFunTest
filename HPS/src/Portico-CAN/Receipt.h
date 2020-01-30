/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Receipt.h
 * Author: Tim.Ma
 *
 * Created on 2019.5.20, 10:26am
 */

#ifndef __CASTLESPAY_CRECEIPT_H__
#define __CASTLESPAY_CRECEIPT_H__


#include <iostream>
#include <vector>
#include <string>

#include "cJSON.h"

typedef std::vector<std::string>                VecLines_t;
typedef std::vector<std::string>::iterator      VecLinesItr_t;

typedef struct
{
    char    szCurrencyTag[8];
    char    szMerchantID[16];
    char    szTerminalID[16];
    char    szInvoiceTag[16];
}RECEIPT_ADDITIONAL_TAG;


class CReceipt 
{
public:
    CReceipt(int nApproval, int nLanguage);
    virtual ~CReceipt();
    
    const char* PrintReceipt(const cJSON *pcJsonRoot);
    static void InitAdditionalTag(const char* pszJsonCfgFile);

private:
    CReceipt&   AppendLine(const char* value, const char* key = NULL);
    char*       Generate(void);

    char*       ConstructLineText(const char* pszInText, char* pszOutText, int nOutTextSize);
    char*       ConstructLineText(const char* pszInKey, const char* pszInText, char* pszOutText, int nOutTextSize);

public:
    static RECEIPT_ADDITIONAL_TAG   m_cAdditionalTag;  

private:
    const unsigned short    m_usMaxCols;
    const char*             m_pPrimaryKey;
    
    unsigned short          m_usLineNbr;
    VecLines_t              m_vecLines;
    
    char*                   m_pszJsonString;
    int                     m_nApproval;
    
    int                     m_nLanguage;
};

#endif /* __CASTLESPAY_CRECEIPT_H__ */

