
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: deftype.h
 * Author: Alan.Ren
 * 
 * Description: Basic date types.
 * Create Date: 2018.06.06
 **/

#ifndef _H_H_DEF_TYPE_20180611_H_H_
#define _H_H_DEF_TYPE_20180611_H_H_

#include <string.h>
#include <pthread.h>


#define IN
#define OUT
#define INOUT

#define CHAR	char
#define UCHAR	unsigned char
#define BYTE	unsigned char

#define SHORT	short			// 2byte
#define USHORT	unsigned short		// 2byte
#define WORD	unsigned short		// 2byte
#define PWORD   unsigned short *
#define INT	int		        // 4byte	
#define PINT	int *		        // 4byte	
#define UINT	unsigned int		// 4byte
#define DWORD	unsigned int		// 4byte
#define PDWORD  unsigned int *
#define BOOL	unsigned char		// 1byte
#define LONG	long			// 4byte
#define ULONG	unsigned long		// 4byte
#define PCSTR   const char *
#define PSTR    char *
#define PCBYTE  const unsigned char *
#define PBYTE   unsigned char *

typedef unsigned int    UINT_PTR, *PUINT_PTR;
typedef long            LONG_PTR, *PLONG_PTR;
typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

/**
 * For update TCP channel param
 * ETH WIFI
**/
typedef struct stSocketParam
{
    BYTE    byIp[64];
    BYTE    byPort[8];
}SOCKETPARAM, *PSOCKETPARAM;

/**
 * COM
**/ 
typedef struct stCOMParam
{
    ULONG   ulBaud;
    BYTE    byParity;
    BYTE    byDataBits;
    BYTE    byStopBits;
}COMPARAM, *PCOMPARAM;

/*
 * CHANNEL_MSGID, PCHANNEL_MSGID
 * 
 * @note:
 *  dwSendMsgID:             Send msg with this type, the COMM MODULE would receive it and then send to HOST
 *  dwRcvMsgID:              User should received this msg from the COMM MODULE, 
 *  dwSendRspMsgID:          When a msg is sent to SERVER, we must push the ErrorCode into the QUEUE with this MsgID. the ErrorCode is refer to ErrorCode.h
 *  dwUpdateReqMsgID:        When this message is received from the QUEUE, the module is about to update the channel parameters.
 *  dwUpdateRspMsgID:        The result of the channel parameters update
 *  dwConnStatusReqMsgID:    
 *  dwConnStatusRspMsgID:    
**/
typedef struct stCHANNEL_MsgID
{
    DWORD   dwSendMsgID; 
    DWORD   dwRcvMsgID;
    DWORD   dwSendRspMsgID;
    DWORD   dwUpdateReqMsgID;
    DWORD   dwUpdateRspMsgID;
    DWORD   dwConnStatusReqMsgID;
    DWORD   dwConnStatusRspMsgID;
}CHANNEL_MSGID, *PCHANNEL_MSGID;


/**
 * THREADINFO, PTHREADINFO
 * 
 * @note:
 *      wStatus     : Thread state specified by ulThreadID.
 *      wErrCode    : Thread errcode specified by ulThreadID.
 *      ulThreadID  : A thread ID in the communication module
**/
typedef struct stSTAT_ThreadStat
{
    WORD            wStatus;
    WORD            wErrCode;
    pthread_t       ulThreadID;
} THREADSTATE, *PTHREADSTATE;


/**
 * THREADINFO, PTHREADINFO
 * 
 * @note:
 *      wMaxThtreadNum  : Max number of all COMM threads
 *      stThreadStat    : See the THREADSTATE
**/
typedef struct stINFO_ThreadInfo
{
    WORD            wMaxThtreadNum;
    THREADSTATE     stThreadStat[8];
} THREADINFO, *PTHREADINFO;


/**
 * GPRSAPNPARAM, PGPRSAPNPARAM
 * 
 * @note:
 *      szApn       : GPRS APN
 *      szUserName  : GPRS APN username
 *      szPwd       : GPRS APN password
**/
typedef struct stGprsAPNParam
{
    char        szApn[128];
    char        szUserName[128];
    char        szPwd[128];
}GPRSAPNPARAM, *PGPRSAPNPARAM;


/**
 * GPRS_CHANNEL_STATE, PGPRS_CHANNEL_STATE
 * 
 * @note:
 *      bySoltIndex     : 0 : SIM1, 1 : SIM2
 *      bySignalQuality : output param of CTOS_GSMSignalQuality
 *      byNetworkType   : output param of CTOS_GSMGetNetworkType
 *      wChannelState   : gprs channel state, 0 : connected, 1 : disconnected
 *      wLinkState      : gprs link state, 0 : connected, 1 : disconnected
 *      szIMSI          : IMSI
 *      szSIMID         : SIM ID
 *      szOperatorName  : operator name
 *      szApn           : gprs APN
 *      szUserName      : gprs APN username
 *      szPwd           : gprs APN password
 *      szSimCardPin    : SIM card pin
**/
typedef struct stGprsChannelState
{
    BYTE            bySoltIndex;
    BYTE            bySignalQuality;
    BYTE            byNetworkType;
    WORD            wChannelState;
    WORD            wLinkState;
    char            szIMSI[128];
    char            szSIMID[128];
    char            szOperatorName[128];
    char            szApn[128];
    char            szUserName[128];
    char            szPwd[128];
    char            szSimCardPin[128];
}GPRS_CHANNEL_STATE, *PGPRS_CHANNEL_STATE;


/**
 * ETH_CHANNEL_STATE, PETH_CHANNEL_STATE
 * 
 * @note:
 *      bEnableDHCP     : enable dhcp, 0 : not DHCP, 1 : DHCP
 *      wChannelState   : ethernet channel state, 0 : connected, 1 : disconnected
 *      wLinkState      : ethernet link state, 0 : connected, 1 : disconnected
 *      szIP            : local ip
 *      szMask          : mask
 *      szGateway       : gateway
 *      szDNS           : dns
**/
typedef struct stEthChannelState
{
    BOOL            bEnableDHCP;
    WORD            wChannelState;
    WORD            wLinkState;
    
    char            szIP[32];
    char            szMask[32];
    char            szGateway[32];
    char            szDNS[32];
}ETH_CHANNEL_STATE, *PETH_CHANNEL_STATE;


/**
 * WIFI_CHANNEL_STATE, PWIFI_CHANNEL_STATE
 * 
 * @note:
 *      bEnableDHCP     : enable/disable dhcp, 0 : not DHCP, 1 : DHCP
 *      bAutoConnAP     : auto connect wifi ap
 *      wLinkState      : wifi link state, 0 : connected, 1 : disconnected
 *      wChannelState   : wifi channel state, 0 : connected, 1 : disconnected
 *      szIP            : local ip
 *      szMask          : mask
 *      szGateway       : gateway
 *      szDNS           : dns
 *      szWifiName      : wifi ESSID
 *      szPassword      : wifi password
**/
typedef struct stWifiChannelState
{
    BOOL            bEnableDHCP;
    BOOL            bAutoConnAP;
    WORD            wChannelState;
    WORD            wLinkState;
    
    char            szIP[32];
    char            szMask[32];
    char            szGateway[32];
    char            szDNS[32];
    char            szWifiName[64];
    char            szPassword[64];
}WIFI_CHANNEL_STATE, *PWIFI_CHANNEL_STATE;


/**
 * CURL_RESPONSE_INFO
 * 
 * @note:
 *      wRetValue     : curl return code
 *      wHeadSize     : head data size of the host's response, maximum is 0x400
 *      wBodySize     : body data size of the host's response, maximum is 0x1000
 *      byHeadData    : head data
 *      byBodyData    : body data
**/
typedef struct
{
    OUT WORD        wRetValue;
    OUT WORD        wHeadSize;
    OUT WORD        wBodySize;
    OUT BYTE        byHeadData[0x400];
    OUT BYTE        byBodyData[0x1000];
}CURL_RESPONSE_INFO;
#endif