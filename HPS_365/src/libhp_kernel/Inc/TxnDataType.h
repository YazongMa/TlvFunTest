
/*
 * File Name: TxnDataType.h 
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.13
 */

#ifndef _H_H_TXNDATATYPE_H_H_
#define _H_H_TXNDATATYPE_H_H_

#define Type_Txn_Cash                         0x01
#define Type_Txn_Goods                        0x00
#define Type_Txn_Service                      0x00
#define Type_Txn_CaseBack                     0x09
#define Type_Txn_Inquiry                      0x30
#define Type_Txn_Transfer                     0x40
#define Type_Txn_Payment                      0x50
#define Type_Txn_Admin                        0x95
#define Type_Txn_Cashdesposit                 0x21

typedef struct _AmountData
{
    // Transaction type
    BYTE byTxnType;

    // Pack soap type
    int nPackSoapType;

    // Amount
    unsigned long long ullAmount;

    // Other Amount
    unsigned long long ullOtherAmount;

    // txnSeqeueceId
    unsigned long ulSeqeueceId;

    // txnGatewayTxnId
    char szGatewayIdBuf[32];

    // txnTokenValue
    char szTokenValueBuf[64];

    // txnRequestData
    int nTxnReqDataLength;

    char szTxnRequestData[3072];

    // szTxnSafSequenceID
    char szTxnSafSequenceID[32];

    // byOnlineFlag
    BYTE byOnlineFlag;

    // Partial auth flag
    BYTE byPartialAuthFlag;

    _AmountData()
    {
        nPackSoapType = -1;
        byTxnType = -1;
        ullAmount = 0;
        ullOtherAmount = 0;
        ulSeqeueceId = 0x00;
		nTxnReqDataLength = 0x00;
        byOnlineFlag = -1;
		byPartialAuthFlag = 0x00;
        bzero(szGatewayIdBuf, sizeof(szGatewayIdBuf));
        bzero(szTokenValueBuf, sizeof(szTokenValueBuf));
        bzero(szTxnRequestData, sizeof(szTxnRequestData));
        bzero(szTxnSafSequenceID, sizeof(szTxnSafSequenceID));
    }
} HEARTLAND_AMOUNT_PARAM;

#endif