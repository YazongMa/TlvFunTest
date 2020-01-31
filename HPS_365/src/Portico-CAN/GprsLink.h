
/*
 *
 */

#ifndef __CASTLESPAY_COMMLIB_GPRSLINK_H__ 
#define __CASTLESPAY_COMMLIB_GPRSLINK_H__

#include "deftype.h"


//
// GPRS 
//
#define GPRS_CONFIG                 "GPRSConfig"
#define DEF_CONFIG_XML_GPRS_NODE    "COMM_GPRS"
#define GPRS_NODE_COMMON            "GPRSCommon"
#define GPRS_NODE_GPRS01            "GPRSConfig01"
#define GPRS_NODE_GPRS02            "GPRSConfig02"
#define GPRS_NODE_GPRS03            "GPRSConfig03"
#define GPRS_NODE_GPRS04            "GPRSConfig04"
#define GPRS_NODE_GPRS05            "GPRSConfig05"
#define GPRS_NODE_GPRS06            "GPRSConfig06"
#define GPRS_CHANNEL_MAX            "GPRSMaxNum"

#define GPRS_P_SLOT                 "Slot"
#define GPRS_P_APN                  "APN"
#define GPRS_P_USERNAME             "UserName"
#define GPRS_P_PWD                  "Password"
#define GPRS_P_REGGSM_TIMEOUT       "RegisterGSMTimeout"
#define GPRS_P_INITGPRS_TIMEOUT     "InitGPRSTimeout"
#define GPRS_P_SIMCARD_PIN          "PINCode"

#define GPRS_P_DFTSENDTIMEOUT       "DftSendTimeout"
#define GPRS_P_DFTRECVTIMEOUT       "DftRecvTimeOut"
#define GPRS_P_HOSTIP               "HostIP"
#define GPRS_P_PORT                 "HostPort"
#define GPRS_P_ACTIVE               "Active"
#define GPRS_P_CHANNELID            "ChannelID"
#define GPRS_P_CONNHOST_TIMEOUT     "ConnectHostTimeout"
#define GPRS_P_WAITUSERAPN          "WaitUserApn"


#define SIM_SLOT_1                   0
#define SIM_SLOT_2                   1
#define PIN_CODE_LEN                 4
#define MAX_SLOT_LEN                 2
#define MAX_APN_LEN                  128
#define MAX_USERNAME_LEN             128
#define MAX_PWD_LEN                  128
#define MAX_IP_LEN                   15
#define MAX_PORT_LEN                 6
#define MIN_REG_GSM_SEC              1         // Seconds
#define MAX_REG_GSM_SEC              60        // Seconds
#define MIN_INITGPRS_SEC             1         // Seconds
#define MAX_INITGPRS_SEC             60        // Seconds
#define MIN_CONNHOST_SEC             1         // Seconds
#define MAX_CONNHOST_SEC             60        // Seconds
#define GPRS_MAX_CHANNEL             6


typedef struct
{
    WORD        wSlotIndex;
    WORD        wRegGsmTimeout;
    WORD        wInitGPRSTimeout;
    WORD        wWaitUserApn;
    char        szApn[128];
    char        szUserName[128];
    char        szPwd[128];
    char        szSimCardPin[128];
}GPRS_LINK_CONFIG;


/**
 * GPRS_LINK_STATE
 * 
 * @note:
 *      szApn       : GPRS APN
 *      szUserName  : GPRS APN username
 *      szPwd       : GPRS APN password
**/
typedef struct
{
    BYTE            bySignalQuality;
    BYTE            byNetworkType;
    WORD            wLinkState;
    char            szIMSI[128];
    char            szSIMID[128];
    char            szOperatorName[128];
}GPRS_LINK_STATE;


class CGprsLink
{
public:
    CGprsLink(const GPRS_LINK_CONFIG *pcLinkConfig = NULL);
    virtual ~CGprsLink();
    
public:
    WORD OpenLink(IN const WORD wOpenGprs);
    WORD CloseLink(void);
    
    WORD SetConfig(IN const GPRS_LINK_CONFIG *pcLinkConfig, IN const WORD wOpenGprs);
    void GetConfig(OUT GPRS_LINK_CONFIG &cLinkConfig);
    
    WORD GetLinkInfo(OUT GPRS_LINK_STATE &cLinkInfo);    
    BOOL GetLinkStatus(OUT WORD &wStatus);
    WORD SwitchToMode(IN WORD wModeType, IN DWORD dwTimeOut);

private:
    WORD VerifyParam(void);
    WORD InitGSMModule(void);
    WORD UnlockPIN(void);
    WORD WaitRegisterGSM(void);
    WORD InitGPRSModule(void);
    
    WORD ShowNetworkType(void);
    WORD ShowSignalQuality(void);
    
    BOOL CheckGPRSStatus(void);
    BOOL CheckAttach(OUT BYTE *pbyState);
    BOOL CheckPDPIP(void);
    
    BOOL IsSupportGprs(void);
private:
    GPRS_LINK_CONFIG    m_cLinkConfig;
    BOOL                m_bOpenGprs;
    BOOL                m_bExistLinkConfig;
};

#endif