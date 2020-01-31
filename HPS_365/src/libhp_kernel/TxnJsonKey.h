
/*
 * File Name: Txn_JsonKey.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.3.26
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_JSONKEY_H__
#define __HEARTLAND_PAYMENT_KERNEL_JSONKEY_H__

// Key
#define JK_Act_Type                         "ActType"
#define JK_Act_Value                        "ActValue"
#define JK_Act_ListValue                    "ActListValue"
#define JK_PAN                              "PAN"


// ActType
#define JV_Act_AidList                      "ActAidList"

#ifdef PLATFORM_ANDROID
#define JV_Act_AidConfim                    "ActAidConfirm"
#else
#define JV_Act_AidConfim                    "ActAidConfim"
#endif

#define JV_Act_TotalAmt                     "ActTotalAmt"
#define JV_Act_CallUBank                    "ActCallUBank"
#define JV_Act_ExceptionCheck               "ActExecptFileCheck"
#define JV_Confirm_Comm_With_Host           "ActConfirmCommWithHost"
#define JV_Confirm_Prompt_Last_PIN_Input    "ActConfirmLastPINInput"
#define JV_Act_Print_Signature              "ActPrintSignature"
#define JV_Act_EMV_InputPIN                 "ActEMVInputPIN"
#define JV_Act_InteracPosSeqNbr             "ActInteracPosSeqNbr"
#define JV_Act_InteracAccountType           "ActInteracAccountType"
#define JV_Act_Select_Language              "ActSelectLanguage"
#define JV_Act_Switch_Language              "ActSwitchLanguage"


#endif /* __HEARTLAND_PAYMENT_KERNEL_JSONKEY_H__ */
