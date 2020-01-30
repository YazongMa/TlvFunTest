
/*
 * File Name: DefUiIndex.h
 * Author: Alan.ren
 * 
 * Create Date: 2019.3.19
 * Description: Public this file for user.
 */

#ifndef _H_H_UI_INDEX_H_H_
#define _H_H_UI_INDEX_H_H_

typedef enum
{
    kInvalidUiIndex = -1,

    kIdleUi,                   // 0x00
    kParsingCmdUi,             // 0x01
    kDetectingCardUi,          // 0x02
    kDetectedCardTimeoutUi,    // 0x03, Delay
    kConnectingHostUi,         // 0x04
    kTxnApproveUi,             // 0x05
            
    kTxnDeclineUi,             // 0x06
    kStartInitLibUi,           // 0x07
    kPlsInputPINUi,            // 0x08
    kPlsInputPINAgainUi,       // 0x09
    kLastInputPINUi,           // 0x0A
            
    kPINErrorUi,               // 0x0B
    kCancelUi,                 // 0x0C
    kPlsRemoveCardUi,          // 0x0D, Delay
    kStartGetPinUi,            // 0x0E, Delay
    kFinishGetPinUi,           // 0x0F, Delay
            
    kDetectCardSuccessUi,      // 0x10, 
    kInitLibFailedUi,          // 0x11, Delay
    kInitLibSuccessUi,         // 0x12, Delay
    kInputPINTimeoutUi,        // 0x13
    kPackingSoapDataUi,        // 0x14
            
    kUserSelectAidUi,          // 0x15, Ui of User Action: select aid
    kUserConfirmAidUi,         // 0x16, Ui of User Action: confirm aid
    kUserGetTotalAmtUi,        // 0x17, Ui of User Action: get total amount
    kUserCheckExceptionFileUi, // 0x18, Ui of User Action: check exception file
    kUserCallBankUi,           // 0x19, Ui of User Action: call your bank

    kPlzInsertCard,            // 0x1A, Ui, while swiping an emv card
    kPlzSwipeCard,             // 0x1B, Ui, while inserting a bad emv card
    kReadCardFailUi,           // 0x1C, Ui, while reading a card
    kMoreCardsDetectedUi,      // 0x1D, Ui, while more cards were detected(emv contactless)
    kPlzSeePhone,              // 0x1E, Ui, while doing an emv contactless transaction

    kPlzTapCard,               // 0x1F, Ui, while tapping an emvcl card
//    kPlzRePollingcard,         // 0x1F, Please re-swipe, re-tap or re-insert a card. it's  abnormal, and need to poll a card again
    kTxnOfflineApproveUi,      // 0x20
    kTxnOfflineDeclineUi,      // 0x21
    kTxnInteracPosSeqNbr,      // 0x22
    kTxnInteracAccountType,    // 0x23
            
    kSAFApproveUi,			   // 0x24, Ui of SAF approval
    kSAFDeclinedUi,			   // 0x25, Ui of SAF decline
	kConnectAvailable,		   // 0x26, Ui of connectivity available
	kConnectNotAvailable,	   // 0x27, Ui of connectivity not available
    kSelectLanguage,           // 0x28, Ui of User Action: select language
	
    kSwitchLanguage,           // 0x29, Ui of User Action: switch language
    kInteracReceiptWarning,    // 0x2A, Ui of User Action: interac receipt warning

    kInTxnUi
} kUiIndex;

#endif
