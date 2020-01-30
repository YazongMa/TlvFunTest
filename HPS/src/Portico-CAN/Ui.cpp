
/*
 * File Name: Ui.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.19
 */

#include <ctosapi.h>
#include <unistd.h>
#include <string.h>

#include "ui.h"
#include "dbghlp.h"
#include "AppMain.h"
#include "Inc/deftype.h"
#include "Inc/TxnInterface.h"
#include "libVKBD.h"
#include "TxnRequest.h"
#include "TxnJsonKey.h"
#include "GprsLink.h"
#include "ErrorCode.h"
#include "Rs232Link.h"
#include "Receipt.h"

#define     Ui_Trd_Nop         (10 * 1000)
#define     Ui_X_Offset        (0)
#define     Ui_Y_Offset        (0)

extern CTxnRequest* getTxnReqModuleObj(void);
extern void EnableTouch(const BOOL &bEnable);
extern void clearCachePoints(DWORD dwMilliseconds);
extern void InitRspThread(void);
extern WORD OpenEthLink(void);
static CGprsLink g_cGprsLink_;
static WORD InitGprsLink_(void);
static WORD CloseGprsLink_(void);
static USHORT CTAP_LCDBackGndColor(BOOL boReverse);
/*
 * Local private.
 */
static const struct
{
    const int nUiIndex;
    const char *pszBmpFilePath;
} cBmpFileBuf_En[] =
{
//    { Ui_Idle, "./Bmp/English/Idle.bmp" },
    { Ui_Idle, "./Bmp/English/SelOpTxnType.bmp" },
    { Ui_Parsing_Json_Data, "./Bmp/English/pj01.bmp\0./Bmp/English/pj02.bmp\0./Bmp/English/pj03.bmp\0./Bmp/English/pj04.bmp\0\0" },
    { Ui_Init_Ethernet_Link, "./Bmp/English/EthInit_0.bmp\0./Bmp/English/EthInit_1.bmp\0./Bmp/English/EthInit_2.bmp\0./Bmp/English/EthInit_3.bmp\0./Bmp/English/EthInit_4.bmp\0\0" },
    { Ui_Init_Gprs_Link, "./Bmp/English/GprsInit_0.bmp\0./Bmp/English/GprsInit_1.bmp\0./Bmp/English/GprsInit_2.bmp\0./Bmp/English/GprsInit_3.bmp\0./Bmp/English/GprsInit_4.bmp\0\0" },
    { Ui_Polling_Card, "./Bmp/English/02.bmp" },
    { Ui_Init_Ethernet_Failed, "./Bmp/English/InitEthFail.bmp" },
    { Ui_Init_Gprs_Failed, "./Bmp/English/InitGprsFail.bmp" },
    { Ui_Polling_Card_Timeout, "./Bmp/English/01.bmp" },
    { Ui_Connecting_Host, "./Bmp/English/p1.bmp\0./Bmp/English/p2.bmp\0./Bmp/English/p3.bmp\0./Bmp/English/p4.bmp\0\0" },
    { Ui_Connecting_Host_New, "./Bmp/English/p1_New.bmp\0./Bmp/English/p2_New.bmp\0./Bmp/English/p3_New.bmp\0./Bmp/English/p4_New.bmp\0\0" },
    { Ui_Txn_Approval, "./Bmp/English/04.bmp" },
    { Ui_Txn_Decline, "./Bmp/English/05.bmp" },
    { Ui_Initialing, "./Bmp/English/Init01.bmp\0./Bmp/English/Init02.bmp\0./Bmp/English/Init03.bmp\0./Bmp/English/Init04.bmp\0\0" },
    { Ui_Packing_Data, "./Bmp/English/packing01.bmp\0./Bmp/English/packing02.bmp\0./Bmp/English/packing03.bmp\0./Bmp/English/packing04.bmp\0\0" },
    { Ui_Entering_PIN, "./Bmp/English/12.bmp" },
    { Ui_Entering_PIN_Again, "./Bmp/English/38.bmp" },
    { Ui_Last_PIN_Input, "./Bmp/English/32.bmp" },
    { Ui_Start_Get_PIN, "./Bmp/English/12.bmp" },
    { Ui_PIN_Error, "./Bmp/English/14.bmp" },
    { Ui_Select_Card_Type, "./Bmp/English/SelCard.bmp"},
    { Ui_Select_Polling_Card_Type, "./Bmp/English/SelCard.bmp"},
    { Ui_Select_Link_Type, "./Bmp/English/SelLinkType.bmp"},
    { Ui_Select_Sim_Slot, "./Bmp/English/SelSim.bmp"},
    { Ui_Welcome_Page, "./Bmp/English/wlcm.bmp" },
    { Ui_Cancel, "./Bmp/English/CancelDeal.bmp" },
    { Ui_Pls_Remove_Card, "./Bmp/English/RmCard.bmp" },
    { Ui_Manual_Sel_Txn_Type, "./Bmp/English/SelOpTxnType.bmp" },
    { Ui_Select_Aid, "./Bmp/English/UserSelAid.bmp" },
    { Ui_Confirm_Select_Aid, "./Bmp/English/UserConfirmAid_2.bmp" },
    { Ui_Get_Total_Amt, "./Bmp/English/TotalAmt.bmp" },
    { Ui_Chk_Exp_File, "./Bmp/English/ChkExpFile.bmp" },
    { Ui_Call_Your_Bank, "./Bmp/English/CallYourBank.bmp" },
    { Ui_Plz_Insert_SC_Card, "./Bmp/English/PlzInsertSc.bmp" },
    { Ui_Plz_Swipe_MSR_Card, "./Bmp/English/PlzSwiptMsr.bmp" },
    { Ui_Plz_Tap_Card, "./Bmp/English/PlzTapCard.bmp" },
    { Ui_More_Card_Detected, "./Bmp/English/MoreCard.bmp" },
    { Ui_Plz_See_Phone, "./Bmp/English/PlzSeePhone.bmp" },
    { Ui_Txn_Offline_Approval, "./Bmp/English/04.bmp" },
    { Ui_Txn_Offline_Decline, "./Bmp/English/05.bmp" },
    { Ui_Txn_Interac_Account_Type, "./Bmp/English/InteracAccountType.bmp" },
    { Ui_Select_Language, "./Bmp/English/select language.bmp" },
};

static const struct
{
    const int nUiIndex;
    const char *pszBmpFilePath;
} cBmpFileBuf_Fr[] =
{
//    { Ui_Idle, "./Bmp/French/Idle.bmp" },
    { Ui_Idle, "./Bmp/French/SelOpTxnType.bmp" },
    { Ui_Parsing_Json_Data, "./Bmp/French/pj01.bmp\0./Bmp/French/pj02.bmp\0./Bmp/French/pj03.bmp\0./Bmp/French/pj04.bmp\0\0" },
    { Ui_Init_Ethernet_Link, "./Bmp/French/EthInit_0.bmp\0./Bmp/French/EthInit_1.bmp\0./Bmp/French/EthInit_2.bmp\0./Bmp/French/EthInit_3.bmp\0./Bmp/French/EthInit_4.bmp\0\0" },
    { Ui_Init_Gprs_Link, "./Bmp/French/GprsInit_0.bmp\0./Bmp/French/GprsInit_1.bmp\0./Bmp/French/GprsInit_2.bmp\0./Bmp/French/GprsInit_3.bmp\0./Bmp/French/GprsInit_4.bmp\0\0" },
    { Ui_Polling_Card, "./Bmp/French/02.bmp" },
    { Ui_Init_Ethernet_Failed, "./Bmp/French/InitEthFail.bmp" },
    { Ui_Init_Gprs_Failed, "./Bmp/French/InitGprsFail.bmp" },
    { Ui_Polling_Card_Timeout, "./Bmp/French/01.bmp" },
    { Ui_Connecting_Host, "./Bmp/French/p1.bmp\0./Bmp/French/p2.bmp\0./Bmp/French/p3.bmp\0./Bmp/French/p4.bmp\0\0" },
    { Ui_Connecting_Host_New, "./Bmp/French/p1_New.bmp\0./Bmp/French/p2_New.bmp\0./Bmp/French/p3_New.bmp\0./Bmp/French/p4_New.bmp\0\0" },
    { Ui_Txn_Approval, "./Bmp/French/04.bmp" },
    { Ui_Txn_Decline, "./Bmp/French/05.bmp" },
    { Ui_Initialing, "./Bmp/French/Init01.bmp\0./Bmp/French/Init02.bmp\0./Bmp/French/Init03.bmp\0./Bmp/French/Init04.bmp\0\0" },
    { Ui_Packing_Data, "./Bmp/French/packing01.bmp\0./Bmp/French/packing02.bmp\0./Bmp/French/packing03.bmp\0./Bmp/French/packing04.bmp\0\0" },
    { Ui_Entering_PIN, "./Bmp/French/12.bmp" },
    { Ui_Entering_PIN_Again, "./Bmp/French/38.bmp" },
    { Ui_Last_PIN_Input, "./Bmp/French/32.bmp" },
    { Ui_Start_Get_PIN, "./Bmp/French/12.bmp" },
    { Ui_PIN_Error, "./Bmp/French/14.bmp" },
    { Ui_Select_Card_Type, "./Bmp/French/SelCard.bmp"},
    { Ui_Select_Polling_Card_Type, "./Bmp/French/SelCard.bmp"},
    { Ui_Select_Link_Type, "./Bmp/French/SelLinkType.bmp"},
    { Ui_Select_Sim_Slot, "./Bmp/French/SelSim.bmp"},
    { Ui_Welcome_Page, "./Bmp/French/wlcm.bmp" },
    { Ui_Cancel, "./Bmp/French/CancelDeal.bmp" },
    { Ui_Pls_Remove_Card, "./Bmp/French/RmCard.bmp" },
    { Ui_Manual_Sel_Txn_Type, "./Bmp/French/SelOpTxnType.bmp" },
    { Ui_Select_Aid, "./Bmp/French/UserSelAid.bmp" },
    { Ui_Confirm_Select_Aid, "./Bmp/French/UserConfirmAid_2.bmp" },
    { Ui_Get_Total_Amt, "./Bmp/French/TotalAmt.bmp" },
    { Ui_Chk_Exp_File, "./Bmp/French/ChkExpFile.bmp" },
    { Ui_Call_Your_Bank, "./Bmp/French/CallYourBank.bmp" },
    { Ui_Plz_Insert_SC_Card, "./Bmp/French/PlzInsertSc.bmp" },
    { Ui_Plz_Swipe_MSR_Card, "./Bmp/French/PlzSwiptMsr.bmp" },
    { Ui_Plz_Tap_Card, "./Bmp/French/PlzTapCard.bmp" },
    { Ui_More_Card_Detected, "./Bmp/French/MoreCard.bmp" },
    { Ui_Plz_See_Phone, "./Bmp/French/PlzSeePhone.bmp" },
    { Ui_Txn_Offline_Approval, "./Bmp/French/04.bmp" },
    { Ui_Txn_Offline_Decline, "./Bmp/French/05.bmp" },
    { Ui_Txn_Interac_Account_Type, "./Bmp/French/InteracAccountType.bmp" },
    { Ui_Select_Language, "./Bmp/French/select language.bmp" },
};

/*
 * Public.
 * Construct function.
 */
CUi::CUi(): m_nCurUiIndex(Ui_Invalid_Index),
m_nTxnType(kTxnInvalid),
m_nCardType(kUnknownType),
m_pszAPN(NULL),
m_pszID(NULL),
m_pszPwd(NULL),
m_pszGatewayId(NULL),
m_bContinue(TRUE),
m_cInitEthTrdId(-1),
m_cInitGprsTrdId(-1),
m_nLanguage(kEnglish)
{ 
    strcpy(m_pszPOSSequenceNbr, "000010010010");
}

/*
 * Public.
 */
CUi::~CUi()
{
    if (NULL != m_pszAPN)
    {
        free((void *)m_pszAPN);
        m_pszAPN = NULL;
    }
    if (NULL != m_pszID)
    {
        free((void *)m_pszID);
        m_pszID = NULL;
    }
    if (NULL != m_pszPwd)
    {
        free((void *)m_pszPwd);
        m_pszPwd = NULL;
    }
    if (NULL != m_pszGatewayId)
    {
        free((void *)m_pszGatewayId);
        m_pszGatewayId = NULL;
    }

    SetContinue(FALSE);
    pthread_join(m_cUiThreadId, NULL); // Wait thread exit.

    pthread_mutex_destroy(&m_cMutex);
}

/*
 * Public.
 */
void CUi::SetGatewayId(const char *pszId)
{
    if (m_pszGatewayId != NULL)
    {
        free((void *)m_pszGatewayId);
        m_pszGatewayId = NULL;
    }
    m_pszGatewayId = strdup(pszId);
}

void CUi::SetAidList(IN char szAppLabel[][17], IN int nAppNum)
{
    TraceMsg("SetAidList Entry, nAppNum:%d", nAppNum);
    memset(m_szAppLabel, 0, sizeof(char) * 10 * 17);
    memcpy(m_szAppLabel, szAppLabel, sizeof(char) * 17 * nAppNum);
    
    m_nAppNum = nAppNum;
    TraceMsg("SetAidList Exit");
}

void CUi::SetAidConfirm(IN char* pszLabel)
{
    TraceMsg("SetAidConfirm Entry");
    memset(m_szLabel, 0, sizeof(m_szLabel));
    if(strlen(pszLabel) < 17)
        memcpy(m_szLabel, pszLabel, strlen(pszLabel));
    else
        memcpy(m_szLabel, pszLabel, 16);
    TraceMsg("SetAidConfirm Exit");
}


/*
 * Public.
 * [ST]
 */
int CUi::GetUiMode(void)
{
    pthread_mutex_lock(&m_cMutex);
    const int nMode = m_nUiMode;
    pthread_mutex_unlock(&m_cMutex);
    return nMode;
}

/*
 * Public.
 */
BOOL CUi::InitUiModule(int nMode)
{
    int re = 0;
    
    re = pthread_mutex_init(&m_cMutex, NULL);
    if (re == -1)
    {
        TraceMsg("Error: pthread_mutex_init() failed.");
        return FALSE;
    }

    if (kNormal == nMode)
    {
        re = pthread_create(&m_cUiThreadId, NULL,
                            uiModuleThread, (void *)this);
        if (re == -1)
        {
            TraceMsg("Error: pthread_create() failed.");
            return FALSE;
        }
        m_nUiMode = kNormal;
    }
    else if (kRs232Mode == nMode)
    {
        TraceMsg("Rs232 mode");
        re = pthread_create(&m_cUiThreadId, NULL, 
                            uiModuleTrd_Rs232Mode, (void *)this);
        if (-1 == re)
        {
            TraceMsg("Error: pthread_create() failed.");
            return FALSE;
        }
        TraceMsg("create rs232 thread sucess");
        m_nUiMode = kRs232Mode;
    }
    else
    {
        TraceMsg("Error: Parameter invalid.");
        return FALSE;
    }
    return TRUE;
}

/*
 * Public.
 */
void CUi::SwitchUi(const int &uiIndex)
{
    TraceMsg("Info: UI %X ==> %X", m_nCurUiIndex, uiIndex);
    pthread_mutex_lock(&m_cMutex);
    if (Ui_Exit_App != m_nCurUiIndex)
    {
        m_nCurUiIndex = uiIndex;
    }
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Public.
 */
const int CUi::GetCurUiIndex(void)
{
    pthread_mutex_lock(&m_cMutex);
    const int nIndex = m_nCurUiIndex;
    pthread_mutex_unlock(&m_cMutex);

    return UiIndexMap(nIndex);
}

/*
 * Private
 */
int CUi::UiIndexMap(const int &nUiIndex)
{
    const int nCardType = GetCardType();
    const int nTxnType = GetCurTxnType();
    int nNewUiMode = nUiIndex;

    if (kRs232Mode == GetUiMode())
    {
        switch (nUiIndex)
        {
        case Ui_Idle:
        {
            nNewUiMode = Ui_Welcome_Page;
        }
        break;

        case Ui_Start_Use_Eth_Init:
        case Ui_Start_Use_Gprs_Init:
        {
            nNewUiMode = Ui_Start_New_Init;
        }
        break;

        default:
            break;
        }
    }
    else if (kNormal == GetUiMode())
    {
        switch (nUiIndex)
        {
        case Ui_Start_Use_Eth_Init:
        case Ui_Start_Use_Gprs_Init:
        {
            nNewUiMode = Ui_Start_New_Init;
        }
        break;

        default:
            break;
        }
    }

    if ((kTxnPreAuth == nTxnType) ||
        (kTxnPollingCard == nTxnType) ||
        ((kCredit == nCardType) && (kTxnSale == nTxnType || kTxnReturn == nTxnType)))
    {
        switch (nNewUiMode)
        {
            case Ui_Connecting_Host:
                nNewUiMode = Ui_Connecting_Host_New;
                break;

            default:
                break;
        }
    }
    return nNewUiMode;
}

/*
 * Private
 */
void CUi::ShowUi(const char *pszMultiBmpPath, const int &nOffset)
{
    if (NULL != pszMultiBmpPath)
    {
        for (int i=0; i < nOffset; ++i)
        {
            pszMultiBmpPath += (strlen(pszMultiBmpPath) + 1);
            if (0 == strlen(pszMultiBmpPath))
                break;
        }
        if (0 != strlen(pszMultiBmpPath))
        {
            ShowBmp(pszMultiBmpPath);
        }
    }
}

/*
 * Private static function.
 */
void* CUi::uiModuleThread(void *pVoid)
{
    CUi *pUi = reinterpret_cast<CUi *>(pVoid);
    CTxnRequest *pcTxnReq = getTxnReqModuleObj();
    ULONG ulTickStart = 0;
    int nAmt = 0;
    char szGatewayId[32] = { 0 };
    BYTE byGatewayIdLength = 0;

    if (NULL == pUi)
    {
        TraceMsg("Error: Ui module initialize failed.");
    }
    while (NULL != pUi && pUi->IsContinue())
    {
        const int nCurUiIndex = pUi->GetCurUiIndex();
        static int nLastIndex = Ui_Invalid_Index;

        switch (nCurUiIndex)
        {
            case Ui_Load_Def_Link_Type:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    pUi->LoadDefLinkType();
                }
            }
            break;

            case Ui_Start_New_Init:
                if (nLastIndex != nCurUiIndex)
                {
                    TraceMsg("Init thread");
                    CReceipt::InitAdditionalTag(Cfg_Txn_Init_Path);
                    CByteStream cData = pcTxnReq->GenTxnInitJsonData(Cfg_Txn_Init_Path);
                    PostRequest((char *)cData.PushZero(), cData.GetLength());
                    pUi->SwitchUi(Ui_After_Call_PostRequest);
                    // TraceMsg("Info: New init json data:\r\n%s", (char *)cData.PushZero());
                }
                break;

            case Ui_Parsing_Json_Data:
            case Ui_Initialing:
            case Ui_Connecting_Host:
            case Ui_Connecting_Host_New:
            case Ui_Packing_Data:
            {
                const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 4;
                pUi->ShowUi(pszBmpFilePath, nOffset);
            }
            break;

            case Ui_Select_Card_Type:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    if (d_OK != pUi->ShowSelectCardType())
                    {
                        pUi->SwitchUi(Ui_Idle);
                    }
                }
            }
            break;

            case Ui_Post_Polling_Card_Cmd:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    pUi->ResetSequenceId();
                    CByteStream cData = pcTxnReq->GenPollingCardJsonData((kCardType)pUi->GetCardType());
                    PostRequest((char *)cData.PushZero(), cData.GetLength());
                }
            }
            break;

            case Ui_Init_Ethernet_Link:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    pUi->InitEthLink();
                }
                else
                {
                    const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                    const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 5;
                    pUi->ShowUi(pszBmpFilePath, nOffset);
                }
            }
            break;

            case Ui_Init_Gprs_Link:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    pUi->InitGprsLink();
                }
                else
                {
                    const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                    const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 5;
                    pUi->ShowUi(pszBmpFilePath, nOffset);
                }
            }
            break;

            case Ui_Input_APN_Name:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    CByteStream cData;
                    if (d_KBD_ENTER != pUi->GetInputString((BYTE *)"Plz Input APN", cData))
                    {
                        pUi->SwitchUi(Ui_Select_Link_Type);
                    }
                    else
                    {
                        pUi->SaveAPNName(cData.PushZero());
                        pUi->SwitchUi(Ui_Input_APN_Id);
                    }
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Input_APN_Id:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    CByteStream cData;
                    if (pUi->GetInputString((BYTE *)"Plz Input ID", cData) != d_KBD_ENTER)
                    {
                        pUi->SwitchUi(Ui_Select_Link_Type);
                    }
                    else
                    {
                        pUi->SaveID(cData.PushZero());
                        pUi->SwitchUi(Ui_Input_Pwd);
                    }
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Input_Pwd:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    CByteStream cData;
                    if (pUi->GetInputString((BYTE *)"Plz Input Pwd", cData) != d_KBD_ENTER)
                    {
                        pUi->SwitchUi(Ui_Select_Link_Type);
                    }
                    else
                    {
                        pUi->SavePwd(cData.PushZero());
                        pUi->SwitchUi(Ui_Select_Sim_Slot);
                    }
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Start_Use_Eth_Init:
            case Ui_Start_Use_Gprs_Init:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    // CByteStream cData = pcTxnReq->GenTxnInitJsonConfig(FALSE);
                    // PostRequest((char *)cData.PushZero(), cData.GetLength());
                    pUi->SwitchUi(Ui_After_Call_PostRequest);
                }
            }
            break;

            case Ui_Input_Amount:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    TraceMsg("Info: User start input amount.");
                    CTOS_LCDTClearDisplay();
                    if (d_KBD_ENTER != pUi->ShowInputAmt(&nAmt))
                    {
                        TraceMsg("Info: User cancel input amount.");
                        pUi->SwitchUi(Ui_Idle);
                    }
                    else
                    {
                        TraceMsg("Info: User finish inputing amount,\r\nstart transaction.");
                        const int nTxnType = pUi->GetCurTxnType();
                        const int nCardType = pUi->GetCardType();
                        CByteStream cData;

                        if (kTxnSale == nTxnType)
                        {
                            cData = pcTxnReq->GenSaleJsonData(nAmt, (kCardType)nCardType);
                        }
                        else if (kTxnPreAuth == nTxnType)
                        {
                            cData = pcTxnReq->GenPreAuthJsonData(nAmt);
                        }
                        // else if (kTxnAuthComplete == nTxnType)
                        // {
                        //     TraceMsg("Info: GatewayID=%s", (char *)pUi->m_pszGatewayId);
                        //     cData = pcTxnReq->GenAuthCompleteJsonData(nAmt, (char *)pUi->m_pszGatewayId);
                        // }
                        else if (kTxnReturn == nTxnType)
                        {
                            cData = pcTxnReq->GenReturnJsonData(nAmt, (kCardType)nCardType);
                        }
                        else if (kTxnPollingCard == nTxnType)
                        {
                            cData = pcTxnReq->GenSaleJsonData(nAmt, (kCardType)nCardType, pUi->m_dbSequenceId);
                        }

                        PostRequest((char *)cData.PushZero(), cData.GetLength());
                        pUi->SwitchUi(Ui_After_Call_PostRequest);
                        TraceMsg("Info: Call PostRequest() to send transaction.");
                    }
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Input_GatewayID:
            {

                if (nLastIndex != nCurUiIndex)
                {
                    byGatewayIdLength = sizeof(szGatewayId);
                    memset(szGatewayId, 0, byGatewayIdLength);
                    if (d_KBD_ENTER != pUi->ShowInputGatewayId(szGatewayId, &byGatewayIdLength))
                    {
                        TraceMsg("Info: User cancel input GatewayTxnId.");
                        pUi->SwitchUi(Ui_Idle);
                    }
                    else
                    {
                        TraceMsg("Info: User finish inputing GatewayTxnId,\r\nstart transaction.");
                        const int nTxnType = pUi->GetCurTxnType();
                        CByteStream cData;
                        if (kTxnAuthComplete == nTxnType)
                        {
                            TraceMsg("Info: GatewayID=%s", (char *)szGatewayId);
                            cData = pcTxnReq->GenAuthCompleteJsonData(nAmt, (char *)szGatewayId);
                        }

                        PostRequest((char *)cData.PushZero(), cData.GetLength());
                        pUi->SwitchUi(Ui_After_Call_PostRequest);
                        TraceMsg("Info: Call PostRequest() to send transaction.");
                    }
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Polling_Card_Success:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    CTOS_LCDTClearDisplay();
                    pUi->SetTxnType(kTxnPollingCard);
                    pUi->SwitchUi(Ui_Input_Amount);
                }
            }
            break;

    //        case Ui_Select_Aid:
    //        case Ui_Confirm_Select_Aid: 
            case Ui_Get_Total_Amt:
            case Ui_Chk_Exp_File:
            case Ui_Call_Your_Bank:
            {
                if (nLastIndex != nCurUiIndex)
                {
                    TraceMsg("Info: Special Ui Index, UiIndex=%#04x", nCurUiIndex);
                }
            }
            break;

            default:
                break;
        }// end of switch

        if (nLastIndex != nCurUiIndex)
        {
            pUi->ShowUi(nCurUiIndex);
            TraceMsg("Info: Show Ui, UiIndex=0x%08X", nCurUiIndex);
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
                nCurUiIndex == Ui_Select_Language)
            {
                TraceMsg("Enable Touch");
                EnableTouch(TRUE);
            }
            else if(nCurUiIndex == Ui_Switch_Language)
            {
                TraceMsg("nCurUiIndex is: Ui_Switch_Language");
                CByteStream cData;
                cData = pcTxnReq->GenLanguageSwitchJsonData((BYTE *)"French");

                if(pUi->GetLanguage() == kEnglish)
                {
                    pUi->SetLanguage(kFrench);
                    TraceMsg("Switch language to: [French]");
                }

                PostRequest((char *)cData.PushZero(), cData.GetLength());
                pUi->SwitchUi(Ui_After_Call_PostRequest);
            }
            else if(nCurUiIndex == Ui_Txn_Interac_POS_SeqNbr)
            {
                TraceMsg("Ui_Txn_Interac_POS_SeqNbr");
                TraceMsg("szPosSeqNbr is %s", pUi->GetPOSSequenceNbr());

                CByteStream cData = pcTxnReq->GenInteracPOSSeqNbr((BYTE *)pUi->GetPOSSequenceNbr());

                PostRequest((char *)cData.PushZero(), cData.GetLength());
                pUi->SwitchUi(Ui_After_Call_PostRequest);
            }
            else
            {
                EnableTouch(FALSE);
            }
            nLastIndex = nCurUiIndex;
            ulTickStart = CTOS_TickGet();
            if (nCurUiIndex == Ui_Idle)
            {
                pUi->SetTxnType(kTxnInvalid); // Reset TXN type.
            }
        }

        // Exit
        if (Ui_Exit_App == nCurUiIndex)
        {
            break;
        }
        pUi->waitUserInputCancel();
        usleep(Ui_Trd_Nop);
        // TraceMsg("Info: Ui Thread.");
    }

    pthread_exit(NULL);
    return NULL;
}

/*
 * Private.
 */
static void parseResponse(const char *pszJsonData, CUi *pcUi)
{
    cJSON *pcJsonRoot = NULL;
    cJSON *pcJsonSubItem = NULL;

    do
    {
        pcJsonRoot = cJSON_Parse(pszJsonData);
        if (NULL == pcJsonRoot)
        {
            TraceMsg("Error: Error Json Data:\n%s", pszJsonData);
            break;
        }

        pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Txn_Ui_Index);
        if (NULL != pcJsonSubItem)
        {
            static int nLastUiIndex = pcUi->GetCurUiIndex();
            if (nLastUiIndex != pcJsonSubItem->valueint)
            {
                pcUi->SwitchUi(pcJsonSubItem->valueint);
                nLastUiIndex = pcJsonSubItem->valueint;
                if (Ui_Polling_Card_Success == nLastUiIndex)
                {
                    pcJsonSubItem = cJSON_GetObjectItem(pcJsonRoot, Txn_SequenceId);
                    if (NULL != pcJsonSubItem)
                    {
                        pcUi->SetSequenceId(pcJsonSubItem->valuedouble);
                        TraceMsg("Info: Ui Index=%d", pcJsonSubItem->valueint);
                        TraceMsg("Info: Resopnse JsonData=\r\n%s", pszJsonData);
                    }
                }
            }
            // TraceMsg("Info: Ui Index=%d", pcJsonSubItem->valueint);
            TraceMsg("Info: Resopnse JsonData=\r\n%s", pszJsonData);
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);
}

void CUi::ExecuteGetResponse(void)
{
    char szJsonRsp[4096] = {0};
    int nBufSize = sizeof(szJsonRsp);

    GetResponse(szJsonRsp, &nBufSize);
    if (nBufSize != 0)
    {
        // TraceMsg("Info: Response Json Data:\n%s", szJsonRsp);
        parseResponse(szJsonRsp, this);
    }
    else
    {
        TraceMsg("Error: Response length=%d", nBufSize);
    }
}

/*
 * Private.
 */
void CUi::ShowUi(const int &nUiIndex)
{
    TraceMsg("Info: %s Entry", __FUNCTION__);
    //display Bmp
    const char *pszBmpFile = GetBmpFilePath(nUiIndex);
    if (NULL != pszBmpFile)
    {
        ShowBmp(pszBmpFile);
    }

    //display text
    BOOL bReverse = TRUE;
    if (Ui_Select_Aid == nUiIndex)
    {
        CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
        CTOS_LCDTSelectFontSize(d_FONT_20x40);
        for (int i=0; i<m_nAppNum; ++i)
        {
            char szLabel[21] = { 0x00 };
            sprintf(szLabel, "%d.%s%*.s", i+1, m_szAppLabel[i], sizeof(szLabel) - 2 - strlen(m_szAppLabel[i]), " ");
            TraceMsg("Info: [%s] [%s] [%d]", szLabel, m_szAppLabel[i], strlen(m_szAppLabel[i]));
            CTAP_LCDBackGndColor(bReverse);
            CTOS_LCDTPrintXY(1, 5+i, (BYTE *)szLabel);
            
            bReverse = !bReverse;
        }
        CTOS_LCDTSelectFontSize(d_FONT_16x30);
        CTOS_LCDBackGndColor(0x00FFFFFF);
    }
    if(Ui_Confirm_Select_Aid == nUiIndex)
    {
        if(!memcmp(m_szLabel, "Interac", strlen("Interac")))
        {
            CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
            CTOS_LCDTSelectFontSize(d_FONT_20x40);
            CTOS_LCDTPrintAligned(5, (unsigned char*)m_szLabel, d_LCD_ALIGNCENTER);
            CTOS_LCDTSelectFontSize(d_FONT_16x30);
        }
    }
    
    TraceMsg("Info: %s Exit", __FUNCTION__);
}

/*
 * Private.
 */
void CUi::ShowBmp(const char *pszBmpFile)
{
    CTOS_LCDGShowBMPPic(Ui_X_Offset, Ui_Y_Offset, (BYTE *)(pszBmpFile));
}

/*
 * Private.
 */
const char *CUi::GetBmpFilePath(const int &nUiIndex)
{
    if(GetLanguage() == kEnglish)//Show English UI
    {
        const int nBufSize = sizeof(cBmpFileBuf_En)/sizeof(cBmpFileBuf_En[0]);

        for (int nIndex=0; nIndex < nBufSize; ++nIndex)
        {
            if (cBmpFileBuf_En[nIndex].nUiIndex == nUiIndex)
            {
                return cBmpFileBuf_En[nIndex].pszBmpFilePath;
            }
        }
    }
    else//Show French UI
    {
        const int nBufSize = sizeof(cBmpFileBuf_Fr)/sizeof(cBmpFileBuf_Fr[0]);

        for (int nIndex=0; nIndex < nBufSize; ++nIndex)
        {
            if (cBmpFileBuf_Fr[nIndex].nUiIndex == nUiIndex)
            {
                return cBmpFileBuf_Fr[nIndex].pszBmpFilePath;
            }
        }
    }
    return NULL;
}

/*
 * Private.
 */
USHORT CUi::ShowInputAmt(int *pnAmt) 
{
    unsigned long long lValue = 0;
    USHORT usRe;

    usRe = GetInputAmount(&lValue, (BYTE *)"Input Amount: ", 4);
    CTOS_LCDTClearDisplay();
    if (d_KBD_ENTER == usRe)
    {
        *pnAmt = (int)lValue;
        return d_KBD_ENTER;
    }
    return usRe;
}

/*
 * Private.
 */
 USHORT CUi::GetInputAmount(unsigned long long *ullAmount, BYTE * pbyLable, IN BYTE byStartY)
{
    BYTE byAmountKeyIn = 0x30;
    BYTE byAmountArrayIndex = 0;
    BOOL bFinshflag = FALSE;
    BYTE byStr[64] = {0};

    *ullAmount = 0;
    CTOS_LCDTPrintXY(1, byStartY, pbyLable);
    sprintf((CHAR *)byStr, "%lld.%02lld       ", (*ullAmount / 100), (*ullAmount % 100));
    CTOS_LCDTPrintXY(8, byStartY + 1, byStr);

    CTAP_VirtualKBDSetDefaultMode(1);
    CTAP_VirtualKBDShow();
    while (bFinshflag == FALSE)
    {
        CTAP_VirtualKBDGet(&byAmountKeyIn);
        switch (byAmountKeyIn)
        {
        case d_KBD_0:
        case d_KBD_1:
        case d_KBD_2:
        case d_KBD_3:
        case d_KBD_4:
        case d_KBD_5:
        case d_KBD_6:
        case d_KBD_7:
        case d_KBD_8:
        case d_KBD_9:
            if (byAmountArrayIndex < 12)
            {
                *ullAmount *= 10;
                *ullAmount += (byAmountKeyIn - 0x30);
                byAmountArrayIndex++;
            }
            else
            {
                CTOS_Beep();
            }
            break;

        case d_KBD_CLEAR:
            if (byAmountArrayIndex > 0)
            {
                *ullAmount /= 10;
                byAmountArrayIndex--;
            }
            break;

        case d_KBD_CANCEL:
            bFinshflag = TRUE;
            break;

        case d_KBD_ENTER:
            //*ullAmount /= 100;
            bFinshflag = TRUE;
            break;

        default:
            break;
        }
        sprintf((CHAR *)byStr, "%lld.%02lld      ", (*ullAmount / 100), (*ullAmount % 100));
        CTOS_LCDTPrintXY(8, byStartY + 1, byStr);
    }

    ClearLines(byStartY, byStartY + 1); 
    CTAP_VirtualKBDHide();

    return byAmountKeyIn;
}



/*
 * Private.
 */
USHORT CUi::ShowInputGatewayId(char* pszGatewayId, BYTE* pbySize) 
{
    BOOL bFinshflag = FALSE;
    BYTE byKeyIn = d_KBD_INVALID;
    BYTE byStr[64] = {0};
    BYTE byIndex = 0;
    
    CTOS_LCDTPrintXY(1, 4, (BYTE *)"Plz Input GatewayId");
    CTAP_VirtualKBDSetDefaultMode(1);
    CTAP_VirtualKBDShow();
    while (bFinshflag == FALSE)
    {
        CTAP_VirtualKBDGet(&byKeyIn);
        switch(byKeyIn)
        {
        case d_KBD_0:
        case d_KBD_1:
        case d_KBD_2:
        case d_KBD_3:
        case d_KBD_4:
        case d_KBD_5:
        case d_KBD_6:
        case d_KBD_7:
        case d_KBD_8:
        case d_KBD_9:
            if (byIndex < 12 && byIndex < *pbySize)
            {
                pszGatewayId[byIndex++] = byKeyIn;
            }
            else
            {
                CTOS_Beep();
            }
            break;

        case d_KBD_CLEAR:
            if (byIndex > 0)
            {
                pszGatewayId[byIndex--] = 0x00;
            }

        case d_KBD_CANCEL:
        case d_KBD_ENTER:
            bFinshflag = TRUE;
            break;
        }
        CTOS_LCDTPrintXY(8, 5, (BYTE *)pszGatewayId);
    }
    CTAP_VirtualKBDHide();
    ClearLines(4, 5); 
    *pbySize = byIndex;

    return byKeyIn;
}

/*
 * Private.
 */
void CUi::ClearLines(BYTE byFirstLine, BYTE byLastLine)
{
    for(BYTE byIndex = byFirstLine; byIndex <= byLastLine; byIndex++)
    {
        CTOS_LCDTPrintXY(1, byIndex, (PBYTE)"                             ");
    }
}

/*
 * Private.
 */
USHORT CUi::ShowSelectCardType(void) 
{
    const char *pszBmpFilePath = GetBmpFilePath(Ui_Select_Card_Type);

    if (NULL != pszBmpFilePath)
    {
        ShowUi(pszBmpFilePath, 0);
    }
    return d_OK;
}

/*
 * Public.
 */
void CUi::SetTxnType(const int &nType)
{
    pthread_mutex_lock(&m_cMutex);
    m_nTxnType = nType;
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Private.
 */
int CUi::GetCurTxnType(void)
{
    pthread_mutex_lock(&m_cMutex);
    const int nTxnType = m_nTxnType;
    pthread_mutex_unlock(&m_cMutex);
    return nTxnType;
}

/*
 * Public.
 */
void CUi::SetCardType(const int &nType)
{
    pthread_mutex_lock(&m_cMutex);
    m_nCardType = nType;
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Public.
 */
int CUi::GetLanguage(void)
{
    pthread_mutex_lock(&m_cMutex);
    int nLanguage = m_nLanguage;
    pthread_mutex_unlock(&m_cMutex);
    return nLanguage;
}

/*
 * Public.
 */
void CUi::SetLanguage(int nLanguage)
{
    pthread_mutex_lock(&m_cMutex);
    m_nLanguage = nLanguage;
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Private.
 */
int CUi::GetCardType(void)
{
    pthread_mutex_lock(&m_cMutex);
    const int nCardType = m_nCardType;
    pthread_mutex_unlock(&m_cMutex);
    return nCardType;
}

/*
 * Public.
 */
void CUi::SetSimSlot(const int &nSlot)
{
    pthread_mutex_lock(&m_cMutex);
    m_nSimSlot = nSlot;
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Private.
 */
int CUi::GetSimSlot(void)
{
    pthread_mutex_lock(&m_cMutex);
    const int nSimSlot = m_nSimSlot;
    pthread_mutex_unlock(&m_cMutex);
    return nSimSlot;
}

/*
 * Private.
 */
WORD CUi::GetInputString(BYTE *pbyLable, CByteStream &cData)
{
    WORD wIndex = 0;
    WORD wRet = d_KBD_ENTER;
    BYTE byKey;

    CTOS_LCDTPrintXY(1, 1, pbyLable);

    CTAP_VirtualKBDShow();

    while (1)
    {
        wRet = CTAP_VirtualKBDGet(&byKey);
        if (d_OK != wRet)
        {
            continue;
        }

        if (d_KBD_ENTER == byKey ||
            d_KBD_CANCEL == byKey )
        {
            wRet = byKey;
            break;
        }
        else if (d_KBD_CLEAR == byKey)
        {
            cData.PopTailByte();
            byKey = d_KBD_INVALID;
        }

        if (d_KBD_INVALID != byKey)
        {
            cData.Write(&byKey, 1);
        }
        CTOS_LCDTPrintXY(1, 5, (BYTE *)"                              ");
        if (0 != cData.GetLength())
        {
            CTOS_LCDTPrintXY(1, 5, (BYTE *)cData.PushZero());
            cData.PopTailByte();
        }
    }
    CTAP_VirtualKBDHide();
    return wRet;
}

/*
 * Private.
 */
void CUi::SaveAPNName(const char *pszAPN)
{
    if (NULL != m_pszAPN)
    {
        free((void *)m_pszAPN);
        m_pszAPN = NULL;
    }
    if (0 != strlen(pszAPN))
        m_pszAPN = strdup(pszAPN);
}

/*
 * Private.
 */
void CUi::SaveID(const char *pszID)
{
    if (NULL != m_pszID)
    {
        free((void *)m_pszID);
        m_pszID = NULL;
    }
    if (0 != strlen(pszID))
        m_pszID = strdup(pszID);
}

/*
 * Private.
 */
void CUi::SavePwd(const char *pszPwd)
{
    if (NULL != m_pszPwd)
    {
        free((void *)m_pszPwd);
        m_pszPwd = NULL;
    }
    if (0 != strlen(pszPwd))
        m_pszPwd = strdup(pszPwd);
}

/*
 * Private, Static
 */
void *CUi::uiModuleTrd_Rs232Mode(void *pVoid)
{
    CUi *pUi = reinterpret_cast<CUi*>(pVoid);
    CTxnRequest *pTxnReq = getTxnReqModuleObj();
    ULONG ulTickStart = 0;

    while (NULL != pUi && pUi->IsContinue())
    {
        static int nLastUiIndex_ = Ui_Invalid_Index;
        const int nCurUiIndex = pUi->GetCurUiIndex();

        switch (nCurUiIndex)
        {
            case Ui_Start_New_Init:
                if (nCurUiIndex != nLastUiIndex_)
                {
                    TraceMsg("Info: Send initialize json date, begin");
                    CByteStream cData = pTxnReq->GenTxnInitJsonData(Cfg_Txn_Init_Path);
                    PostRequest((char *)cData.PushZero(), cData.GetLength());
                    pUi->SwitchUi(Ui_After_Call_PostRequest);
                    TraceMsg("Info: Send initialize json date, end");
                }
                break;

            case Ui_Init_Ethernet_Link:
            {
                if (nLastUiIndex_ != nCurUiIndex)
                {
                    pUi->InitEthLink();
                }
                else
                {
                    const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                    const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 5;
                    pUi->ShowUi(pszBmpFilePath, nOffset);
                }
            }
            break;

            case Ui_Init_Gprs_Link:
            {
                if (nLastUiIndex_ != nCurUiIndex)
                {
                    pUi->InitGprsLink();
                }
                else
                {
                    const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                    const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 5;
                    pUi->ShowUi(pszBmpFilePath, nOffset);
                }
            }
            break;

            case Ui_Load_Def_Link_Type:
            {
                if (nLastUiIndex_ != nCurUiIndex)
                {
                    pUi->LoadDefLinkType();
                }
            }
            break;

            //case Ui_Start_Get_PIN:
            case Ui_Finish_Get_PIN:
            {
                if (nLastUiIndex_ != nCurUiIndex)
                {
                    CTOS_LCDTClearDisplay();
                }
            }
            break;

            case Ui_Parsing_Json_Data:
            case Ui_Initialing:
            case Ui_Connecting_Host:
            case Ui_Connecting_Host_New:
            case Ui_Packing_Data:
            {
                const char *pszBmpFilePath = pUi->GetBmpFilePath(nCurUiIndex);
                const int nOffset = ((CTOS_TickGet() - ulTickStart) / 50) % 4;
                pUi->ShowUi(pszBmpFilePath, nOffset);
            }
            break;

            default:
                break;
        }

        if (nCurUiIndex != nLastUiIndex_)
        {
            nLastUiIndex_ = nCurUiIndex;
            pUi->ShowUi(nCurUiIndex);
            ulTickStart = CTOS_TickGet();
            TraceMsg("Info: UiThreadIndex=0x%08X", nCurUiIndex);
        }
        else if (Ui_Exit_App == nCurUiIndex)
        {
            break;
        }
        pUi->waitUserInputCancel();
        usleep(Ui_Trd_Nop);
    }

    pthread_exit(NULL);
    return NULL;
}

/*
 * Private
 */
void CUi::SetContinue(const BOOL bContinue)
{
    pthread_mutex_lock(&m_cMutex);
    m_bContinue = bContinue;
    m_nCurUiIndex = Ui_Exit_App;
    pthread_mutex_unlock(&m_cMutex);
}

/*
 * Private.
 */
BOOL CUi::IsContinue(void)
{
    pthread_mutex_lock(&m_cMutex);
    const BOOL bContinue = m_bContinue;
    pthread_mutex_unlock(&m_cMutex);
    return bContinue;
}

/*
 * Private.
 */
void *CUi::uiInitEthThread(void *pVoid)
{
    pthread_detach(pthread_self());
    CUi *pUi = reinterpret_cast<CUi*>(pVoid);

    if (NULL != pUi &&
        Ui_Exit_App != pUi->GetCurUiIndex())
    {
        WORD wErr = OpenEthLink();
        if (d_OK == wErr)
        {
            pUi->SwitchUi(Ui_Start_Use_Eth_Init);
        }
        else
        {
            pUi->SwitchUi(Ui_Init_Ethernet_Failed);
        }
    }
    return NULL;
}

/*
 * Private
 */
void *CUi::uiInitGprsThread(void *pVoid)
{
    pthread_detach(pthread_self());
    CUi *pUi = reinterpret_cast<CUi*>(pVoid);

    if (NULL != pUi &&
        Ui_Exit_App != pUi->GetCurUiIndex())
    {
        WORD wErr = InitGprsLink_();
        if (OK == wErr)
        {
            pUi->SwitchUi(Ui_Start_Use_Gprs_Init);
        }
        else
        {
            pUi->SwitchUi(Ui_Init_Gprs_Failed);
        }
    }
    return NULL;
}

/*
 * Private.
 */
void CUi::InitEthLink(void)
{
    if (-1 == m_cInitEthTrdId)
    {
        int nRet = pthread_create(&m_cInitEthTrdId, NULL, uiInitEthThread, (void *)this);
        if (-1 == nRet)
        {
            m_cInitEthTrdId = -1;
            SwitchUi(Ui_Init_Ethernet_Failed);
        }
    }
}

/*
 * Private.
 */
void CUi::InitGprsLink(void)
{
    if (-1 == m_cInitGprsTrdId)
    {
        int nRet = pthread_create(&m_cInitGprsTrdId, NULL, uiInitGprsThread, (void *)this);
        if (-1 == nRet)
        {
            m_cInitEthTrdId = -1;
            SwitchUi(Ui_Init_Gprs_Failed);
        }
    }
}

/*
 * Private.
 */
void CUi::waitUserInputCancel(void)
{
#if 1
    if (Ui_Idle == GetCurUiIndex() ||
        Ui_Welcome_Page == GetCurUiIndex() ||
        Ui_Select_Card_Type == GetCurUiIndex() ||
        Ui_Select_Polling_Card_Type == GetCurUiIndex() ||
        Ui_Manual_Sel_Txn_Type == GetCurUiIndex())
    {
        BYTE byKey;
        CTOS_KBDHit(&byKey);
        if (d_KBD_CANCEL == byKey)
            SwitchUi(Ui_Exit_App);
    }
#endif
}

/*
 * Private.
 */
BOOL CUi::IsEnableUseVoltage(void)
{
    static const char szFileName_[] = "/home/ap/pub/6D87B081-C702-4a67-85CE-924138911471.ini";
    static const char szEnableValue_[] = "{79E0335F-7400-43e9-A632-45D0A42272F7}";
    FILE *pcFile = NULL;
    CByteStream cFileData;
    BOOL bEnableVoltage = FALSE; 

    do
    {
        pcFile = fopen(szFileName_, "rb");
        if (NULL == pcFile)
            break;

        while (!feof(pcFile))
        {
            BYTE byBuf[1024];
            int nLen;

            nLen = fread(byBuf, 1, 1023, pcFile);
            if (0 != nLen)
            {
                cFileData.Write(byBuf, nLen);
            }
        }

        if (0 == strcmp(cFileData.PushZero(), szEnableValue_))
        {
            bEnableVoltage = TRUE;
        }

        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    PrintVoltageInfo(bEnableVoltage);
    return bEnableVoltage;
}

void CUi::PrintVoltageInfo(const BOOL &bEnable)
{
    TraceMsg("------------------------------------------");
    TraceMsg("Voltage status: %s", bEnable ? "Enable" : "Disable");
    TraceMsg("------------------------------------------");
}

static WORD InitGprsLink_(void)
{
    GPRS_LINK_CONFIG cLinkConfig = { 0 };
    CByteStream cFileData;
    FILE *pcFile = NULL;
    cJSON *pcJsonRoot = NULL;
    cJSON *pcSubItem = NULL;
    
    cLinkConfig.wInitGPRSTimeout = 60;
    cLinkConfig.wRegGsmTimeout = 60;
    cLinkConfig.wWaitUserApn = 0;

    // Read file data.
    do
    {
        pcFile = fopen(Cfg_Gprs_Config_Path, "rb");
        if (NULL == pcFile)
            break;

        while (!feof(pcFile))
        {
            BYTE byBuf[1024];
            int nLen;

            nLen = fread(byBuf, 1, 1024, pcFile);
            if (nLen > 0)
                cFileData.Write(byBuf, nLen);
        }

        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    do
    {
        pcJsonRoot = cJSON_Parse(cFileData.PushZero());
        if (NULL == pcJsonRoot)
            break;

        // Slot
        pcSubItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Gprs_Slot);
        if (NULL != pcSubItem)
        {
            cLinkConfig.wSlotIndex = atoi(pcSubItem->valuestring);
        }

        // APN
        pcSubItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Gprs_APN);
        if (NULL != pcSubItem)
        {
            strcpy(cLinkConfig.szApn, pcSubItem->valuestring);
        }

        // User
        pcSubItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Gprs_User);
        if (NULL != pcSubItem)
        {
            strcpy(cLinkConfig.szUserName, pcSubItem->valuestring);
        }

        // Password
        pcSubItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Gprs_Pwd);
        if (NULL != pcSubItem)
        {
            strcpy(cLinkConfig.szPwd, pcSubItem->valuestring);
        }

        TraceMsg("Info: Slot=%d", cLinkConfig.wSlotIndex);
        TraceMsg("Info: Apn=%s", cLinkConfig.szApn);
        TraceMsg("Info: User=%s", cLinkConfig.szUserName);
        TraceMsg("Info: Pwd=%s", cLinkConfig.szPwd);

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    WORD wError = ERROR_INVALID_PARAM;
    wError = g_cGprsLink_.SetConfig(&cLinkConfig, 0);
    if (OK != wError)
    {
        return wError;
    }
    
    wError = g_cGprsLink_.OpenLink(1);
    if (OK != wError)
    {
        return wError;
    }
    
    return OK;
}

static WORD CloseGprsLink_(void)
{
    return g_cGprsLink_.CloseLink();
}

/*
 * Private.
 */
void CUi::LoadDefLinkType()
{
    CByteStream cFileData;
    FILE *pcFile = NULL;
    cJSON *pcJsonRoot = NULL;
    BOOL bParseOK = FALSE;

    // Read file data.
    do
    {
        pcFile = fopen(Cfg_Link_Config_Path, "rb");
        if (NULL == pcFile)
            break;

        while (!feof(pcFile))
        {
            BYTE byBuf[1024];
            int nLen;

            nLen = fread(byBuf, 1, 1024, pcFile);
            if (nLen > 0)
                cFileData.Write(byBuf, nLen);
        }

        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    // Parsing file data
    do
    {
        pcJsonRoot = cJSON_Parse((char *)cFileData.PushZero());
        if (NULL == pcJsonRoot)
            break;
        
        cJSON *pcSubItem = NULL;

        pcSubItem = cJSON_GetObjectItem(pcJsonRoot, CFG_Link_Type);
        if (NULL != pcSubItem)
        {
            if (!strcmp(Link_Ethernet, pcSubItem->valuestring))
            {
                bParseOK = TRUE;
                SwitchUi(Ui_Init_Ethernet_Link);
            }
            else if (!strcmp(Link_Gprs, pcSubItem->valuestring))
            {
                bParseOK = TRUE;
                SwitchUi(Ui_Init_Gprs_Link);
            }
        }

        cJSON_Delete(pcJsonRoot);
        pcJsonRoot = NULL;
    } while (0);

    if (!bParseOK)
    {
        SwitchUi(Ui_Init_Ethernet_Link);
    }
}


USHORT CTAP_LCDBackGndColor(BOOL boReverse)
{
    if (boReverse)
    {
        CTOS_LCDBackGndColor(0x00C0C0C0);
    }
    else
    {
        CTOS_LCDBackGndColor(0x00B0B0B0);
    }
    
}