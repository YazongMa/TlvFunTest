
/*
 * File Name: HP_ProcStatus.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.05.15
 */

#ifndef _H_H_PROC_STATUS_H_H_
#define _H_H_PROC_STATUS_H_H_

enum __txn_ProcStatus__
{
    ProcStatus_InitSuccess        = 0x00000001,
    ProcStatus_StartPollingCard   = 0x00000002,
    ProcStatus_PollingCardSuccess = 0x00000004,
    ProcStatus_PollingCardTimeout = 0x00000008,
    ProcStatus_CancelPollingCard  = 0x00000010,
    ProcStatus_StartTxn           = 0x00000020,
    ProcStatus_FinishTxn          = 0x00000040,
    ProcStatus_TxnApproval        = 0x00000080,
    ProcStatus_TxnDecline         = 0x00000100,
    ProcStatus_ReadCardDataFailed = 0x00000200
};

#endif
