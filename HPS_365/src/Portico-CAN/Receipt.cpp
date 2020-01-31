/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Receipt.cpp
 * Author: Tim.Ma
 * 
 * Created on 2019.5.20, 10:26am
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "TxnJsonKey.h"
#include "Receipt.h"
#include "dbghlp.h"
#include "./Inc/deftype.h"
#include "./Inc/ByteStream.h"
#include "Ui.h"

extern void getFileData(const char *pszFileName, CByteStream *pcByteStream);


RECEIPT_ADDITIONAL_TAG CReceipt::m_cAdditionalTag = { 0 };

typedef struct
{
    const char*   pszPrintKey;
    const char*   pszJsonKey;
    const char*   pszValue;
    
    bool IsEmpty()
    {
        return 	pszPrintKey==NULL && 
                pszJsonKey==NULL && 
                pszValue==NULL;
    }
}ReceiptAttr;


static ReceiptAttr receiptApprovalAry[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n" },
    { NULL, NULL, "APPROVAL"},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "APPROVAL"},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "CARDHOLDER COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptPartialApprovalAry[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "CARDHOLDER COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptDeclinedAry[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "DECLINED:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "DECLINED" },
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "DECLINED:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "DECLINED" },
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "CARDHOLDER COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptPartialDeclinedAry[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "CARDHOLDER COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


/*
 * French receipt ary
 */
static ReceiptAttr receiptApprovalAry_Fr[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n" },
    { NULL, NULL, "APPROVAL"},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DATE HEURE:", "txnDateTime", NULL},
    { "No de commerçant:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "No de terminal:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "No d'enregistrement #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARTE BANCAIRE:", "txnCardBrand", NULL },
    { "COMPTE:", "txnCardNum", NULL},
    { "NOM APPLI.:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "MODE LECTURE:", "txnCardEntryMode", NULL},
    { "LÉGENDE D'ENTRÉE:", "txnEntryLegend", NULL},
    { "CODE AUTOR.:", "txnAuthCode", NULL},
    { "NO. REF.:", "txnRefNbr", NULL},
    { "REF. TRANSACTION:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITE:", NULL, "1"},
    { "DESCRIPTION:", NULL, "MARCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "J'accepte de payer plus" },
    { NULL, NULL, "que le montant total," },
    { NULL, NULL, "conformément à l'accord" },
    { NULL, NULL, "entre émetteurs de cartes" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "AUTORISATION"},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "PAS RETOURNE" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "TICKET CLIENT" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptPartialApprovalAry_Fr[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "I agree to pay above" },
    { NULL, NULL, "total amount according to" },
    { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DATE HEURE:", "txnDateTime", NULL},
    { "No de commerçant:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "No de terminal:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "No d'enregistrement #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARTE BANCAIRE:", "txnCardBrand", NULL },
    { "COMPTE:", "txnCardNum", NULL},
    { "NOM APPLI.:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "MODE LECTURE:", "txnCardEntryMode", NULL},
    { "LÉGENDE D'ENTRÉE:", "txnEntryLegend", NULL},
    { "CODE AUTOR.:", "txnAuthCode", NULL},
    { "NO. REF.:", "txnRefNbr", NULL},
    { "REF. TRANSACTION:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITE:", NULL, "1"},
    { "DESCRIPTION:", NULL, "MARCHANDISE"},
    { NULL, NULL, "\n"},
    { NULL, NULL, "J'accepte de payer plus" },
    { NULL, NULL, "que le montant total," },
    { NULL, NULL, "conformément à l'accord" },
    { NULL, NULL, "entre émetteurs de cartes" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "PAS RETOURNE" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "TICKET CLIENT" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptDeclinedAry_Fr[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "DECLINED:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "DECLINED" },
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DATE HEURE:", "txnDateTime", NULL},
    { "No de commerçant:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "No de terminal:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "No d'enregistrement #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARTE BANCAIRE:", "txnCardBrand", NULL },
    { "COMPTE:", "txnCardNum", NULL},
    { "NOM APPLI.:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "MODE LECTURE:", "txnCardEntryMode", NULL},
    { "LÉGENDE D'ENTRÉE:", "txnEntryLegend", NULL},
    { "CODE AUTOR.:", "txnAuthCode", NULL},
    { "NO. REF.:", "txnRefNbr", NULL},
    { "REF. TRANSACTION:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITÉ:", NULL, "1"},
    { "DESC:", NULL, "MARCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "J'accepte de payer plus" },
    // { NULL, NULL, "que le montant total," },
    // { NULL, NULL, "conformément à l'accord" },
    // { NULL, NULL, "entre émetteurs de cartes" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "REFUSEE" },
    { NULL, NULL, "\n" },
//    { NULL, NULL, "PAS RETOURNE" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "TICKET CLIENT" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};


static ReceiptAttr receiptPartialDeclinedAry_Fr[] = 
{
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DT:", "txnDateTime", NULL},
    { "MID:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "TERM ID:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "REC #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARD BRAND:", "txnCardBrand", NULL },
    { "ACCT:", "txnCardNum", NULL},
    { "APP NAME:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "ENTRY MODE:", "txnCardEntryMode", NULL},
    { "ENTRY LEGEND:", "txnEntryLegend", NULL},
    { "AUTHCODE:", "txnAuthCode", NULL},
    { "REF NBR:", "txnRefNbr", NULL},
    { "TXN ID:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITY:", NULL, "1"},
    { "DESC:", NULL, "MERCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "I agree to pay above" },
    // { NULL, NULL, "total amount according to" },
    // { NULL, NULL, "card issuer agreement" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "NO RETURNED" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "MERCHANT COPY" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
//    { NULL, NULL, "PAS RETOURNE" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "TICKET COMMERCANT" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    /*********************************/
    { NULL, NULL, "CASTLESPAY RETAIL" },
    { NULL, NULL, "Heartland Way" },
    { NULL, NULL, "Suzhou, JiangSu, IN 215000" },
    { NULL, NULL, "010-66888888" },
    { NULL, NULL, "\n" },
    { "DATE HEURE:", "txnDateTime", NULL},
    { "No de commerçant:", NULL, CReceipt::m_cAdditionalTag.szMerchantID},
    { "No de terminal:", NULL, CReceipt::m_cAdditionalTag.szTerminalID},
    { "No d'enregistrement #:", NULL, CReceipt::m_cAdditionalTag.szInvoiceTag},
    { NULL, NULL, "\n" },
    { NULL, "txnCardType", NULL },
    { NULL, "txnTypeReceipt", NULL },
    { NULL, NULL, "\n" },
    { "CARTE BANCAIRE:", "txnCardBrand", NULL },
    { "COMPTE:", "txnCardNum", NULL},
    { "NOM APPLI.:", "txnAppLabel", NULL},
    { "AID:", "txnEMVAID", NULL},
    { "TVR:", "txnEMVTVR", NULL},
    { "TSI:", "txnEMVTSI", NULL},
    //11-25 add for Interac
    { "RESP CD:", "txnStatusCode", NULL},
    { "ISO RESP CD:", "txnEMVARC", NULL},
    { "BANK RSP CD:", "txnBankRespCode", NULL},
    { "SEQ:", "txnSequenceNbr", NULL},
    { "ACCT TYPE:", "txnAccountType", NULL},
    { "MODE LECTURE:", "txnCardEntryMode", NULL},
    { "LÉGENDE D'ENTRÉE:", "txnEntryLegend", NULL},
    { "CODE AUTOR.:", "txnAuthCode", NULL},
    { "NO. REF.:", "txnRefNbr", NULL},
    { "REF. TRANSACTION:", "txnGatewayTxnID", NULL},
    { NULL, NULL, "\n" },
    { "TOTAL:", "txnAuthAmt", CReceipt::m_cAdditionalTag.szCurrencyTag},
    { NULL, NULL, "\n" },
    { "QUANTITE:", NULL, "1"},
    { "DESCRIPTION:", NULL, "MARCHANDISE"},
    // { NULL, NULL, "\n"},
    // { NULL, NULL, "J'accepte de payer plus" },
    // { NULL, NULL, "que le montant total," },
    // { NULL, NULL, "conformément à l'accord" },
    // { NULL, NULL, "entre émetteurs de cartes" },
    { NULL, NULL, "\n" },
    { NULL, "txnResult", NULL},
    { NULL, NULL, "\n" },
//    { NULL, NULL, "PAS RETOURNE" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "TICKET CLIENT" },
    { NULL, NULL, "\n" },
    { NULL, NULL, "\n" },
    { NULL, NULL, NULL }
};

CReceipt::CReceipt(int nApproval, int nLanguage) 
:
m_usMaxCols(35),
m_pPrimaryKey("Line"),
m_usLineNbr(0),
m_pszJsonString(NULL),
m_nApproval(nApproval),
m_nLanguage(nLanguage)
{
    VecLines_t().swap(m_vecLines);
}


CReceipt::~CReceipt()
{
    if (m_pszJsonString)
    {
        free(m_pszJsonString);
        m_pszJsonString == NULL;
    }
}


CReceipt& CReceipt::AppendLine(const char* value, const char* key)
{
    char szTmpJsonValue[64] = { 0 };
    if (key == NULL)
    {
        ConstructLineText(value, szTmpJsonValue, sizeof(szTmpJsonValue));
    }
    else if (strlen(value) < m_usMaxCols * 3 / 5)
    {
        ConstructLineText(key, value, szTmpJsonValue, sizeof(szTmpJsonValue));
    }
    else 
    {
        char szTmpValue[32] = { 0 };
        memset(szTmpValue, 0, sizeof(szTmpValue));
        memcpy(szTmpValue, value, m_usMaxCols * 3 / 5);
        ConstructLineText(key, szTmpValue, szTmpJsonValue, sizeof(szTmpJsonValue));
        m_vecLines.push_back(szTmpJsonValue);

        memset(szTmpValue, 0, sizeof(szTmpValue));
        memcpy(szTmpValue, (void *)&value[m_usMaxCols * 3 / 5], strlen(value) - m_usMaxCols * 3 / 5);
        ConstructLineText(szTmpValue, szTmpJsonValue, sizeof(szTmpJsonValue));
    }

    m_vecLines.push_back(szTmpJsonValue);
    return *this;
}


char* CReceipt::Generate()
{
    char szJsonTmp[16] = { 0 };
    cJSON* pcJsonRoot = cJSON_CreateObject();
    for(VecLinesItr_t it = m_vecLines.begin(); it != m_vecLines.end(); ++it)
    {   
        sprintf(szJsonTmp, "Line%02d", m_usLineNbr++);
        cJSON_AddStringToObject(pcJsonRoot, szJsonTmp, (*it).c_str());
    }
    memset(szJsonTmp, 0, sizeof(szJsonTmp));
    sprintf(szJsonTmp, "%d", m_vecLines.size());
    cJSON_AddStringToObject(pcJsonRoot, "Line Count", szJsonTmp);
    
    if (m_pszJsonString)
    {
        free(m_pszJsonString);
        m_pszJsonString == NULL;
    }
    
    m_pszJsonString = cJSON_Print(pcJsonRoot);
    
    if (pcJsonRoot)
    {
        cJSON_Delete(pcJsonRoot);
    }
    
    return m_pszJsonString;
}


char* CReceipt::ConstructLineText(const char* pszInText, char* pszOutText, int nOutTextSize)
{
    memset(pszOutText, 0, nOutTextSize);
    if (strcmp(pszInText, "\n") != 0)
    {
        int  nValueLength = strlen(pszInText);
        for (int i=0; i<(m_usMaxCols-nValueLength)/2; ++i)
        {
            pszOutText[i] = ' ';
        }
    }
    strcat(pszOutText, pszInText);
    return pszOutText;
}



char* CReceipt::ConstructLineText(const char* pszInKey, const char* pszInText, char* pszOutText, int nOutTextSize)
{
    memset(pszOutText, 0, nOutTextSize);
    strcat(pszOutText, pszInKey);
    for (int i=strlen(pszInKey); i<m_usMaxCols/3; ++i)
    {
        pszOutText[i] = ' ';
    }
    strcat(pszOutText, pszInText);
    return pszOutText;
}


const char*  CReceipt::PrintReceipt(const cJSON *pcJsonRoot)
{
    cJSON* pcJsonTag = NULL;
    const char *pszReceipt = NULL;

    if (NULL == pcJsonRoot)
        return NULL;
    
    ReceiptAttr *pReceiptAry = NULL;
    if (m_nApproval == 0)//Aproval
    {
        if(m_nLanguage == kEnglish)
        {
            pReceiptAry = &receiptApprovalAry[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptApprovalAry\n");
        }
        else
        {
            pReceiptAry = &receiptApprovalAry_Fr[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptApprovalAry_Fr\n");
        }
    }
    else if (m_nApproval == 1)//Partial Approval
    {
        if(m_nLanguage == kEnglish)
        {
            pReceiptAry = &receiptPartialApprovalAry[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptPartialApprovalAry\n");
        }
        else
        {
            pReceiptAry = &receiptPartialApprovalAry_Fr[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptPartialApprovalAry_Fr\n");
        }
    }
    else if (m_nApproval == 2)//Declined
    {
        if(m_nLanguage == kEnglish)
        {
            pReceiptAry = &receiptDeclinedAry[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptDeclinedAry\n");
        }
        else
        {
            pReceiptAry = &receiptDeclinedAry_Fr[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptDeclinedAry_Fr\n");
        }
    }
    else//Partial Declined
    {
        if(m_nLanguage == kEnglish)
        {
            pReceiptAry = &receiptPartialDeclinedAry[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptPartialDeclinedAry\n");
        }
        else
        {
            pReceiptAry = &receiptPartialDeclinedAry_Fr[0];
            TraceMsg("Info: CReceipt::PrintReceipt: receiptPartialDeclinedAry_Fr\n");
        }
    }
    

    for(; pReceiptAry->IsEmpty()==false; ++pReceiptAry)
    {
        if(pReceiptAry->pszPrintKey == NULL &&
                pReceiptAry->pszJsonKey == NULL &&
                pReceiptAry->pszValue != NULL)
            AppendLine(pReceiptAry->pszValue);
        else if(pReceiptAry->pszPrintKey != NULL &&
                pReceiptAry->pszJsonKey == NULL &&
                pReceiptAry->pszValue != NULL)
            AppendLine(pReceiptAry->pszValue, pReceiptAry->pszPrintKey);
        else if(pReceiptAry->pszJsonKey != NULL)
        {            
            pcJsonTag = cJSON_GetObjectItem(pcJsonRoot, pReceiptAry->pszJsonKey);
            if (pcJsonTag)
            {
                // TraceMsg("Info: CReceipt::PrintReceipt: pszPrintKey:%s, pszJsonKey:%s, pszValue:%s, pcJsonTag->valuestring:%s\n", 
                //         pReceiptAry->pszPrintKey, pReceiptAry->pszJsonKey, pReceiptAry->pszValue?pReceiptAry->pszValue:"", pcJsonTag->valuestring);
                std::string value;
                if (pReceiptAry->pszValue)
                {
                    value.append(pReceiptAry->pszValue);
                }
                value.append(pcJsonTag->valuestring);
                AppendLine(value.c_str(), pReceiptAry->pszPrintKey);
            }
        }
    }
    
    sprintf(CReceipt::m_cAdditionalTag.szInvoiceTag, "%06d", atoi(CReceipt::m_cAdditionalTag.szInvoiceTag) + 1);

    return  Generate();
}


void CReceipt::InitAdditionalTag(const char* pszJsonCfgFile)
{
    CByteStream cFileData;

    cJSON *pcJsonRoot = NULL;
    cJSON *pcJsonTag = NULL;

    getFileData(pszJsonCfgFile, &cFileData);
    do
    {
        pcJsonRoot = cJSON_Parse((char *)cFileData.GetBuffer());
        if (NULL == pcJsonRoot)
            break;
        
        pcJsonTag = cJSON_GetObjectItem(pcJsonRoot, CFG_Terminal_ID);
        if (NULL != pcJsonTag)
        {
            // strcpy(m_cAdditionalTag.szTerminalID, pcJsonTag->valuestring);
            sprintf(m_cAdditionalTag.szTerminalID, "%.*s", sizeof(m_cAdditionalTag.szTerminalID) - 1, pcJsonTag->valuestring);
            if (strlen(m_cAdditionalTag.szTerminalID) > 3)
            {
                for (int i=0; i<strlen(m_cAdditionalTag.szTerminalID) - 3; ++i)
                {
                    m_cAdditionalTag.szTerminalID[i] = '*';
                }
            }
        }
        
        pcJsonTag = cJSON_GetObjectItem(pcJsonRoot, CFG_Merchant_ID);
        if (NULL != pcJsonTag)
        {
            // strcpy(m_cAdditionalTag.szMerchantID, pcJsonTag->valuestring);
            sprintf(m_cAdditionalTag.szMerchantID, "%.*s", sizeof(m_cAdditionalTag.szMerchantID) - 1, pcJsonTag->valuestring);
            if (strlen(m_cAdditionalTag.szMerchantID) > 3)
            {
                for (int i=0; i<strlen(m_cAdditionalTag.szMerchantID) - 3; ++i)
                {
                    m_cAdditionalTag.szMerchantID[i] = '*';
                }
            }
        }
        
        pcJsonTag = cJSON_GetObjectItem(pcJsonRoot, CFG_CurrencyCode);
        if (NULL != pcJsonTag)
        {
            // strcpy(m_cAdditionalTag.szCurrencyTag, pcJsonTag->valuestring);
            sprintf(m_cAdditionalTag.szCurrencyTag, "%.*s", sizeof(m_cAdditionalTag.szCurrencyTag) - 1, pcJsonTag->valuestring);
        }
        
        pcJsonTag = cJSON_GetObjectItem(pcJsonRoot, CFG_TxnInvoiceNbr);
        if (NULL != pcJsonTag)
        {
            // strcpy(m_cAdditionalTag.szInvoiceTag, pcJsonTag->valuestring);
            sprintf(m_cAdditionalTag.szInvoiceTag, "%.*s", sizeof(m_cAdditionalTag.szInvoiceTag) - 1, pcJsonTag->valuestring);
        }
    } while (0);
}