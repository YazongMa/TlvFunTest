
/*
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: AppMain.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2018.11.13
 * Device: UPT1000.
 * 
 * N.B: For Heartland Certification
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <unistd.h>

#include "Inc/TxnInterface.h"
#include "Inc/deftype.h"
#include "dbghlp.h"
#include "Ui.h"
#include "TxnRequest.h"
#include "libVKBD.h"
#include "AppMain.h"
#include "TxnJsonKey.h"
#include "DateMacro.h"
#include "Rs232Link.h"

#define ENABLE_CTRL_THREAD (0)
#define CK_TESTAP
//#define HL_CENTI_TESTAP


static CUi cUiObj_;
static CTxnRequest cTxnRequest_;

CUi *getUiModuleObj(void);
CTxnRequest *getTxnReqModuleObj(void);
extern void InitCtrlThread(void);
extern void UpzipResFile(void);
extern void InitRspThread(void);
extern void InitTouchThread(void);

/*
 * Different functions Mode entry.
 */
static void CK_TestAPEntry(void);
static void HL_TestAPEntry(void);
static void waitKBDCancelAndUiExit(void);
static void PrintTestAPVersion(void);

/*
 * Main function entry.
 */
int main(int argc, char *argv[])
{
    PrintTestAPVersion();
#ifdef CK_TESTAP 
    CK_TestAPEntry();
#endif 

#ifdef HL_CENTI_TESTAP
    HL_TestAPEntry();
#endif
    exit(0);
}

/*
 * Mode type RS232.
 */
static void HL_TestAPEntry()
{
    UpzipResFile();
    cUiObj_.InitUiModule(kRs232Mode);
    cUiObj_.SwitchUi(Ui_Load_Def_Link_Type);
    InitRs232Module(Cfg_Rs232_Config_Path);
    InitRspThread();
    InitTouchThread();
    waitKBDCancelAndUiExit();
}

/*
 * Mode type Manual.
 */
static void CK_TestAPEntry()
{
#if ENABLE_CTRL_THREAD
    InitCtrlThread();
#endif
    TraceMsg("CK_TestAP Entry.");
    UpzipResFile();
    InitTouchThread();
    CTAP_VirtualKBDOpen();
    cUiObj_.InitUiModule();
    InitRspThread();
    cUiObj_.SwitchUi(Ui_Load_Def_Link_Type);
    waitKBDCancelAndUiExit();
    CTAP_VirtualKBDClose();
}

/*
 * Public.
 */
CUi *getUiModuleObj(void)
{
    return (&cUiObj_);
}

/*
 * Public.
 */
CTxnRequest *getTxnReqModuleObj(void)
{
    return (&cTxnRequest_);
}

/*
 * Private.
 */
static void waitKBDCancelAndUiExit(void)
{
    unsigned int i = 0;
    while (Ui_Exit_App != cUiObj_.GetCurUiIndex())
    {
        if (0 == ((++i) % 60))
        {
            TraceMsg("...");
            i = 0;
        }
        usleep(500 * 1000);
    }
}

/*
 * Private.
 */
static void PrintTestAPVersion(void)
{
    static BOOL bGenAppVersion = FALSE;
    static char szAppVer[64] = {0};

    if (!bGenAppVersion)
    {
        sprintf(szAppVer, "%s_%04d.%02d.%02d",
                TestAp_Version,
                YEAR(),
                MONTH() + 1,
                DAY());
        bGenAppVersion = TRUE;
    }

    TraceMsg("------------------------------------------");
    TraceMsg("Current Application Version: %s", szAppVer);
    TraceMsg("------------------------------------------");
    TraceMsg("Current Api layer Version: %s", GetVersion(LIB_API_VERSION));
    TraceMsg("------------------------------------------");
    TraceMsg("Current Kernel layer Version: %s", GetVersion(LIB_HPKERNEL_VERSION));
    TraceMsg("------------------------------------------");
}