#ifndef EMVEDL_H
#define	EMVEDL_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef ulong
#define ulong        ULONG
#endif
#ifndef ushort
#define ushort       USHORT
#endif
#ifndef byte
#define byte         BYTE
#endif


//Error Code
#define d_EMVEDL_VOLTAGE_RANDOM_SYMMETRIC_KEY_INIT_ERR			0x50000141
#define d_EMVEDL_VOLTAGE_CHECK_VER_ERR							0x50000142
#define d_EMVEDL_VOLTAGE_ROLL_KEY_ERR							0x50000143
#define d_EMVEDL_VOLTAGE_DESTROY_KEY_ERR						0x50000144
#define d_EMVEDL_VOLTAGE_BBPARAMS_LEN_ERR						0x50000145
#define d_EMVEDL_VOLTAGE_BBPARAMS_NOT_FOUND						0x50000146
#define d_EMVEDL_VOLTAGE_CHECKSUM_TYPE_NOT_SUPPORT				0x50000147
#define d_EMVEDL_VOLTAGE_ENCRPYTION_PROCEDURE_NOT_SUPPORT		0x50000148
#define d_EMVEDL_VOLTAGE_GENERATE_ETB_FAIL						0x50000149
#define d_EMVEDL_VOLTAGE_BASE_64_ENCODE_FAIL					0x5000014A
#define d_EMVEDL_VOLTAGE_BUF_OVERFLOW							0x5000014B
#define d_EMVEDL_VOLTAGE_ENCRYPTION_GET_ERR						0x5000014C
#define d_EMVEDL_VOLTAGE_DATA_TYPE_NOT_SUPPORT					0x5000014D
#define d_EMVEDL_VOLTAGE_CREATE_CONTEXT_ERR						0x5000014E
#define d_EMVEDL_VOLTAGE_KEY_STORAGE_ERR						0x5000014F
	
	
//Error Code For EMVEDL_VoltgaeGetLastError()  [From HPE-SecureData-Payments-Terminal-SDK-Linux-4.2.0_r206478]
#define VP_POS_ERROR_BASE								10000
#define VP_POS_ERROR_INVALID_POS_CONTEXT				VP_POS_ERROR_BASE				//An invalid VpPosContext was supplied.
#define VP_POS_ERROR_INVALID_PARAMS_DATA				VP_POS_ERROR_BASE + 1			//BB params malformed.
#define VP_POS_ERROR_ENCODE_IDENTITY					VP_POS_ERROR_BASE + 2			//Error encoding the identity.
#define VP_POS_ERROR_OUT_OF_MEMORY						VP_POS_ERROR_BASE + 3			//Out of memory.
#define VP_POS_ERROR_CREATE_PARAMS_OBJECT				VP_POS_ERROR_BASE + 4			//Could not build BB params object from parameters.
#define VP_POS_ERROR_SET_PARAMS_OBJECT					VP_POS_ERROR_BASE + 5			//Could not set BB params object with parameters.
#define VP_POS_ERROR_CREATE_KEY							VP_POS_ERROR_BASE + 6			//Error creating the key.
#define VP_POS_ERROR_SET_PUBLIC_KEY						VP_POS_ERROR_BASE + 7			//Error setting the public key.
#define VP_POS_ERROR_BB_ENCRYPT							VP_POS_ERROR_BASE + 8			//Error in BB encrypt.
#define VP_POS_ERROR_INVALID_RANDOM_OBJECT				VP_POS_ERROR_BASE + 9			//Invalid random object.
#define VP_POS_ERROR_SEED_RANDOM						VP_POS_ERROR_BASE + 10			//Error seeding the random object.
#define VP_POS_ERROR_BUFFER_TOO_SMALL					VP_POS_ERROR_BASE + 11			//Buffer too small.
#define VP_POS_ERROR_NULL_ARG							VP_POS_ERROR_BASE + 12			//Unexpected null argument.
#define VP_POS_ERROR_GENERATE_RANDOM_BYTES				VP_POS_ERROR_BASE + 13			//Error generating random bytes.
#define VP_POS_ERROR_INSUFFICIENT_RANDOM_SEED			VP_POS_ERROR_BASE + 14			//Insufficient random seed supplied.
#define VP_POS_ERROR_INVALID_KEY						VP_POS_ERROR_BASE + 15			//Bad VpPosKey given.
#define VP_POS_ERROR_TOOLKIT_ERROR						VP_POS_ERROR_BASE + 16			//Internal error.
#define VP_POS_ERROR_CARD_DATA_ENCRYPT_INIT				VP_POS_ERROR_BASE + 17			//Could not initialize for encrypting.
#define VP_POS_ERROR_CARD_DATA_ENCRYPT					VP_POS_ERROR_BASE + 18			//Error on encrypting.
#define VP_POS_ERROR_INVALID_DATA_TYPE					VP_POS_ERROR_BASE + 19			//Invalid data type.
#define VP_POS_ERROR_CREATE_ENCRYPT_ALGORITHM			VP_POS_ERROR_BASE + 20			//Could not create algorithm object with which to encrypt.
#define VP_POS_ERROR_INVALID_KEY_DATA					VP_POS_ERROR_BASE + 21			//Could not create a VpPosKey from this data.
#define VP_POS_ERROR_KEY_DATA_FORMAT_TOO_NEW			VP_POS_ERROR_BASE + 22			//Key data format is too new.
#define VP_POS_ERROR_INVALID_IDENTITY_STRING			VP_POS_ERROR_BASE + 23			//Identity string longer than 255 chars.
#define VP_POS_ERROR_PARAMS_DATA_TOO_OLD				VP_POS_ERROR_BASE + 24			//BB params too old.
#define VP_POS_ERROR_PARAMS_DATA_TOO_NEW				VP_POS_ERROR_BASE + 25			//BB params too new.
#define VP_POS_ERROR_OPERATION_CANCELED					VP_POS_ERROR_BASE + 26			//User cancelled operation.
#define VP_POS_ERROR_INTERNAL_ERROR						VP_POS_ERROR_BASE + 27			//Internal error.
#define VP_POS_ERROR_INVALID_CARD_DATA					VP_POS_ERROR_BASE + 28			//Card data does not correspond to proper format.
#define VP_POS_ERROR_INVALID_IDENTITY_DATE				VP_POS_ERROR_BASE + 29			//Invalid date used.
#define VP_POS_ERROR_INPUT_LENGTH_TOO_LONG				VP_POS_ERROR_BASE + 30			//Input longer than valid format.
#define VP_POS_ERROR_INPUT_LENGTH_TOO_SHORT				VP_POS_ERROR_BASE + 31			//Input shorter than valid format.
#define VP_POS_ERROR_REPETITIVE_RANDOM_SEED				VP_POS_ERROR_BASE + 32			//Random seed contains an illegal pattern.
#define VP_POS_ERROR_CANNOT_SET_ALPHABET				VP_POS_ERROR_BASE + 33			//Internal error.
#define VP_POS_ERROR_INVALID_RANDOM_TWEAK_OPTION		VP_POS_ERROR_BASE + 34			//Internal error.
#define VP_POS_ERROR_INVALID_ALPHABET_RANGE				VP_POS_ERROR_BASE + 35			//Internal error.
#define VP_POS_ERROR_ALPHABET_TOO_SHORT					VP_POS_ERROR_BASE + 36			//Internal error.
#define VP_POS_ERROR_INVALID_PROTOCOL_VERSION			VP_POS_ERROR_BASE + 37			//Unrecognized protocol version.
#define VP_POS_ERROR_UNSUPPORTED_FORMAT_CODE			VP_POS_ERROR_BASE + 38			//Track 1 data has unsupported format code. (Only 'B' is supported.)
#define VP_POS_ERROR_SPACES_IN_PAN						VP_POS_ERROR_BASE + 39			//Invalid number of spaces in pan (either as standalone or in tracks 1/2).
#define VP_POS_ERROR_ILLEGAL_CONTEXT_FLAGS				VP_POS_ERROR_BASE + 40			//An illegal combination of flags were specified on context create.
#define VP_POS_ERROR_BASE64_ENCODE						VP_POS_ERROR_BASE + 41			//Error base64 encoding a string.
#define VP_POS_ERROR_NO_TERMINAL_RSECRET				VP_POS_ERROR_BASE + 42			//Context lacks Terminal RSecret
#define VP_POS_ERROR_NO_TERMINAL_ID						VP_POS_ERROR_BASE + 43			//Context lacks Terminal ID
#define VP_POS_ERROR_UNSUPPORTED_232_CALL				VP_POS_ERROR_BASE + 44			//Error to invoke a function unsupported from POS 232.

	
//bEncProce
#define d_EMVEDL_VOLTAGE_ENC_PROCE_TEP1					0
#define d_EMVEDL_VOLTAGE_ENC_PROCE_TEP2					1
#define d_EMVEDL_VOLTAGE_ENC_PROCE_TEP1x				2
#define d_EMVEDL_VOLTAGE_ENC_PROCE_TEP2x				3
#define d_EMVEDL_VOLTAGE_ENC_PROCE_TEP4					4

//pbKeyStatus
#define d_EMVEDL_VOLTAGE_KEY_STATUS_NO_PRESENT			0
#define d_EMVEDL_VOLTAGE_KEY_STATUS_PRESENT				1

//Checksum Type
#define d_EMVEDL_VOLTAGE_CHECKSUM_TYPE_SHA1				1

//Data Type
#define d_EMVEDL_VOLTAGE_DATA_TYPE_MID					0x0100
#define d_EMVEDL_VOLTAGE_DATA_TYPE_PAN					0x0200
#define d_EMVEDL_VOLTAGE_DATA_TYPE_PANCVV				0x0300
#define d_EMVEDL_VOLTAGE_DATA_TYPE_TRACK1				0x0400
#define d_EMVEDL_VOLTAGE_DATA_TYPE_TRACK2				0x0500
#define d_EMVEDL_VOLTAGE_DATA_TYPE_TRACK3				0x0600
#define d_EMVEDL_VOLTAGE_DATA_TYPE_ALPHANUM				0x0700
#define d_EMVEDL_VOLTAGE_DATA_TYPE_BYTEARRAY			0x0800

//-------------------------------------------------------------------------
//EMVEDL
//-------------------------------------------------------------------------
ULONG EMVEDL_Initialize(void);

	
//-------------------------------------------------------------------------
//Voltage
//-------------------------------------------------------------------------
ULONG EMVEDL_VoltageInitialize(void);
ULONG EMVEDL_VoltageRandomSymmetricKeyStatusGet(IN BYTE bSingleKeyForAll, IN BYTE bEncProce, OUT BYTE *pbKeyStatus);
ULONG EMVEDL_VoltageRandomSymmetricKeyRoll(IN BYTE bSingleKeyForAll, IN BYTE bEncProce);
ULONG EMVEDL_VoltageRandomSymmetricKeyDestroy(IN BYTE bSingleKeyForAll, IN BYTE bEncProce);

ULONG EMVEDL_VoltageBbparams1024Set(IN BYTE *baBbparams, IN USHORT usBbparamsLen);
ULONG EMVEDL_VoltageBbparams1024ChecksumGet(IN BYTE bType, OUT BYTE *baChecksum, OUT USHORT *pusChecksumLen);

ULONG EMVEDL_VoltageBbparams3072Set(IN BYTE *baBbparams, IN USHORT usBbparamsLen);
ULONG EMVEDL_VoltageBbparams3072ChecksumGet(IN BYTE bType, OUT BYTE *baChecksum, OUT USHORT *pusChecksumLen);

ULONG EMVEDL_VoltageEtbGenerate(IN BYTE bEncProce, IN BYTE *baIdentityString, IN BYTE bIdentityStringLen, 
                                                                 OUT BYTE *baBase64Etb, OUT USHORT *pusBase64EtbLen, IN USHORT usBase64EtbMaxSize);

typedef struct
{
	IN BYTE bEncProce;
	IN USHORT usDataType;
	IN BYTE *pInputData;
	IN USHORT usInputDataLen;
	
	IN USHORT usCiphertextDataMaxSize;
	OUT BYTE *pCiphertextData;
	OUT USHORT usCiphertextDataLen;
	OUT BYTE bEncStatus;
	
}EMVEDL_VOLTAGE_ENC_DATA;
ULONG EMVEDL_VoltageEncryptionGet(BYTE bNum, EMVEDL_VOLTAGE_ENC_DATA *pstVoltageEncData);

//Get detail error code when getting 
//1. d_EMVEDL_VOLTAGE_CREATE_CONTEXT_ERR 
//2. d_EMVEDL_VOLTAGE_ROLL_KEY_ERR
//3. d_EMVEDL_VOLTAGE_DESTROY_KEY_ERR
//4. d_EMVEDL_VOLTAGE_GENERATE_ETB_FAIL
ULONG EMVEDL_VoltageGetLastError(void);


#ifdef	__cplusplus
}
#endif

#endif	/* EMVEDL_H */

