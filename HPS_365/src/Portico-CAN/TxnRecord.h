
/*
 * File Name: TxnRecord.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.04.03
 */

#ifndef _H_H_TXN_RECORD_H_H_
#define _H_H_TXN_RECORD_H_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef IN 
#define IN
#endif

typedef struct 
{
    char szPAN[64];
    long long llTxnAmt;
} TxnRecord;

void ClearTxnRecord(void);

void SaveOneTxnRecord(IN const TxnRecord *pcTxnRecord);

unsigned long long GetTotalTxnAmount(IN const char *pszPAN);

#ifdef __cplusplus
}
#endif

#endif
