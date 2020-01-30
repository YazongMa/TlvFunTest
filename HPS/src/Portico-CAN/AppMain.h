
/*
 * File Name: AppMain.h
 * 
 * Author: Alan.Ren
 * Create Date: 2018.11.15
 */

#ifndef _H_App_Main_H_ 
#define _H_App_Main_H_

#define Cfg_Cert_File_Path                        "Config/ca-certificates.crt"
#define Cfg_Cert_Folder_Path                      "Config"
#define Cfg_Emv_Config_Xml_Path                   "Config/emv_config.xml"
#define Cfg_EmvCL_Config_Xml_Path                 "Config/emvcl_config.xml"
#define Cfg_Link_Config_Path                      "Config/LinkCfg.json"
#define Cfg_Rs232_Config_Path                     "Config/Rs232Cfg.json"
#define Cfg_Txn_Init_Path                         "Config/TxnInit.json"
#define Cfg_Gprs_Config_Path                      "Config/GprsCfg.json"

// Ui Index
#define Ui_Idle                      (0x00)
#define Ui_Parsing_Json_Data         (0x01)
#define Ui_Polling_Card              (0x02)
#define Ui_Polling_Card_Timeout      (0x03)
#define Ui_Connecting_Host           (0x04)
#define Ui_Txn_Approval              (0x05)
#define Ui_Txn_Decline               (0x06)
#define Ui_Initialing                (0x07)
#define Ui_Entering_PIN              (0x08)
#define Ui_Entering_PIN_Again        (0x09)
#define Ui_Last_PIN_Input            (0x0A)
#define Ui_PIN_Error                 (0x0B)
#define Ui_Cancel                    (0x0C)
#define Ui_Pls_Remove_Card           (0x0D)
#define Ui_Start_Get_PIN             (0x0E)
#define Ui_Finish_Get_PIN            (0x0F)
#define Ui_Polling_Card_Success      (0x10)
#define Ui_Packing_Data              (0x14)
#define Ui_Select_Aid                (0x15)  // Add
#define Ui_Confirm_Select_Aid        (0x16)  // Add
#define Ui_Get_Total_Amt             (0x17)  // Add
#define Ui_Chk_Exp_File              (0x18)  // Add
#define Ui_Call_Your_Bank            (0x19)  // Add
#define Ui_Plz_Insert_SC_Card        (0x1A)  // Add
#define Ui_Plz_Swipe_MSR_Card        (0x1B)  // Add
#define Ui_Read_Card_Fail            (0x1C)  // Add
#define Ui_More_Card_Detected        (0x1D)  // Add
#define Ui_Plz_See_Phone             (0x1E)  // Add
#define Ui_Plz_Tap_Card              (0x1F)  // Add
#define Ui_Txn_Offline_Approval      (0x20)  // Add
#define Ui_Txn_Offline_Decline       (0x21)  // Add
#define Ui_Txn_Interac_POS_SeqNbr    (0x22)  // Add
#define Ui_Txn_Interac_Account_Type  (0x23)  // Add
#define Ui_Txn_SAFApproval           (0x24)  // Add
#define Ui_Txn_SAFDecline            (0x25)  // Add
#define Ui_Txn_Connect_Available     (0x26)  // Add
#define Ui_Txn_Connect_NotAvailable  (0x27)  // Add
#define Ui_Select_Language           (0x28)  // Add
#define Ui_Switch_Language           (0x29)  // Add
#define Ui_Interac_Receipt_Warning   (0x2A)  // Add

// Error UI page index.
#define Ui_Unkown                    (-1)

// User define UI page index.
#define Ui_Invalid_Index             (0x7FFFFFFF)
#define Ui_Select_Card_Type          (0x70000000)
#define Ui_Input_Amount              (0x70000001)
#define Ui_Select_Link_Type          (0x70000002)
#define Ui_Start_Use_Eth_Init        (0x70000003)
#define Ui_Input_APN_Name            (0x70000004)
#define Ui_Input_APN_Id              (0x70000005)
#define Ui_Input_Pwd                 (0x70000006)
#define Ui_Select_Sim_Slot           (0x70000007)
#define Ui_Start_Use_Gprs_Init       (0x70000008)
#define Ui_Start_New_Init            (0x70000009)
#define Ui_Welcome_Page              (0x7000000A)
#define Ui_Connecting_Host_New       (0x7000000B)
#define Ui_Init_Ethernet_Link        (0x7000000C)
#define Ui_Init_Ethernet_Failed      (0x7000000D)
#define Ui_After_Call_PostRequest    (0x7000000E) // Only used after calling thet PostRequest API.
#define Ui_Manual_Sel_Txn_Type       (0x7000000F) // User select Auto Polling card or enter Manual select Txn type.
#define Ui_Post_Polling_Card_Cmd     (0x70000010) 
#define Ui_Select_Polling_Card_Type  (0x70000011) // Polling Select card type
#define Ui_Init_Gprs_Link            (0x70000012) // Initialize Init GPRS Link
#define Ui_Init_Gprs_Failed          (0x70000013) // Initialize Gprs failed.
#define Ui_Load_Def_Link_Type        (0x70000014) // Read default link type from config.
#define Ui_Input_GatewayID           (0x70000015) // For Authcomplete

#define Ui_Exit_App                  (0x7FFFFFFE)

#endif 