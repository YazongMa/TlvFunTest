
/*
 * File Name: JsonCmdProp.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.15
 */

#ifndef _H_H_json_cmd_prop_H_H
#define _H_H_json_cmd_prop_H_H

//
// A json command contain a sub item will contain card type 
// data.
//
typedef enum
{
    kCmdUnsetCardType   = 0, // Json command not contain card type
    kCmdSetCredit,           // Json command contain credit card type.
    kCmdSetDebit             // Json command contain debit card type.
} CmdSetCardType;

//
// Txntype properity
//
typedef enum
{
    kCmdUnsetTxnType = 0,
    kWriteData,
    kSale,
    kVoid,
    kPreAuth,
    kReturn,
    kAuthComplete,
    kCancel,
    kSettlement,
    kGetAppVer,
    kGetKernelVer,
    kReversal,
    kUpdateData,
    kPollingCard,
    kSAF,
    kCheckConnectivity
} CmdTxnType;

//
// Error code.
//
typedef enum
{
    kNoError = 0,                // OK
    kUninitError,                // Not initialize kernel.
    kInitKernelErr,              // Initialize kernel failed.
    kPraseCmdErr,                // Json command parse failed.
    kDetectCardTimeoutErr        // Detecting card timeout.

} HPKernelCode;

#endif