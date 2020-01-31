
/*
 * File Name: KernelState.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.14
 */

#ifndef _H_H_KERNEL_STATE_H_H_
#define _H_H_KERNEL_STATE_H_H_

/*
 * Api kernel states.
 */
typedef enum
{
    kKernelStateBeg_ = 0xFFFF,         // Begin
    kInvalid,                          // Uninitialze kernel status.
    kNoSendInitJsonCmd,                // Not Send write data to initialize kernel. 
    kStartInitHLLibrary,               // Receive initialize command and to initialize.
    kInitHLLibraryFailed,              // Kernel initialize failed.
    kIdle,                             // Kernel initialize success and next status is idle.
    kProcessingUpdateData,             // Processing update data command.
    kFinishUpdateData,                 // Finish update data.
    kStartDetectingCard,               // Detecting the card.
    kDetectCardTimeout,                // Detecting card timeout.
    kCancelDetectCard,                 // Cancel Detect card.
    kDetectedCard,                     // Detected card.
    kStartTxn,                         // Start txn.
    kProcessingTxn,                    // Processing txn.
    kStartCancelTxn,                   // Start Cancel current txn
    kFinishCancelTxn,                  // Finish Cancel Txn.
    kFinishTxn,                        // Finish txn.
    kUserActionType                    // User Action, need to exchange action type to ui index
} KernelState;

/*
 * Calling PostRequest and GetResponse.
 */
typedef enum
{
    kWaitingNewJsonCmd,
    kWaitingUserInputCmd,
    kStartParsingJsonCmd,
    kBeforeStartProcessingJsonCmd,
    kStartProcessingJsonCmd,
    kStartProcessingUserInputCmd,
    kFinishProcessingUserInputCmd
} RequestResponseMode;

#endif 