/*
 * Copyright(c) Castles Technology Co,. Ltd
 *
 * File Name: EMVManage.h
 * Author: Amber Sun
 *
 * Create Date: 6/03/2019
 */

#ifndef __NETSAPI_EMVMANAGE_H__
#define __NETSAPI_EMVMANAGE_H__

#include <typedef.h>
#include <emvaplib.h>

#include "../export.h"


//EMV CAPK
#define RID_SIZE    16
#define INDEX_SIZE   3
#define MODULE_SIZE    512
#define EXPONENT_SIZE    3
#define HASH_SIZE    40
#define AID_SIZE    16 + 1
#define ASI_SIZE  3

#define d_EMV_BUF_SIZE 512
#define d_EMV_TAG_SIZE  4
#define D_AID_NUM  40

#define d_EMV_RC_INVALID_VISA_CA_KEY		0xA00000F4
#define d_EMV_DEBUG_PORT_USB				0xFF

#define d_SC_Present    0x00
#define d_SC_Remove     0x01

//CAPK
#define d_CLEAR_CAPK_RID     (BYTE *)"\xA0\x00\x00\x99\x99"
#define d_CLEAR_CAPK_RID_SIZE  5
#define d_CLEAR_CAPK_MODULE  (BYTE *)"\x99\xC5\xB7\x0A\xA6\x1B\x4F\x4C\x51\xB6\xF9\x0B\x0E\x3B\xFB\x7A\x3E\xE0\xE7\xDB\x41\xBC\x46\x68\x88\xB3\xEC\x8E\x99\x77\xC7\x62\x40\x7E\xF1\xD7\x9E\x0A\xFB\x28\x23\x10\x0A\x02\x0C\x3E\x80\x20\x59\x3D\xB5\x0E\x90\xDB\xEA\xC1\x8B\x78\xD1\x3F\x96\xBB\x2F\x57\xEE\xDD\xC3\x0F\x25\x65\x92\x41\x7C\xDF\x73\x9C\xA6\x80\x4A\x10\xA2\x9D\x28\x06\xE7\x74\xBF\xA7\x51\xF2\x2C\xF3\xB6\x5B\x38\xF3\x7F\x91\xB4\xDA\xF8\xAE\xC9\xB8\x03\xF7\x61\x0E\x06\xAC\x9E\x6B"
#define d_CLEAR_CAPK_MODULE_SIZE 112
#define d_CLEAR_CAPK_HASH    (BYTE *)"\xDD\x92\x06\x3B\x14\x60\x3A\x07\x18\x9B\x99\x8B\xE6\x6B\xC0\xA4\x27\x49\xC6\xC2"
#define  d_CLEAR_CAPK_HASH_SIZE 20

//terminal tag data
#define d_TERM_CONFIG_NEW   (BYTE*)"\x9F\x33\x03\xE0\xF1\xC8\x9F\x40\x05\xF0\x00\xF0\xA0\x01\x9F\x1E\x04\x12\x34\x56\x78\x5F\x2A\x02\x09\x49\x9F\x1A\x02\x07\x92\xDF\xC0\x0F\x9F\x02\x06\x5F\x2A\x02\x9A\x03\x9C\x01\x95\x05\x9F\x37\x04\xDF\xC1\x03\x9F\x37\x04\x9F\x09\x02\x00\x84\x9F\x1B\x04\x00\x00\x03\xE8\xDF\xC4\x04\x00\x00\x00\x05\xDF\xC2\x01\x20\xDF\xC3\x01\x40\xDF\xC6\x05\x00\x00\x00\x00\x00\xDF\xC7\x05\x00\x00\x00\x00\x00\xDF\xC8\x05\x00\x00\x00\x00\x00"
#define d_TERM_CONFIG_LEN_NEW   106

//App Tag data
#define d_CLEAR_APP_AID (BYTE *)"\xA0\x00\x00\x00\x03\x10\x10"
#define d_CLEAR_APP_AID_SIZE    7
#define d_APP_VISA_CONFIG_NEW (BYTE *)"\x9F\x33\x03\xE0\xF1\xC8\x9F\x40\x05\xF0\x00\xF0\xA0\x01\x9F\x35\x01\x22\x9F\x09\x02\x00\x8C\x9F\x1B\x04\x00\x00\x27\x10\xDF\xC4\x04\x00\x00\x00\x05\xDF\xC2\x01\x20\xDF\xC3\x01\x40\xDF\xC6\x05\x00\x10\x00\x00\x00\xDF\xC7\x05\xDC\x40\x04\xF8\x00\xDF\xC8\x05\xDC\x40\x00\xA8\x00"
//(byte*)"\x9F\x09\x02\x00\x8C\x9F\x1B\x04\x00\x00\x27\x10\xDF\xC4\x04\x00\x00\x00\x05\xDF\xC2\x01\x20\xDF\xC3\x01\x40\xDF\xC6\x05\x00\x10\x00\x00\x00\xDF\xC7\x05\xDC\x40\x04\xF8\x00\xDF\xC8\x05\xDC\x40\x00\xA8\x00"
#define d_APP_VISA_CONFIG_LEN_NEW   (69)

//Secure Data White list
#define d_EMV_LIST_DATA (BYTE *)"\x00\x00\x00\x0A\x01\x30\x01\x31\x01\x32\x01\x33\x01\x34\x01\x35\x01\x36\x01\x37\x01\x38\x01\x39"
#define d_EMV_LIST_DATA_SIZE   24


class PRIVATE_EXPORT EMVManage {
public:
    EMVManage();
//    EMVManage(const EMVManage& orig);
    virtual ~EMVManage();
    BOOL EMVInit(BOOL isBReInit, char *pszEmvCfgPath);

    DWORD IsEMVCardExist(void);
    DWORD EMVSetTerminalData(BYTE* pbyAID, BYTE byAIDLen, BYTE* pbyTlvData, INT nTlvDataLen);

//    DWORD EMVPeform(void);

//    DWORD EMVKernelClear(void);

private:
    //EMV set configuration
    DWORD EMVKernelClear(void);
    DWORD EMVSetAPPList(void);
    DWORD EMVSetTerminalConfig(void);
    DWORD EMVSetCAPK(void);
    DWORD EMVSetAppConfig(void);

    //xml operate
    WORD OpenFile(char *pszFileName, BYTE *pbyXmlNode);
    void CloseFile(void);

    DWORD EMVGetCAPK(void);
    DWORD EMVGetAppList(BYTE *pbyAID, BYTE &byAIDLen, BYTE *pbyASI);
    DWORD EMVGetTerminalConfig(BYTE *pbyTag, BYTE &byTagLen, BYTE* pbyTagValue, BYTE &byTagValueLen);
    DWORD EMVGetAppConfig(BYTE *pbyAID, BYTE &byAIDLen, BYTE *pbyTag, BYTE &byTagLen,
                          BYTE *pbyTagValue, BYTE &byTagValueLen);

    void LOGPointer(BYTE * pbyPointer,BYTE byLen, char *pbyStr);


private:
    BYTE *pbyConfigFile;

    //EMV CAPK
    BYTE m_byRID[RID_SIZE];
    BYTE m_byIndex[INDEX_SIZE];
    BYTE m_byModule[MODULE_SIZE];
    BYTE m_byExponent[EXPONENT_SIZE];
    BYTE m_byHash[HASH_SIZE];

};
#endif //NETSAPI_EMVMANAGE_H
