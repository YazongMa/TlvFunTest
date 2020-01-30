
/* 
 * File:   TxnCommon.h
 * Author: Tim.Ma
 *
 * Created on 2018/6/21, 11:13 a.m.
 */

#ifndef __HEARTLAND_TXNCOMMON_H__
#define __HEARTLAND_TXNCOMMON_H__

#include "BasicTypeDef.h"

#include <emv_cl.h>


#define d_APP_NAME                                  (char *)"HeartlandAP"
#define d_VERSION_STRING                            (char *)"1.1.0_0001"


#define d_TXN_RSP_CODE_SUCCESS                      "00"
#define d_TXN_RSP_CODE_REVERSAL                     "30"
#define d_TXN_RSP_CODE_NOREPLY                      "91"
#define d_GATEWAY_RSP_CODE_SUCCESS                  "0"
#define d_TXN_INTERAC_INVALID_POS_SEQNBR            "899"


#define HEARTLAND_REVERSAL_FILE_NAME                "heartlandReversal"

// Key, Key Index
#define HEARTLAND_PRIVATE_KEY_SET_1                 0xC001; // Online KeySet
#define HEARTLAND_PRIVATE_KEY_IDX_1                 0x001A; // Online KeyIndex

//language
#define LANGUAGE_INVALID                            0X00
#define LANGUAGE_EN                                 0X01 // English
#define LANGUAGE_FR                                 0X02 // French

// Timeout
#define TIMEOUT_120S                                120000
#define TIMEOUT_60S                                 60000
#define TIMEOUT_1S                                  1000
#define TIMEOUT_10MS                                10


// pos entry mode
#define POS_ENTRY_MODE_MANUAL                       0x01
#define POS_ENTRY_MODE_MSR                          0x02
#define POS_ENTRY_MODE_ICC                          0x05
#define POS_ENTRY_MODE_CTLS_CHIP                    0x07
#define POS_ENTRY_MODE_ICC_MSR                      0x80 // fallback
#define POS_ENTRY_MODE_ICC_MSR_SER_101              0x90 // without techincal fallback, unknown aid
#define POS_ENTRY_MODE_CTLS_MSR                     0x91


/**
 * Transaction Result State
 */
#define d_TXN_DECLINED                              0x00
#define d_TXN_APPROVED_WITH_NO_CVM                  0x01
#define d_TXN_APPROVED_WITH_SIGNATURE               0x02
#define d_TXN_APPROVED_WITH_PIN                     0x03
#define d_TXN_APPROVED_WITH_SIGNATURE_AND_PIN       0x04
#define d_TXN_APPROVED                              0x05
#define d_TXN_SUCCESS                               0x06
#define d_TXN_FAIL                                  0x07
#define d_TXN_APPROVAL_WITH_KERNEL_DECLINE          0x08
#define d_TXN_OFFLINE_APPROVED                      0x09
#define d_TXN_OFFLINE_DECLINED                      0x0A
#define d_TXN_HOST_TIMEOUT                          0x0B
#define d_TXN_SAF_APPROVED                          0x0C
#define d_TXN_SAF_DECLINED                          0x0D
#define d_TXN_INTERAC_RETRY_WITH_POS_SEQNBR         0x0E


#define d_PAYMENT_TXN_TYPE_SALE                     0xD2
#define d_PAYMENT_TXN_TYPE_VOID                     0xD3
#define d_PAYMENT_TXN_TYPE_RETURN                   0xD4
#define d_PAYMENT_TXN_TYPE_SETTLEMENT               0xD5
#define d_PAYMENT_TXN_TYPE_PRE_AUTH                 0xD6
#define d_PAYMENT_TXN_TYPE_AUTH_COMPLETE            0xD7
#define d_PAYMENT_TXN_TYPE_TOP_UP                   0xD8
#define d_PAYMENT_TXN_TYPE_TRANSACTION_INFO         0xD9
#define d_PAYMENT_TXN_TYPE_CANCEL                   0xDA
#define d_PAYMENT_TXN_TYPE_READ_CARD_BIN            0xDB
#define d_PAYMENT_TXN_TYPE_READ_CARD_NUMBER         0xDC


/**
 *  ARQC Type
 **/
#define d_ICC_ARQC                                  0x52
#define d_MSR_ARQC                                  0x53
#define d_CTLS_ARQC                                 0x54
#define d_MSD_ARQC                                  0x55


/**
 *  Card Type
 **/
#define d_CREDIT_CARD_TYPE                          0x00
#define d_DEBIT_CARD_TYPE                           0x01


/**
 *  transaction parameter
 **/
typedef struct
{
    unsigned short          usRequestDataLen;
    unsigned char           ucaRequestData[0x1000];
//    BYTE                    bAmount[6];
//    BYTE                    bDate[3];
//    BYTE                    bTime[3];
//    BYTE                    bTransType;             // refer to kSoapType(DefParamTypeId.h)
    BYTE                    bARQCType;
    BYTE                    bCardType;

    //output transaction result info.
    USHORT                  bResultLen;
    BYTE                    bResult[0x1000];
} HEARTLAND_TRANS_PARAM;


/**
 *  curl parameter
 **/
typedef struct
{
    char                    szHttpMethod[8];
    char                    szCertPath[260];
    char                    szCertFile[260];
    char                    szHostUrl[260];
}HEARTLAND_CURL_PARAM;


/**
 *  emvcl parameter
 **/
typedef struct
{
    int                     m_nDebugFlag;
    EMVCL_INIT_DATA         m_cInitData;
    EMVCL_PARAMETER_DATA    m_cParamData;
}HEARTLAND_EMVCL_PARAM;


/**
 *  emv parameter
 **/
typedef struct
{
    int                     m_nDebugFlag;
    char*                   m_pszConfigFile;
}HEARTLAND_EMV_PARAM;


typedef struct
{
    char                    szARQCTimeout[8];
    char                    szPINEntryTimeout[8];
    char                    szPollCardTimeout[8];
    char                    szAIDSelectTimeout[8];
    char                    szPulloutCardTimeout[16];
    
    char                    szVoltageFlag[8];
    char                    szBBParamFile[260];
    
    char                    szTokenizationFlag[8];

    char                    szTxnUniqueDebiceId[16];
    char                    szTxnDescriptor[32];
    
    char                    szCertName[16];
    char                    szStrategyFlag[8];
    char                    szSAFModeFlag[8];
    char                    szMaskedAcctMode[8];
    char                    szLimitAmount[16];
    
    DWORD                   dwUserActionBitMap;
}HEARTLAND_GENERAL_PARAM;

typedef struct
{
    BYTE                    byPanLength;
    BYTE                    byPanData[32];
    
    WORD                    wPinErrorCode;
    CTOS_KMS2PINGET_PARA    cKms2PinObj;
}HEARTLAND_PINENTRY_PARAM;


/**
 *  polling card cache
 **/
typedef struct
{
    unsigned short          usCardEntryMode;
    unsigned long           ulSequenceNbr;
    unsigned char           ucaCardNumber[0x20];
    unsigned short          usRequestDataLen;
    unsigned char           ucaRequestData[0x1000];
} HEARTLAND_POLLINGCARD_CACHE;

#endif

