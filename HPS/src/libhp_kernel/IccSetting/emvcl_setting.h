/*
 * Copyright(c) Castles Technology Co,. Ltd
 *
 * File Name: EMVCLManage.h
 * Author: Amber Sun
 *
 * Create Date: 5/17/2019
 */

#ifndef NETSAPI_EMVCLMANAGE_H
#define NETSAPI_EMVCLMANAGE_H



#include <ctosapi.h>
#include <emv_cl.h>

#include "../ByteStream.h"
#include "../FunBase.h"
#include "../export.h"

#define TAG_NOT_FOUND   1

//EMVCL CAPK
#define RID_SIZE    16
#define INDEX_SIZE   3
#define MODULE_SIZE    512
#define EXPONENT_SIZE    3
#define HASH_SIZE    40

//EMVCL TagCombination
#define AID_SIZE 16 + 1 
#define KERNEL_ID_SIZE 2 + 1
#define TXN_TYPE_SIZE 2 + 1

#define PARA_INDEX_SIZE  5

#define d_TAG_SIZE  4
#define TLV_DATA_SIZE 1110
#define d_EMVCL_BUF_LEN 256

#define d_EMVCL_LIST_DATA  (BYTE *) "\x00\x00\x00\x0A\x01\x30\x01\x31\x01\x32\x01\x33\x01\x34\x01\x35\x01\x36\x01\x37\x01\x38\x01\x39"
#define d_EMVCL_LIST_DATA_SIZE 24

#define d_UDOL_DF8F49  (BYTE *) "DF8F4963FFC3604F00500057005A0082008400C500C7005F20005F24005F28009F33009F35009F26009F27009F10009F37009F360095009F03009A009B009C009F02009F09005F2A009F1A009F1E009F34005F34009F41009F42009F44009F53009F6E009F7700DF8F4E0CFFC3094F0057009B005F3400"
#define d_UDOL_DF8F49_SIZE   238
#define d_UDOL_DF5A  (BYTE*) "DF5A63FFC3604F00500057005A0082008400C500C7005F20005F24005F28009F33009F35009F26009F27009F10009F37009F360095009F03009A009B009C009F02009F09005F2A009F1A009F1E009F34005F34009F41009F42009F44009F53009F6E009F7700DF8F4E0CFFC3094F0057009B005F3400"
#define d_UDOL_DF5A_SIZE     236

#define d_AID_VISA       "A000000003"
#define d_AID_NETS       "A000000541"
#define d_AID_AMEX       "A000000025"
#define d_AID_MASTER     "A000000004"
#define d_AID_JCB        "A000000065"
#define d_AID_UNIONPAY   "A000000333"


class PRIVATE_EXPORT EMVCLManage {
public:
    EMVCLManage();
//    EMVCLManage(const EMVCLManage& orig);
    virtual ~EMVCLManage();

    BOOL EMVCLInit(BOOL isBReInit, char *pszEmvclCfgPath);
    WORD IsCTLSCardExist(void);

private:

    //EMVCL set configuration
    void EMVCLKernelClear(void);
    DWORD EMVCLSetCAPK(void);
    DWORD EMVCLSetTagData(void);

    DWORD EMVCLSetPara(void);
    
    //xml operate 
    WORD OpenFile(char *pszFileName, BYTE *pbyXmlNode);
    void CloseFile(void);
    DWORD EMVGetCAPK(void);
    DWORD EMVCLGetTagData(void);

private:    
    BYTE *pbyConfigFile;

    //EMVCL CAPK
    BYTE m_byRID[RID_SIZE];
    BYTE m_byIndex[INDEX_SIZE];
    BYTE m_byModule[MODULE_SIZE];
    BYTE m_byExponent[EXPONENT_SIZE];
    BYTE m_byHash[HASH_SIZE];
    
    //EMVCL TagCombination
    BYTE m_byAID[AID_SIZE];
    BYTE m_byKernelID[KERNEL_ID_SIZE];
    BYTE m_byTxnType[TXN_TYPE_SIZE];
    BYTE m_byTagData[TLV_DATA_SIZE];

};
#endif //NETSAPI_EMVCLMANAGE_H
