#ifndef __EMV_TAG_H__
#define __EMV_TAG_H__

#ifdef  __cplusplus
extern "C"
{
#endif

    // standard EMV tag
#define TAG_ADF                                         (char *)"4F"   // application label
#define TAG_APP_LABEL                                   (char *)"50"   // application label
#define TAG_TK1                                         (char *)"56"   // tk1 equivelent data, without SS/ES/LRC
#define TAG_TK2                                         (char *)"57"   // tk2 equivelent data, without SS/ES/LRC
#define TAG_PAN                                         (char *)"5A"   // PAN data
#define TAG_CARDHOLDER_NAME                             (char *)"5F20"   //cardholder name
#define TAG_EXPIRE_DATE                                 (char *)"5F24"   //expiration date
#define TAG_EFFECT_DATE                                 (char *)"5F25"   //effect date
#define TAG_ISSUER_COUNTRY_CODE                         (char *)"5F28"   //issuer country code
#define TAG_TXN_COUNTRY_CODE                            (char *)"5F2A"   //transaction country code
#define TAG_LANG_PREFERENCE                             (char *)"5F2D"   //transaction country code
#define TAG_SERVICE_CODE                                (char *)"5F30"   //service code
#define TAG_PAN_SEQ_NUM                                 (char *)"5F34"   //PAN sequence num
#define TAG_ISSUER_SCRIPT_1                             (char *)"71"   // before 2nd gen AC
#define TAG_ISSUER_SCRIPT_2                             (char *)"72"   //after 2nd gen AC
#define TAG_AMT_AUTH_BIN                                (char *)"81"   //auth amount in binary format
#define TAG_AIP                                         (char *)"82"   //AIP
#define TAG_FD_NAME                                     (char *)"84"   //FD NAME
#define TAG_CDOL2                                       (char *)"8D"   //CDOL2 for 2nd gen ac
#define TAG_ARC                                         (char *)"8A"   // ARC or ARPC
#define TAG_ISSUER_AUTH_DATA                            (char *)"91"   // issuer authentication data
#define TAG_TVR                                         (char *)"95"   // TVR
#define TAG_TXN_DATE                                    (char *)"9A"   // txn date
#define TAG_TSI                                         (char *)"9B"   // TSI
#define TAG_TXN_TYPE                                    (char *)"9C"   // txn type, 00 = sale, 20 = refund...etc
#define TAG_ACQUIRE_ID                                  (char *)"9F01"   // acquirer identifier
#define TAG_AMT_AUTH                                    (char *)"9F02"   // amout authorized, excluding adjustment
#define TAG_AMT_OTHER                                   (char *)"9F03"   // amout other
#define TAG_APP_ID                                      (char *)"9F06"   // application ID
#define TAG_APP_USAGE_CTRL                              (char *)"9F07"   // application usage control
#define TAG_APP_VER                                     (char *)"9F09"   // application version
#define TAG_ISSUER_ACTION_CODE_DEFAULT                  (char *)"9F0D"   // issuer action code
#define TAG_ISSUER_ACTION_CODE_DENAIL                   (char *)"9F0E"   //
#define TAG_ISSUER_ACTION_CODE_ONLINE                   (char *)"9F0F"   //
#define TAG_ISSUER_APP_DATA                             (char *)"9F10"   // issuer application data
#define TAG_ISSUER_CODE_IDX                             (char *)"9F11"   // issuer code table index
#define TAG_APP_PREFER_NAME                             (char *)"9F12"   // application prefer name
#define TAG_TM_COUNTRY_CODE                             (char *)"9F1A"   // terminal country code
#define TAG_TID                                         (char *)"9F1C"   // terminal ID
#define TAG_PP_TM_RISK_MGM                              (char *)"9F1D"   // paypass terminal risk management
#define TAG_IFD_SN                                      (char *)"9F1E"   // IFD SN
#define TAG_TXN_TIME                                    (char *)"9F21"   // txn time
#define TAG_AC                                          (char *)"9F26"   // application cryptogram
#define TAG_CID                                         (char *)"9F27"   // CID
#define TAG_KERNEL_IDENTIFIER                           (char *)"9F2A"   // Kernel Identifier
#define TAG_TM_CAP                                      (char *)"9F33"   // terminal capbility
#define TAG_CVM_RESULT                                  (char *)"9F34"   // CVM result
#define TAG_TM_TYPE                                     (char *)"9F35"   // terminal type
#define TAG_TXN_COUNTER                                 (char *)"9F36"   // application counter
#define TAG_UN_NUM                                      (char *)"9F37"   // unpredictable num
#define TAG_POS_ENTRY_MODE                              (char *)"9F39"   // pos entry mode
#define TAG_AMT_REF_CURRENCY                            (char *)"9F3A"   // amount ref. currency
#define TAG_TXN_REF_CURRENCY                            (char *)"9F3C"   // transaction ref. currency code
#define TAG_ADDITIONAL_TM_CAP                           (char *)"9F40"   // txn seq counter
#define TAG_TXN_SEQ_COUNTER                             (char *)"9F41"   // txn seq counter
#define TAG_DATA_AUTH_CODE                              (char *)"9F45"   // txn seq counter
#define TAG_ICC_DYNAMIC_NUM                             (char *)"9F4C"   // ICC dynamic num
#define TAG_MERCHANT_NAME_LOCATION                      (char *)"9F4E"   // merchant name and location
#define TAG_TXN_CAT_CODE                                (char *)"9F53"   // txn seq counter
#define TAG_CARD_PRODUCT_INFO                           (char *)"9F63"   // paypass msd tk2 data
#define TAG_TTQ                                         (char *)"9F66"   // paypass msd tk2 data
#define TAG_PAYPASS_TK2                                 (char *)"9F6B"   // paypass msd tk2 data
#define TAG_PAYPASS_3RD_DATA                            (char *)"9F6E"   // paypass msd tk2 data

    
    
/**
 * Propretary tags, please refer to castles POS command set:
 * "7-11 EDC API V2.5.pdf"
 **/

// Information tags
#define d_TAG_MODEL_NAME                                    (char *)"DF01"
#define d_TAG_SN_INFO                                       (char *)"DF02"
#define d_TAG_SW_VERSION                                    (char *)"DF03"
#define d_TAG_HW_VERSION                                    (char *)"DF04"
#define d_TAG_VENDOR_ID                                     (char *)"DF05"
#define d_TAG_SECURITY_STATE                                (char *)"DF06"
#define d_TAG_BATTERY_STATE                                 (char *)"DF07"
#define d_TAG_DEVICE_RTC                                    (char *)"DF08"
#define d_TAG_DLL_VERSION                                   (char *)"DF09"
#define d_TAG_SW_DATE                                       (char *)"DF0A"

// Configuration tags
#define d_TAG_TERMINAL_ID_1                                 (char *)"DF10"
#define d_TAG_MERCHANT_ID_1                                 (char *)"DF11"
#define d_TAG_TERMINAL_ID_2                                 (char *)"DF12"
#define d_TAG_MERCHANT_ID_2                                 (char *)"DF13"
#define d_TAG_TERMINAL_ID_3                                 (char *)"DF14"
#define d_TAG_MERCHANT_ID_3                                 (char *)"DF15"
#define d_TAG_TERMINAL_ID_4                                 (char *)"DF16"
#define d_TAG_MERCHANT_ID_4                                 (char *)"DF17"
#define d_TAG_TERMINAL_ID_5                                 (char *)"DF18"
#define d_TAG_MERCHANT_ID_5                                 (char *)"DF19"
#define d_TAG_TERMINAL_ID_6                                 (char *)"DF1A"
#define d_TAG_MERCHANT_ID_6                                 (char *)"DF1B"
#define d_TAG_TERMINAL_ID_7                                 (char *)"DF1C"
#define d_TAG_MERCHANT_ID_7                                 (char *)"DF1D"
#define d_TAG_TERMINAL_ID_8                                 (char *)"DF1E"
#define d_TAG_MERCHANT_ID_8                                 (char *)"DF1F"
#define d_TAG_DEVICE_CLIENT_IP                              (char *)"DF20"
#define d_TAG_PAYMENT_HOST_IP                               (char *)"DF21"
#define d_TAG_PAYMENT_HOST_PORT                             (char *)"DF22"
#define d_TAG_PAYMENT_HOST_CONNECTION_TIMEOUT               (char *)"DF23"
#define d_TAG_PAYMENT_HOST_CONNECTION_RETRY                 (char *)"DF24"
#define d_TAG_TMS_HOST_IP                                   (char *)"DF25"
#define d_TAG_TMS_HOST_PORT                                 (char *)"DF26"
#define d_TAG_TMS_DEVICE_SN                                 (char *)"DF27"
#define d_TAG_PRE_AUTH_AMOUNT                               (char *)"DF2A"
#define d_TAG_CARD_INPUT_TIMEOUT                            (char *)"DF2B"
#define d_TAG_PIN_ENTRY_TIMEOUT                             (char *)"DF2C"
#define d_TAG_WAIT_ARQC_TIMEOUT                             (char *)"DF2D"
#define d_TAG_ENABLE_CARD_PRESENT_RELATED_NOTIFICATION      (char *)"DF2E"
#define d_TAG_ENABLE_PIN_INPUT_NOTIFICATION                 (char *)"DF2F"
#define d_TAG_ENABLE_SW_UPDATE_INPUT_NOTIFICATION           (char *)"DF30"
#define d_TAG_DEVICE_VOLUME                                 (char *)"DF31"
#define d_TAG_COMMUNICATION_INTERFACE                       (char *)"DF32"
#define d_TAG_POS_ID                                        (char *)"DF33"
#define d_TAG_DEVICE_ID                                     (char *)"DF34"
#define d_TAG_USER_PASSWORD                                 (char *)"DF35"
#define d_TAG_USER_ID                                       (char *)"DF36"
#define d_TAG_ADMIN_PASSWORD                                (char *)"DF37"
#define d_TAG_ADMIN_ID                                      (char *)"DF38"
#define d_TAG_SIGN_TIMEOUT                                  (char *)"DF39"
#define d_TAG_SIGN_CONFIRM_TIMEOUT                          (char *)"DF3A"
#define d_TAG_ONLINE_MODE                                   (char *)"DF3B"
#define d_TAG_ERM_KEY_MODE                                  (char *)"DFB000"
#define d_TAG_ERM_HOST_TIMEOUT                              (char *)"DFB001"

// Transaction tags
#define d_TAG_TXN_RESULT                                    (char *)"DF40"
#define d_TAG_STAN                                          (char *)"DF41"
#define d_TAG_RETRIEVAL_REFERENCE_NUMBER                    (char *)"DF42"
#define d_TAG_TANSACTION_SERIAL_NUMBER                      (char *)"DF43"
#define d_TAG_BATCH_NUMBER                                  (char *)"DF44"
#define d_TAG_INVOICE_NUMBER                                (char *)"DF45"
#define d_TAG_APPROVED_CODE                                 (char *)"DF46"
#define d_TAG_TOTAL_SALE_AMOUNT                             (char *)"DF47"
#define d_TAG_TOTAL_SALE_COUNT                              (char *)"DF48"
#define d_TAG_TOTAL_VOID_AMOUNT                             (char *)"DF49"
#define d_TAG_TOTAL_VOID_COUNT                              (char *)"DF4A"
#define d_TAG_TOTAL_REFUND_AMOUNT                           (char *)"DF4B"
#define d_TAG_TOTAL_REFUND_COUNT                            (char *)"DF4C"
#define d_TAG_TOTAL_VOID_REFUND_AMOUNT                      (char *)"DF4D"
#define d_TAG_TOTAL_VOID_REFUND_COUNT                       (char *)"DF4E"
#define d_TAG_TOTAL_TOPUP_AMOUNT                            (char *)"DF4F"
#define d_TAG_TOTAL_TOPUP_COUNT                             (char *)"DF50"
#define d_TAG_MASKED_PAN_DATA                               (char *)"DF51"
#define d_TAG_ENCRYPTED_TK2_DATA                            (char *)"DF52"
#define d_TAG_DUKPT_KSN_FOR_ENCRYPTED_TK2_DATA              (char *)"DF53"
#define d_TAG_ENCRYPTED_PIN_BLOCK                           (char *)"DF54"
#define d_TAG_DUKPT_KSN_FOR_ENCRYPTED_PIN_BLOCK             (char *)"DF55"
#define d_TAG_ROC                                           (char *)"DF56"
#define d_TAG_POS_TXN_TYPE                                  (char *)"DF57"
#define d_TAG_TXN_EVENT_TYPE                                (char *)"DF58"
#define d_TAG_TXN_CARD_TYPE                                 (char *)"DF59"
#define d_TAG_CARD_BRAND                                    (char *)"DF5A"
#define d_TAG_POS_CARD_INTERFACE                            (char *)"DF5B"
#define d_TAG_DEVICE_UPDATE_FILE                            (char *)"DF5C"
#define d_TAG_ERROR_CODE                                    (char *)"DF5D"
#define d_TAG_INSTALLMENT_INDICATOR                         (char *)"DF5E"
#define d_TAG_INSTALLMENT_TIMES                             (char *)"DF5F"
#define d_TAG_INSTALLMENT_FIRST_AMT                         (char *)"DF60"
#define d_TAG_INSTALLMENT_REGULAR_AMT                       (char *)"DF61"
#define d_TAG_REDEEM_INDICATOR                              (char *)"DF62"
#define d_TAG_AMT_AFTER_REDEEM                              (char *)"DF63"
#define d_TAG_REDEEM_AMT                                    (char *)"DF64"
#define d_TAG_REDEEM_POINT_USED                             (char *)"DF65"
#define d_TAG_CARD_EXPIRE_DATE                              (char *)"DF66"
#define d_TAG_CUP_TSN                                       (char *)"DF67"
#define d_TAG_CUP_DATE                                      (char *)"DF68"
#define d_TAG_CUP_TIME                                      (char *)"DF69"
#define d_TAG_CUP_SETTLEMENT_DATE                           (char *)"DF6A"
#define d_TAG_HOST_DATE                                     (char *)"DF6B"
#define d_TAG_HOST_TIME                                     (char *)"DF6C"
#define d_TAG_TXN_AMT_AUTH                                  (char *)"DF6D"
#define d_TAG_TXN_MID                                       (char *)"DF6E"
#define d_TAG_TXN_TID                                       (char *)"DF6F"
#define d_TAG_DYNAMIC_HOST_IP                               (char *)"DFBF01"
#define d_TAG_DYNAMIC_HOST_PORT                             (char *)"DFBF02"
#define d_TAG_BYPASS_MESSAGE_DATA                           (char *)"DFBF03"

#define d_TAG_TXN_SIGN_TIMEOUT                              (char *)"DF9000"
#define d_TAG_TXN_SIGN_CONFIRM_TIMEOUT                      (char *)"DF9001"
#define d_TAG_TXN_SIGN_IMG_FILE                             (char *)"DF9002"
#define d_TAG_TXN_TOKEN_PAY_STYLE                           (char *)"DF9003"
#define d_TAG_TXN_SIGN_RESULT                               (char *)"DF9004"
#define d_TAG_TXN_IMS_MSG                                   (char *)"DF9005"
#define d_TAG_TXN_IMS_RSP_CODE                              (char *)"DF9006"

// Cathay Seven Only
#define d_TAG_POS_TXN_ID                                    (char *)"DF70"
#define d_TAG_POS_DATETIME                                  (char *)"DF71"
#define d_TAG_POS_WALLET_ID                                 (char *)"DF72"
#define d_TAG_POS_MASKED_CARD_NUM                           (char *)"DF73"
#define d_TAG_POS_CARD_TYPE                                 (char *)"DF74"
#define d_TAG_POS_ENCRYPTED_PAN                             (char *)"DF75"
#define d_TAG_POS_SERVICE_ENTRY_MODE                        (char *)"DF76"

#define d_TAG_TXN_MID_TID_INDEX                             (char *)"DF8000"
#define d_TAG_TXN_PRODUCT_AMT                               (char *)"DF8001"
#define d_TAG_TXN_CUSTODY_AMT                               (char *)"DF8002"
#define d_TAG_TXN_RESELL_AMT                                (char *)"DF8003"
#define d_TAG_TXN_ENC_EINVOICE_NUM                          (char *)"DF8004"
#define d_TAG_TXN_POS_INPUT_FIELD                           (char *)"DF8005"
#define d_TAG_TXN_OLTP_MSG                                  (char *)"DF8006"
#define d_TAG_TXN_POS_REMOVE_CARD_ENABLE                    (char *)"DF8007"
#define d_TAG_TXN_POS_CAMPAIGN_ID_1                         (char *)"DF8008"
#define d_TAG_TXN_POS_CAMPAIGN_ID_2                         (char *)"DF8009"
#define d_TAG_TXN_POS_CAMPAIGN_ID_3                         (char *)"DF800A"
#define d_TAG_TXN_OLTP_STATUS_CODE                          (char *)"DF800B"
#define d_TAG_TXN_POS_CMD_AFTER_READ_CARD                   (char *)"DF800C"

/**
 * Heartland Gateway Parameters
 **/
#define d_TAG_LICENSE_IDENTITY                              (char *)"DFCF01"
#define d_TAG_SITE_IDENTITY                                 (char *)"DFCF02"
#define d_TAG_TRANSACTION_IDENTITY                          (char *)"DFCF03"
#define d_TAG_BATCH_IDENTITY                                (char *)"DFCF04"
#define d_TAG_RESPONSE_CODE                                 (char *)"DFCF05"
#define d_TAG_RESPONSE_TXT                                  (char *)"DFCF06"
#define d_TAG_DEVICE_IDENTITY                               (char *)"DFCF07"
#define d_TAG_UNIQUE_DEVICE_ID                              (char *)"DFCF08"
#define d_TAG_TRANSACTION_DESCRIPTOR                        (char *)"DFCF09"
#define d_TAG_HEARTLAND_LICENSE_ID                          (char *)"DFCF0A"
#define d_TAG_HEARTLAND_SITE_ID                             (char *)"DFCF0B"
#define d_TAG_HEARTLAND_DEVICE_ID                           (char *)"DFCF0C"
#define d_TAG_HEARTLAND_USER_NAME                           (char *)"DFCF0D"
#define d_TAG_HEARTLAND_PASSWORD                            (char *)"DFCF0E"
#define d_TAG_HEARTLAND_SITE_TRACE                          (char *)"DFCF0F"
#define d_TAG_HEARTLAND_DEVELOPER_ID                        (char *)"DFCF10"
#define d_TAG_HEARTLAND_VERSION_NBR                         (char *)"DFCF11"
#define d_TAG_HEARTLAND_SERVER_URL                          (char *)"DFCF12"
#define d_TAG_HEARTLAND_POS_SEQUENCE_NBR                    (char *)"DFCF13"
#define d_TAG_HEARTLAND_ACCOUNT_TYPE                        (char *)"DFCF14"

#define d_TAG_TRANS_RESPONSE_CODE                           (char *)"DFCF15"
#define d_TAG_TRANS_RESPONSE_TXT                            (char *)"DFCF16"
#define d_TAG_TRANS_TOKEN_VALUE                             (char *)"DFCF17"
#define d_TAG_TRANS_EMV_ISSUER_RESP                         (char *)"DFCF18"
#define d_TAG_TRANS_RESP_DT                                 (char *)"DFCF19"
#define d_TAG_TRANS_EMV_AID                                 (char *)"DFCF1A"
#define d_TAG_TRANS_EMV_TVR                                 (char *)"DFCF1B"
#define d_TAG_TRANS_EMV_TSI                                 (char *)"DFCF1C"
#define d_TAG_TRANS_AMOUNT                                  (char *)"DFCF1D"
#define d_TAG_TRANS_CARD_ENTRY_MODE                         (char *)"DFCF1E"
#define d_TAG_TRANS_CARD_ENTRY_LEGEND                       (char *)"DFCF1F"
#define d_TAG_TRANS_CARD_MASKED_PAN                         (char *)"DFCF20"
#define d_TAG_TRANS_CARD_HOLDER_NAME                        (char *)"DFCF21"
#define d_TAG_TRANS_POLLCARD_SEQUENCE_NBR                   (char *)"DFCF22"
#define d_TAG_TRANS_QUICKCHIP_ACTURAL_AMT                   (char *)"DFCF23" // use this tag update hps:Amt instead of 9F02,
                                                                             // pollingcard with an presuppose amount, but the actual amount is different with this one,
                                                                             // so we can set actual amount to hps:amt, and set presuppose amount to 9F02 in hps:Tagdata
#define d_TAG_TRANS_CARD_EXPIRE_DATE                        (char *)"DFCF24"
#define d_TAG_TRANS_AMOUNTOTHER                             (char *)"DFCF25"
#define d_TAG_TRANS_ALLOW_PARTIAL_AUTH                      (char *)"DFCF26"
#define d_TAG_TRANS_RSP_DATA                                (char *)"DFCF27"
#define d_TAG_TRANS_REQ_DATA                                (char *)"DFCF28"
#define d_TAG_TRANS_SAF_SEQUENCEID                          (char *)"DFCF29"

// Interac Debit Related Tags
#define d_TAG_TRANS_TRANSACTION_CODE                        (char *)"DFCF2A"
#define d_TAG_TRANS_TRANSMISSION_NUMBER                     (char *)"DFCF2B"
#define d_TAG_TRANS_BANK_RESPONSE_CODE                      (char *)"DFCF2C"
#define d_TAG_TRANS_MAC_KEY                                 (char *)"DFCF2D"
#define d_TAG_TRANS_PIN_KEY                                 (char *)"DFCF2E"
#define d_TAG_TRANS_FIELD_KEY                               (char *)"DFCF2F"
#define d_TAG_TRANS_TRACE_NUMBER                            (char *)"DFCF30"
#define d_TAG_TRANS_MESSAGE_AUTHENTICATION_CODE             (char *)"DFCF31"

/**
 * Propretary tags, please refer to ctpay document:
 * "CTPay Payment Acceptance Application API 2.3.37.pdf"
 **/
#define d_TAG_SIG_REQ                                       (char *)"DF28" // signature required
#define d_TAG_PIN_REQ                                       (char *)"DF29" // pin required, TBD
#define d_TAG_DEV_SN                                        (char *)"DF78" // device SN for TSYS
#define d_TAG_KERNEL_VER                                    (char *)"DF79" // kernel version for TSYS

#define d_TAG_PP30_CARD_INPUT_CAP                           (char *)"DF8117"
#define d_TAG_PP30_CVM_REQUIRED_CAP                         (char *)"DF8118"
#define d_TAG_PP30_CVM_NO_REQUIRED_CAP                      (char *)"DF8119"
#define d_TAG_MAG_CVM_REQUIRED_CAP                          (char *)"DF811E"
#define d_TAG_PP30_SECURITY_CAP                             (char *)"DF811F"
#define d_TAG_MAG_CVM_NO_REQUIRED_CAP                       (char *)"DF812C"

//==
#define d_TAG_TRANS_SETUP_MENU_PWD                          (char *)"DF8400"
#define d_TAG_INIT_TYPE                                     (char *)"DF8401"

#define d_TAG_TRANS_CARD_TYPE                               (char *)"DF8404"
#define d_TAG_TRANS_CARD_BRAND                              (char *)"DF8405"
#define d_TAG_TRANS_TIMEOUT                                 (char *)"DF8406"
#define d_TAG_TRANS_BALANCE_AFTER                           (char *)"DF8407"
#define d_TAG_TRANS_BALANCE_BEFORE                          (char *)"DF8408"
#define d_TAG_TRANS_AMT                                     (char *)"DF8409"
#define d_TAG_TRANS_NO                                      (char *)"DF840A"
#define d_TAG_TRANS_DATE_TIME                               (char *)"DF840B"
#define d_TAG_TRANS_CARD_NO                                 (char *)"DF840C"
#define d_TAG_TRANS_AUTO_LOAD_AMT                           (char *)"DF840D"
#define d_TAG_TRANS_TXN_LOG_DATA                            (char *)"DF840E"

#define d_TAG_LOGON_SAM_SLOT                                (char *)"DF8415"
#define d_TAG_LOGON_TERMINAL_ID                             (char *)"DF8416"
#define d_TAG_CFG_SAM_SLOT                                  (char *)"DF8417"
#define d_TAG_CFG_ETHERNET_TX_TIMEOUT                       (char *)"DF8418"
#define d_TAG_CFG_ETHERNET_RX_TIMEOUT                       (char *)"DF8419"
#define d_TAG_CFG_SERVER_IP                                 (char *)"DF841A"
#define d_TAG_CFG_SERVER_PORT                               (char *)"DF841B"
#define d_TAG_CFG_TERMINAL_ID                               (char *)"DF841C"
#define d_TAG_CFG_POS_ID                                    (char *)"DF841D"
#define d_TAG_TRANS_TERMINAL_ID                             (char *)"DF841E"
#define d_TAG_TRANS_MERCHANT_ID                             (char *)"DF841F"
#define d_TAG_TRANS_POS_ID                                  (char *)"DF8420"
#define d_TAG_TRANS_RRN                                     (char *)"DF8421"
#define d_TAG_TRANS_AUTO_LOAD_LOG_DATA                      (char *)"DF8422"
#define d_TAG_SETTLE_TOTAL_TXN_CNT                          (char *)"DF8440"
#define d_TAG_CFG_ENABLE_LOG                                (char *)"DF8441"
#define d_TAG_CFG_DLL_VERSION                               (char *)"DF8442"
#define d_TAG_CFG_ETHERNET_TIMEOUT                          (char *)"DF8443"
#define d_TAG_CFG_LOG_CNT                                   (char *)"DF8444"
#define d_TAG_CFG_COM_PORT                                  (char *)"DF8445"
#define d_TAG_CFG_ECC_IP                                    (char *)"DF8446"
#define d_TAG_CFG_ECC_PORT                                  (char *)"DF8447"
#define d_TAG_CFG_ICER_IP                                   (char *)"DF8448"
#define d_TAG_CFG_ICER_PORT                                 (char *)"DF8449"
#define d_TAG_CFG_CMAS_IP                                   (char *)"DF844A"
#define d_TAG_CFG_CMAS_PORT                                 (char *)"DF844B"
#define d_TAG_CFG_TM_LOCATION_ID                            (char *)"DF844C"
#define d_TAG_CFG_TM_ID                                     (char *)"DF844D"
#define d_TAG_CFG_TM_AGENT_NUMBER                           (char *)"DF844E"
#define d_TAG_CFG_LOCATION_ID                               (char *)"DF844F"
#define d_TAG_CFG_NEW_LOCATION_ID                           (char *)"DF8450"
#define d_TAG_CFG_SPID                                      (char *)"DF8451"
#define d_TAG_CFG_NEW_SPID                                  (char *)"DF8452"
#define d_TAG_CFG_BAUDRATE                                  (char *)"DF8453"
#define d_TAG_CFG_OPEN_COM                                  (char *)"DF8454"
#define d_TAG_CFG_MUST_SETTLE_DATE                          (char *)"DF8455"
#define d_TAG_CFG_READER_MODE                               (char *)"DF8456"
#define d_TAG_CFG_BATCH_FLAG                                (char *)"DF8457"
#define d_TAG_CFG_ONLINE_FLAG                               (char *)"DF8458"
#define d_TAG_CFG_ICER_DATA_FLAG                            (char *)"DF8459"
#define d_TAG_CFG_MESSAGE_HEADER                            (char *)"DF845A"
#define d_TAG_CFG_DLL_MODE                                  (char *)"DF845B"
#define d_TAG_CFG_AUTO_LOAD_MODE                            (char *)"DF845C"
#define d_TAG_CFG_MAX_AL_AMT                                (char *)"DF845D"
#define d_TAG_CFG_DEV_INFO                                  (char *)"DF845E"
#define d_TAG_CFG_ENABLE_SSL                                (char *)"DF845F"
#define d_TAG_CFG_CMAS_ADVICE_VERIFY                        (char *)"DF8460"
#define d_TAG_CFG_AUTO_SIGN_ON_PERCENT                      (char *)"DF8461"
#define d_TAG_CFG_AUTO_LOAD_FUNCITON                        (char *)"DF8462"
#define d_TAG_CFG_VERIFICATION_CODE                         (char *)"DF8463"
#define d_TAG_CFG_RESEND_READER_AVR                         (char *)"DF8464"
#define d_TAG_CFG_XML_HEADER_FLAG                           (char *)"DF8465"
#define d_TAG_CFG_FOLDER_CREATE_FLAG                        (char *)"DF8466"
#define d_TAG_CFG_BLC_NAME                                  (char *)"DF8467"
#define d_TAG_CFG_CMAS_MODE                                 (char *)"DF8468"
#define d_TAG_CFG_ADDITIONAL_TCPIP_DATA                     (char *)"DF8469"
#define d_TAG_TRANS_CARD_INFO                               (char *)"DF8500"
#define d_TAG_TRANS_INIT_INFO                               (char *)"DF8501"
#define d_TAG_REG_AUTH_INFO                                 (char *)"DF8502"
#define d_TAG_REG_CUSTOMER_CODE                             (char *)"DF8503"
#define d_TAG_REG_STORE_NO                                  (char *)"DF8504"
#define d_TAG_REG_REG_ID                                    (char *)"DF8505"
#define d_TAG_REG_SAM_SLOT                                  (char *)"DF8506"
#define d_TAG_LOGON_CUSTOMER_CODE                           (char *)"DF8507"
#define d_TAG_LOGON_STORE_NO                                (char *)"DF8508"
#define d_TAG_LOGON_REG_ID                                  (char *)"DF8509"
#define d_TAG_CFG_CUSTOMER_CODE                             (char *)"DF850A"
#define d_TAG_CFG_STORE_NO                                  (char *)"DF850B"
#define d_TAG_CFG_REG_ID                                    (char *)"DF850C"
#define d_TAG_CFG_SQEIENCE_NO                               (char *)"DF850D"
#define d_TAG_CFG_CASHIER_NO                                (char *)"DF850E"
#define d_TAG_REG_SHOP_NO                                   (char *)"DF8540"
#define d_TAG_REG_POS_ID                                    (char *)"DF8541"
#define d_TAG_REG_SUB_COMPANY_ID                            (char *)"DF8542"
#define d_TAG_REG_USER_ID                                   (char *)"DF8543"
#define d_TAG_REG_USER_PWD                                  (char *)"DF8544"
#define d_TAG_REG_SP_USER_ID                                (char *)"DF8545"
#define d_TAG_REG_SP_USER_PWD                               (char *)"DF8546"
#define d_TAG_LOGON_CLIENT_IP                               (char *)"DF8547"
#define d_TAG_LOGON_SYS_ID                                  (char *)"DF8548"
#define d_TAG_LOGON_SP_ID                                   (char *)"DF8549"
#define d_TAG_LOGON_SHOP_NO                                 (char *)"DF854A"
#define d_TAG_LOGON_POS_ID                                  (char *)"DF854B"
#define d_TAG_LOGON_SUB_COMPANY_ID                          (char *)"DF854C"
#define d_TAG_LOGON_EQUIP_TYPE                              (char *)"DF854D"
#define d_TAG_CFG_READER_IP                                 (char *)"DF854E"
#define d_TAG_CFG_SYSTEM_ID                                 (char *)"DF854F"
#define d_TAG_CFG_SP_ID                                     (char *)"DF8550"
#define d_TAG_CFG_SHOP_NO                                   (char *)"DF8551"
#define d_TAG_CFG_SUB_COMPANY_ID                            (char *)"DF8552"
#define d_TAG_CFG_USER_ID                                   (char *)"DF8553"
#define d_TAG_CFG_USER_PWD                                  (char *)"DF8554"
#define d_TAG_CFG_SUPER_USER_ID                             (char *)"DF8555"
#define d_TAG_CFG_SUPER_USER_PWD                            (char *)"DF8556"
#define d_TAG_CFG_EQUIP_TYPE                                (char *)"DF8557"
#define d_TAG_CFG_TMS_SERVER_IP                             (char *)"DF8558"
#define d_TAG_CFG_TMS_SERVER_PORT                           (char *)"DF8559"
#define d_TAG_CFG_BMS_SERVER_IP                             (char *)"DF855A"
#define d_TAG_CFG_BMS_SERVER_PORT                           (char *)"DF855B"
#define d_TAG_CFG_SFTP_IP                                   (char *)"DF855C"
#define d_TAG_CFG_SFTP_PORT                                 (char *)"DF855D"
#define d_TAG_CFG_SFTP_ID                                   (char *)"DF855E"
#define d_TAG_CFG_SFTP_PWD                                  (char *)"DF855F"
#define d_TAG_LOGON_MID                                     (char *)"DF8600"
#define d_TAG_LOGON_NII                                     (char *)"DF8601"
#define d_TAG_CFG_MID                                       (char *)"DF8602"
#define d_TAG_CFG_NII                                       (char *)"DF8603"
#define d_TAG_CFG_SERVER_IP_2                               (char *)"DF8604"
#define d_TAG_CFG_SERVER_PORT_2                             (char *)"DF8605"
#define d_TAG_CFG_TX_TIMEOUT                                (char *)"DF8606"
#define d_TAG_CFG_RX_TIMEOUT                                (char *)"DF8607"


//==================================================================
#define d_TAG_TK2_DF8310                                    (char *)"DF8310" // tk2 equivelent data for collis project
#define d_TAG_STR_TK2                                       (char *)"DF8311" // TK2 data format is string.
#define d_TAG_PREFER_AID_LIST                               (char *)"DF9F02"
#define d_TAG_ISSUER_AMT_AUTH                               (char *)"DF9F21"
#define d_TAG_ISSUER_AMT_OTHER                              (char *)"DF9F22"
#define d_TAG_ISSUER_TXN_CURRENCY_CODE                      (char *)"DF9F23"
#define d_TAG_AID_DECISION                                  (char *)"DFAE01" // select AID by whom
#define d_TAG_P2PE_DATA                                     (char *)"DFAE02" // encrypted pan and tk1, tk2, TBD
#define d_TAG_P2PE_KSN                                      (char *)"DFAE03" // dukpt ksn for p2pe encryption key
#ifdef ENABLE_OUTPUT_C0
#define d_TAG_EPB                                           "C0"     // online pin block for collis project
#else
#define d_TAG_EPB                                           (char *)"DFAE04" // online pin block for normal project
#endif
#define d_TAG_EPB_KSN                                       (char *)"DFAE05" // online pin block KSN
#define d_TAG_P2PE_DATA_EX                                  (char *)"DFAE06" // encrypted tk2 + amout

#define d_TAG_DISP_CENT                                     (char *)"DFAE10" // display amount cent
#define d_TAG_GROUP_SEPARATOR                               (char *)"DFAE11" // group seperator
#define d_TAG_DECIMAL_SEPARATOR                             (char *)"DFAE12" // decimal seperator
#define d_TAG_TXN_METHOD                                    (char *)"DFAE15" // TXN method, 00 = ICC, 01 = MSR, not exist = FF = card decide
#define d_TAG_SP_CLR_PAN                                    (char *)"DFAE11"
#define d_TAG_SP_CURRENCY                                   (char *)"DFAE14"
#define d_TAG_SP_TXN_SN                                     (char *)"DFAE15"
#define d_TAG_SP_TXN_TAC                                    (char *)"DFAE16"
#define d_TAG_SP_TXN_TIME                                   (char *)"DFAE17"
#define d_TAG_SP_TM_TYPE                                    (char *)"DFAE18"
#define d_TAG_SP_TXN_AMT                                    (char *)"DFAE19"

#define d_TAG_SP_ERROR_MSG                                  (char *)"DFAE21"

#define d_TAG_TAC_DECLINE                                   (char *)"DFAE36" // TAC decline
#define d_TAG_TAC_ONLINE_1                                  (char *)"DFAE37" // TAC online
#define d_TAG_TAC_DEFAULT_1                                 (char *)"DFAE38" // TAC default

#define d_TAG_MASKED_TK1                                    (char *)"DFAE56" // masked TK1, TBD
#define d_TAG_MASKED_TK2                                    (char *)"DFAE57" // masked TK2, TBD
#define d_TAG_MASKED_PAN                                    (char *)"DFAE5A" // masked PAN

#define d_TAG_MANUAL_CVV                                    (char *)"DFAE61" // manual CVV

#define d_TAG_PIN_SK                                        (char *)"DFBE01" // PIN SK TLV, FOR MKSK
#define d_TAG_PAN_SK                                        (char *)"DFBE02" // PAN SK TLV, FOR MKSK
#define d_TAG_USE_CUP                                       (char *)"DFBE03" // identify using CUP card, force pin entry
#define d_TAG_CTBC_SP_INDICATOR                             (char *)"DFBE04"
#define d_TAG_CTBC_SP_TID                                   (char *)"DFBE05"
#define d_TAG_CTBC_SP_MID                                   (char *)"DFBE06"
#define d_TAG_TIP_PERCENT                                   (char *)"DFBE04" // default 3 tip percentages
#define d_TAG_PHONE_NUM                                     (char *)"DFBE05" // phone num
#define d_TAG_PHONE_COUNTRY_CODE                            (char *)"DFBE06" // default phone country code
#define d_TAG_CARD_TYPE                                     (char *)"DFBE07" // card type
#define d_TAG_CARD_INTERFACE                                (char *)"DFBE09" // card interface
#define d_TAG_ARQC_TIMEOUT                                  (char *)"DFBE0A" // wait arc timeout
#define d_TAG_RET_TK1                                       (char *)"DFBE0B" // return tk1 info
#define d_TAG_FORCE_ONLINE                                  (char *)"DFBE0C" // force online, new
#define d_TAG_SUS_TIMEOUT                                   (char *)"DFBE0D" // suspend timeout

#define d_TAG_CARD_TIMEOUT                                  (char *)"DFBE10" // card input timeout
#define d_TAG_CUSTOM_MSG                                    (char *)"DFBE11"
#define d_TAG_ENABLE_KEY_LIST                               (char *)"DFBE12"
#define d_TAG_ENABLE_MANUAL_PAN                             (char *)"DFBE13"
#define d_TAG_CUSTOMER_APPROVAL                             (char *)"DFBE14" // customer approval image
#define d_TAG_CUSTOMER_DECLINE                              (char *)"DFBE15" // customer decline image
#define d_TAG_TRM                                           (char *)"DFBE16" // TRM Tag
#define d_TAG_DCC_SELECT                                    (char *)"DFBE17" // DCC selection menu
#define d_TAG_AMT_HEADER                                    (char *)"DFBE1A" // card input timeout
#define d_TAG_USE_SMARTPAY                                  (char *)"DFBE1B" // smartpay indicator
#define d_TAG_NEW_TXN_DISP                                  (char *)"DFBE1C" // new txn disp required or not
#define d_TAG_ENABLE_TXN_AMT_DISP                           (char *)"DFBE1D" // enable or disable display of transaction amount

#define d_TAG_MAX_PIN                                       (char *)"DFBE21" // max pin, 12
#define d_TAG_MIN_PIN                                       (char *)"DFBE22" // min pin, 4
#define d_TAG_BYPASS_PIN                                    (char *)"DFBE23" // bypass pin setup, 0
#define d_TAG_PIN_TIMEOUT                                   (char *)"DFBE24" // pin timeout, 30
#define d_TAG_ENABLE_MSR                                    (char *)"DFBE25" // enable msr, 1
#define d_TAG_ENABLE_SCR                                    (char *)"DFBE26" // enable scr, 1
#define d_TAG_ENABLE_CTLS                                   (char *)"DFBE27" // enable ctls, 1
#define d_TAG_MASTER_ENABLE                                 (char *)"DFBE28" // reader as master mode, start txn by reader
#define d_TAG_MASTER_PIN_CHECK                              (char *)"DFBE29"
#define d_TAG_MASTER_SMS_MENU                               (char *)"DFBE2A"
#define d_TAG_MASTER_TIP_MENU                               (char *)"DFBE2B"
#define d_TAG_MASTER_CARD_TYPE_MENU                         (char *)"DFBE2C"
#define d_TAG_MASTER_CASHBACK_MENU                          (char *)"DFBE2D"

#define d_TAG_ENABLE_CANCEL                                 (char *)"DFBE31"

#define d_TAG_SP_MASK_PAN                                   (char *)"DFAE5A"
#define d_TAG_SP_BANK_ID                                    (char *)"DFBE32"
#define d_TAG_SP_REMARK                                     (char *)"DFBE33"
#define d_TAG_SP_TID                                        (char *)"DFBE34"
#define d_TAG_SP_MID                                        (char *)"DFBE35"
#define d_TAG_SP_TM_CHK                                     (char *)"DFBE36"
#define d_TAG_SP_TXN_REC                                    (char *)"DFBE37"
#define d_TAG_DISABLE_CL_CHIP                               (char *)"DFBE38"
#define d_TAG_PIN_BLOCK_FORMAT                              (char *)"DFBE39"
#define d_TAG_CUSTOM_PROMPT                                 (char *)"DFBE3A"

#define d_TAG_ENABLE_CONTINUED_CARD_REMOVAL_DISPLAY         (char *)"DFBE3B"
#define d_TAG_ENABLE_CARD_REMOVAL_NOTIFICATION              (char *)"DFBE3C"

#define d_TAG_AID_LIST                                      (char *)"DFBE40"
#define d_TAG_APP_LABEL_LIST                                (char *)"DFBE41"
#define TAG_CVM_LIMIT                                       (char *)"DFBE42"

#define d_TAG_CTLS_SCHEME                                   (char *)"DFCF51"    
#ifdef  __cplusplus
}
#endif


#endif
