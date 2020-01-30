
/*
 * File Name: TouchThread.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.19
 */

#include <ctosapi.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <unistd.h>

#include "Dbghlp.h" 
#include "Ui.h"
#include "TxnRequest.h"
#include "AppMain.h"
#include "Inc/TxnInterface.h"
#include "CTouch.h"

#define Nop (10 * 1000)

typedef struct _tagPoint
{
    long x;
    long y;
} Point;

typedef struct _tagRect
{
    long left;
    long top;
    long right;
    long bottom;
} Rect;

extern CUi *getUiModuleObj(void);
extern CTxnRequest *getTxnReqModuleObj(void);
static CTouch cTouch_;
static pthread_t cThreadId_ = -1;
static pthread_mutex_t cTouchMutex_;
static BOOL bEnableTouch_ = FALSE;

/*
 * Private.
 */
static BOOL PtInRect_(const Point &pt, const Rect &re)
{
    if (pt.x >= re.left && pt.x <= re.right &&
        pt.y >= re.top && pt.y <= re.bottom)
    {
        return TRUE;
    }
    return FALSE;
}

/*
 * Private.
 */
static BOOL PtInRect_(const long &x, const long &y, const Rect &re)
{
    const Point pt = {x, y};
    return PtInRect_(pt, re);
}

// Cancel Buttons.
static const Rect btnPollingCardCancel_ = {49, 355, 271, 438}; // The cancel button exist in Polling card UI.
static const Rect btnConnNetworkCancel_ = {75, 413, 242, 467}; // The cancel button exist in Connecting UI.

// Transaction type buttons
static const Rect btnSale_ = {25, 65, 295, 118};
static const Rect btnPreAuth_ = {25, 129, 295, 182};
static const Rect btnAuthComplete_ = {25, 194, 295, 247}; 
static const Rect btnSettlement_ = {25, 258, 295, 311};
static const Rect btnVoid_ = {25, 323, 295, 376};
static const Rect btnReturn_ = {25, 387, 295, 440};
static const Rect btnReturnIdle_ = {286, 442, 320, 480};

// Card type buttons
static const Rect btnCreditCard_ = {25, 142, 295, 235};
static const Rect btnDebitCard_ = {25, 249, 295, 342};
static const Rect btnCancelSelCardType_ = {25, 357, 295, 450};

// Link
static const Rect btnEthernet_ = {18, 159, 301, 259};
static const Rect btnGprs_ = {18, 287, 300, 388};
static const Rect btnSimSlot1_ = {18, 122, 303, 227};
static const Rect btnSimSlot2_ = {18, 262, 301, 366};

// Init Etnernet Fail
static const Rect btnContinue_ = {22, 386, 152, 440};
static const Rect btnRestart_ = {169, 386, 301, 440};

// SelOpTxnType Ui
static const Rect btnPollineCard_ = {28, 135, 295, 221};
static const Rect btnManualType_ = {28, 255, 295, 341};

// AID confirm
static const Rect btnCancel_ = {22, 386, 152, 440};
static const Rect btnEnter_ = {169, 386, 301, 440};

// AID list
static const Rect btnFirst_ = {1, 160, 320, 200};
static const Rect btnSecond_ = {1, 200, 320, 240};
static const Rect btnThird_ = {1, 240, 320, 280};
static const Rect btnFourth_ = {1, 280, 320, 320};
static const Rect btnFifth_ = {1, 320, 320, 360};
static const Rect btnSixth_ = {1, 360, 320, 400};
static const Rect btnSeventh_ = {1, 400, 320, 440};

//language select
static const Rect btnEnglish_ = {28, 135, 295, 221};
static const Rect btnFrench_ = {28, 255, 295, 341};

/*
 * Private.
 */
static void processTouchEvent_(CUi *pcUi, CTxnRequest *pcTxnReq, const long lX1, const long lY1,
                              const long lX2, const long lY2)
{
    // Sale
    if (Ui_Polling_Card == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnPollingCardCancel_) && PtInRect_(lX2, lY2, btnPollingCardCancel_))
        {
            CByteStream cData = pcTxnReq->GenCancelJsonData();
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        }
    }
    else if (Ui_Manual_Sel_Txn_Type == pcUi->GetCurUiIndex() ||
             Ui_Idle == pcUi->GetCurUiIndex())
    {
        // Sale
        if (PtInRect_(lX1, lY1, btnSale_) && PtInRect_(lX2, lY2, btnSale_))
        {
            pcUi->SwitchUi(Ui_Select_Card_Type);
            pcUi->SetTxnType(kTxnSale);
        } // Pre Auth
        else if (PtInRect_(lX1, lY1, btnPreAuth_) && PtInRect_(lX2, lY2, btnPreAuth_))
        {
            pcUi->SwitchUi(Ui_Input_Amount);
            pcUi->SetTxnType(kTxnPreAuth);
        } // Auth Complete
        else if (PtInRect_(lX1, lY1, btnAuthComplete_) && PtInRect_(lX2, lY2, btnAuthComplete_))
        {
            pcUi->SwitchUi(Ui_Input_GatewayID);
            pcUi->SetTxnType(kTxnAuthComplete);
        } // Settlement
        else if (PtInRect_(lX1, lY1, btnSettlement_) && PtInRect_(lX2, lY2, btnSettlement_))
        {
            pcUi->SetTxnType(kTxnSettlement);
            CByteStream cData = pcTxnReq->GenSettlementJsonData();
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        } // Void
        else if (PtInRect_(lX1, lY1, btnVoid_) && PtInRect_(lX2, lY2, btnVoid_))
        {
            pcUi->SetTxnType(kTxnVoid);
            CByteStream cData = pcTxnReq->GenVoidJsonData(pcUi->GetGatewayId());
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        } // Return
        else if (PtInRect_(lX1, lY1, btnReturn_) && PtInRect_(lX2, lY2, btnReturn_))
        {
            pcUi->SetTxnType(kTxnReturn);
            pcUi->SwitchUi(Ui_Select_Card_Type);
        }
        else if (PtInRect_(lX1, lY1, btnReturnIdle_) && PtInRect_(lX2, lY2, btnReturnIdle_))
        {
            pcUi->SwitchUi(Ui_Idle);
        }
    }
    else if (Ui_Select_Card_Type == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnCreditCard_) && PtInRect_(lX2, lY2, btnCreditCard_))
        {
            pcUi->SetCardType(kCredit);
            pcUi->SwitchUi(Ui_Input_Amount);
        }
        else if (PtInRect_(lX1, lY1, btnDebitCard_) && PtInRect_(lX2, lY2, btnDebitCard_))
        {
            pcUi->SetCardType(kDebit);
            pcUi->SwitchUi(Ui_Input_Amount);
        }
        else if (PtInRect_(lX1, lY1, btnCancelSelCardType_) && PtInRect_(lX2, lY2, btnCancelSelCardType_))
        {
            pcUi->SetCardType(kUnknownType);
            pcUi->SwitchUi(Ui_Idle);
        }
    }
    else if (Ui_Select_Polling_Card_Type == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnCreditCard_) && PtInRect_(lX2, lY2, btnCreditCard_))
        {
            pcUi->SetCardType(kCredit);
            pcUi->SwitchUi(Ui_Post_Polling_Card_Cmd);
        }
        else if (PtInRect_(lX1, lY1, btnDebitCard_) && PtInRect_(lX2, lY2, btnDebitCard_))
        {
            pcUi->SetCardType(kDebit);
            pcUi->SwitchUi(Ui_Post_Polling_Card_Cmd);
        }
        else if (PtInRect_(lX1, lY1, btnCancelSelCardType_) && PtInRect_(lX2, lY2, btnCancelSelCardType_))
        {
            pcUi->SetCardType(kUnknownType);
            pcUi->SwitchUi(Ui_Idle);
        }
    }
    else if (Ui_Select_Link_Type == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnEthernet_) && PtInRect_(lX2, lY2, btnEthernet_))
        {
            pcUi->SwitchUi(Ui_Start_Use_Eth_Init);
        }
        else if (PtInRect_(lX1, lY1, btnGprs_) && PtInRect_(lX2, lY2, btnGprs_))
        {
            CTOS_LCDTClearDisplay();
            pcUi->SwitchUi(Ui_Input_APN_Name);
        }
    }
    else if (Ui_Select_Sim_Slot == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnSimSlot1_) && PtInRect_(lX2, lY2, btnSimSlot1_))
        {
            pcUi->SetSimSlot(0);
            pcUi->SwitchUi(Ui_Start_Use_Gprs_Init);
        }
        else if (PtInRect_(lX2, lY2, btnSimSlot2_) && PtInRect_(lX1, lY1, btnSimSlot2_))
        {
            pcUi->SetSimSlot(1);
            pcUi->SwitchUi(Ui_Start_Use_Gprs_Init);
        }
    }
    else if (Ui_Connecting_Host_New == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnConnNetworkCancel_) && PtInRect_(lX2, lY2, btnConnNetworkCancel_))
        {
            CByteStream cData = pcTxnReq->GenCancelJsonData();
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        }
    }
    else if (Ui_Init_Ethernet_Failed == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnContinue_) && PtInRect_(lX2, lY2, btnContinue_))
        {
            pcUi->SwitchUi(Ui_Start_Use_Eth_Init);
        }
        else if (PtInRect_(lX2, lY2, btnRestart_) && PtInRect_(lX1, lY1, btnRestart_))
        {
            pcUi->SwitchUi(Ui_Exit_App);
        }
    }
    else if (Ui_Init_Gprs_Failed == pcUi->GetCurUiIndex())
    {
        if (PtInRect_(lX1, lY1, btnContinue_) && PtInRect_(lX2, lY2, btnContinue_))
        {
            pcUi->SwitchUi(Ui_Start_Use_Gprs_Init);
        }
        else if (PtInRect_(lX2, lY2, btnRestart_) && PtInRect_(lX1, lY1, btnRestart_))
        {
            pcUi->SwitchUi(Ui_Exit_App);
        }
    }
    // else if (Ui_Idle == pcUi->GetCurUiIndex())
    // {
    //     if (PtInRect_(lX1, lY1, btnPollineCard_) || PtInRect_(lX2, lY2, btnPollineCard_))
    //     {
    //         pcUi->SwitchUi(Ui_Select_Polling_Card_Type);
    //     }
    //     else if (PtInRect_(lX1, lY1, btnManualType_) || PtInRect_(lX2, lY2, btnManualType_))
    //     {
    //         pcUi->SwitchUi(Ui_Manual_Sel_Txn_Type);
    //     }
    // }
    else if (Ui_Confirm_Select_Aid == pcUi->GetCurUiIndex())
    {
        CByteStream cData;
        if (PtInRect_(lX1, lY1, btnCancel_) && PtInRect_(lX2, lY2, btnCancel_))
        {
            cData = pcTxnReq->GenAIDConfirmJsonData(FALSE);
        }
        else if (PtInRect_(lX1, lY1, btnEnter_) && PtInRect_(lX2, lY2, btnEnter_))
        {
            cData = pcTxnReq->GenAIDConfirmJsonData(TRUE);
        }

        TraceMsg("Json Data: \n%s", cData.IsEmpty()?"empty":(char *)cData.GetBuffer());
        if (!cData.IsEmpty())
        {
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        }
    }
    else if (Ui_Interac_Receipt_Warning == pcUi->GetCurUiIndex())
    {
        CByteStream cData;
        if (PtInRect_(lX1, lY1, btnCancel_) && PtInRect_(lX2, lY2, btnCancel_))
        {
            cData = pcTxnReq->GenInteracReceiptWarningJsonData(FALSE);
        }
        else if (PtInRect_(lX1, lY1, btnEnter_) && PtInRect_(lX2, lY2, btnEnter_))
        {
            cData = pcTxnReq->GenInteracReceiptWarningJsonData(TRUE);
        }

        TraceMsg("Json Data: \n%s", cData.IsEmpty()?"empty":(char *)cData.GetBuffer());
        if (!cData.IsEmpty())
        {
            PostRequest((char *)cData.PushZero(), cData.GetLength());
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        }
    }
    else if (Ui_Select_Aid == pcUi->GetCurUiIndex())
    {
        int nAppNum;
        char szAppLabel[10][17] = { 0 };
        nAppNum = pcUi->GetAidList(szAppLabel);
        TraceMsg("nAppNum: [%d]", nAppNum);
        
        int nSelectID = 0;
        if (PtInRect_(lX1, lY1, btnFirst_) && PtInRect_(lX2, lY2, btnFirst_))
        {
            nSelectID = 1; 
        } 
        else if (PtInRect_(lX1, lY1, btnSecond_) && PtInRect_(lX2, lY2, btnSecond_))
        {
            nSelectID = 2; 
        } 
        else if (PtInRect_(lX1, lY1, btnThird_) && PtInRect_(lX2, lY2, btnThird_))
        {
            nSelectID = 3; 
        } 
        else if (PtInRect_(lX1, lY1, btnFourth_) && PtInRect_(lX2, lY2, btnFourth_))
        {
            nSelectID = 4; 
        } 
        else if (PtInRect_(lX1, lY1, btnFifth_) && PtInRect_(lX2, lY2, btnFifth_))
        {
            nSelectID = 5; 
        } 
        else if (PtInRect_(lX1, lY1, btnSixth_) && PtInRect_(lX2, lY2, btnSixth_))
        {
            nSelectID = 6; 
        }
        TraceMsg("nSelectID: [%d]", nSelectID);

        if (nSelectID > 0 && nSelectID <= nAppNum)
        {
            CByteStream cData;
            cData = pcTxnReq->GenAIDSelectJsonData(nSelectID);

            PostRequest((char *)cData.PushZero(), cData.GetLength());

            CTOS_LCDTClearDisplay();
            pcUi->SwitchUi(Ui_After_Call_PostRequest);
        }
    }
    else if(Ui_Select_Language == pcUi->GetCurUiIndex())
    {
        CByteStream cData;
        if (PtInRect_(lX1, lY1, btnEnglish_) && PtInRect_(lX2, lY2, btnEnglish_))
        {
            cData = pcTxnReq->GenLanguageSelectJsonData((BYTE *)"ENGLISH");
            TraceMsg("Select language: [English]");
            pcUi->SetLanguage(kEnglish);
        }
        else if (PtInRect_(lX1, lY1, btnFrench_) && PtInRect_(lX2, lY2, btnFrench_))
        {
            cData = pcTxnReq->GenLanguageSelectJsonData((BYTE *)"FRENCH");
            TraceMsg("Select language: [French]");
            pcUi->SetLanguage(kFrench);
        }
        
        PostRequest((char *)cData.PushZero(), cData.GetLength());
        pcUi->SwitchUi(Ui_After_Call_PostRequest);
    }
//    else if(Ui_Switch_Language == pcUi->GetCurUiIndex())
//    {
//        TraceMsg("CurUiIndex is Ui_Switch_Language");
//        CByteStream cData;
//        cData = pcTxnReq->GenLanguageSwitchJsonData((BYTE *)"French");
//        
//        if(pcUi->GetLanguage() == kEnglish)
//        {
//            pcUi->SetLanguage(kFrench);
//            TraceMsg("Switch language to: [French]");
//        }
//        
//        PostRequest((char *)cData.PushZero(), cData.GetLength());
//        pcUi->SwitchUi(Ui_After_Call_PostRequest);
//    }
    else if(Ui_Txn_Interac_Account_Type == pcUi->GetCurUiIndex())
    {
        CByteStream cData;
        if (PtInRect_(lX1, lY1, btnManualType_) && PtInRect_(lX2, lY2, btnManualType_))
        {
            cData = pcTxnReq->GenInteracAccountType((BYTE *)"SAVINGS");
        }
        else if (PtInRect_(lX1, lY1, btnPollineCard_) && PtInRect_(lX2, lY2, btnPollineCard_))
        {
            cData = pcTxnReq->GenInteracAccountType((BYTE *)"CHECKING");
        }

        PostRequest((char *)cData.PushZero(), cData.GetLength());
        pcUi->SwitchUi(Ui_After_Call_PostRequest);
    }
    // else if(Ui_Txn_Interac_POS_SeqNbr == pcUi->GetCurUiIndex())
    // {
    //     TraceMsg("Ui_Txn_Interac_POS_SeqNbr");

    //     char szPosSeqNbr[16] = { 0 };
    //     CTOS_RTC rtc = { 0 };
    //     CTOS_RTCGet(&rtc);
    //     sprintf(szPosSeqNbr, "%X%02d%02d%02d%02d", rtc.bMonth, rtc.bDay, rtc.bHour, rtc.bMinute, rtc.bSecond);

    //     CByteStream cData = pcTxnReq->GenInteracPOSSeqNbr((BYTE *)szPosSeqNbr);

    //     PostRequest((char *)cData.PushZero(), cData.GetLength());
    //     pcUi->SwitchUi(Ui_After_Call_PostRequest);
    // }
}

/*
 * Private.
 */
static BOOL IsEnableTouch(void)
{
    pthread_mutex_lock(&cTouchMutex_);
    BOOL bEnable = bEnableTouch_;
    pthread_mutex_unlock(&cTouchMutex_);
    return bEnable;
}

/*
 * Private.
 */
static void NormalUiMode_(CUi *pcUi, CTxnRequest *pcTxnReq)
{
    if (IsEnableTouch())
    {
        long lX1, lY1, lX2, lY2;
        const int nCurUiIndex = pcUi->GetCurUiIndex();

        if (nCurUiIndex == Ui_Idle ||
            nCurUiIndex == Ui_Select_Card_Type ||
            nCurUiIndex == Ui_Polling_Card ||
            nCurUiIndex == Ui_Select_Link_Type ||
            nCurUiIndex == Ui_Select_Sim_Slot ||
            nCurUiIndex == Ui_Connecting_Host_New ||
            nCurUiIndex == Ui_Init_Ethernet_Failed ||
            nCurUiIndex == Ui_Init_Gprs_Failed ||
            nCurUiIndex == Ui_Manual_Sel_Txn_Type ||
            nCurUiIndex == Ui_Select_Polling_Card_Type ||
            nCurUiIndex == Ui_Confirm_Select_Aid ||
            nCurUiIndex == Ui_Select_Aid ||
            nCurUiIndex == Ui_Txn_Interac_Account_Type || 
            nCurUiIndex == Ui_Select_Language ||
            nCurUiIndex == Ui_Interac_Receipt_Warning)
        {
            BOOL bRet = cTouch_.TouchGetPosition(lX1, lY1, lX2, lY2);
            if (bRet)
            {
                TraceMsg("Info: X1=%ld, Y1=%ld, X2=%ld, Y2=%ld", lX1, lY1, lX2, lY2);
                processTouchEvent_(pcUi, pcTxnReq, lX1, lY1, lX2, lY2);
            }
        }
    }
}

/*
 * Private.
 */
static void Rs232UiMode_(CUi *pcUi, CTxnRequest *pcTxnReq)
{
    long lPt1X, lPt1Y, lPt2X, lPt2Y;
    if (Ui_Polling_Card == pcUi->GetCurUiIndex() ||
        Ui_Connecting_Host_New == pcUi->GetCurUiIndex() ||
        Ui_Init_Ethernet_Failed == pcUi->GetCurUiIndex() ||
        Ui_Init_Gprs_Failed == pcUi->GetCurUiIndex())
    {
        BOOL bRet;

        bRet = cTouch_.TouchGetPosition(lPt1X, lPt1Y, lPt2X, lPt2Y);
        if (bRet)
        {
            TraceMsg("Info: Pt1X=%ld, Pt1Y=%ld, Pt2X=%ld, Pt2Y=%ld", lPt1X, lPt2Y, lPt2X, lPt2Y);
            processTouchEvent_(pcUi, pcTxnReq, lPt1X, lPt1Y, lPt2X, lPt2Y);
        }
    }
    else
    {
        int i = 0;
        while (i < 125)
        {
            BOOL bRet = cTouch_.TouchGetPosition(lPt1X, lPt1Y, lPt2X, lPt2Y);
            if (!bRet)
            {
                if (Ui_Polling_Card == pcUi->GetCurUiIndex() ||
                    Ui_Connecting_Host_New == pcUi->GetCurUiIndex() ||
                    Ui_Init_Ethernet_Failed == pcUi->GetCurUiIndex())
                {
                    break;
                }
                ++i;
            }
            usleep(5);
        }
    }
}

/*
 * Private.
 */
static void *touchThread_(void *pVoid)
{
    pthread_detach(pthread_self());
    cTouch_.TouchInit();
    CUi *pcUi = getUiModuleObj();
    CTxnRequest *pcTxnReq = getTxnReqModuleObj();

    while (NULL != pcUi && 
           NULL != pcTxnReq && 
           Ui_Exit_App != pcUi->GetCurUiIndex())
    {
        if (kNormal == pcUi->GetUiMode())
        {
            NormalUiMode_(pcUi, pcTxnReq);
        }
        else if (kRs232Mode == pcUi->GetUiMode())
        {
            Rs232UiMode_(pcUi, pcTxnReq);
        }
        usleep(Nop);
    }
    
    cTouch_.TouchClose();
    pthread_mutex_destroy(&cTouchMutex_); // Free mutex resource.
    return NULL;
}

/*
 * Public.
 */
extern void InitTouchThread(void)
{
    if (-1 == cThreadId_)
    {
        int re;

        re = pthread_create(&cThreadId_, NULL, touchThread_, NULL);
        if (re == -1)
        {
            TraceMsg("Error: pthread_create() failed.");
            return;
        }

        pthread_mutex_init(&cTouchMutex_, NULL);
    }
}

/*
 * Public.
 */
extern void EnableTouch(const BOOL &bEnable)
{
    pthread_mutex_lock(&cTouchMutex_);
    bEnableTouch_ = bEnable;
    if (bEnableTouch_)
    {
        int i = 0;
        while (i < 125)
        {
            long lX1, lY1, lX2, lY2;
            BOOL bRet = cTouch_.TouchGetPosition(lX1, lY1, lX2, lY2);
            if (!bRet)
                ++i;
        }
    }
    pthread_mutex_unlock(&cTouchMutex_);
}
