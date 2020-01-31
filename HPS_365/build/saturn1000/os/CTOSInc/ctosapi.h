/********************************************************************
Copyright (c) Castles Technology Co., Ltd. 2003-2005

Module Name:
	ctosapi.h

Abstract:

Created:
	2005/11/25

Author:
	Peril Chen

Revision History:

*********************************************************************/
#ifndef __CTOS_API_H__
#define __CTOS_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "typedef.h"
#include "ctoserr.h"

//=============================================================================================================================
//
// System Functions
//
//=============================================================================================================================
#define ID_BOOTSULD					0
#define ID_CRYPTO_HAL				1
#define ID_LINUX_KERNEL				3
#define ID_SECURITY_KO				4
#define ID_SYSUPD_KO				5
#define ID_KMS                      6       //NEW ID ID_KMS
#define ID_CADRV_KO					7
#define ID_CAUSB_KO					8
#define ID_LIBCAUART_SO				9
#define ID_LIBCAUSBH_SO				10
#define ID_LIBCAMODEM_SO			11
#define ID_LIBCAETHERNET_SO			12
#define ID_LIBCAFONT_SO				13
#define ID_LIBCALCD_SO				14
#define ID_LIBCAPRT_SO				15
#define ID_LIBCARTC_SO				16
#define ID_LIBCAULDPM_SO   			17
#define ID_LIBCAPMODEM_SO   		18
#define ID_LIBCAGSM_SO   			19
#define ID_LIBCAEMVL2_SO   			20
#define ID_LIBCAKMS_SO    			21
#define ID_LIBCAFS_SO    			22
#define ID_LIBCABARCODE_SO    		23
#define ID_CRADLE_MP	    		24 //for Cradle-Base Main-program
#define ID_LIBTLS_SO				25
#define ID_LIBCLVW_SO				26
#define ID_LIBCTOSAPI_SO			27
#define ID_SAM_KO          		    28      //NEW ID ID_SAM_KO
#define ID_CLVWM_MP					29
#define ID_ROOTFS					30
#define ID_BIOS						31
#define ID_CIF_KO                   32
#define ID_CLDRV_KO                 33
#define ID_TMS                      34
#define ID_ULDPM                    35
#define ID_SC_KO                    36
#define ID_EMV_SO				37
#define ID_EMVCL_SO				38
#define ID_MAXIMUM					38 // Maximum ID number of object files

USHORT CTOS_GetSystemInfo(BYTE bID, BYTE baBuf[17]);
USHORT CTOS_getDefApp(char* apDefPackage);

//=============================================================================================================================
//
// Clock and Time Functions
//
//=============================================================================================================================
// Real Time Clock define for RTC functions
typedef struct
{
    BYTE    bSecond;
    BYTE    bMinute;
    BYTE    bHour;
    BYTE    bDay;
    BYTE    bMonth;
    BYTE    bYear;
    BYTE    bDoW;
} CTOS_RTC;

//Time parameter
#define TIMER_ID_1		0
#define TIMER_ID_2		1
#define TIMER_ID_3		2
#define TIMER_ID_4		3

USHORT CTOS_RTCGet(CTOS_RTC *pstRTC);
USHORT CTOS_RTCSet(CTOS_RTC *pstRTC);
void CTOS_Delay(ULONG ulMSec);
ULONG CTOS_TickGet(void);
USHORT CTOS_TimeOutSet(BYTE bTID, ULONG ulMSec);
USHORT CTOS_TimeOutCheck(BYTE bTID);

//=============================================================================================================================
//
// Sound Functions
//
//=============================================================================================================================
USHORT CTOS_Beep(void);
USHORT CTOS_Sound(USHORT usFreq, USHORT usDuration);

//=============================================================================================================================
//
// File Functions
//
//=============================================================================================================================
// Storage Type
#define	d_STORAGE_FLASH						0
#define	d_STORAGE_SRAM						1

// Attribute
#define d_FA_PRIVATE							0
#define d_FA_READ									1		//	Read by other APs
#define d_FA_WRITE								2		//	Written by other APs
#define d_FA_PUBLIC								0xFF

#define d_SEEK_FROM_BEGINNING			0
#define	d_SEEK_FROM_CURRENT				1
#define	d_SEEK_FROM_EOF						2

typedef struct
{
    BYTE Filename[15];
    ULONG Filesize;
    BYTE FileAttrib;
    ULONG UID;
} CTOS_FILE_ATTRIB;

typedef struct
{
    BYTE  Filename[15];
    ULONG Filesize;
    BYTE  FileStorageType;
    BYTE  RFU[4];
} CTOS_FILE_INFO;

USHORT CTOS_FileOpen(BYTE *caFileName , BYTE bStorageType, ULONG *pulFileHandle);
USHORT CTOS_FileOpenAttrib(BYTE *caFileName , BYTE bStorageType, ULONG *pulFileHandle, BYTE bAttrib);
USHORT CTOS_FileClose(ULONG ulFileHandle);
USHORT CTOS_FileDelete(BYTE *caFileName);
USHORT CTOS_FileGetSize(BYTE *caFileName, ULONG *pulFileSize);
USHORT CTOS_FileSeek(ULONG ulFileHandle, ULONG ulOffset, BYTE bOrigin);
USHORT CTOS_FileRead(ULONG ulFileHandle, BYTE *baBuffer, ULONG *pulActualLength);
USHORT CTOS_FileWrite(ULONG ulFileHandle, BYTE *baBuffer, ULONG ulBufferLength);
USHORT CTOS_FileDir(BYTE *baFileBuf, ULONG *pulFileSizeDir, USHORT *pusLen);
USHORT CTOS_FileCut (BYTE *caFileName, ULONG ulFileLen);
USHORT CTOS_FileRename (BYTE *caFileName, BYTE *caFileNewName);
USHORT CTOS_FileGetPosition(ULONG ulFileHandle, ULONG *pulPosition);
USHORT CTOS_FileReopen(BYTE *caFileName, BYTE bStorageType, ULONG *pulFileHandle);
USHORT CTOS_FileSetAttrib(BYTE* caFileName, BYTE bAttrib);
USHORT CTOS_FileDirAttrib(CTOS_FILE_ATTRIB *pstFA , USHORT *pusFileNumber);
USHORT CTOS_FileDirA(CTOS_FILE_INFO *pstInfo , USHORT *pusLen);
USHORT CTOS_FileGetAttrib(BYTE* caFileName, CTOS_FILE_ATTRIB *pstFA);
USHORT CTOS_FileFormat(BYTE bType);
USHORT CTOS_SystemMemoryStatus(ULONG* ulUsedDiskSize ,ULONG* ulTotalDiskSize ,ULONG* ulUsedRamSize ,ULONG* ulTotalRamSize);

//=============================================================================================================================
//
// Contactless Functions
//
//=============================================================================================================================
// Type definition
#define d_CL_TYPE_A						0
#define d_CL_TYPE_B						1
#define d_CL_TYPE_C						2
 
// AntennaParam baudrate definition
#define d_CL_BR_106						0
#define d_CL_BR_212						1
#define d_CL_BR_424						2
#define d_CL_BR_848						3

// FSDI definition
#define d_CL_FSDI_16						0
#define d_CL_FSDI_24						1
#define d_CL_FSDI_32						2
#define d_CL_FSDI_40						3
#define d_CL_FSDI_48						4
#define d_CL_FSDI_64						5
#define d_CL_FSDI_96						6
#define d_CL_FSDI_128						7
#define d_CL_FSDI_256						8
#define d_CL_DFT_FSDI						8
#define d_CL_MAX_FSDI						8

// NumSlot definition
#define d_CL_NUMSLOT_x1						0x00
#define d_CL_NUMSLOT_x2						0x01
#define d_CL_NUMSLOT_x4						0x02
#define d_CL_NUMSLOT_x8						0x03
#define d_CL_NUMSLOT_x16					0x04

// MaxRate definition
#define d_CL_MAXRATE_106					106
#define d_CL_MAXRATE_212					212
#define d_CL_MAXRATE_424					424
#define d_CL_MAXRATE_848					848

// CL Open/Close Comm Channel
USHORT CTOS_CLInitComm(ULONG uiBaudRate);

// CL General
USHORT CTOS_CLInit(void);
USHORT CTOS_CLPowerOn(void);
USHORT CTOS_CLPowerOff(void);

// ISO14443-3 Type A
USHORT CTOS_CLTypeAHalt(void);
USHORT CTOS_CLTypeAActiveFromIdle(BYTE bBaudRate,BYTE* baATQA,BYTE *bSAK,BYTE* baCSN,BYTE* bCSNLen);
USHORT CTOS_CLTypeAActiveFromHalt(BYTE bBaudRate,BYTE* baATQA,BYTE *bSAK,BYTE* baCSN,BYTE bCSNLen);

// Mifare function
USHORT CTOS_MifareLOADKEY(UCHAR* baKey);
USHORT CTOS_MifareAUTHEx(UCHAR baKeyType,UCHAR bBlockNr,UCHAR* baCardSN,UCHAR bCardSNLen);
USHORT CTOS_MifareREADBLOCK(UCHAR bBlockNr,UCHAR* baBuf);
USHORT CTOS_MifareWRITEBLOCK(UCHAR bBlockNr,UCHAR* baBuf);

// ISO14443-3 Type B
USHORT CTOS_REQB(UCHAR bAFI,UCHAR* baATQB,UCHAR* bATQBLen);
USHORT CTOS_WUPB(UCHAR bAFI,UCHAR* baATQB,UCHAR* bATQBLen);
USHORT CTOS_ATTRIB(UCHAR* baPUPI,UCHAR* bRep);
USHORT CTOS_HALTB(BYTE *baPUPI);
USHORT CTOS_CLTypeBActive(BYTE* baPUPI);
USHORT CTOS_CLTypeBActiveEx(BYTE* baPUPI,UCHAR bAFI,BOOL fWUPB,UCHAR bFSDI,UCHAR bNumAntiColSlot,USHORT usMaxRate);

// ISO14443-4 T=CL
USHORT CTOS_CLRATS(BYTE bAutoBR,BYTE* baATS,USHORT* bATSLen);
USHORT CTOS_CLAPDU(BYTE* baSBuf,USHORT usSLen,BYTE* baRBuf,USHORT* usRLen);
USHORT CTOS_CLDESELECT(void);

// ISO18092 Felica
USHORT CTOS_FelicaPolling(UCHAR* IDm, UCHAR* PMm);

//=============================================================================================================================
//
// Smart Card Functions
//
//=============================================================================================================================
// value for bID
#define d_SC_USER							0
#define d_SC_SAM1							1
#define d_SC_SAM2							2
#define d_SC_SAM3							3
#define d_SC_SAM4							4
#define d_SC_SAM5							5
#define d_SC_SAM6							6
#define d_SC_MAX_SOCKET						4

// value for bVolt
#define d_SC_5V								1
#define d_SC_3V								2
#define d_SC_1_8V							3


// mask for bStatus
#define d_MK_SC_PRESENT						1
#define d_MK_SC_ACTIVE						2


USHORT CTOS_SCStatus(BYTE bID, BYTE *baStatus);
USHORT CTOS_SCPowerOff(BYTE bID);
USHORT CTOS_SCResetEMV(BYTE bID, BYTE bVolt, BYTE *baATR, BYTE *baATRLen, BYTE *baCardType);
USHORT CTOS_SCResetISO(BYTE bID, BYTE bVolt, BYTE *baATR, BYTE *baATRLen, BYTE *baCardType);
USHORT CTOS_SCWarmResetEMV(BYTE bID, BYTE bVolt, BYTE *baATR, BYTE *baATRLen, BYTE *baCardType);
USHORT CTOS_SCWarmResetISO(BYTE bID, BYTE bVolt, BYTE *baATR, BYTE *baATRLen, BYTE *baCardType);
USHORT CTOS_SCCommonReset(BYTE bID, BYTE bTA1, BOOL fColdReset, BOOL fEMV, BOOL fPTS, BOOL fIFSD, BYTE bVolt, BYTE *baATR, BYTE *baATRLen, BYTE *baCardType);
USHORT CTOS_SCSendAPDU(BYTE bID, BYTE *baSBuf, USHORT usSLen, BYTE *baRBuf, USHORT *pusRLen);
USHORT CTOS_I2CCReset(UCHAR *baBuf, BYTE *bLen);
USHORT CTOS_I2CReadMemory(BYTE bCmd,BYTE bNumAddr,USHORT usAddr,BOOL fDummyRead,UCHAR*baBuf,USHORT*usLen);
USHORT CTOS_I2CWriteMemory(BYTE bCmd,BYTE bNumAddr,USHORT usAddr,UCHAR*baBuf,USHORT usLen);
	
//=============================================================================================================================
//
// MSR Functions
//
//=============================================================================================================================
//=============================================================================================================================
//
// MSR Functions
//
//=============================================================================================================================
// output value for bTkxErr
#define d_MSR_SUCCESS						0x00
#define d_MSR_STX_NOT_FOUND					0x81
#define d_MSR_NO_DATA						0x82
#define d_MSR_BUF_OVERFLOW					0x83
#define d_MSR_ETX_NOT_FOUND					0x84
#define d_MSR_LRC_NOT_FOUND					0x85
#define d_MSR_LRC_ERR						0x86
#define d_MSR_UNKNOWN_CHAR					0x87
#define d_MSR_NONE_SWIPE					0x88

// mask for return value of CTOS_MSRRead()
#define d_MK_MSR_TK1						0x0001
#define d_MK_MSR_TK2						0x0002
#define d_MK_MSR_TK3						0x0004
#define d_MK_MSR_TK1_ERR					0x0010
#define d_MK_MSR_TK2_ERR					0x0020
#define d_MK_MSR_TK3_ERR					0x0040
USHORT CTOS_MSRRead(BYTE*baTk1Buf, USHORT*pusTk1Len, BYTE*baTk2Buf, USHORT*pusTk2Len, BYTE*baTk3Buf, USHORT*pusTk3Len);
USHORT CTOS_MSRGetLastErr (BYTE*baTk1Err, BYTE*baTk2Err, BYTE*baTk3Err);

//=============================================================================================================================
//
// Power Saving Functions
//
//=============================================================================================================================
// value for bMode
#define d_PWR_STANDBY_MODE				0x00
#define d_PWR_SLEEP_MODE				0x01
#define d_PWR_REBOOT					0x02
#define d_PWR_POWER_OFF					0x03


//System Wait Event
#define d_EVENT_KBD						0x00000001
#define d_EVENT_SC						0x00000002
#define d_EVENT_MSR						0x00000004
#define d_EVENT_MODEM					0x00000008
#define d_EVENT_ETHERNET				0x00000010
#define d_EVENT_COM1					0x00000020
#define d_EVENT_COM2					0x00000040
#define d_TIME_INFINITE					0xFFFFFFFF

USHORT CTOS_PowerSource(UCHAR* bSrc);
USHORT CTOS_PowerMode(BYTE bMode);
void CTOS_SystemReset(void);
USHORT CTOS_SystemWait(DWORD dwTimeout, DWORD dwWaitEvent, DWORD* pdwWakeupEvent);
USHORT CTOS_PowerAwakening(SHORT sSecond, SHORT sMinute, SHORT sHour, SHORT sDay, SHORT sMonth, SHORT sYear);

//=============================================================================================================================
//
// CRYPTO Functions
//
//=============================================================================================================================


#define SHA_CTX SHA1Context

typedef struct SHA1Context
{
    unsigned long Message_Digest[5];
    unsigned long Length_Low;
    unsigned long Length_High;
    unsigned char Message_Block[64];
    int Message_Block_Index;
    int Computed;
    int Corrupted;
} SHA1Context;


typedef struct
{
    IN BYTE Version;
    IN USHORT bits;
    OUT USHORT m_len;
    OUT BYTE *m;
    OUT USHORT d_len;
    OUT BYTE *d;
    IN  USHORT e_len;
    IN  BYTE *e;
}CTOS_RSA_KEY_GEN;


#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA256_CTX stSHA256_ctx
#define CTOS_SHA256_CTX stSHA256_ctx
typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA256_BLOCK_SIZE];
    ULONG h[8];
} stSHA256_ctx;



USHORT CTOS_RNG(BYTE *baResult);
USHORT CTOS_DES (BYTE bEncDec, BYTE* baKey, BYTE bKeyLen, BYTE* baData, USHORT usDataLen, BYTE* baResult);
USHORT CTOS_DES_CBC (BYTE bEncDec, BYTE* baKey, BYTE bKeyLen, BYTE* baICV, BYTE* baData, USHORT usDataLen, BYTE* baResult);
USHORT CTOS_AES_ECB (BYTE bEncDec, BYTE* baKey, BYTE bKeyLen, BYTE* baData, USHORT usDataLen, BYTE* baResult);
USHORT CTOS_AES_CBC (BYTE bEncDec, BYTE* baKey, BYTE bKeyLen, BYTE* baICV, BYTE* baData, USHORT usDataLen, BYTE* baResult);
USHORT CTOS_MAC (BYTE* baKey, BYTE bKeyLen, BYTE* baICV, BYTE* baData, USHORT usDataLen, BYTE* baMAC);
void CTOS_SHA1Init(SHA_CTX *pstInfo);
void CTOS_SHA1Update(SHA_CTX *pstInfo, BYTE* baBuffer, USHORT usCount);
void CTOS_SHA1Final(BYTE* baOutput, SHA_CTX *pstInfo);
void CTOS_SHA1(BYTE *baBuffer, USHORT usCount, BYTE *baOutput);
void CTOS_SHA256Init(SHA256_CTX *ctx);
void CTOS_SHA256Update(SHA256_CTX *ctx, const BYTE *message, UINT len);
void CTOS_SHA256Final(SHA256_CTX *ctx, BYTE *digest);
void CTOS_SHA256(const BYTE *message, UINT len, BYTE *digest);
USHORT CTOS_RSA(BYTE *baModulus, USHORT usModulousLen, BYTE *baExponent, USHORT usExponentLen, BYTE *baData, BYTE *baResult);
USHORT CTOS_RSAKeyGenerate(CTOS_RSA_KEY_GEN *para);




//=============================================================================================================================
//
// KMS2 Functions
//
//=============================================================================================================================
#define d_KBD_INVALID                       0xFF
#define d_KBD_1                                                         '1'
#define d_KBD_2                                                         '2'
#define d_KBD_3                                                         '3'
#define d_KBD_4                                                         '4'
#define d_KBD_5                                                         '5'
#define d_KBD_6                                                         '6'
#define d_KBD_7                                                         '7'
#define d_KBD_8                                                         '8'
#define d_KBD_9                                                         '9'
#define d_KBD_0                                                         '0'

#define d_KBD_K1                                                        'X'
#define d_KBD_K2                                                        'Y'
#define d_KBD_K3                                                        'I'
#define d_KBD_K4                                                        'J'
#define d_KBD_UP                                                        'U'
#define d_KBD_DOWN                                                      'D'

#define d_KBD_CANCEL                                                    'C'
#define d_KBD_CLEAR                                                     'R'
#define d_KBD_ENTER                                                     'A'
#define d_KBD_DOT                                                       'Q'
#define d_KBD_00                                                        'P'

#define d_KBD_X                                                         'Q'
#define d_KBD_COMMA                                                     'P'

#define d_KBD_CREDITCARD                                                'D'
#define d_KBD_SUBTOTAL                                                  'S'
#define d_KBD_POWER                                                     'W'
#define d_KBD_PERCENTAGE                                                'E'
#define d_KBD_PLU                                                       'U'
#define d_KBD_BACK                                                      'B'

#define d_KBD_F1                                                        'X'
#define d_KBD_F2                                                        'Y'
#define d_KBD_F3                                                        'I'
#define d_KBD_F4                                                        'J'
#define d_KBD_F5                                                        'K'
#define d_KBD_F6                                                        'L'
#define d_KBD_F7                                                        'M'
#define d_KBD_F8                                                        'N'


//=============================================================================================================================
//
// KMS2 Functions
//
//=============================================================================================================================

// cadf_crypto
#define d_CRYPTO_LIB_BAD_PARAMETER          0x0901
#define d_CRYPTO_INVALID_PARA				0x0801
#define d_CRYPTO_RSA_GEN_KEY_FAILED         0x0804
#define d_OK 								0x0000

// cadf_kms2
#define CAKMS2_KSN_LEN									20
#define CAKMS2_MAX_INPUT_DATA_LEN						2038
#define CAKMS2_MAX_OUTPUT_DATA_LEN						CAKMS2_MAX_INPUT_DATA_LEN + CAKMS2_KSN_LEN

// PIN Block Type
#define KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0						0x00
#define KMS2_PINBLOCKTYPE_ISBAN_REVERSIBLE_PIN_4B_MODE			0x01
#define KMS2_PINBLOCKTYPE_ISBAN_IRREVERSIBLE_PIN_4B_MODE		0x02
#define KMS2_PINBLOCKTYPE_THAILAND_SCB_BANK 				    0x10

// PIN Protection Cipher Method
#define KMS2_PINCIPHERMETHOD_ECB								0x00
#define KMS2_PINCIPHERMETHOD_EMV_OFFLINE_PIN			        0x02
#define KMS2_PINCIPHERMETHOD_CBC								0x03

// Reserved Keys for System
#define KMS2_RESERVED_KEY_SETS_0000				0x0000
#define KMS2_RESERVED_KEY_SETS_START			0xFF00
#define KMS2_RESERVED_KEY_SETS_END				0xFFFF

//---------------------------------------------------------------------------------------------
// Key Type
#define KMS2_KEYTYPE_3DES						0x01
#define KMS2_KEYTYPE_3DES_DUKPT					0x02 	// ANS X9.24 - 2004
#define KMS2_KEYTYPE_AES						0x03
#define KMS2_KEYTYPE_RSA						0x04
#define KMS2_KEYTYPE_DES_DUKPT					0x05 	// ANS X9.24 - 1998
#define KMS2_KEYTYPE_HMAC						0x06

// Key Attribute
#define KMS2_KEYATTRIBUTE_PIN											0x00000001
#define KMS2_KEYATTRIBUTE_ENCRYPT										0x00000002
#define KMS2_KEYATTRIBUTE_MAC											0x00000004
#define KMS2_KEYATTRIBUTE_KPK											0x00000008
#define KMS2_KEYATTRIBUTE_DECRYPT										0x00000010
#define KMS2_KEYATTRIBUTE_KBPK										0x00000020
#define KMS2_KEYATTRIBUTE_SK_ENCRYPT									0x00000040
#define KMS2_KEYATTRIBUTE_INTERMEDIATE									0x00000080
#define KMS2_KEYATTRIBUTE_FREEZE_RSA_WRITE_KEY_BY_CERTIFICATE			0x00000100
#define KMS2_KEYATTRIBUTE_FREEZE_RSA_ENCRYPT							0x00000200
#define KMS2_KEYATTRIBUTE_CONSIDER_INVALID_BIT_AS_VALID_FOR_KEY_VALUE_UNIQUE	0x10000000
#define KMS2_KEYATTRIBUTE_VALUE_UNIQUE									0x20000000
#define KMS2_KEYATTRIBUTE_PROTECTED										0x40000000

// Key Protection Mode
#define KMS2_KEYPROTECTIONMODE_PLAINTEXT		0x00 //
#define KMS2_KEYPROTECTIONMODE_KPK_ECB			0x01 // 3DES, AES, 3DES_DUKPT for cipher
#define KMS2_KEYPROTECTIONMODE_KPK_CBC			0x05 // 3DES, AES, 3DES_DUKPT for cipher

// Key Verification Method
#define KMS2_KEYVERIFICATIONMETHOD_DEFAULT		0x00

// Cipher method
#define KMS2_DATAENCRYPTCIHERMETHOD_ECB_REVERSED_KEY_ORDER         0x80

// User Data Area
#define KMS2_PRIVATE_USER_DATA_MAX_SIZE			(16 * 1024)
#define KMS2_COMMON_USER_DATA_MAX_SIZE			(64 * 1024)



/****************************************************************************
 * For CTOS_KMS2KeySwap Function                                            *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN USHORT KeySet;

        IN USHORT KeyIndex;

    }Source1;

    struct
    {
        IN USHORT KeySet;

        IN USHORT KeyIndex;

    }Source2;

}CTOS_KMS2KEYSWAP_PARA;

/****************************************************************************
 * For CTOS_KMS2DATAENCRYPT Function                                        *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;
        IN BYTE CipherMethod;

        // This is used for KeyType is KMS2_KEYTYPE_3DES/KMS2_KEYTYPE_AES
        // If SK_Length is 0, SK will not be calculated and used.
        IN BYTE SK_Length;
        IN BYTE* pSK;

    }Protection;

    // This is used for KeyType is KMS2_KEYTYPE_3DES_DUKPT
    struct
    {
        IN BOOL IsUseCurrentKey;
    }DUKPT_PARA;

    struct
    { 
        IN USHORT Length;
        IN BYTE* pData;

        IN USHORT ICVLength;
        IN BYTE* pICV;
    }Input;

    // This is not used if CipherMethod is KMS2_DATAENCRYPTCIPHERMETHOD_EXTPIN_ECB
    struct
    {
        OUT USHORT Length;
        OUT BYTE* pData;
        OUT BYTE KSNLen;
        OUT BYTE* pKSN;
    }Output;

}CTOS_KMS2DATAENCRYPT_PARA;

/****************************************************************************
 * For CTOS_KMS2MAC Function                                                *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;
        IN BYTE CipherMethod;

        // This is used for KeyType is KMS2_KEYTYPE_3DES/KMS2_KEYTYPE_AES
        // If SK_Length is 0, SK will not be calculated and used.
        IN BYTE SK_Length;
        IN BYTE* pSK;
    }Protection;

    struct
    {
        BYTE Length;
        BYTE* pData;
    }ICV;

    // This is used for KeyType is KMS2_KEYTYPE_3DES_DUKPT
    struct
    {
        IN BOOL IsUseCurrentKey;
    }DUKPT_PARA;

    struct
    {
        IN USHORT Length;
        IN BYTE* pData;
    }Input;

    struct
    {
        OUT USHORT Length;
        OUT BYTE* pData;
        OUT BYTE KSNLen;
        OUT BYTE* pKSN;
    }Output;

}CTOS_KMS2MAC_PARA;

/****************************************************************************
 * For CTOS_KMS2KeyGetInfo Function                                         *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN USHORT KeySet;

        IN USHORT KeyIndex;

        // Only used for KeyType 3DES/3DES-DUKPT/AES
        IN BYTE CVLen;

        // Only used for KeyType RSA
        IN BYTE HashAlgorithm;

    }Input;

    struct
    {
        OUT BYTE KeyType;
        OUT BYTE KeyVersion;
        OUT DWORD KeyAttribute;
        OUT USHORT KeyLength;

        // Only used for KeyType 3DES/3DES-DUKPT/AES
        OUT BYTE* pCV;

        // Only used for KeyType RSA
        OUT USHORT KeyExponentLength;

        // Only used for KeyType RSA
        // Calculated with the following input data in order:
        //      Modulus Length - 2 bytes, MSB to LSB
        //      Modulus
        //      Exponent Length - 2 bytes, MSB to LSB
        //      Exponent
        OUT BYTE* pHash;
    }Output;

}CTOS_KMS2KEYGETINFO_PARA;

/****************************************************************************
 * For CTOS_KMS2KeyWriteByTR31 Function                                     *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN USHORT KeySet;
        IN USHORT KeyIndex;

    }Info;

    struct
    {
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;

    }Protection;

    struct
    {
        IN USHORT KeyLength;
        IN BYTE* pKeyData;

    }Value;

}CTOS_KMS2KEYWRITEBYTR31_PARA;

/****************************************************************************
 * For SB_KMS2KEYWRITE_PARA Function                                        *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        // Note that 0x0000 and 0xFF00 to 0xFFFF are reserved for system.
        IN USHORT KeySet;

        IN USHORT KeyIndex;

        IN BYTE KeyType;

        IN BYTE KeyVersion;

        // For KeyType RSA, only accepts KMS2_KEYATTRIBUTE_PROTECTED
        IN DWORD KeyAttribute;
    }Info;

    // This is not used for KeyType RSA
    struct
    {
        IN BYTE Mode;

        // This is used for KPK_ECB and TR31 Mode
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;

        struct
        {
            // This is used for TR31 Mode
            // This is used as ICV for SELF_UPDATE_CBC mode
            IN USHORT Length;
            IN BYTE* pData;
        }AdditionalData;

    }Protection;

    struct
    {
        IN USHORT KeyLength;
        // For KeyType RSA, this is used for Modulus.
        IN BYTE* pKeyData;
    }Value;

    // This is not used if Protection.Mode is KMS2_KEYPROTECTIONMODE_PLAINTEXT
    // This is not used for KeyType RSA
    struct
    {
        IN BYTE Method;
        IN USHORT KeyCheckValueLength;
        IN BYTE* pKeyCheckValue;
    }Verification;

    // Optional. Used only for KeyType RSA
    struct
    {
        IN USHORT Length;
        IN BYTE* pValue;
    }Exponent;

    // Optional. Used only for KeyType 3DES_DUKPT
    struct
    {
        IN BYTE KSNLength;
        IN BYTE *pKSN;
    }DUKPT_DATA;

}CTOS_KMS2KEYWRITE_PARA;
/****************************************************************************
 * For CTOS_KMS2PINGET_PARA Function                                        *
 ****************************************************************************/
typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN BYTE BlockType;
        IN BYTE PINDigitMaxLength;
        IN BYTE PINDigitMinLength;
    }PIN_Info;

    struct
    {
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;
        IN BYTE CipherMethod;

        // This is used for KeyType is KMS2_KEYTYPE_3DES/KMS2_KEYTYPE_AES
        // If SK_Length is 0, SK will not be calculated and used.
        IN BYTE SK_Length;
        IN BYTE* pSK;
    }Protection;

    struct
    {
        // This is used for PAN if BlockType is KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0.
        // Ths is used for terminal pseudo random if BlockType  is KMS2_PINBLOCKTYPE_ISBAN_REVERSIBLE_PIN_4B_MODE.
        // This is used for PK if BlockType is KMS2_PINBLOCKTYPE_ISBAN_IRREVERSIBLE_PIN_4B_MODE.
        IN BYTE InLength;
        IN BYTE* pInData;

    }AdditionalData;

    // This is used for KeyType is KMS2_KEYTYPE_3DES_DUKPT
    struct
    {
        IN BOOL IsUseCurrentKey;
    }DUKPT_PARA;

    struct
    {
        INOUT USHORT EncryptedBlockLength;
        OUT BYTE* pEncryptedBlock;
        OUT BYTE PINDigitActualLength;
    }PINOutput;

    struct
    {
        IN DWORD Timeout;
        IN BYTE AsteriskPositionX;
        IN BYTE AsteriskPositionY;
        IN BYTE NULLPIN;
        IN int (*piTestCancel)(void);
    }Control;

}CTOS_KMS2PINGET_PARA;

typedef struct
{
    // Should be 0x07
    IN BYTE Version;
    
    struct
    {
        IN BYTE BlockType;
        IN BYTE PINDigitMaxLength;
        IN BYTE PINDigitMinLength;      
    }PIN_Info;
    
    struct
    {
        IN USHORT CipherKeySet;
        IN USHORT CipherKeyIndex;
        IN BYTE CipherMethod;
                
        // This is used for KeyType is KMS2_KEYTYPE_3DES/KMS2_KEYTYPE_AES
        // If SK_Length is 0, SK will not be calculated and used.
        IN BYTE SK_Length;
        IN BYTE* pSK;
    }Protection;
    
    struct
    {
        // This is used for PAN if BlockType is KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0.
        // Ths is used for terminal pseudo random if BlockType  is KMS2_PINBLOCKTYPE_ISBAN_REVERSIBLE_PIN_4B_MODE.
        // This is used for PK if BlockType is KMS2_PINBLOCKTYPE_ISBAN_IRREVERSIBLE_PIN_4B_MODE.
        IN BYTE InLength;
        IN BYTE* pInData;
                        
    }AdditionalData;
    
    // This is used for KeyType is KMS2_KEYTYPE_3DES_DUKPT
    struct
    {       
        IN BOOL IsUseCurrentKey;
    }DUKPT_PARA;
    
    struct
    {               
        INOUT USHORT EncryptedBlockLength; 
        OUT BYTE* pEncryptedBlock;
        OUT BYTE PINDigitActualLength;
        OUT BYTE KSNLen;
        OUT BYTE* pKSN;
    }PINOutput;
    
    struct
    {
        IN DWORD Timeout;
        IN BYTE NULLPIN;
        IN int (*piTestCancel)(void);
        IN DWORD FirstKeyEntryTimeout;
    }Control;
    
    struct
    {
        void (*OnGetPINDigit)(BYTE NoDigits);
        void (*OnGetPINCancel)(void);
        void (*OnGetPINBackspace)(BYTE NoDigits);           
        void (*OnGetPINOtherKeys)(BYTE bFunctionKey);
    }EventFunction;
    
}CTOS_KMS2PINGET_PARA_VERSION_7;

#define KMS2_VIRTUALPINPADMETHOD_SCRAMBLING         0x01
#define KMS2_VIRTUALPINPADMETHOD_MOVINGPINPAD       0x02
#define KMS2_VIRTUALPINPADMETHOD_CUSTOM             0x04
typedef struct
{
    USHORT x;
    USHORT y;
    USHORT width;
    USHORT height;
}BUTTON_BASIC;

typedef struct
{
    USHORT x;
    USHORT y;
    USHORT width;
    USHORT height;
    BYTE value;
}BUTTON_EXTERNAL;

typedef struct
{
    // Should be 0x00 or 0x01
    IN BYTE Version;

    struct
    {
        IN BYTE Method;
    }Common;

    struct
    {
        IN USHORT Xtop;
        IN USHORT Ytop;
        IN USHORT Xbottom;
        IN USHORT Ybottom;
        IN USHORT Xoffset;
        IN USHORT Yoffset;
    }MovingRange;

    struct
    {
        BUTTON_BASIC BUTTON_0;
        BUTTON_BASIC BUTTON_1;
        BUTTON_BASIC BUTTON_2;
        BUTTON_BASIC BUTTON_3;
        BUTTON_BASIC BUTTON_4;
        BUTTON_BASIC BUTTON_5;
        BUTTON_BASIC BUTTON_6;
        BUTTON_BASIC BUTTON_7;
        BUTTON_BASIC BUTTON_8;
        BUTTON_BASIC BUTTON_9;
        BUTTON_BASIC BUTTON_ENTER;
        BUTTON_BASIC BUTTON_CANCEL;
        BUTTON_BASIC BUTTON_BACKSPACE;

        BUTTON_EXTERNAL BUTTON_FUNCTIONKEY1;
        BUTTON_EXTERNAL BUTTON_FUNCTIONKEY2;
        BUTTON_EXTERNAL BUTTON_FUNCTIONKEY3;
    }CustomLocation;
}CTOS_KMS2SETVIRTUALPINPAD_PARA;
USHORT CTOS_KMS2Init (void);
USHORT CTOS_KMS2KeyCheck(USHORT ikeyset, USHORT ikeyindex);
USHORT CTOS_KMS2KeyDelete(USHORT ikeyset, USHORT ikeyindex);
USHORT CTOS_KMS2KeyCheckAll (void);
USHORT CTOS_KMS2KeyDeleteAll (void);
USHORT CTOS_KMS2KeySwap(CTOS_KMS2KEYSWAP_PARA *para);
USHORT CTOS_KMS2KeyGetInfo(CTOS_KMS2KEYGETINFO_PARA *pKeyGetInfoPara);
USHORT CTOS_KMS2DataEncrypt(CTOS_KMS2DATAENCRYPT_PARA *pDataEncPara);
USHORT CTOS_KMS2DataDecrypt(CTOS_KMS2DATAENCRYPT_PARA *pDataDecPara);
USHORT CTOS_KMS2MAC(CTOS_KMS2MAC_PARA *pMacPara);
USHORT CTOS_KMS2UserDataWrite(BOOL isCommon, UINT offset, BYTE* baInBuf, UINT bufLen);
USHORT CTOS_KMS2UserDataRead(BOOL isCommon, UINT offset, BYTE* baOutBuf, UINT bufLen);
USHORT CTOS_KMS2KeyWriteByTR31(CTOS_KMS2KEYWRITEBYTR31_PARA* pKeyWritebyTR31Para);
USHORT CTOS_KMS2DUKPTGetKSN(USHORT iKeySet, USHORT iKeyIndex, BYTE* baOutBuf, UINT* bufLen);
USHORT CTOS_KMS2PINGet(CTOS_KMS2PINGET_PARA *pPinGetPara);

USHORT CTOS_KMS2SetVirtualPINPad(CTOS_KMS2SETVIRTUALPINPAD_PARA* pSetVirtualPINPad);
USHORT CTOS_KMS2KeyWrite(CTOS_KMS2KEYWRITE_PARA *pKeyWritePara);

//=============================================================================================================================
//
// Font Functions
//
//=============================================================================================================================

typedef struct
{
    USHORT FontSize;
    USHORT X_Zoom;
    USHORT Y_Zoom;
    USHORT X_Space;
    USHORT Y_Space;
} CTOS_FONT_ATTRIB;

#define d_PRT_ASCII_DEFAULT    0    //check
#define d_PRT_ASCII_BOLD       1    //check
//----------------------------------------------------------------------------------------Mark_Bottom
#define d_UTF_16
#define d_FONT_MAX_PAT_LEN  255*32 //support maximum font size 255*255

#define d_FNT_MODE          0
#define d_TTF_NORM_MODE     1
#define d_TTF_HB_MODE       2
//-----------------------------------
#define d_LANGUAGE_DEFAULT    0
#define d_LANGUAGE_KHMER      1
#define d_LANGUAGE_ARABIC     2
#define d_LANGUAGE_THAI       3
//-----------------------------------
// font device definition
#define d_FONT_DEVICE_LCD_0                                                 0x00
#define d_FONT_DEVICE_LCD_1                                                 0x01
#define d_FONT_DEVICE_PRINTER                                               0x80

// FNT Lauguage definition
#define d_FNT_LANGUAGE_ALPHABET                                             0x0000
#define d_FNT_LANGUAGE_CHINESE_TRADITIONAL                                  0x0001  //Chinese(Traiditional)
#define d_FNT_LANGUAGE_CHINESE_SIMPLIFIED                                   0x0002    //Chinese(Simplified)
#define d_FNT_LANGUAGE_JAPANESE                                             0x0003    //Japanese
#define d_FNT_LANGUAGE_THAI                                                 0x0004    //Thai
#define d_FNT_LANGUAGE_VIETNAMESE                                           0x0005    //Vietnamese
#define d_FNT_LANGUAGE_KOREAN                                               0x0006    //Korean
#define d_FNT_LANGUAGE_RUSSIAN                                              0x0007  //Russian
#define d_FNT_LANGUAGE_PORTUGUESE_PORTUGAL                                  d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_SPANISH                                              d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_TURKISH                                              d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_FARSI                                                d_FNT_LANGUAGE_ALPHABET

#define d_FNT_LANGUAGE_PORTUGUESE_BRAZIL                                    d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_CZECH                                                d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_ARABIC_QATAR                                         d_FNT_LANGUAGE_ALPHABET

// FNT FontID definition
#define d_FNT_FONTID_EMPTY                                                  0       //No font
#define d_FNT_FONTID_CHINESE_TAIWAN                                         1028    //Chinese(Taiwan)
#define d_FNT_FONTID_CZECH                                                  1029    //Czech
#define d_FNT_FONTID_JAPANESE                                               1041    //Japanese
#define d_FNT_FONTID_KOREAN                                                 1042    //Korean
#define d_FNT_FONTID_PORTUGUESE_BRAZIL                                      1046    //Portuguese(Brazil)
#define d_FNT_FONTID_RUSSIAN                                                1049    //Russian
#define d_FNT_FONTID_THAI                                                   1054    //Thai
#define d_FNT_FONTID_TURKISH                                                1055    //Turkish
#define d_FNT_FONTID_FARSI                                                  1065    //Farsi
#define d_FNT_FONTID_VIETNAMESE                                             1066    //Vietnamese
#define d_FNT_FONTID_CHINESE_PRC                                            2052    //Chinese(PRC)
#define d_FNT_FONTID_CHINESE_PRC1                                           2054    //Simple(Song)
#define d_FNT_FONTID_PORTUGUESE_PORTUGAL                                    2070    //Portuguese(Portugal)
#define d_FNT_FONTID_SPANISH                                                3082    //Spanish(Spain)
#define d_FNT_FONTID_ARABIC_QATAR                                           16385   //Arabic(Qatar)
#define d_FNT_FONTID_CHINESE_TAIWAN_PLUS                                    64507   //Chinese(Taiwan) with Symbol
#define d_FNT_FONTID_DEFAULTASCII                                           36864

// Font language
// ASCII < 0x80 is world-wide same
// ASCII >=0x80 is language dependent
#define d_TTF_C0_CONTROLS                                         0x0000
#define d_TTF_BASIC_LATIN                                         0x0020
#define d_TTF_C1_CONTROLS                                         0x0080
#define d_TTF_LATIN_1_SUPPLEMENT                                  0x00A0
#define d_TTF_LATIN_EXTENDED_A                                    0x0100
#define d_TTF_LATIN_EXTENDED_B                                    0x0180
#define d_TTF_IPA_EXTENSIONS                                      0x0250
#define d_TTF_SPACING_MODIFIERS                                   0x02B0
#define d_TTF_COMBINING_DIACRITICS_MARKS                          0x0300
#define d_TTF_GREEK_AND_COPTIC                                    0x0370
#define d_TTF_CYRILLIC                                            0x0400
#define d_TTF_CYRILLIC_SUPPLEMENT                                 0x0500
#define d_TTF_ARMENIAN                                            0x0530
#define d_TTF_HEBREW                                              0x0590
#define d_TTF_ARABIC                                              0x0600
#define d_TTF_SYRIAC                                              0x0700
#define d_TTF_ARABIC_SUPPLEMENT                                   0x0750
#define d_TTF_THAANA                                              0x0780
#define d_TTF_N_KO                                                0x07C0
#define d_TTF_SAMARITAN                                           0x0800
#define d_TTF_MANDAIC                                             0x0840
#define d_TTF_DEVANAGARI                                          0x0900
#define d_TTF_BENGALI                                             0x0980
#define d_TTF_GURMUKHI                                            0x0A00
#define d_TTF_GUJARATI                                            0x0A80
#define d_TTF_ORIYA                                               0x0B00
#define d_TTF_TAMIL                                               0x0B80
#define d_TTF_TELUGU                                              0x0C00
#define d_TTF_KANNADA                                             0x0C80
#define d_TTF_MALAYALAM                                           0x0D00
#define d_TTF_SINHALA                                             0x0D80
#define d_TTF_THAI                                                0x0E00
#define d_TTF_LAO                                                 0x0E80
#define d_TTF_TIBETAN                                             0x0F00
#define d_TTF_MYANMAR                                             0x1000
#define d_TTF_GEORGIAN                                            0x10A0
#define d_TTF_HANGUL_JAMO                                         0x1100
#define d_TTF_ETHIOPIC                                            0x1200
#define d_TTF_ETHIOPIC_SUPPLEMENT                                 0x1380
#define d_TTF_CHEROKEE                                            0x13A0
#define d_TTF_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS               0x1400
#define d_TTF_OGHAM                                               0x1680
#define d_TTF_RUNIC                                               0x16A0
#define d_TTF_TAGALOG                                             0x1700
#define d_TTF_HANUNOO                                             0x1720
#define d_TTF_BUHID                                               0x1740
#define d_TTF_TAGBANWA                                            0x1760
#define d_TTF_KHMER                                               0x1780
#define d_TTF_MONGOLIAN                                           0x1800
#define d_TTF_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED      0x18B0
#define d_TTF_LIMBU                                               0x1900
#define d_TTF_TAI_LE                                              0x1950
#define d_TTF_NEW_TAI_LUE                                         0x1980
#define d_TTF_KHMER_SYMBOLS                                       0x19E0
#define d_TTF_BUGINESE                                            0x1A00
#define d_TTF_TAI_THAM                                            0x1A20
#define d_TTF_BALINESE                                            0x1B00
#define d_TTF_SUNDANESE                                           0x1B80
#define d_TTF_BATAK                                               0x1BC0
#define d_TTF_LEPCHA                                              0x1C00
#define d_TTF_OL_CHIKI                                            0x1C50
#define d_TTF_SUDANESE_SUPPLEMENT                                 0x1CC0
#define d_TTF_VEDIC_EXTENSIONS                                    0x1CD0
#define d_TTF_PHONETIC_EXTENSIONS                                 0x1D00
#define d_TTF_PHONETIC_EXTENSIONS_SUPPLEMENT                      0x1D80
#define d_TTF_COMBINING_DIACRITICS_MARKS_SUPPLEMENT               0x1DC0
#define d_TTF_LATIN_EXTENDED_ADDITIONAL                           0x1E00
#define d_TTF_GREEK_EXTENDED                                      0x1F00
#define d_TTF_GENERAL_PUNCTUATION                                 0x2000
#define d_TTF_SUPERSCRIPTS_AND_SUBSCRIPTS                         0x2070
#define d_TTF_CURRENCY_SYMBOLS                                    0x20A0
#define d_TTF_COMBINING_DIACRITICS_MARKS_FOR_SYMBOLS              0x20D0
#define d_TTF_LETTERLIKE_SYMBOLS                                  0x2100
#define d_TTF_NUMBER_FORM                                         0x2150
#define d_TTF_ARROWS                                              0x2190
#define d_TTF_MATHEMATICAL_OPERATOR                               0x2200
#define d_TTF_MISCELLANEOUS_TECHNICAL                             0x2300
#define d_TTF_CONTROL_PICTURES                                    0x2400
#define d_TTF_OPTICAL_CHARACTER_RECOGNITION                       0x2440
#define d_TTF_ENCLOSED_ALPHANUMERICS                              0x2460
#define d_TTF_BOX_DRAWING                                         0x2500
#define d_TTF_BLOCK_ELEMENT                                       0x2580
#define d_TTF_GEOMETRIC_SHAPES                                    0x25A0
#define d_TTF_MISCELLANEOUS_SYMBOLS                               0x2600
#define d_TTF_DINGBATS                                            0x2700
#define d_TTF_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A                0x27C0
#define d_TTF_SUPPLEMENTAL_ARROWS_A                               0x27F0
#define d_TTF_BRAILLE_PATTERNS                                    0x2800
#define d_TTF_SUPPLEMENTAL_ARROWS_B                               0x2900
#define d_TTF_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B                0x2980
#define d_TTF_SUPPLEMENTAL_MATHEMATICAL_OPERATOR                  0x2A00
#define d_TTF_MISCELLANEOUS_SYMBOLS_AND_ARROWS                    0x2B00
#define d_TTF_GLAGOLITIC                                          0x2C00
#define d_TTF_LATIN_EXTENDED_C                                    0x2C60
#define d_TTF_COPTIC                                              0x2C80
#define d_TTF_GEORGIAN_SUPPLEMENT                                 0x2D00
#define d_TTF_TIFINAGH                                            0x2D30
#define d_TTF_ETHIOPIC_EXTENDED                                   0x2D80
#define d_TTF_SUPPLEMENTAL_PUNCTUATION                            0x2E00
#define d_TTF_CJK_RADICALS_SUPPLEMENT                             0x2E80
#define d_TTF_KANGXI_RADICALS                                     0x2F00
#define d_TTF_IDEOGRAPHIC_DESCRIPTION_CHARACTERS                  0x2FF0
#define d_TTF_CJK_SYMBOLS_AND_PUNCTUATION                         0x3000
#define d_TTF_HIRAGANA                                            0x3040
#define d_TTF_KATAKANA                                            0x30A0
#define d_TTF_BOPOMOFO                                            0x3100
#define d_TTF_HANGUL_COMPATIBILITY_JAMO                           0x3130
#define d_TTF_KANBUN                                              0x3190
#define d_TTF_BOPOMOFO_EXTENDED                                   0x31A0
#define d_TTF_CJK_STROKES                                         0x31C0
#define d_TTF_KATAKANA_PHONETIC_EXTENSIONS                        0x31F0
#define d_TTF_ENCLOSED_CJK_LETTERS_AND_MONTHS                     0x3200
#define d_TTF_CJK_COMPATIBILITY                                   0x3300
#define d_TTF_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A                  0x3400
#define d_TTF_YIJING_HEXAGRAMS_SYMBOLS                            0x4DC0
#define d_TTF_CJK_UNIFIED_IDEOGRAPHS                              0x4E00
#define d_TTF_YI_SYLLABLES                                        0xA000
#define d_TTF_YI_RADICALS                                         0xA490
#define d_TTF_LISU                                                0xA4D0
#define d_TTF_VAI                                                 0xA500
#define d_TTF_CYRILLIC_EXTENDED_B                                 0xA640
#define d_TTF_BAMUM                                               0xA6A0
#define d_TTF_MODIFIER_TONE_LETTERS                               0xA700
#define d_TTF_LATIN_EXTENDED_D                                    0xA720
#define d_TTF_SYLOTI_NAGRI                                        0xA800
#define d_TTF_IND_NO                                              0xA830
#define d_TTF_PHAGS_PA                                            0xA840
#define d_TTF_SAURASHTRA                                          0xA880
#define d_TTF_DEVA_EXT                                            0xA8E0
#define d_TTF_KAYAH_LI                                            0xA900
#define d_TTF_REJANG                                              0xA930
#define d_TTF_JAVANESE                                            0xA980
#define d_TTF_CHAM                                                0xAA00
#define d_TTF_MYANMAR_EXTA                                        0xAA60
#define d_TTF_TAI_VIET                                            0xAA80
#define d_TTF_MEETEI_EXT                                          0xAAE0
#define d_TTF_ETHIOPIC_EXT_A                                      0xAB00
#define d_TTF_MEETEI_MAYEK                                        0xABC0
#define d_TTF_HANGUL_SYLLABLES                                    0xAC00
#define d_TTF_HANGUL_JAMO_EXTENDED_B                              0xD7B0
#define d_TTF_HIGH_HALF_ZONE_OF_UTF_16                            0xD800
#define d_TTF_LOW_HALF_ZONE_OF_UTF_16                             0xDC00
#define d_TTF_PRIVATE_USE_ZONE                                    0xE000
#define d_TTF_CJK_COMPATIBILITY_IDEOGRAPHS                        0xF900
#define d_TTF_ALPHABETIC_PRESENTATION_FORMS                       0xFB00
#define d_TTF_ARABIC_PRESENTATION_FORMS_A                         0xFB50
#define d_TTF_VARIATION_SELECTOR                                  0xFE00
#define d_TTF_VERTICAL_FORMS                                      0xFE10
#define d_TTF_COMBINING_HALF_MARKS                                0xFE20
#define d_TTF_CJK_COMPATIBILITY_FORMS                             0xFE30
#define d_TTF_SMALL_FORM_VARIANTS                                 0xFE50
#define d_TTF_ARABIC_PRESENTATION_FORMS_B                         0xFE70
#define d_TTF_HALFWIDTH_AND_FULLWIDTH_FORMS                       0xFF00
#define d_TTF_SPECIALS                                            0xFFF0

#define d_FONT_EMPTY                                              0       //No font
#define d_FONT_CHINESE_TAIWAN                                     1028    //Chinese(Taiwan)
#define d_FONT_CZECH                                              1029    //Czech
#define d_FONT_JAPANESE                                           1041    //Japanese
#define d_FONT_KOREAN                                             1042    //Korean
#define d_FONT_PORTUGUESE_BRAZIL                                  1046    //Portuguese(Brazil)
#define d_FONT_RUSSIAN                                            1049    //Russian
#define d_FONT_THAI                                               1054    //Thai
#define d_FONT_TURKISH                                            1055    //Turkish
#define d_FONT_FARSI                                              1065    //Farsi
#define d_FONT_VIETNAMESE                                         1066    //Vietnamese
#define d_FONT_CHINESE_PRC                                        2052    //Chinese(PRC)
#define d_FONT_CHINESE_PRC1                                       2054    //Simple(Song)
#define d_FONT_PORTUGUESE_PORTUGAL                                2070    //Portuguese(Portugal)
#define d_FONT_SPANISH                                            3082    //Spanish(Spain)
#define d_FONT_ARABIC_QATAR                                       16385   //Arabic(Qatar)
#define d_FONT_CHINESE_TAIWAN_PLUS                                64507   //Chinese(Taiwan) with Symbol
#define d_FONT_DEFAULTASCII                                       36864
//SwitchDisplayMode()
#define d_TTF_MODE_DEFAULT                                        0x0000
#define d_TTF_MODE_VIETNAM                                        0xA001
#define d_TTF_MODE_BURMA                                          0xA002
#define d_TTF_MODE_KHMER                                          0xA003
#define d_TTF_MODE_SPAIN                                          0xA004

// for font usage
#define d_FONT_LCD_USE                                            0x00
#define d_FONT_PRINTER_USE                                        0x01
#define d_FONT_FNT_MODE                                           0
#define d_FONT_TTF_MODE                                           1
#define d_FONT_DEFAULT_TTF                                        "ca_default.ttf"

// font style definition
#define d_FONT_STYLE_NORMAL                                                 0x0000
#define d_FONT_STYLE_REVERSE                                                0x0001
#define d_FONT_STYLE_UNDERLINE                                              0x0002
#define d_FONT_STYLE_BOLD                                                   0x0004
#define d_FONT_STYLE_ITALIC                                                 0x0010

// Font size
#define d_FONT_8x8                                                0x0808
#define d_FONT_8x16                                               0x0810
#define d_FONT_16x16                                              0x1010
#define d_FONT_12x24                                              0x0C18
#define d_FONT_24x24                                              0x1818
#define d_FONT_9x9                                                0x0909
#define d_FONT_9x18                                               0x0912
#define d_FONT_16x30                                              0x101E
#define d_FONT_20x40                                              0x1428

// for printer font type set
#define d_FONT_NO_SET_TYPE                                        0
#define d_FONT_TYPE1                                              1
#define d_FONT_TYPE2                                              2
#define d_FONT_TYPE3                                              3
//==================================================
//-------------------------------------------------------------------------
//    LCD
//-------------------------------------------------------------------------
USHORT CTOS_FontSelectMode(BYTE bDevice,BYTE bMode);
USHORT CTOS_FontFNTNum(USHORT* pusIndex);
USHORT CTOS_FontFNTInfo(USHORT usIndex,USHORT* pusFontID,USHORT* pusFontSize);
USHORT CTOS_LibCAFontGetVersion(BYTE* verion_str);
USHORT CTOS_FontFNTSelectLanguage(BYTE bDevice,USHORT usLanguage);
USHORT CTOS_FontFNTSelectFont(BYTE bDevice,USHORT usFontID);
USHORT CTOS_FontFNTSelectASCIIFont(BYTE bDevice,USHORT usFontID);
USHORT CTOS_FontFNTSelectStyle(BYTE bDevice,USHORT usStyle);
USHORT CTOS_FontFNTSelectSize(BYTE bDevice,USHORT usFontSize);
USHORT CTOS_FontTTFCheckSupport(ULONG ulEncoding, BOOL *fSupported );
USHORT CTOS_FontTTFSelectFontFile(BYTE bDevice,BYTE *baFontFileName, BYTE bIndex);
USHORT CTOS_FontTTFSelectStyle(BYTE bDevice,USHORT usStyle);
USHORT CTOS_FontTTFSelectSize(BYTE bDevice,USHORT usFontSize);
USHORT CTOS_FontTTFSwitchDisplayMode(BYTE bDevice, USHORT usMode);
USHORT CTOS_FontTTFSwichDisplayMode(BYTE bDevice, USHORT usMode);
USHORT CTOS_LanguageConfig(USHORT usLanguage,USHORT usFontSize,USHORT usFontStyle, BOOL boSetDefault);
USHORT CTOS_LanguageLCDFontSize(USHORT usFontSize,USHORT usFontStyle);
USHORT CTOS_LanguagePrinterFontSize(USHORT usFontSize,USHORT usFontStyle, USHORT usSetType);
USHORT CTOS_LanguageInfo(USHORT usIndex,USHORT* pusLanguage,USHORT* pusFontSize,USHORT* pusFontStyle);
USHORT CTOS_LanguageNum(USHORT* pusIndex);
USHORT CTOS_LanguagePrinterSelectASCII(USHORT usASCIIFontID);
USHORT CTOS_LanguageLCDSelectASCII(USHORT usASCIIFontID);
USHORT CTOS_LanguagePrinterGetFontInfo(USHORT* pusASCIIFontID,USHORT* pusFontSize,USHORT* pusFontStyle);
USHORT CTOS_LanguageLCDGetFontInfo(USHORT* pusASCIIFontID,USHORT* pusFontSize,USHORT* pusFontStyle);
//-------------------------------------------------------------------------
//    Printer
//-------------------------------------------------------------------------
#define d_PRINTER_ALIGNLEFT	0
#define d_PRINTER_ALIGNCENTER	1
#define d_PRINTER_ALIGNRIGHT	2
// for Printer function
#define PAPER_X_SIZE			384

#define PB_CANVAS_X_SIZE			384
#define PB_CANVAS_Y_SIZE 			80
#define MAX_PB_CANVAS_NUM			2

USHORT CTOS_PrinterSetDefaultASCIIStyle( UCHAR bStyle);
USHORT CTOS_PrinterBufferInit(BYTE *pPtr,USHORT usHeight);
USHORT CTOS_PrinterLogo(IN BYTE* baLogo,IN USHORT usXstart,IN USHORT usXsize,IN USHORT usY8Size);
USHORT CTOS_PrinterBufferPutString(BYTE* pbPtr, USHORT usXPos, USHORT usYPos, BYTE *baStr, CTOS_FONT_ATTRIB* ATTRIB);
USHORT CTOS_PrinterBufferOutput(BYTE* pbPtr, USHORT usY8Len);
USHORT CTOS_PrinterFline(IN USHORT usLines);
USHORT CTOS_PrinterPutString(UCHAR* baBuf);
//-------------------------------------------------------------------------

USHORT CTOS_PrinterFontSelectMode(BYTE bMode);
USHORT CTOS_PrinterTTFSelect(BYTE *baFilename, BYTE bIndex);
USHORT CTOS_PrinterTTFSwitchDisplayMode(USHORT usMode);
USHORT CTOS_PrinterTTFSwichDisplayMode(USHORT usMode);
USHORT CTOS_PrinterStatus(void);
USHORT CTOS_PrinterBMPPic(USHORT usX, BYTE *baFilename);
USHORT CTOS_PrinterBufferFill(USHORT usXStart, USHORT usYStart, USHORT usXEnd, USHORT usYEnd, BOOL fPat);
USHORT CTOS_PrinterBufferHLine(USHORT usXStart, USHORT usYStart, USHORT usXEnd, BOOL fPat);
USHORT CTOS_PrinterBufferVLine(USHORT usXStart, USHORT usYStart, USHORT usYEnd, BOOL fPat);
USHORT CTOS_PrinterBufferLogo(USHORT usXPos, USHORT usYPos, USHORT usWidth, USHORT usHeight, BYTE *baPat);
USHORT CTOS_PrinterBufferPixel(USHORT usXPos, USHORT usYPos,BOOL fPat);
USHORT CTOS_PrinterBufferBMPPic(BYTE* pbPtr, USHORT usXPos, USHORT usYPos, BYTE *baFilename);
void CTOS_PrinterBufferEnable(void);
USHORT CTOS_PrinterBufferSelectActiveAddress (BYTE *pPtr);
USHORT CTOS_PrinterPutStringAligned (UCHAR* pbBuf, BYTE bMode);
USHORT CTOS_PrinterBufferPutStringAligned(BYTE* pbPtr,  USHORT usYPos, BYTE *baStr, CTOS_FONT_ATTRIB* ATTRIB, BYTE bMode);
USHORT CTOS_PrinterSetHeatLevel(UCHAR bHeatLevel);

//=============================================================================================================================
//
// LCD Functions
//
//=============================================================================================================================
#define d_LCD_WHITE 											  0xFFFFFF
#define d_LCD_BLACK 											  0x000000
#define d_LCD_BLUE 												  0xFF0000
#define d_LCD_GREEN 											  0x00FF00
#define d_LCD_RED 												  0x0000FF

#define d_FNT_LANGUAGE_SET 		0
#define d_FNT_ASCII_SET 		1

#define d_FNT_ID_CHECK			0
#define d_FNT_ID_SIZE_CHECK		1

#define d_UTF16               0
#define d_UTF32               1
#define d_UTF8				  3
//-----------------------------------
#define d_FNT_MODE 			0
#define d_TTF_NORM_MODE 	1
#define d_TTF_HB_MODE 		2
//-----------------------------------
#define d_LANGUAGE_DEFAULT    0
#define d_LANGUAGE_KHMER 	  1
#define d_LANGUAGE_ARABIC 	  2
#define d_LANGUAGE_THAI 	  3

#define d_FNT_ID_CHECK			0
#define d_FNT_ID_SIZE_CHECK		1

#define d_FNT_LANGUAGE_SET 		0
#define d_FNT_ASCII_SET 		1

// for show pic mode
#define d_LCD_SHOWPIC_MONO  									  0
#define d_LCD_SHOWPIC_RGB  										  1

#define d_LCD_ALIGNLEFT											  0
#define d_LCD_ALIGNCENTER										  1
#define d_LCD_ALIGNRIGHT										  2

// value for bMode
#define d_LCD_GRAPHIC_MODE										  0
#define d_LCD_TEXT_MODE											  1
#define d_LCD_GRAPHIC_HIGH_RES_MODE						  		  3
#define d_LCD_TEXT_HIGH_RES_MODE						  		  4

//for LCD attribute
#define d_RESOLUTION_720x1280		  ((720 << 16) | 1280)
#define d_RESOLUTION_480x320		  ((480 << 16) | 320)
#define d_RESOLUTION_320x480          ((320 << 16) | 480)
#define d_RESOLUTION_320x240          ((320 << 16) | 240)
#define d_RESOLUTION_128x64			  ((128 << 16) | 64)
#define d_RESOLUTION_128x32			  ((128 << 16) | 32)
#define d_COLOR_MONO  			                                  1
#define d_COLOR_262K       		                                  2
#define d_COLOR_16M        		                                  3
#define d_TOUCH_NONE	   		                                  0
#define d_TOUCH_RESISTOR   		                                  1
#define d_TOUCH_CAPACITOR_1P	                                  2


// value for English Size
#define d_LCD_FONT_8x8											  d_FONT_8x8
#define d_LCD_FONT_8x16											  d_FONT_8x16
#define d_LCD_FONT_12x24										  d_FONT_12x24

// value for bFill
#define d_LCD_FILL_0											  0
#define d_LCD_FILL_1											  1
#define d_LCD_FILL_XOR											  2

// font device definition
#define d_FONT_DEVICE_LCD_0													0x00
#define d_FONT_DEVICE_LCD_1													0x01
#define d_FONT_DEVICE_PRINTER												0x80

#define d_LCD_DEVICE_0													d_FONT_DEVICE_LCD_0
#define d_LCD_DEVICE_1													d_FONT_DEVICE_LCD_1
// Font size
#define d_FONT_8x8												  0x0808
#define d_FONT_8x16												  0x0810
#define d_FONT_16x16											  0x1010
#define d_FONT_12x24											  0x0C18
#define d_FONT_24x24											  0x1818
#define d_FONT_9x9												  0x0909
#define d_FONT_9x18												  0x0912
#define d_FONT_16x30											  0x101E
#define d_FONT_20x40											  0x1428
//--------------------Language-----------------------------------------
//----------------------------------------------------------------------
// FONT Definition
//----------------------------------------------------------------------
// font device definition
#define d_FONT_DEVICE_LCD_0													0x00
#define d_FONT_DEVICE_LCD_1													0x01
#define d_FONT_DEVICE_PRINTER												0x80

// font style definition
#define d_FONT_STYLE_NORMAL													0x0000
#define d_FONT_STYLE_REVERSE												0x0001
#define d_FONT_STYLE_UNDERLINE												0x0002
#define d_FONT_STYLE_BOLD													0x0004
#define d_FONT_STYLE_ITALIC													0x0010

// FNT Lauguage definition
#define d_FNT_LANGUAGE_ALPHABET												0x0000
#define d_FNT_LANGUAGE_CHINESE_TRADITIONAL   								0x0001	//Chinese(Traiditional)
#define d_FNT_LANGUAGE_CHINESE_SIMPLIFIED									0x0002    //Chinese(Simplified)
#define d_FNT_LANGUAGE_JAPANESE												0x0003    //Japanese
#define d_FNT_LANGUAGE_THAI  												0x0004    //Thai
#define d_FNT_LANGUAGE_VIETNAMESE										 	0x0005    //Vietnamese
#define d_FNT_LANGUAGE_KOREAN												0x0006    //Korean
#define d_FNT_LANGUAGE_RUSSIAN											 	0x0007	//Russian
#define d_FNT_LANGUAGE_PORTUGUESE_PORTUGAL								  	d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_SPANISH												d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_TURKISH 											  	d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_FARSI											 	d_FNT_LANGUAGE_ALPHABET

#define d_FNT_LANGUAGE_PORTUGUESE_BRAZIL        							d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_CZECH												d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_ARABIC_QATAR											d_FNT_LANGUAGE_ALPHABET


#define d_FONT_EMPTY   											  0		  //No font
#define d_FONT_CHINESE_TAIWAN   								  1028	  //Chinese(Taiwan)
#define d_FONT_CZECH											  1029	  //Czech
#define d_FONT_JAPANESE											  1041    //Japanese
#define d_FONT_KOREAN											  1042    //Korean
#define d_FONT_PORTUGUESE_BRAZIL        						  1046    //Portuguese(Brazil)
#define d_FONT_RUSSIAN											  1049    //Russian
#define d_FONT_THAI  											  1054    //Thai
#define d_FONT_TURKISH 											  1055    //Turkish
#define d_FONT_FARSI											  1065    //Farsi
#define d_FONT_VIETNAMESE										  1066    //Vietnamese
#define d_FONT_CHINESE_PRC										  2052    //Chinese(PRC)
#define d_FONT_CHINESE_PRC1										  2054    //Simple(Song)
#define d_FONT_PORTUGUESE_PORTUGAL								  2070	  //Portuguese(Portugal)
#define d_FONT_SPANISH 											  3082    //Spanish(Spain)
#define d_FONT_ARABIC_QATAR										  16385   //Arabic(Qatar)
#define d_FONT_CHINESE_TAIWAN_PLUS 								  64507	  //Chinese(Taiwan) with Symbol
#define d_FONT_DEFAULTASCII             						  36864

// FNT FontID definition
#define d_FNT_FONTID_EMPTY   												0		//No font
#define d_FNT_FONTID_CHINESE_TAIWAN   										1028	//Chinese(Taiwan)
#define d_FNT_FONTID_CZECH													1029	//Czech
#define d_FNT_FONTID_JAPANESE												1041    //Japanese
#define d_FNT_FONTID_KOREAN													1042    //Korean
#define d_FNT_FONTID_PORTUGUESE_BRAZIL        								1046    //Portuguese(Brazil)
#define d_FNT_FONTID_RUSSIAN												1049    //Russian
#define d_FNT_FONTID_THAI  													1054    //Thai
#define d_FNT_FONTID_TURKISH 												1055    //Turkish
#define d_FNT_FONTID_FARSI													1065    //Farsi
#define d_FNT_FONTID_VIETNAMESE												1066    //Vietnamese
#define d_FNT_FONTID_CHINESE_PRC											2052    //Chinese(PRC)
#define d_FNT_FONTID_CHINESE_PRC1											2054    //Simple(Song)
#define d_FNT_FONTID_PORTUGUESE_PORTUGAL									2070	//Portuguese(Portugal)
#define d_FNT_FONTID_SPANISH 												3082    //Spanish(Spain)
#define d_FNT_FONTID_ARABIC_QATAR											16385   //Arabic(Qatar)
#define d_FNT_FONTID_CHINESE_TAIWAN_PLUS 									64507	//Chinese(Taiwan) with Symbol
#define d_FNT_FONTID_DEFAULTASCII             								36864
// for font usage
#define d_FONT_LCD_USE											  0x00
#define d_FONT_PRINTER_USE										  0x01
#define d_FONT_FNT_MODE   										  0
#define d_FONT_TTF_MODE   										  1
#define	d_FONT_DEFAULT_TTF										  "ca_default.ttf"

USHORT CTOS_LCDSelectMode(BYTE bMode);
USHORT CTOS_LCDSelectModeEx(BYTE bMode,BOOL fClear);
// USHORT CTOS_LCDSetContrast(BYTE bValue);
USHORT CTOS_LCDReflesh(void);
USHORT CTOS_LCDForeGndColor(ULONG ulColor);
USHORT CTOS_LCDBackGndColor(ULONG ulColor);
USHORT CTOS_LCDFontSelectMode(BYTE bMode);
USHORT CTOS_LCDTTFSelect(BYTE *baFilename, BYTE bIndex);
USHORT CTOS_LCDTTFCheckLanguageSupport(ULONG ulLanguage, BOOL *fSupported );
USHORT CTOS_LCDTTFSwitchDisplayMode(USHORT usMode);
USHORT CTOS_LCDGetResolution(USHORT *pusXsize, USHORT *pusYsize);
USHORT CTOS_LCDAttributeGet(ULONG *pulResolution, BYTE *pbColor, BYTE *pbTouch);

// Graphic Mode
USHORT CTOS_LCDGClearCanvas(void);
USHORT CTOS_LCDGPixel(USHORT usX,USHORT usY, BOOL boPat);
USHORT CTOS_LCDGTextOut(USHORT usX, USHORT usY, UCHAR* pusBuf, USHORT usFontSize, BOOL boReverse);
USHORT CTOS_LCDGTextOutAligned( USHORT usY, UCHAR* pusBuf, USHORT usFontSize, BOOL boReverse, BYTE bMode );
USHORT CTOS_LCDGSetBox(USHORT usX, USHORT usY, USHORT usXSize, USHORT usYSize, BYTE bFill);
USHORT CTOS_LCDGShowPic(USHORT usX, USHORT usY, BYTE* baPat, ULONG ulPatLen, USHORT usXSize);
USHORT CTOS_LCDGClearWindow(void);
USHORT CTOS_LCDGMoveWindow(USHORT usOffset);
USHORT CTOS_LCDGGetWindowOffset(void);
// USHORT CTOS_LCDGMenu(BYTE bAttribute, STR *pbaHeaderString, STR *pcaItemString);
// USHORT CTOS_LCDGMenuEx(BYTE bAttribute, STR *pbaHeaderString, STR *pcaItemString,USHORT usShowItem);
USHORT CTOS_LCDGShowPicEx(UCHAR bMode,USHORT usX, USHORT usY, BYTE* baPat, ULONG ulPatLen, USHORT usXSize);
USHORT CTOS_LCDGShowBMPPic(USHORT usX, USHORT usY, BYTE *baFilename);

// Text Mode
USHORT CTOS_LCDTClearDisplay(void);
USHORT CTOS_LCDTGotoXY(USHORT usX,USHORT usY);
USHORT CTOS_LCDTWhereX(void);
USHORT CTOS_LCDTWhereY(void);
USHORT CTOS_LCDTPrint(UCHAR* sBuf);
USHORT CTOS_LCDTPrintXY(USHORT usX, USHORT usY, UCHAR* pbBuf);
USHORT CTOS_LCDTPutch(UCHAR ch);
USHORT CTOS_LCDTPutchXY (USHORT usX, USHORT usY, UCHAR bChar);
USHORT CTOS_LCDTClear2EOL(void);
USHORT CTOS_LCDTSetReverse(BOOL boReverse);
USHORT CTOS_LCDTSelectFontSize(USHORT usFontSize);
// USHORT CTOS_LCDTSetASCIIVerticalOffset(BOOL fVDirection, BYTE bVOffect);
// USHORT CTOS_LCDTSetASCIIHorizontalOffset(BOOL fHDirection, BYTE bHOffect);
// USHORT CTOS_LCDTMenu(BYTE bAttribute, STR *pbaHeaderString, STR *pcaItemString);
USHORT CTOS_LCDTPrintAligned(USHORT usY, UCHAR* pbBuf, BYTE bMode);
USHORT CTOS_LCDTGetStringWidth(BYTE *pbBuf, USHORT *pusStrWidth);

BYTE* CTOS_GETGBBuffer(void);

//=============================================================================================================================
//
// MDB Functions
//
//=============================================================================================================================

#define d_MDB_VMC_CMD_IDLE					0x01
#define d_MDB_VMC_CMD_VMC_CONFIG_AND_SETUP			0x02
#define d_MDB_VMC_CMD_SETUP_DISABLE				0x03
#define d_MDB_VMC_CMD_SETUP_ENABLE				0x04
#define d_MDB_VMC_CMD_VEND_REQUEST				0x05
#define d_MDB_VMC_CMD_VEND_SUCCESS				0x06
#define d_MDB_VMC_CMD_VEND_SESSION_COMPLETE			0x07
#define d_MDB_VMC_CMD_SETUP_RESET				0x08
#define d_MDB_VMC_CMD_VEND_FAILURE				0x09
#define d_MDB_VMC_CMD_CASH_SALE					0x0A
#define d_MDB_VMC_CMD_ITEM_NUMBER				0x0B

#define d_MDB_VMC_RSP_BEGIN_SESSION				0x01
#define d_MDB_VMC_RSP_SESSION_CANCEL_REQUEST			0x02
#define d_MDB_VMC_RSP_APPEOVED					0x03
#define d_MDB_VMC_RSP_DENIED					0x04

#define d_MDB_CONFIG_LEVEL_1					0x01
#define d_MDB_CONFIG_LEVEL_2					0x02

#define d_MDB_CONFIG_MISC_RESTORE_FUNDS				0x01
#define d_MDB_CONFIG_MISC_MULTIVEND				0x02
#define d_MDB_CONFIG_MISC_OWN_DISPLAY				0x04
#define d_MDB_CONFIG_MISC_SUPPORT_CASH_SALE			0x08

#define d_MDB_PAYMENT_MEDIA_NORMAL_CARD				0x00
#define d_MDB_PAYMENT_MEDIA_TEST_CARD				0x01
#define d_MDB_PAYMENT_MEDIA_FREE_CARD				0x02

#define d_MDB_FWINFO_VER					0x01

typedef struct MDB_Config{
	BYTE bLevel;
	USHORT usCountryCode;
	BYTE bScaleFactor;
	BYTE bDecimalPlaces;
	UINT uiManufacturerCode;
	USHORT usSoftVer;
	BYTE bRSPTime;
	BYTE bMiscellaneous;
}CTOS_MDBConfig;

USHORT CTOS_MDBOpen(void);
USHORT CTOS_MDBClose(void);
USHORT CTOS_MDBConfigSet(CTOS_MDBConfig stMDBConfig);
USHORT CTOS_MDBConfigGet(CTOS_MDBConfig* pstMDBConfig);
USHORT CTOS_MDBResponseSend(BYTE bRSP,BYTE bLevel, void* pValue);
USHORT CTOS_MDBCommandGet(BYTE* bCMD,BYTE* baData, USHORT* usLen);
USHORT CTOS_MDBFWInfoGet(BYTE bTag, BYTE *baData, BYTE *pbLen);

//=============================================================================================================================
//
// TLS2 Functions
//
//=============================================================================================================================

USHORT CTOS_TLS2_Init(void);
//=============================================================
#define d_TLS2_METHOD_CLIENT_SSL_V3         1
#define d_TLS2_METHOD_CLIENT_SSL_V23        2
#define d_TLS2_METHOD_CLIENT_TLS_V1         3
#define d_TLS2_METHOD_CLIENT_TLS_V1_1       4
#define d_TLS2_METHOD_CLIENT_TLS_V1_2       5

USHORT CTOS_TLS2_CTX_New(IN INT Method, OUT UINT* pCTX_ID);
//=============================================================
#define d_TLS2_FILETYPE_ASN1                2
#define d_TLS2_FILETYPE_PEM                 1 

USHORT CTOS_TLS2_CTX_LoadCACertificationFromBuffer(IN UINT CTX_ID, IN const BYTE* pCA, IN LONG Length, IN UINT Format);
USHORT CTOS_TLS2_CTX_LoadCertificateFromBuffer(IN UINT CTX_ID, IN const BYTE* pCertificate, IN LONG Length, IN UINT Format);
USHORT CTOS_TLS2_CTX_LoadPrivateKeyFromBuffer(IN UINT CTX_ID, IN const BYTE* pPrivateKey, IN LONG Length, IN UINT Format);
//=============================================================
#define d_TLS2_VERIFY_NONE                  0
#define d_TLS2_VERIFY_PEER                  1

USHORT CTOS_TLS2_SetVerificationMode(IN UINT SSL_ID, IN UINT VerificationMode);
USHORT CTOS_TLS2_CTX_SetVerificationMode(IN UINT CTX_ID, IN UINT VerificationMode);
//=============================================================
USHORT CTOS_TLS2_CTX_Free(IN UINT CTX_ID);
USHORT CTOS_TLS2_New(IN UINT CTX_ID, OUT UINT* pSSL_ID);
USHORT CTOS_TLS2_Free(IN UINT SSL_ID);
USHORT CTOS_TLS2_Connect(IN UINT SSL_ID);
USHORT CTOS_TLS2_Read(IN UINT SSL_ID, OUT BYTE* pData, INOUT INT *pLength);
USHORT CTOS_TLS2_Write(IN UINT SSL_ID, IN BYTE *pData, INOUT INT *pLength);
USHORT CTOS_TLS2_Disconnect(IN UINT SSL_ID);
USHORT CTOS_TLS2_GetSession(IN UINT SSL_ID);
//=============================================================
#define SSL_RSA_WITH_RC4_128_SHA                        "RC4-SHA"
#define SSL_RSA_WITH_RC4_128_MD5                        "RC4-MD5"
#define SSL_RSA_WITH_3DES_EDE_CBC_SHA                   "DES-CBC3-SHA"
#define TLS_RSA_WITH_AES_256_CBC_SHA                    "AES256-SHA"
#define TLS_RSA_WITH_AES_128_CBC_SHA                    "AES128-SHA"
#define TLS_RSA_WITH_NULL_SHA                           "NULL-SHA"
#define TLS_RSA_WITH_NULL_SHA256                        "NULL-SHA256"
#define TLS_DHE_RSA_WITH_AES_128_CBC_SHA                "DHE-RSA-AES128-SHA"
#define TLS_DHE_RSA_WITH_AES_256_CBC_SHA                "DHE-RSA-AES256-SHA"
#define TLS_RSA_WITH_HC_128_MD5                         "HC128-MD5"
#define TLS_RSA_WITH_HC_128_SHA                         "HC128-SHA"
#define TLS_RSA_WITH_HC_128_B2B256                      "HC128-B2B256"
#define TLS_RSA_WITH_AES_128_CBC_B2B256                 "AES128-B2B256"
#define TLS_RSA_WITH_AES_256_CBC_B2B256                 "AES256-B2B256"
#define TLS_RSA_WITH_RABBIT_SHA                         "RABBIT-SHA"
#define TLS_NTRU_RSA_WITH_RC4_128_SHA                   "NTRU-RC4-SHA"
#define TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA              "NTRU-DES-CBC3-SHA"
#define TLS_NTRU_RSA_WITH_AES_128_CBC_SHA               "NTRU-AES128-SHA"
#define TLS_NTRU_RSA_WITH_AES_256_CBC_SHA               "NTRU-AES256-SHA"  
#define TLS_RSA_WITH_AES_128_CCM_8                      "AES128-CCM-8"
#define TLS_RSA_WITH_AES_256_CCM_8                      "AES256-CCM-8"
#define TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8              "ECDHE-ECDSA-AES128-CCM-8"
#define TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8              "ECDHE-ECDSA-AES256-CCM-8"

#define TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA              "ECDHE-RSA-AES256-SHA"
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA              "ECDHE-RSA-AES128-SHA"
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA            "ECDHE-ECDSA-AES256-SHA"
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA            "ECDHE-ECDSA-AES128-SHA"
#define TLS_ECDHE_RSA_WITH_RC4_128_SHA                  "ECDHE-RSA-RC4-SHA"
#define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA                "ECDHE-ECDSA-RC4-SHA"
#define TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA             "ECDHE-RSA-DES-CBC3-SHA"
#define TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA           "ECDHE-ECDSA-DES-CBC3-SHA"
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256           "ECDHE-RSA-AES128-SHA256"       
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256         "ECDHE-ECDSA-AES128-SHA256"         
#define TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384           "ECDHE-RSA-AES256-SHA384"    
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384         "ECDHE-ECDSA-AES256-SHA384"       
#define TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256         "ECDHE-ECDSA-AES128-GCM-SHA256"  
#define TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384         "ECDHE-ECDSA-AES256-GCM-SHA384"
#define TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256           "ECDHE-RSA-AES128-GCM-SHA256"               
#define TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384           "ECDHE-RSA-AES256-GCM-SHA384" 

#define TLS_RSA_WITH_AES_128_CBC_SHA256                 "AES128-SHA256"        
#define TLS_RSA_WITH_AES_256_CBC_SHA256                 "AES256-SHA256"
#define TLS_DHE_RSA_WITH_AES_128_CBC_SHA256             "DHE-RSA-AES128-SHA256"
#define TLS_DHE_RSA_WITH_AES_256_CBC_SHA256             "DHE-RSA-AES256-SHA256"

#define TLS_RSA_WITH_AES_128_GCM_SHA256                 "AES128-GCM-SHA256"
#define TLS_RSA_WITH_AES_256_GCM_SHA384                 "AES256-GCM-SHA384"
#define TLS_DHE_RSA_WITH_AES_128_GCM_SHA256             "DHE-RSA-AES128-GCM-SHA256"
#define TLS_DHE_RSA_WITH_AES_256_GCM_SHA384             "DHE-RSA-AES256-GCM-SHA384"

#define TLS_ECDH_RSA_WITH_AES_256_CBC_SHA               "ECDH-RSA-AES256-SHA"
#define TLS_ECDH_RSA_WITH_AES_128_CBC_SHA               "ECDH-RSA-AES128-SHA"
#define TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA             "ECDH-ECDSA-AES256-SHA"
#define TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA             "ECDH-ECDSA-AES128-SHA"
#define TLS_ECDH_RSA_WITH_RC4_128_SHA                   "ECDH-RSA-RC4-SHA"
#define TLS_ECDH_ECDSA_WITH_RC4_128_SHA                 "ECDH-ECDSA-RC4-SHA"
#define TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA              "ECDH-RSA-DES-CBC3-SHA"
#define TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA            "ECDH-ECDSA-DES-CBC3-SHA"
#define TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256            "ECDH-RSA-AES128-SHA256"
#define TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256          "ECDH-ECDSA-AES128-SHA256"
#define TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384            "ECDH-RSA-AES256-SHA384"
#define TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384          "ECDH-ECDSA-AES256-SHA384"
#define TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256          "ECDH-ECDSA-AES128-GCM-SHA256"
#define TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384          "ECDH-ECDSA-AES256-GCM-SHA384"

#define TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256            "ECDH-RSA-AES128-GCM-SHA256"  
#define TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384            "ECDH-RSA-AES256-GCM-SHA384"

#define TLS_RSA_WITH_CAMELLIA_128_CBC_SHA               "CAMELLIA128-SHA"
#define TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA           "DHE-RSA-CAMELLIA128-SHA"
#define TLS_RSA_WITH_CAMELLIA_256_CBC_SHA               "CAMELLIA256-SHA"
#define TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA           "DHE-RSA-CAMELLIA256-SHA"
#define TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256            "CAMELLIA128-SHA256"
#define TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256        "DHE-RSA-CAMELLIA128-SHA256"
#define TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256            "CAMELLIA256-SHA256"
#define TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256        "DHE-RSA-CAMELLIA256-SHA256"

USHORT CTOS_TLS2_SetCipherList(IN UINT SSL_ID, IN const BYTE* pListString);
//=============================================================
USHORT CTOS_TLS2_SetSocket(IN UINT SSL_ID, IN INT Socket);
USHORT CTOS_TLS2_SetTimeout(IN INT SSL_ID, IN UINT Timeout);
void CTOS_TLS2_GetErrorMessage(OUT BYTE* pErrMsg);

//=============================================================================================================================
//
// LED & Back Light Functions
//
//=============================================================================================================================

//LED  parameter
#define d_LED1						0	// => RED
#define d_LED2						1	// => GREEN
#define d_LED3						2	// => ORANGE
#define d_LED4						3	// => BLUE

#define d_LED_RED					0
#define d_LED_GREEN					1
#define d_LED_ORANG					2
#define d_LED_BLUE					3

#define d_LED_MSR					0x80
#define d_LED_ICC					0x81

// Back Light parameter
#define d_BKLIT_LCD					0
#define d_BKLIT_KBD					1
#define d_BKLIT_LCD_DEVICE_0		d_BKLIT_LCD
#define d_BKLIT_LCD_DEVICE_1		2

USHORT CTOS_LEDSet(BYTE bLED, BYTE bOnOff);
USHORT CTOS_BackLightSet (BYTE bDevice, BYTE bOnOff);
USHORT CTOS_BackLightSetBrightness(BYTE bDevice,BYTE bValue);

//=============================================================================================================================
//
// RS232 Functions
//
//=============================================================================================================================

// port num
#define d_COM1						0
#define d_COM2						1
#define d_COM3                      2
#define d_COM4                      3
// bardrate
#define BAUD_115200                 115200
#define BAUD_57600                  57600
#define BAUD_38400                  38400
#define BAUD_19200                  19200
#define BAUD_9600                   9600
// parity
#define PARITY_E                    'E'
#define PARITY_N                    'N'
#define PARITY_O                    'O'
// data bits
#define DATABITS_7                  7
#define DATABITS_8                  8
// stop bits
#define STOPBITS_1                  1
#define STOPBITS_2                  2

USHORT CTOS_RS232Open(BYTE bPort, ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits);
USHORT CTOS_RS232TxReady(BYTE bPort);
USHORT CTOS_RS232TxData(BYTE bPort, BYTE* baBuf, USHORT usLen);
USHORT CTOS_RS232RxReady(BYTE bPort, USHORT* pusLen);
USHORT CTOS_RS232RxData(BYTE bPort, BYTE* baBuf, USHORT* pusLen);
USHORT CTOS_RS232SetRTS(BYTE bPort, BYTE bOnOff);
USHORT CTOS_RS232GetCTS(BYTE bPort);
USHORT CTOS_RS232FlushRxBuffer(BYTE bPort);
USHORT CTOS_RS232FlushTxBuffer(BYTE bPort);
USHORT CTOS_RS232Close(BYTE bPort);
//=============================================================================================================================
//
// USB Functions
//
//=============================================================================================================================
USHORT CTOS_USBOpen(void);
USHORT CTOS_USBClose(void);
USHORT CTOS_USBTxReady(void);
USHORT CTOS_USBTxData(BYTE* baSBuf, USHORT usSLen);
USHORT CTOS_USBRxReady(USHORT* pusRLen);
USHORT CTOS_USBRxData(BYTE* baRBuf, USHORT* pusRLen);
USHORT CTOS_USBTxFlush(void);
USHORT CTOS_USBRxFlush(void);
USHORT CTOS_USBSelectMode(BYTE bMode);


//=============================================================================================================================
//
// System Functions
//
//=============================================================================================================================
void CTOS_PowerOff(void);
USHORT CTOS_GetSystemInfo(BYTE bID, BYTE baBuf[17]);
USHORT CTOS_GetSerialNumber(BYTE baBuf[16]);
USHORT CTOS_GetFactorySN(BYTE baFactorySN[16]);
USHORT CTOS_HWSettingGet(BOOL* fPortable,BOOL* fPCI,USHORT* mkHWSupport);
USHORT CTOS_DeviceModelGet(BYTE *bModel);



//=============================================================================================================================
//
// Battery Functions
//
//=============================================================================================================================

// definition for battery status
#define d_MK_BATTERY_EXIST				0x00000001
#define d_MK_BATTERY_CHARGE				0x00000002
#define d_MK_BATTERY_FULL				0x00000004

USHORT CTOS_BatteryGetCapacity(BYTE* bPercentage);
USHORT CTOS_BatteryStatus(DWORD* pdwStatus);

//=============================================================================================================================
//
// Keyboard Functions
//
//=============================================================================================================================

#define d_KBD_INVALID						0xFF

#define d_KBD_1                                                         '1'
#define d_KBD_2                                                         '2'
#define d_KBD_3                                                         '3'
#define d_KBD_4                                                         '4'
#define d_KBD_5                                                         '5'
#define d_KBD_6                                                         '6'
#define d_KBD_7                                                         '7'
#define d_KBD_8                                                         '8'
#define d_KBD_9                                                         '9'
#define d_KBD_0                                                         '0'

#define d_KBD_K1                                                        'X'
#define d_KBD_K2                                                        'Y'
#define d_KBD_K3                                                        'I'
#define d_KBD_K4                                                        'J'
#define d_KBD_UP							                            'U'
#define d_KBD_DOWN							                            'D'

#define d_KBD_CANCEL                                                    'C'
#define d_KBD_CLEAR                                                     'R'
#define d_KBD_ENTER							                            'A'
#define d_KBD_DOT							                            'Q'
#define d_KBD_00							                            'P'

#define d_KBD_X                                                         'Q'
#define d_KBD_COMMA                                                     'P'

#define d_KBD_CREDITCARD                                                'D'
#define d_KBD_SUBTOTAL                                                  'S'
#define d_KBD_POWER                                                     'W'
#define d_KBD_PERCENTAGE                                                'E'
#define d_KBD_PLU                                                       'U'
#define d_KBD_BACK                                                      'B'

#define d_KBD_F1                                                        'X'
#define d_KBD_F2                                                        'Y'
#define d_KBD_F3                                                        'I'
#define d_KBD_F4                                                        'J'
#define d_KBD_F5                                                        'K'
#define d_KBD_F6                                                        'L'
#define d_KBD_F7                                                        'M'
#define d_KBD_F8                                                        'N'

USHORT CTOS_KBDGet(BYTE* pbKey);
USHORT CTOS_KBDHit(BYTE* pbKey);
USHORT CTOS_KBDInKey(BOOL* fKeyIn);
USHORT CTOS_KBDInKeyCheck(BYTE *pbKey);
USHORT CTOS_KBDSetResetEnable(BOOL boIsEnable);
USHORT CTOS_KBDScan(BYTE *pbKey);
USHORT CTOS_KBDBufCheck(BOOL *fKeyPressed);
USHORT CTOS_KBDBufGet(UCHAR *bKey);
USHORT CTOS_KBDBufPut(UCHAR bKey);
USHORT CTOS_KBDBufFlush(void);
USHORT Get_Key_Value(BYTE bKey);

//GSM
#define d_GPRS_SIM1						0
#define d_GPRS_SIM2						1
USHORT CTOS_GSMOpen(ULONG ulbaud, BYTE do_init);
USHORT CTOS_GSMClose(void);
USHORT CTOS_SIMCheckReady(void);
USHORT CTOS_SIMGetIMSI(BYTE *pbInfo);
USHORT CTOS_GSMSelectSIM(BYTE bID);
USHORT CTOS_GSMSignalQuality(BYTE *pbStrength);
USHORT CTOS_GSMQueryOperatorName(BYTE *baName, BYTE *pbLen);
USHORT CTOS_GSMGetIMEI(BYTE *pbInfo);


#ifdef __cplusplus
}
#endif

#endif

