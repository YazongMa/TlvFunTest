
/*
 * File Name: HL_JsonKey.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.14
 * 
 * N.B: This file is forbidden to modify.
 */

#ifndef _H_H_HL_JSON_KEY_H_H_ 
#define _H_H_HL_JSON_KEY_H_H_

//
// JK_* prefix represent JSON KEY
// JV_* prefix represent JSON VALUE
//

// Key. 
#define JK_Txn_Type                                     "txnType"
#define JK_Txn_Card_Type                                "txnCardType"
#define JK_Txn_Ui_Index                                 "txnUiIndex"
#define JK_Txn_Amount                                   "txnAmt"
#define JK_Txn_OtherAmount                              "txnAuthAmt"
#define JK_Txn_CashbackAmount                           "txnCashBackAmt"
#define JK_Txn_Gateway_Id                               "txnGatewayTxnID"
#define JK_Txn_SequenceId                               "txnSequenceID"
#define JK_Txn_BatchId                                  "txnBatchId"
#define JK_Txn_TotalSaleCnt                             "txnTotalCnt"
#define JK_Txn_TotalSaleAmt                             "txnTotalAmt"
#define JK_Txn_AuthCode                                 "txnAuthCode"
#define JK_Txn_RefNumber                                "txnRefNbr"
#define JK_Txn_CardHolderName                           "txnCardholderName"
#define JK_Txn_Gateway_RspCode                          "txnGatewayRspCode"
#define JK_Txn_Gateway_RspInfo                          "txnGatewayRspMsg"
#define JK_Txn_Trans_RspCode                            "txnStatusCode"
#define JK_Txn_Trans_RspInfo                            "txnResult"
#define JK_Txn_UserActData                              "txnUserActData"
#define JK_Txn_Tokenization_Value                       "txnTokenValue"
#define JK_Txn_Card_EntryMode                           "txnCardEntryMode"
#define JK_Txn_MaskedAcct                               "txnMaskedAcct"
#define JK_Txn_MaskedCardNum                            "txnCardNum"
#define JK_Txn_DateTime                                 "txnDateTime"
#define JK_Txn_CardBrand                                "txnCardBrand"
#define JK_Txn_EMV_AuthData                             "txnEMVAuthData"
#define JK_Txn_EMV_AID                                  "txnEMVAID"
#define JK_Txn_EMV_TVR                                  "txnEMVTVR"
#define JK_Txn_EMV_TSI                                  "txnEMVTSI"
#define JK_Txn_EMV_ARC                                  "txnEMVARC"
#define JK_Txn_EMV_AppName                              "txnAppName"
#define JK_Txn_EMV_AppLabel                             "txnAppLabel"
#define JK_Txn_Type_Receipt                             "txnTypeReceipt"
#define JK_Txn_EntryLegend                              "txnEntryLegend"
//11-25 add for interac receipt
#define JK_Txn_AccountType                              "txnAccountType"
#define JK_Txn_SequenceNbr                              "txnSequenceNbr"
#define JK_Txn_TraceNumber                              "txnTraceNumber"
#define JK_Txn_BankRespCode                             "txnBankRespCode"
#define JK_Txn_CardExpire_Date                          "txnCardExpireDate"
#define JK_Txn_RspData                                  "txnResponseData"
#define JK_Txn_ReqData                                  "txnRequestData"

#define JK_CFG_ARQC_TIMEOUT                             "cfgARQCTimeout"
#define JK_CFG_CertFile                                 "cfgCertFile"
#define JK_CFG_Cert_Path                                "cfgCertPath"
#define JK_CFG_EMV_Config                               "cfgEMVConfig"
#define JK_CFG_EMVCL_Config                             "cfgEMVCLConfig"
#define JK_CFG_Developer_ID                             "cfgDeveloperID"
#define JK_CFG_Device_ID                                "cfgDeviceId"
#define JK_CFG_Host_Url                                 "cfgHostUrl"
#define JK_CFG_Http_Method                              "cfgHttpMethod"
#define JK_CFG_License_ID                               "cfgLicenseId"
#define JK_CFG_PIN_Entry_Timeout                        "cfgPINEntryTimeout"
#define JK_CFG_PWD                                      "cfgPassword"
#define JK_CFG_Poll_Card_Timeout                        "cfgPollCardTimeout"
#define JK_CFG_AID_Select_Timeout                       "cfgAIDSelectTimeout"
#define JK_CFG_Pullout_Card_Timeout                     "cfgPulloutCardTimeout"
#define JK_CFG_POS_Gateway_Url                          "cfgPosGatewayUrl"
#define JK_CFG_Site_ID                                  "cfgSiteId"
#define JK_CFG_User_Name                                "cfgUserName"
#define JK_CFG_Version_Nbr                              "cfgVersionNbr"
#define JK_CFG_Rs232_Cfg                                "cfgRS232Cfg"
#define JK_CFG_Voltage_Flag                             "cfgVoltageFlag" 
#define JK_CFG_Token_Flag                               "cfgTokenizationFlag"
#define JK_CFG_EMV_Debug_Flag                           "cfgEMVDebugFlag"
#define JK_CFG_EMVCL_Debug_Flag                         "cfgEMVCLDebugFlag"
#define JK_CFG_BB_Param_File                            "cfgBBParamFile"
#define JK_CFG_UniqueDeviceId                           "cfgUniqueDeviceId"
#define JK_CFG_Txn_Descriptor                           "cfgTxnDescriptor"
#define JK_CFG_QuickChip_Flag                           "cfgQuickChipFlag"
#define JK_CFG_Cert_Name                                "cfgCertName"
#define JK_CFG_Strategy_Flag                            "cfgStrategyFlag"       // only for 365
#define JK_CFG_UserAct_Bitmap                           "cfgTxnUserActBitmap"
#define JK_CFG_OnlineFlag                               "cfgOnlineFlag"
#define JK_CFG_FallbackFlag                             "cfgFallbackFlag"
#define JK_CFG_PaymentDebugFlag                         "cfgPaymentDebugFlag"
#define JK_CFG_AllowPartialAuthFlag                     "cfgAllowPartialAuth"
#define JK_Txn_Processing_Status                        "txnProcessingStatus"
#define JK_Txn_SafSequenceID                            "txnSafSequenceID"
#define JK_CFG_SAF_MODE                                 "cfgSAFMode"
#define JK_CFG_SAF_LIMIT_AMOUNT                         "cfgSAFTotalLimit"
#define JK_CFG_MaskedAcctMode                           "cfgMaskedAcctMode"

// Value.
#define JV_Invalid_Value                                "InvalidValue"
#define JV_Sale                                         "sale"
#define JV_PreAuth                                      "preAuth"
#define JV_AuthComplete                                 "authComplete"
#define JV_Settlement                                   "settlement"
#define JV_Cancel                                       "cancel"
#define JV_Void                                         "void"
#define JV_Return                                       "return"
#define JV_GetAppVer                                    "getAppVer"
#define JV_GetKernelVer                                 "getKernelVer"
#define JV_WriteData                                    "writeData"
#define JV_ReadData                                     "readData"
#define JV_Credit                                       "credit"
#define JV_Debit                                        "debit"
#define JV_UpdateData                                   "updateData"
#define JV_PollingCard                                  "pollingcard"
#define JV_SAF                                          "safTransaction"
#define JV_CheckConnectivity                            "checkConnectivity"

#ifdef PLATFORM_ANDROID
#define JV_Reversal                                     "reversal"
#else
#define JV_Reversal                                     "txnReversal"
#endif

#endif
