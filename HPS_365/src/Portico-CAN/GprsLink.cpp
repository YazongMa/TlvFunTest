/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StdafxHead.h"
#include "DbgMacro.h"
#include "ErrorCode.h"
#include "UIDisplay.h"
#include "GprsLink.h"

#include <unistd.h>
#include <ctosapi.h>


#define GSM_PIN_LEN                 4
#define MAX_FAILED_COUNT            3

CDebugInfo cDebug;
/**
 * CGprsLink
 * 
 * Function:
 *      Construct function.
 * 
 * @param 
 *      none
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 07/11/2018
 */
CGprsLink::CGprsLink(const GPRS_LINK_CONFIG *pcLinkConfig)
: 
m_bOpenGprs(FALSE),
m_bExistLinkConfig(FALSE)
{
    memset((void *)&m_cLinkConfig, 0, sizeof(m_cLinkConfig));
    if (NULL != pcLinkConfig)
    {
        memcpy((void *)&m_cLinkConfig, (void *)pcLinkConfig, sizeof(m_cLinkConfig));
        m_bExistLinkConfig = TRUE;
    }
}

/**
 * CGprsLink
 * 
 * Function:
 *      Deconstruct function.
 * 
 * @param 
 *      none
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 07/11/2018
 */
CGprsLink::~CGprsLink()
{
}


/**
 * OpenLink
 * 
 * Function:
 *      1. Verify gprs parameter
 *      2. init gsm
 *      3. check sim pin input
 *      4. wait gsm register
 *      5. init gprs moudle
 * 
 * @param 
 *      wOpenGprs   :
 *              0 - not open gprs
 *              1 - open gprs
 *                     
 * @return 
 *      OK      - gprs link open success.
 *      Other   - Please refer to the <ErrorCode.h> <ctoserr.h> for details.
 * 
 * modified: Tim.Ma, 07/11/2018
 */
WORD CGprsLink::OpenLink(IN const WORD wOpenGprs)
{
    DEBUG_ENTRY();

    WORD wError = ERROR_INVALID_PARAM;
    
    if (!m_bExistLinkConfig)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: Not set gprs link config.");
        return DEBUG_RETCODE("OpenLink", ERROR_INVALID_PARAM);
    }
    
    if (m_bOpenGprs)
    {
        DEBUG_INFO(DEBUGINFO_WARNING, "Warning: gprs is already open.");
        return DEBUG_RETCODE("OpenLink", ERROR_INVALID_PARAM);
    }
    
    wError = VerifyParam();
    if (OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: verify gprs param failed.");
        return DEBUG_RETCODE("OpenLink", wError);
    }
    
    if (!IsSupportGprs())
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: Not support gprs.");
        return DEBUG_RETCODE("OpenLink", ERROR_NOT_SUPPORT_GPRS);
    }
    
    wError = InitGSMModule();
    if (OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: init gsm module failed.");
        return DEBUG_RETCODE("OpenLink", wError);
    }

    wError = UnlockPIN();
    if (OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: UnlockPIN failed.");
        return DEBUG_RETCODE("OpenLink", wError);
    }

    wError = WaitRegisterGSM();
    if (OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: WaitRegisterGSM failed.");
        return DEBUG_RETCODE("OpenLink", wError);
    }
    
    if (0 != wOpenGprs)
    {
        wError = InitGPRSModule();
        if (OK != wError)
        {
            DEBUG_INFO(DEBUGINFO_ERROR, "Error: Init GPRS Module failed.");
            return DEBUG_RETCODE("InitGPRSModule", wError);
        }
    }
    
    return DEBUG_EXIT(OK);
}


/**
 * CloseLink
 * 
 * Function:
 *      1. close gprs module
 *      2. close gsm module
 * 
 * @param 
 *      none
 *                     
 * @return 
 *      OK      - gprs link close success.
 *      Other   - Please refer to the <ErrorCode.h> <ctoserr.h> for details.
 * 
 * modified: Tim.Ma, 07/11/2018
 */
WORD CGprsLink::CloseLink(void)
{
    DEBUG_ENTRY();
    
    WORD wError = ERROR_INVALID_PARAM;
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSClose Begin...");
    wError = CTOS_TCP_GPRSClose();
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSClose OK, wError:0x%04X", wError);
    
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMClose Begin...");
    wError = CTOS_GSMClose();
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMClose OK, wError:0x%04X", wError);
    
    m_bOpenGprs = FALSE;
    
    return DEBUG_EXIT(wError);
}


/**
 * SetConfig
 * 
 * Function:
 *      1. set gprs link's config
 *      2. restart gprs module if the config is updated
 * 
 * @param 
 *      [IN] pcLinkConfig - GPRS link's parameters
 *      [IN] wGprsFlag    - when gprs is not open, the parameter is effective
 *                          0 : don't open gprs
 *                          1 : open gprs
 *                     0xFFFF : reset GSM module
 *                     
 * @return 
 *      OK      - gprs link reopen success.
 *      Other   - Please refer to the <ErrorCode.h> <ctoserr.h> for details.
 * 
 * modified: Tim.Ma, 07/11/2018
 */
WORD CGprsLink::SetConfig(IN const GPRS_LINK_CONFIG *pcLinkConfig, IN const WORD wGprsFlag)
{
    DEBUG_ENTRY();

    if (NULL == pcLinkConfig)
    {
        return DEBUG_RETCODE("SetConfig", ERROR_INVALID_PARAM);
    }
    memcpy(&m_cLinkConfig, pcLinkConfig, sizeof(GPRS_LINK_CONFIG));
    
    WORD    wError;
    if (wGprsFlag == 0xFFFF)
    {
        CloseLink();
        CTOS_GSMReset();
        CTOS_GSMPowerOff();
        CTOS_GSMPowerOn();
        return DEBUG_RETCODE("CTOS_TCP_GPRSOpen: SetConfig", OpenLink(1));
    }
    
    if (m_bOpenGprs)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSClose Begin...");
        wError = CTOS_TCP_GPRSClose();
        m_bOpenGprs = FALSE;
        DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSClose OK, wError:0x%04X", wError);
        
        wError = InitGPRSModule();
        return DEBUG_RETCODE("CTOS_TCP_GPRSSwitchAPN: SetConfig", wError);
    }
    else if(wGprsFlag == 1)
    {
        wError = InitGPRSModule();
        return DEBUG_RETCODE("CTOS_TCP_GPRSOpen: SetConfig", wError);
    }
    
    m_bExistLinkConfig = TRUE;
    return DEBUG_EXIT(OK);
}


/**
 * GetConfig
 * 
 * Function:
 *      1. Get gprs link's config
 * 
 * @param 
 *      [OUT] GPRS_LINK_CONFIG - GPRS link's parameters
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 07/11/2018
 */
void CGprsLink::GetConfig(OUT GPRS_LINK_CONFIG &cLinkConfig)
{
    DEBUG_ENTRY();
    memcpy((void *)&cLinkConfig, (void *)&m_cLinkConfig, sizeof(m_cLinkConfig));
    DEBUG_EXIT(OK);
}


/**
 * GetLinkStatus
 * 
 * Function:
 *      Get gprs link's status
 * 
 * @param 
 *      [OUT] wStatus - 0 : gprs is OK
 *                      1 : gprs is not OK
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 07/11/2018
 */
BOOL CGprsLink::GetLinkStatus(OUT WORD &wStatus)
{
    DEBUG_ENTRY();
    DWORD dwStatus;
    
    wStatus = ERROR_NOT_CONNECTED;
    
    WORD wError = ERROR_INVALID_PARAM;
    
    while (1)
    {
        wError = CTOS_TCP_GPRSStatus(&dwStatus);
        if (wError != d_TCP_IO_PROCESSING)
        {
            break;
        }
        
        DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSStatu: d_TCP_IO_PROCESSING");
        usleep(200* 000);
    }
    
    if(dwStatus & TCP_GPRS_STATE_ESTABLISHED ||
        dwStatus & TCP_GPRS_STATE_CONNECTED)
    {
        wStatus = OK;
    }

    return DEBUG_EXIT(TRUE);
}


/**
 * SwitchToMode
 * 
 * Function:
 *          switch the GSM mode
 * 
 * Params:
 *          wModeType   - 
 *              d_GSM_MODULE_DATA_MODE: CTOS_GSMSwitchToDataMode
 *              d_GSM_MODULE_CMD_MODE: CTOS_GSMSwitchToCmdMode
 *          dwTimeOut   - the timeout for SwitchToMode
 * 
 * @return 
 *          OK - SwitchToMode OK
 *          Other - refer to ctoserr.h
 * 
 * modified: Tim.Ma, 05/04/2018
 */
WORD CGprsLink::SwitchToMode(IN WORD wModeType, IN DWORD dwTimeOut)
{
    WORD wError;
    BYTE byOpmode;
    
    if (m_bOpenGprs==FALSE && wModeType==d_GSM_MODULE_DATA_MODE)
    {
        return DEBUG_RETCODE("GPRS not open, GSM don't switch to data mode", ERROR_INVALID_PARAM);
    }
    
    CTOS_GSMGetModuleOpMode(&byOpmode);
    if(byOpmode == wModeType)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "The current GSMOpMode is same as wModeType, Don't need switch GSM Mode");
        return OK;
    }
    
    ULONG ulStart = GetTickCount();
    while (1)
    {
        if (d_GSM_MODULE_DATA_MODE == wModeType)
        {
            wError = CTOS_GSMSwitchToDataMode();
        }
        else if (d_GSM_MODULE_CMD_MODE == wModeType)
        {
            wError = CTOS_GSMSwitchToCmdMode();
        }
        else
        {
            return DEBUG_RETCODE("Invalid wModeType", ERROR_GPRS);
        }
        
        if(wError == d_OK)
        {
            break;
        }
        else if (GetTickCount() - ulStart > dwTimeOut)
        {
            return DEBUG_RETCODE("CTOS_GSMSwitchToDataMode", wError);
        }
        
        usleep( 1000 * 10);
    }
    
    return OK;
}


/**
 * InitGSMModule
 * 
 * Function:
 *          Select the SIM slot and initialize the GSM module.
 *          
 * @param:
 *          None
 *
 * @return:
 *          OK - Successfully select the SIM slot and initialize 
 *               the GSM module.
 *          Other values - Please refer to the ErrorCode.h file.
 * 
 * modified: Tim.Ma, 05/04/2018
**/
WORD CGprsLink::InitGSMModule()
{
    DEBUG_ENTRY();

    WORD        wError;
    const BYTE  bySlotBuf[] = { d_GPRS_SIM1, d_GPRS_SIM2 };

    wError = CTOS_GSMOpen(115200, TRUE);
    if (d_OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Open GSM module failed, error code 0x%04X", wError);
        return DEBUG_RETCODE("InitGSMModule", wError);
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMOpen OK");
    
    wError = CTOS_GSMSelectSIM(bySlotBuf[m_cLinkConfig.wSlotIndex]);
    if (d_OK != wError)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Select SIM error, error code 0x%04X.", wError);
        return DEBUG_RETCODE("InitGSMModule", wError);
    }
    
    // Check the GSM ready to use the SMS or Phone Book with the SIM card inserted //
    DWORD dwDistance    = 0;
    DWORD dwStart       = CTOS_TickGet();
    
    while ((wError != d_GSM_SIM_READY) && (dwDistance < 3000))
    {
        wError = CTOS_SIMCheckReady();
        
        usleep( 1000 * 500);
        dwDistance = CTOS_TickGet() - dwStart;
    }
    if (d_GSM_SIM_READY != wError)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "Error: SIM card is not ready.");
        return DEBUG_RETCODE("InitGSMModule", wError);
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "SIM Check Ready OK");
    
    // Get signal quality of the GSM network //
    dwDistance  = 0;
    dwStart     = CTOS_TickGet();
    
    BYTE byStrength = 99;
    while ((byStrength<10 || byStrength>30) && 
            dwDistance<500)
    {
        ShowSignalQuality();
        usleep( 1000 * 1500);
        dwDistance = CTOS_TickGet() - dwStart;
    }
    
    // Init the GPRS Library
    static BOOL bGprsInitFlag = FALSE;
    if (FALSE == bGprsInitFlag)
    {
        CTOS_TCP_GPRSInit();
        bGprsInitFlag = TRUE;
    }

    return DEBUG_EXIT(OK);
}



/**
 * UnlockPIN
 * 
 * Function:
 *          Check if you need to enter your PIN code, if you need to enter,
 *          enter and verify that the correct PIN.
 *          
 * @param:
 *          None
 *
 * @return:
 *          OK - No need to enter a PIN or PIN is correct.
 *          Other values - Please refer to the ErrorCode.h file.
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
WORD CGprsLink::UnlockPIN()
{
    DEBUG_ENTRY();

    WORD wError;

    wError = CTOS_PINGetAuthStatus();
    if (d_GSM_UNKNOWN_MODULE == wError)
    {
        wError = CTOS_GSMOpen(115200, TRUE);
        if (d_OK != wError)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Open GSM module failed, error code 0x%04X", wError);
            return DEBUG_RETCODE("UnlockPIN", wError);
        }
    }

    if (d_GSM_AUTH_PIN != wError)
    {
        return DEBUG_RETCODE("UnlockPIN", OK);
    }
    
    while (1)
    {
        int         nTryCount = 0;
        CUIDisplay  cInputUI;
        CByteStream cInputData;

        wError = cInputUI.GetUserInputData((PSTR) "Input PIN:", (PSTR) "",
                GSM_PIN_LEN, DATATYPE_N, cInputData, TRUE);
        if (d_KBD_CANCEL == wError)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Error: User cancel connect operation.");
            return DEBUG_RETCODE("UnlockPIN", ERROR_USER_CANCEL);
        }

        wError = CTOS_PINVerify(d_GSM_AUTH_SC, cInputData.GetBuffer(),
                cInputData.GetLength());
        if (d_OK != wError)
        {
            nTryCount++;
            if (nTryCount >= MAX_FAILED_COUNT)
            {
                DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Verify PIN failed.");
                return DEBUG_RETCODE("UnlockPIN", wError);
            }
        }
        else
        {
            break;
        }
    }

    return DEBUG_EXIT(OK);
}



/**
 * WaitRegisterGSM
 * 
 * Function:
 *          Waiting to complete the registration of the GSM network.
 *          
 * @param:
 *          None
 *
 * @return:
 *          OK - Successfully complete the registration of the GSM network.
 *          Other values - Please refer to the ErrorCode.h file.
 * 
 * modified: Tim.Ma, 05/04/2018
**/
WORD CGprsLink::WaitRegisterGSM()
{
    DEBUG_ENTRY();

    BYTE byState;
    ULONG ulStart = GetTickCount();

    while (1)
    {
        CTOS_GPRSGetRegState(&byState);
        if (d_GSM_GPRS_STATE_REG == byState)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Register GSM in home network OK.");
            break;
        }
        else if (d_GSM_GPRS_STATE_ROAM == byState)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Register GSM in roaming condition OK.");
            break;
        }
        
        ShowSignalQuality();
        
        if ((GetTickCount() - ulStart) > (m_cLinkConfig.wRegGsmTimeout * 1000))
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Register GSM module timeout State:%d.", byState);
            return DEBUG_RETCODE("WaitRegisterGSM", ERROR_TIMEOUT);
        }
        usleep( 1000 * 200);
    }

    return DEBUG_RETCODE("WaitRegisterGSM", OK);
}


/**
 * InitGPRSModule
 * 
 * Function:
 *          GPRS module is initialized and wait for Initialization to
 *          complete.
 *          
 * @param:
 *          None
 *
 * @return:
 *          OK - Successfully complete the Initialization of the 
 *          GPRS module..
 *          Other values - Please refer to the ErrorCode.h file.
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
WORD CGprsLink::InitGPRSModule()
{
    DEBUG_ENTRY();
    
    WORD    wError;
    DWORD   dwState;
    ULONG   ulStart;
    BYTE    byIP[] = {0x00, 0x00, 0x00, 0x00};
    
    wError = CTOS_TCP_GPRSOpen(byIP, 
            (BYTE *) m_cLinkConfig.szApn, 
            (BYTE *) m_cLinkConfig.szUserName, 
            (BYTE *) m_cLinkConfig.szPwd);
    
    ulStart = GetTickCount();
    while (d_TCP_IO_PROCESSING == wError)
    {
        wError = CTOS_TCP_GPRSStatus(&dwState);
        if ((GetTickCount() - ulStart) > (m_cLinkConfig.wInitGPRSTimeout * 1000))
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Error: Init GPRS module timeout.");
            return DEBUG_RETCODE("InitGPRSModule", ERROR_TIMEOUT);
        }
        usleep( 1000 * 100);
    }
    
    wError = CTOS_TCP_GPRSStatus(&dwState);
    if (d_OK != wError)
    {
        return DEBUG_RETCODE("Error: InitGPRSModule", wError);
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSOpen OK");
    
    // Get the local IP //
    CTOS_TCP_GPRSGetIP(byIP);
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_TCP_GPRSGetIP: %d.%d.%d.%d", byIP[0], byIP[1], byIP[2], byIP[3]);
    
    m_bOpenGprs = TRUE;
    return DEBUG_EXIT(OK);
}

    
/**
 * ShowNetworkType
 * 
 * Function:
 *      Output network communication type.
 * 
 * @param 
 *      none
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 05/04/2018
 */
WORD CGprsLink::ShowNetworkType(void)
{
    BYTE byType = 0;
    
    WORD wError = CTOS_GSMGetNetworkType(&byType);
    switch (byType)
    {
        case d_GSM_NETWORK_GPRS :
            DEBUG_INFO(DEBUGINFO_NORMAL, "GPRS (2G) network");
            break;
            
        case d_GSM_NETWORK_EGPRS :
            DEBUG_INFO(DEBUGINFO_NORMAL, "EDGE GPRS (2G) network");
            break;
            
        case d_GSM_NETWORK_WCDMA :
            DEBUG_INFO(DEBUGINFO_NORMAL, "W-CDMA (3G) network");
            break;
            
        case d_GSM_NETWORK_HSDPA :
            DEBUG_INFO(DEBUGINFO_NORMAL, "HSDPA (3G) network");
            break;
            
        case d_GSM_NETWORK_HSUPA :
            DEBUG_INFO(DEBUGINFO_NORMAL, "HSUPA (3G) network");
            break;
            
        case d_GSM_NETWORK_LTE :
            DEBUG_INFO(DEBUGINFO_NORMAL, "LTE (4G) GPRS network");
            break;
            
        case d_GSM_NETWORK_UNKNOWN :
            DEBUG_INFO(DEBUGINFO_NORMAL, "UNKNOW network");
            break;
            
        default:
            DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMGetNetworkType Failed wError:0x%04X", wError);
            break;
    }
    
    return OK;
}


/**
 * ShowSignalQuality
 * 
 * Function:
 *      Output network signal quality.
 * 
 * @param 
 *      none
 *                     
 * @return 
 *      none
 * 
 * modified: Tim.Ma, 05/04/2018
 */
WORD CGprsLink::ShowSignalQuality(void)
{
    BYTE byRet;
    CTOS_GSMSignalQuality(&byRet);
    if (byRet == 0)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSM byStrength:%d, Signal strength: <-113dBm", byRet);
    }
    else if (byRet >= 99)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSM byStrength:%d, Quality not known or not detectable", byRet);
    }
    else
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSM byStrength:%d, Signal strength: %ddBm", byRet, (-113 + byRet * 2));
    }
    
    return OK;
}


/**
 * CheckGPRSStatus
 * 
 * Function:
 *          Check the GPRS-Connection status.
 *          
 * @param:
 *          None
 *
 * @return:
 *          d_FALSE - the GPRS-Connection is not OK
 *          d_TRUE  - the GPRS-Connection is OK
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
BOOL CGprsLink::CheckGPRSStatus(void)
{
    BYTE	byRet,byStrength;
    WORD	wError;

    //Switch to Cmd mode
    wError = SwitchToMode(d_GSM_MODULE_CMD_MODE, 3000);
    if(wError != OK)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSMSwitchToCmdMode failed, wError:0x%04X", wError);
        return d_FALSE;
    }

    do
    {
        //Check register of the GSM
        wError = CTOS_GPRSGetRegState(&byStrength);
        if((wError != d_OK) || ((byStrength != 1) && (byStrength != 5)))
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "GPRSGetRegState failed, wError:0x%04X", wError);
            break;
        }

        //Check register of the GPRS
        wError = CheckAttach(&byStrength);
        if((wError != d_TRUE) || ((byStrength != 1) && (byStrength != 5)))
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "GPRSGetRegState failed, wError:0x%04X, byStrength:%d", wError, byStrength);
            break;
        }

        //Check the IP of the PPP connection
        byRet = CheckPDPIP();
        if(byRet != d_TRUE)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "CheckPDPIP failed, byRet:%d", byRet);
            break;
        }
    }while (0);

    //Switch to Data mode
    wError = SwitchToMode(d_GSM_MODULE_DATA_MODE, 3000);
    if(wError != OK)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSMSwitchToDataMode failed, wError:0x%04X", wError);
    }

    return d_TRUE;
}


/**
 * CheckAttach
 * 
 * Function:
 *          Get the GPRS register status by sending a command.
 *          
 * @param:
 *          [OUT]pbyState   - the GPRS register status
 *
 * @return:
 *          d_FALSE - CheckAttach failed
 *          d_TRUE  - CheckAttach OK
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
BOOL CGprsLink::CheckAttach(OUT BYTE *pbyState)
{
    BYTE	baBuf[128];
    USHORT	usRtn, usBufLen, usLen;
    ULONG	ulTimeout;

    usRtn = CTOS_GSMSendData((BYTE *)"AT+CGREG?\x0D", 10);
    if (usRtn != d_OK)
    {
        return d_FALSE;
    }

    memset(baBuf, 0x00, sizeof(baBuf));
    usBufLen = 0;

    ulTimeout = CTOS_TickGet () + 10000; // Timeout 10s
    while (1)
    {
        if (CTOS_TickGet () > ulTimeout)
        {
            return d_FALSE;
        }

        usLen = sizeof (baBuf) - usBufLen;
        usRtn = CTOS_GSMRecvData(&baBuf[usBufLen], &usLen);
        if (usRtn != d_OK)
        {
            return d_FALSE;
        }

        usBufLen += usLen;
        if (usBufLen >= 8 && memcmp(&baBuf[usBufLen - 8], "\x0D\x0A\x0D\x0AOK\x0D\x0A", 8) == 0)
        {
            break;
        }
        if (usBufLen >= 7 && memcmp(&baBuf[usBufLen - 7], "ERROR\x0D\x0A", 7) == 0)
        {
            return d_FALSE;
        }
    }

    if (usBufLen >= 9 && memcmp(&baBuf[usBufLen - 9], "1", 1) == 0)
    {   
        *pbyState = 1;	
    }
    else if (usBufLen >= 9 && memcmp(&baBuf[usBufLen - 9], "2", 1) == 0)
    {
        *pbyState = 2;
    }
    else if (usBufLen >= 9 && memcmp(&baBuf[usBufLen - 9], "3", 1) == 0)
    {
        *pbyState = 3;
    }
    else if (usBufLen >= 9 && memcmp(&baBuf[usBufLen - 9], "4", 1) == 0)
    {
        *pbyState = 4;
    }
    else if (usBufLen >= 9 && memcmp(&baBuf[usBufLen - 9], "5", 1) == 0)
    {
        *pbyState = 5;
    }

    return d_TRUE;
}


/**
 * CheckPDPIP
 * 
 * Function:
 *          Check the IP of the PPP connection by sending a command
 *          
 * @param:
 *          [none]
 *
 * @return:
 *          d_FALSE - CheckPDPIP failed
 *          d_TRUE  - CheckPDPIP OK
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
BOOL CGprsLink::CheckPDPIP(void)
{
    BYTE	baBuf[128];
    USHORT	usRtn, usBufLen, usLen;
    ULONG	ulTimeout;

    usRtn = CTOS_GSMSendData((BYTE *)"AT+CGPADDR=1\x0D", 13);
    if (usRtn != d_OK)
    {
        return d_FALSE;
    }

    memset(baBuf, 0x00, sizeof(baBuf));
    usBufLen = 0;

    ulTimeout = CTOS_TickGet() + 10000; // Timeout 10s
    while (1)
    {
        if (CTOS_TickGet() > ulTimeout)
        {	
            return d_FALSE;
        }

        usLen = sizeof (baBuf) - usBufLen;
        usRtn = CTOS_GSMRecvData(&baBuf[usBufLen], &usLen);
        if (usRtn != d_OK)
        {
            return d_FALSE;
        }

        usBufLen += usLen;
        if (usBufLen >= 8 && memcmp(&baBuf[usBufLen - 8], "\x0D\x0A\x0D\x0AOK\x0D\x0A", 8) == 0)
        {
            if(usBufLen > 24)
            {
                return d_TRUE;
            }
            else
            {
                return d_FALSE;
            }
        }

        if (usBufLen >= 7 && memcmp(&baBuf[usBufLen - 7], "ERROR\x0D\x0A", 7) == 0)
        {
            if(usBufLen > 24)
            {
                return d_TRUE;
            }
            else
            {
                return d_FALSE;
            }
        }
    }
}


/**
 * Verify Parameter
 *
 * Function: Verify GPRS Parameter load for XML file.
 *
 * @param:
 *          None
 *
 * @return 
 *      OK - Verify Parameter successfully,
 *      Other - Please refer to the ErrorCode
 * 
 * modified: Tim.Ma, 05/04/2018
 **/
WORD CGprsLink::VerifyParam(void)
{
    DEBUG_ENTRY();
    
    if (m_cLinkConfig.wSlotIndex != SIM_SLOT_1 &&
        m_cLinkConfig.wSlotIndex != SIM_SLOT_2)
    {
        DEBUG_INFO(DEBUGINFO_ERROR, "Error: \"%s\" data error.", GPRS_P_SLOT);
        return DEBUG_RETCODE("LoadConfig", ERROR_INVALID_PARAM);
    }

    if (m_cLinkConfig.wRegGsmTimeout < MIN_REG_GSM_SEC)
    {
        m_cLinkConfig.wRegGsmTimeout = MIN_REG_GSM_SEC;
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "\"%s\":%d.", GPRS_P_REGGSM_TIMEOUT, m_cLinkConfig.wRegGsmTimeout);

    if (m_cLinkConfig.wInitGPRSTimeout < MIN_INITGPRS_SEC)
    {
        m_cLinkConfig.wInitGPRSTimeout = MIN_INITGPRS_SEC;
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "\"%s\":%d.", GPRS_P_INITGPRS_TIMEOUT, m_cLinkConfig.wInitGPRSTimeout);

    return DEBUG_EXIT(OK);
}


/**
 * GetSIMID
 * 
 * Function:
 *      Get SIM ID
 * 
 * @param 
 *      [OUT]pbyBuffer  - the SIM ID
 *                     
 * @return 
 *      OK      - ModifyConfig succefull
 *      Other   - Please refer to the <ctoserr.h> for details.
 * 
 * modified: Tim.Ma, 07/05/2018
 */
WORD CGprsLink::GetLinkInfo(OUT GPRS_LINK_STATE &cLinkInfo)
{
    DEBUG_ENTRY();
    const BYTE ResultCode1[] = {0x0d, 0x0a, 'O', 'K', 0x0d, 0x0a};
    const BYTE ResultCode2[] = {0x0d, 0x0a, 'E', 'R', 'R', 'O', 'R', 0x0d, 0x0a};
    
    BYTE baszBuf[128 + 1] = {0};
    WORD wRet = OK;
    WORD wBufLen = 0;
    WORD wLen = 0;
    ULONG ulTimeout;

    wRet = CTOS_SIMGetIMSI((unsigned char*)cLinkInfo.szIMSI);
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_SIMGetIMSI: wRet:0x%04X, IMSI:%s", wRet, cLinkInfo.szIMSI);

    BYTE byLength = sizeof(cLinkInfo.szOperatorName);
    wRet = CTOS_GSMQueryOperatorName((unsigned char*)cLinkInfo.szOperatorName, &byLength);
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMQueryOperatorName: wRet:0x%04X, OperatorName:%s", wRet, cLinkInfo.szOperatorName);

    wRet = CTOS_GSMSignalQuality(&cLinkInfo.bySignalQuality);
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMSignalQuality: wRet:0x%04X, SignalQuality:%d", wRet, cLinkInfo.bySignalQuality);

    wRet = CTOS_GSMGetNetworkType(&cLinkInfo.byNetworkType);
    DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMGetNetworkType: wRet:0x%04X, NetworkType:%d", wRet, cLinkInfo.byNetworkType);
    
    GetLinkStatus(cLinkInfo.wLinkState);
    
    //Switch to Cmd mode
    wRet = SwitchToMode(d_GSM_MODULE_CMD_MODE, 3000);
    if(wRet != OK)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSMSwitchToCmdMode failed, wError:0x%04X", wRet);
        return wRet;
    }
    
    wRet = CTOS_GSMSendData((unsigned char*)"AT#CCID\x0D", 8);
    if(wRet != d_OK)
    {
        SwitchToMode(d_GSM_MODULE_DATA_MODE, 3000);
        return DEBUG_RETCODE("CTOS_GSMSendData", wRet);
    }

    bzero(baszBuf, sizeof(baszBuf));
    wBufLen = 0;

    ulTimeout = CTOS_TickGet() + 500;
    while(1)
    {
        if(CTOS_TickGet() > ulTimeout)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "Get Data TimeOut");
            wRet = d_GSM_NO_RESP;
            break;
        }

        wLen = sizeof(baszBuf) - wBufLen;
        wRet = CTOS_GSMRecvData(&baszBuf[wBufLen], &wLen);
        if(wRet != d_OK)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMRecvData Error.wRet:0x%04X", wRet);
            break;
        }
        
        DEBUG_INFO(DEBUGINFO_NORMAL, "CTOS_GSMRecvData OK. %s ---- %d", baszBuf, wLen);
        DEBUG_BYTEBUF(DEBUGINFO_NORMAL, baszBuf, wLen, "CTOS_GSMRecvData !!!");
        //CTOS_GSMRecvData !!!:415423434349440D0D0A23434349443A2038393031343130343237393436313731333536320D0A0D0A4F4B0D0A
        /* AT#CCID
#CCID: 89014104279461713562
OK
         */
        wBufLen += wLen;

        if(wBufLen >= 6 && memcmp(&baszBuf[wBufLen - 6], ResultCode1, 6) == 0)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "wBufLen >= 6");
            memcpy(cLinkInfo.szSIMID, &baszBuf[17], 20);
            
            wRet = OK;
            break;
        }

        if(wBufLen >= 9 && memcmp(&baszBuf[wBufLen - 9], ResultCode2, 9) == 0)
        {
            DEBUG_INFO(DEBUGINFO_NORMAL, "d_GSM_CMD_FAIL, wBufLen >= 9");
            wRet = d_GSM_CMD_FAIL;
            break;
        }
    }
    DEBUG_INFO(DEBUGINFO_NORMAL, "Get SIM ID: wRet:0x%04X, SIMID:%d", wRet, cLinkInfo.szSIMID);
    
    //Switch to Data mode
    WORD wRet2 = SwitchToMode(d_GSM_MODULE_DATA_MODE, 3000);
    if(wRet2 != OK)
    {
        DEBUG_INFO(DEBUGINFO_NORMAL, "GSMSwitchToDataMode failed, wError:0x%04X", wRet2);
    }
    
    return DEBUG_EXIT(wRet);
}


//////////////////////////////////////////////////////////////////////////////////
// IsSupportGprs
//
// Function:
//           Call this function to check the POS is support ethernet or not.
//
// @param:
//           None.
//
// @return:
//           TRUE - Support.
//           FALSE - Not support.
//
// @author:
//           Alan.Ren, 2018.07.13
//////////////////////////////////////////////////////////////////////////////////
BOOL CGprsLink::IsSupportGprs(void)
{
    WORD wErr;
    WORD wHwSupport = 0x00;
    BOOL bPortable;
    BOOL bPCI;
    BOOL bSupportEth = FALSE;

    wErr = CTOS_HWSettingGet(&bPortable, &bPCI, &wHwSupport); 
    if (OK != wErr)
    {
        return FALSE;
    }
    if (wHwSupport & d_MK_HW_GPRS)
    {
        bSupportEth = TRUE;
    }
    return bSupportEth;
}