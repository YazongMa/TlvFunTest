//
// Created by Administrator on 2018/10/23.
//

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNSTATE_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNSTATE_H__


/**
 * TXN_STATE:
 *
 *  0 : idle
 *  10: initialize
 *  20: polling card
 *  30: reading card
 *  40: select aid
 *  ...
 */
typedef enum
{
    /*-1, invalid*/
    TXN_STATE_INVALID = -1,
            
    /*0, idle*/
    TXN_STATE_IDLE = 0,

    /*10, initialize*/
    TXN_STATE_INITIALIZE = 10,

    /*11, initialize success*/
    TXN_STATE_INITIALIZE_SUCCESS,

    /*12, initialize fail*/
    TXN_STATE_INITIALIZE_FAIL,
    
    /*20, polling card*/
    TXN_STATE_POLLING_CARD = 20,

    /*21, polling card success*/
    TXN_STATE_POLLING_CARD_SUCCESS,

    /*22, polling card timeout*/
    TXN_STATE_POLLING_CARD_TIMEOUT,

    /*23, polling card failure*/
    TXN_STATE_POLLING_CARD_FAILURE,
            
    /*30, reading card*/
    TXN_STATE_READING_CARD = 30,
            
    /*31, reading card success*/
    TXN_STATE_READING_CARD_SUCCESS,
            
    /*32, reading card fail*/
    TXN_STATE_READING_CARD_FAIL,
            
    /*33, plz insert card*/
    TXN_STATE_PLEASE_INSERT_CARD,
            
    /*34, plz swipe card*/
    TXN_STATE_PLEASE_SWIPE_CARD,
            
    /*35, cancel reading card*/
    TXN_STATE_CANCEL_READING_CARD,
            
    /*36, detected more card*/
    TXN_STATE_MORE_CARD_WERE_DETECTED,
            
    /*37, plz see phone */
    TXN_STATE_PLEASE_SEE_PHONE,

    /*38, plz remove card */
    TXN_STATE_PLEASE_REMOVE_CARD,
            
    /*39, plz tap card*/
    TXN_STATE_PLEASE_TAP_CARD,

    /*40, select aid*/
    TXN_STATE_AID_SELECT = 40,

    /*41, confirm aid*/
    TXN_STATE_AID_CONFIRM,

    /*42, total amt get*/
    TXN_STATE_TOTAL_AMOUNT_GET,

    /*43, exception file check*/
    TXN_STATE_EXCEPTION_FILE_CHECK,

    /*44, call your bank*/
    TXN_STATE_CALL_YOUR_BANK,

    /*45, Confirm communication with host*/
    TXN_STATE_CONFIRM_COMM_WITH_HOST,

    /*50, pin enter*/
    TXN_STATE_PIN_ENTER,

    /*51, pin enter timeout*/
    TXN_STATE_PIN_ENTER_TIMEOUT,

    /*52, pin enter success*/
    TXN_STATE_PIN_ENTER_SUCCESS,

    /*53, pin enter fail*/
    TXN_STATE_PIN_ENTER_FAIL,

    /*54, pin enter again*/
    TXN_STATE_PIN_ENTER_AGAIN,

    /*55, pin enter again*/
    TXN_STATE_PIN_ENTER_LASTTIME,

    /*56, pin enter start*/
    TXN_STATE_PIN_ENTER_START,

    /*57, pin enter finish*/
    TXN_STATE_PIN_ENTER_FINISH,

    /*58, Last PIN input*/
    TXN_STATE_LAST_PIN_INPUT,

    /*60, preparing transaction data*/
    TXN_STATE_PREPARING_TXNDATA = 60,

    /*61, preparing transaction data success*/
    TXN_STATE_PREPARING_TXNDATA_SUCCESS,

    /*61, preparing transaction data fail*/
    TXN_STATE_PREPARING_TXNDATA_FAIL,

    /*70, packing soap data*/
    TXN_STATE_PACKING_SOAPDATA = 70,

    /*71, packing soap data success*/
    TXN_STATE_PACKING_SOAPDATA_SUCCESS,

    /*72, packing soap data fail*/
    TXN_STATE_PACKING_SOAPDATA_FAIL,

    /*80, connecting host*/
    TXN_STATE_CONNECTING = 80,

    /*81, connecting host timeout*/
    TXN_STATE_CONNECTING_TIMEOUT,

    /*82, connecting host success*/
    TXN_STATE_CONNECTING_SUCCESS,  

    /*83, connecting host fail*/
    TXN_STATE_CONNECTING_FAIL,

    /*84, connectivity available*/
    TXN_STATE_CONNECTIVITY_AVAILABLE,

    /*85, connectivity not available*/
    TXN_STATE_CONNECTIVITY_NOT_AVAILABLE,

    /*90, unpacking soap data*/
    TXN_STATE_UNPACKING_SOAPDATA = 90,

    /*91, unpacking soap data success*/
    TXN_STATE_UNPACKING_SOAPDATA_SUCCESS,

    /*92, unpacking soap data fail*/
    TXN_STATE_UNPACKING_SOAPDATA_FAIL,

    /*93, SAF approval*/
    TXN_STATE_SAF_APPROVED,

    /*94, SAF decline*/
    TXN_STATE_SAF_DECLINE,
            
    /*100, Print signature*/
    TXN_STATE_PRINT_SIGNATURE = 100,

    /*100, transaction approval*/
    TXN_STATE_APPROVAL = 101,

    /*110, transaction declined*/
    TXN_STATE_DECLINED = 110,

    /*111, transaction declined*/
    TXN_STATE_OFFLINE_APPROVED = 111,

    /*112, transaction declined*/
    TXN_STATE_OFFLINE_DECLINED = 112,

    /*120, for interac, pos sequence number*/
    TXN_STATE_INTERAC_POS_SEQNBR = 120,

    /*121, for interac, account type*/
    TXN_STATE_INTERAC_ACCOUNT_TYPE = 121,
            
    /*130, select language*/
    TXN_STATE_SELECT_LANGUAGE = 130,
            
    /*131, switch language*/
    TXN_STATE_SWITCH_LANGUAGE = 131,      
            
    /*132, interac receipt warning*/
    TXN_STATE_INTERAC_RECEIPT_WARNINIG = 132,

    TXN_STATE_COMPLETION = 1000
}HEARTLAND_TRANS_STATE;



#endif //__HEARTLAND_PAYMENT_KERNEL_TXNSTATE_H__
