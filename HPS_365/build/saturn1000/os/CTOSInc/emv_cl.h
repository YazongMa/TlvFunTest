#ifndef __EMV_CL_H__
#define	__EMV_CL_H__

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//Return Code
//------------------------------------------------------------------------------
#define d_EMVCL_NO_ERROR									0x00000000
#define d_EMVCL_PENDING										0x80000020
#define d_EMVCL_TX_CANCEL									0x80000021
#define d_EMVCL_INIT_TAGSETTING_ERROR						0x80000022
#define d_EMVCL_INIT_CAPK_ERROR								0x80000023
#define d_EMVCL_INIT_PARAMETER_ERROR						0x80000024
#define d_EMVCL_INIT_CAPABILITY								0x80000025
#define d_EMVCL_INIT_REVOCATION_ERROR						0x80000026
#define d_EMVCL_INIT_EXCEPTION_FILE_ERROR					0x80000027
#define d_EMVCL_DATA_NOT_FOUND								0x80000030
#define d_EMVCL_NOT_OPEN									0x80000050
#define d_EMVCL_TRY_AGAIN									0x80000051
#define d_EMVCL_FUN_NOT_SUPPORT								0x80000060
#define d_EMVCL_PAYMENT_NOT_SUPPORT_COMPLETE				0x80000061
#define d_EMVCL_NON_EMV_CARD								0x80000062
#define d_EMVCL_APP_LIST_CANCEL								0x80000063
#define d_EMVCL_APP_LIST_FAILURE							0x80000064
#define d_EMVCL_VISA_NO_AFL									0x80000065
#define d_EMVCL_TXN_BUSY									0x80000066
#define d_EMVCL_KERNEL_NOT_FOUND							0x80000067			
#define d_EMVCL_BUF_OVERFLOW								0x80000068
#define d_EMVCL_ASRPD_USER_STOP_TXN							0x80000069
#define d_EMVCL_AMOUNT_NOT_PRESENT							0x8000006A
#define d_EMVCL_RC_DATA										0xA0000001
#define d_EMVCL_RC_SCHEME_SUPPORTED							0xA0000004
#define d_EMVCL_RC_FAILURE									0xA00000FF
#define d_EMVCL_RC_INVALID_DATA								0xA00000F8
#define d_EMVCL_RC_INVALID_PARAM							0xA00000F7
#define d_EMVCL_RC_INVALID_SCHEME							0xA00000F5
#define d_EMVCL_RC_INVALID_VISA_CA_KEY						0xA00000F4
#define d_EMVCL_RC_MORE_CARDS								0xA00000F3
#define d_EMVCL_RC_NO_CARD									0xA00000F2
#define d_EMVCL_RC_NO_EMV_TAGS								0xA00000F1
#define d_EMVCL_RC_FALLBACK									0xA00000E9
#define d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT				0xA00000E8
#define d_EMVCL_RC_NO_AP_FOUND								0xA00000E7
#define d_EMVCL_RC_DEK_SIGNAL								0xA00000E0
#define d_EMVCL_RC_SEE_PHONE      							0xA00000E4
#define d_EMVCL_RC_AMOUNT_0_NOT_ALLOW						0xA00000D0
	
#define d_EMVCL_PPSE_SW_6A81								0x70000010
#define d_EMVCL_SAID_LIST_SW_6A81							0x70000011

//Backward Compatible Return Code
#define d_EMVCL_TX_TIMEOUT									0x00000002
#define d_EMVCL_RX_TIMEOUT									0x00000003
#define d_EMVCL_CHECKSUM_INCORRECT							0x00000004
#define d_EMVCL_CRC_INCORRECT								0x00000004
#define d_EMVCL_LRC_INCORRECT								0x00000004
#define d_EMVCL_BUF_NOT_ENOUGH								0x00000005
#define	d_EMVCL_INIT_COM_ERROR								0x80000001
#define	d_EMVCL_INVALID_PARA								0x80000003
#define	d_EMVCL_COMM_FAIL									0x80000004
#define	d_EMVCL_RSP_LEN_ERROR								0x80000005
#define d_EMVCL_SN_INCORRECT								0x80000008
#define d_EMVCL_RSP_DATA_LEN_ERROR							0x80000009
#define	d_EMVCL_RSP_ID_ERROR								0x8000000A
#define d_EMVCL_BASE_INDEX_ERROR							0x8000000B
#define d_EMVCL_RC_SIGNATURE								0xA0000005
#define d_EMVCL_RC_SECOND_APPLICATION						0xA0000008
#define d_EMVCL_RC_DDA_AUTH_FAILURE							0xA00000FA
#define d_EMVCL_RC_CARD_DIFF_FROM_OTHER_AP_COUNTRY			0xA00000EE
#define d_EMVCL_RC_US_CARDS									0xA00000ED
#define d_EMVCL_RC_USE_CONTACT_ONLY							0xA00000E3

	
//------------------------------------------------------------------------------
//Definition
//------------------------------------------------------------------------------

//Used for EMVCL_ACT_DATA --> bStart
#define d_EMVCL_ACT_DATA_START_A							0x00
#define d_EMVCL_ACT_DATA_START_B							0x01

//Scheme ID : Used for EMVCL_RC_DATA_EX --> bSID
#define d_EMVCL_SID_VISA_OLD_US								0x13
#define d_EMVCL_SID_VISA_WAVE_2								0x16
#define d_EMVCL_SID_VISA_WAVE_QVSDC							0x17
#define d_EMVCL_SID_VISA_WAVE_MSD							0x18
#define d_EMVCL_SID_PAYPASS_MAG_STRIPE						0x20
#define d_EMVCL_SID_PAYPASS_MCHIP							0x21
#define d_EMVCL_SID_JCB_WAVE_2								0x61
#define d_EMVCL_SID_JCB_WAVE_QVSDC							0x62
#define d_EMVCL_SID_JCB_EMV									0x63
#define d_EMVCL_SID_JCB_MSD									0x64
#define d_EMVCL_SID_JCB_LEGACY								0x65
#define d_EMVCL_SID_AE_EMV									0x50
#define d_EMVCL_SID_AE_MAG_STRIPE							0x52
#define d_EMVCL_SID_DISCOVER								0x41
#define d_EMVCL_SID_DISCOVER_DPAS							0x42
#define d_EMVCL_SID_DISCOVER_DPAS_MAG_STRIPE				0x43
#define d_EMVCL_SID_INTERAC_FLASH							0x48
#define d_EMVCL_SID_MEPS_MCCS								0x81
#define d_EMVCL_SID_CUP_QPBOC								0x91
#define d_EMVCL_SID_CB_PAGOBANCOMAT							0x86
	
//Transction Result : Used for EMVCL_RC_DATA_ANALYZE--> usTransResult
#define d_EMVCL_OUTCOME_APPROVAL							0x0002
#define d_EMVCL_OUTCOME_DECLINED							0x0003
#define d_EMVCL_OUTCOME_ONL									0x0004

#define d_EMVCL_OUTCOME_ERROR_CARD_DATA_MISSING				0x0B01
#define d_EMVCL_OUTCOME_ERROR_CAM_FAILED					0x0B02
#define d_EMVCL_OUTCOME_ERROR_SATTUS_BYTES					0x0B03
#define d_EMVCL_OUTCOME_ERROR_PARSING_ERROR					0x0B04
#define d_EMVCL_OUTCOME_ERROR_MAX_LIMIT_EXCEEDED			0x0B05
#define d_EMVCL_OUTCOME_ERROR_CARD_DATA_ERROR				0x0B06
#define d_EMVCL_OUTCOME_ERROR_MAGSTRIPE_NOT_SUPPORTED		0x0B07
#define d_EMVCL_OUTCOME_ERROR_NO_PPSE						0x0B08
#define d_EMVCL_OUTCOME_ERROR_PPSE_FAULT					0x0B09
#define d_EMVCL_OUTCOME_ERROR_EMPTY_CANDIDATE_LIST			0x0B0A
#define d_EMVCL_OUTCOME_ERROR_TERMINAL_DATA_ERROR			0x0B0F
#define d_EMVCL_OUTCOME_ERROR_TERMINAL_PROCESS_ERROR		0x0B10

//CVM Analysis : Used for EMVCL_RC_DATA_ANALYZE--> bCVMAnalysis
#define d_EMVCL_CVM_REQUIRED_NONE							0x00
#define d_EMVCL_CVM_REQUIRED_SIGNATURE						0x01
#define d_EMVCL_CVM_REQUIRED_ONLPIN							0x02
#define d_EMVCL_CVM_FAIL									0x03
#define d_EMVCL_CVM_REQUIRED_NOCVM							0x04
#define d_EMVCL_CVM_REQUIRED_CONFIRMATION_CODE_VERIFIED		0x05

//Used for EMVCL_SetUIType/EMVCL_GetUIType --> bType
#define d_EMVCL_UITYPE_NORMAL								0x00
#define d_EMVCL_UITYPE_EUR									0x01

//Txn return code is d_EMVCL_RC_DEK_SIGNAL, check the baChipData[0]
#define d_DEK_VISA_MOBILE_DEVICE_SEE_PHONE					0x06
#define d_DEK_VISA_EUR_CL_TIG_FATAL_COMMUNICATION			0x07
	
//Used for EMVCL_USER_INTERFACE_REQ_DATA -> bMessageIdentifier
#define d_EMVCL_USR_REQ_MSG_CARD_READ_OK					0x17 
#define d_EMVCL_USR_REQ_MSG_TRY_AGAIN						0x21 
#define d_EMVCL_USR_REQ_MSG_APPROVED						0x03 
#define d_EMVCL_USR_REQ_MSG_APPROVED_SIGN					0x1A 
#define d_EMVCL_USR_REQ_MSG_DECLINED						0x07 
#define d_EMVCL_USR_REQ_MSG_ERROR_OTHER_CARD				0x1C 
#define d_EMVCL_USR_REQ_MSG_INSERT_CARD						0x1D 
#define d_EMVCL_USR_REQ_MSG_SEE_PHONE						0x20 
#define d_EMVCL_USR_REQ_MSG_AUTHORISING_PLEASE_WAIT			0x1B     
#define d_EMVCL_USR_REQ_MSG_CLEAR_DISPLAY					0x1E 
#define d_EMVCL_USR_REQ_MSG_ENTER_PIN						0x09 
#define d_EMVCL_USR_REQ_MSG_PROCESSING_ERR					0x0F 
#define d_EMVCL_USR_REQ_MSG_REMOVE_CARD						0x10     
#define d_EMVCL_USR_REQ_MSG_WELCOME							0x14     
#define d_EMVCL_USR_REQ_MSG_PRESENT_CARD					0x15 
#define d_EMVCL_USR_REQ_MSG_PROCESSING						0x16 
#define d_EMVCL_USR_REQ_MSG_INSERT_OR_SWIPE_CARD			0x18 
#define d_EMVCL_USR_REQ_MSG_PRESENT_1_CARD_ONLY				0x19     
#define d_EMVCL_USR_REQ_MSG_PRESENT_CARD_AGAIN				0x21 
#define d_EMVCL_USR_REQ_MSG_NO_CARD							0xA0     
#define d_EMVCL_USR_REQ_MSG_NA								0xFF 

//Used for EMVCL_USER_INTERFACE_REQ_DATA -> bStatus
#define d_EMVCL_USR_REQ_STATUS_NOT_READY					0x00
#define d_EMVCL_USR_REQ_STATUS_IDLE							0x01
#define d_EMVCL_USR_REQ_STATUS_READY_TO_READ				0x02
#define d_EMVCL_USR_REQ_STATUS_PROCESSING					0x03
#define d_EMVCL_USR_REQ_STATUS_CARD_READ_SUCCESSFULLY		0x04
#define d_EMVCL_USR_REQ_STATUS_PROCESSING_ERROR				0x05
#define d_EMVCL_USR_REQ_STATUS_NA							0xFF

// Kernel ID
#define d_EMVCL_KERNEL_ID_MASTERCARD						0x02
#define d_EMVCL_KERNEL_ID_VISA								0x03
#define d_EMVCL_KERNEL_ID_AMERICAN_EXPRESS					0x04
#define d_EMVCL_KERNEL_ID_JCB								0x05
#define d_EMVCL_KERNEL_ID_DISCOVER							0x06
#define d_EMVCL_KERNEL_ID_UNIONPAY							0x07

// Kernel Name
#define d_EMVCL_KERNEL_NAME_CLENTRY							"CLENTRY"	//Entrypoint
#define d_EMVCL_KERNEL_NAME_MPP								"MPP"		//MasterCard PayPass / EMVCo Book C-2
#define d_EMVCL_KERNEL_NAME_VPW								"VPW"		//VISA Global qVSDC and MSD / EMVCo Book C-3
#define d_EMVCL_KERNEL_NAME_AEP								"AEP"		//AMEX Expresspay / EMVCo Book C-4
#define d_EMVCL_KERNEL_NAME_JCT								"JCT"		//JCB Contactless Payment / EMVCo Book C-5
#define d_EMVCL_KERNEL_NAME_VAP								"VAP"		//VISA Asia Pacific Contactless Payment / EMVCo Book C-1
#define d_EMVCL_KERNEL_NAME_CQP								"CQP"		//China UnionPay qPBOC and QuickPass / EMVCo Book C-7
#define d_EMVCL_KERNEL_NAME_IFH								"IFH"		//INTERAC Flash
#define d_EMVCL_KERNEL_NAME_DDP								"DDP"		//Discover D-PAS / EMVCo Book C-6
#define d_EMVCL_KERNEL_NAME_MMS								"MMS"		//MEPS MCCS / PURE
	
// Transaction Type
#define d_EMVCL_TXN_TYPE_PURCHASE							0x00
#define d_EMVCL_TXN_TYPE_CASH								0x01
#define d_EMVCL_TXN_TYPE_CASHBACK							0x09
#define d_EMVCL_TXN_TYPE_REFUND								0x20

// Parameter ID
#define d_EMVCL_PARA_INDEX_MSG_TIMEOUT						0x0001			
#define d_EMVCL_PARA_INDEX_SALE_TIMEOUT						0x0002
#define d_EMVCL_PARA_INDEX_POLL_MSG							0x0003
#define d_EMVCL_PARA_INDEX_BUF_TIMEOUT						0x0004
#define d_EMVCL_PARA_INDEX_ENCRYPTION						0x0005
#define d_EMVCL_PARA_INDEX_DISPLAY							0x0006
#define d_EMVCL_PARA_INDEX_MAX_BUF_SIZE						0x0007
#define d_EMVCL_PARA_INDEX_DOUBLE_DIP						0x0008
#define d_EMVCL_PARA_INDEX_READER_INDEX						0x0009
#define d_EMVCL_PARA_INDEX_LAUGUAGE							0x000A
#define d_EMVCL_PARA_INDEX_DISPLAY_S_MSG					0x000B
#define d_EMVCL_PARA_INDEX_DISPLAY_L_MSG					0x000C
#define d_EMVCL_PARA_INDEX_DISPLAY_SS_MSG					0x000D
#define d_EMVCL_PARA_INDEX_DISPLAY_SR_MSG					0x000E
#define d_EMVCL_PARA_INDEX_DISPLAY_PIN_MSG					0x000F
#define d_EMVCL_PARA_INDEX_DISPLAY_E_MSG					0x0010
#define d_EMVCL_PARA_INDEX_UI_TYPE							0x100A
#define d_EMVCL_PARA_INDEX_VISA_EUR_CL_TIG_FOLLOW			0x100B
#define d_EMVCL_PARA_INDEX_SELECT_AID_LIST_COMPLIANT		0x3004

// Action Value for EMVCL_CA_PUBLIC_KEY
#define d_EMVCL_CAPK_ACTION_ADD								0x00
#define d_EMVCL_CAPK_ACTION_DELETE							0x01
#define d_EMVCL_CAPK_ACTION_DELETE_ALL						0x02
	
// Action for EMVCL_CompleteEx
#define d_EMVCL_COMPLETE_ACTION_ONL_APPROVED				0x01
#define d_EMVCL_COMPLETE_ACTION_ONL_DECLINED				0x02
#define d_EMVCL_COMPLETE_ACTION_UNAB_ONL					0x03
	
// bDebugPort for EMVCL_SetDebug
#define d_EMVCL_DEBUG_PORT_USB								0xFF
#define d_EMVCL_DEBUG_PORT_COM1								d_COM1
#define d_EMVCL_DEBUG_PORT_COM2								d_COM2
#define d_EMVCL_DEBUG_PORT_COM3								d_COM3
#define d_EMVCL_DEBUG_PORT_SP_EVENT							0xF4
	
// bAction for EMVCL_SecureDataWhitelistSet
#define d_EMVCL_WHITELIST_ACTION_NEW            	          0x00
#define d_EMVCL_WHITELIST_ACTION_DELETE_ALL                  0x01
	

//------------------------------------------------------------------------------
//Data Structure
//------------------------------------------------------------------------------
	
//------------------------------------------------------------------------------
typedef struct 
{
	BYTE	bMessageIdentifier;
	BYTE	bStatus;
	BYTE	baHoldTime[3];
	BYTE	baLanguagePreference[8];
	BYTE	bValueQualifier;
	BYTE	baValue[6];
	BYTE	baCurrencyCode[2];
	
}EMVCL_USER_INTERFACE_REQ_DATA;

typedef struct 
{
	BOOL (*OnCancelTransaction)(void);
	void (*OnShowMessage)(BYTE bKernel, EMVCL_USER_INTERFACE_REQ_DATA *baUserInterfaceRequestData);
	
}EMVCL_EVENT;

typedef struct 
{
	BYTE	bConfigFilenameLen;
	BYTE	*pConfigFilename;
	EMVCL_EVENT stOnEvent;
	
}EMVCL_INIT_DATA;
	
//------------------------------------------------------------------------------
typedef struct 
{
	BYTE	bStart;
	BYTE	bTagNum;
	USHORT	usTransactionDataLen;
	BYTE	*pbaTransactionData;

}EMVCL_ACT_DATA;
	
//------------------------------------------------------------------------------
typedef struct 
{
	BYTE	bSID;
	BYTE	baDateTime[15];
	BYTE	bTrack1Len;
	BYTE	baTrack1Data[100];
	BYTE	bTrack2Len;
	BYTE	baTrack2Data[100];
	USHORT	usChipDataLen;
	BYTE	baChipData[1024];
	USHORT	usAdditionalDataLen;
	BYTE	baAdditionalData[1024];
	
}EMVCL_RC_DATA_EX;

//------------------------------------------------------------------------------
typedef struct
{
	USHORT	usTransResult;			// Transction Result
	BYTE	bCVMAnalysis;			//
	BYTE	baCVMResults[3];		//CVM Result
	BYTE	bVisaAOSAPresent;		//Visa Available Offline Spending Amount(AOSA) Present
	BYTE	baVisaAOSA[6];
	BOOL	bODAFail;				//Offline Data Authentication Fail , ODA FAIL -> TRUE, ODA OK --> FALSE
	BOOL	bCompleteFunRequired;	//Complete Function Required, Yes -> TRUE, No -> FALSE
	
}EMVCL_RC_DATA_ANALYZE;

//------------------------------------------------------------------------------
typedef struct 
{
	IN BYTE			bAIDLen;
	IN BYTE			baAID[16];
	IN BYTE			bKernelID;
	IN BYTE			bTransactionType;
	IN USHORT		usTAGDataLen;
	IN BYTE			*pbaTAGData;

}EMVCL_AID_SET_TAG_DATA;

//------------------------------------------------------------------------------
typedef struct 
{
	IN BYTE			bAIDLen;
	IN BYTE			baAID[16];
	IN BYTE			bKernelID;
	IN BYTE			bTransactionType;
	INOUT USHORT	usTAGDataLen;
	OUT BYTE		*pbaTAGData;
	
}EMVCL_AID_GET_TAG_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	IN UINT uiVersion;				//fixed to 0x01
	OUT UINT uiNo;					//Number of AID Combination List {TransType-AID-KERNEL ID}
	OUT BYTE bAIDLen[256];			//list all of AID Len
	OUT BYTE baAID[256][16];		//list all of AID data
	OUT BYTE bKernelID[256];		//list all of Kernel ID data
	OUT BYTE bTransactionType[256];	//list all of Transaction Type data

	// One AID Combination List {bAIDLen[x] + baAID[x][] + bKernelID[x] + bTransactionType[x]}  (x = 0 ~ uiNo)
}EMVCL_AID_COMBINATION_LIST;

//------------------------------------------------------------------------------
typedef struct
{
	BYTE	bAction;
	BYTE	bIndex;
	UINT	uiModulusLen;
	BYTE	baModulus[248];
	UINT	uiExponentLen;		// Length of Extension
	BYTE	baExponent[3];		// Extension
	BYTE	baHash[20];			// Key Hash (SHA-1) Result

}EMVCL_CA_PUBLIC_KEY;

//------------------------------------------------------------------------------
typedef struct
{
	UINT	uiNoP;				// Number of Parameters
	UINT	uiIndex[100];		// Parameter Index
	UINT	uiLen[100];			// Length of Parameter Data
	BYTE	baData[100][20];	// Parameter Data
	
}EMVCL_PARAMETER_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	BYTE	bNoS;				// Number of Schemes
	BYTE	baID[255];			// Scheme ID
	BYTE	baAction[255];		// Active or deactive
	
}EMVCL_SCHEME_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	IN		BYTE	bTagNum;
	IN		USHORT	usTransactionDataLen;
	IN		BYTE	*pbaTransactionData;
	INOUT	BYTE	bSelectedAIDLen;
	OUT		BYTE	*pbaSelectedAID;
	INOUT	ULONG	ulSelectAIDRspLen;
	OUT		BYTE	*pbaSelectAIDRsp;
	INOUT	ULONG	ulSelectPPSERspLen;
	OUT		BYTE	*pbaSelectPPSERsp;
	
}EMVCL_DETECT_TXN_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	BYTE		bSID;				// Scheme Identifier
	BYTE		baDateTime[15];		// YYYYMMDDHHMMSS format
	BYTE		bTrack1Len;
	BYTE		baTrack1Data[100];	// ANS 
	BYTE		bTrack2Len;
	BYTE		baTrack2Data[100];	// ASCII
	BYTE		bChipDataLen;		// Chip Data
	BYTE		baChipData[256];
	BYTE		bAdditionalDataLen;	// Additional Data
	BYTE		baAdditionalData[256];
	
}EMVCL_RC_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	BYTE		bNoCVM;				// Number of CVM
	BYTE		baCVMID[10];		// CVM ID
	BYTE		baAction[10];		// Active or deactive
	
}EMVCL_CVM_DATA;

//------------------------------------------------------------------------------
typedef struct
{
	IN		UINT	uiVersion;				//fixed to 0x01
	
	IN		USHORT	usTagNum;
	IN		USHORT	usTransactionDataLen;
	IN		USHORT	usRequestedCardTagLen;
	IN		BYTE	*pbaTransactionData;
	IN		BYTE	*pbaRequestedCardTag;
	
	INOUT	USHORT	usSelectedAIDLen;
	INOUT	USHORT	usSelectAIDRspLen;
	INOUT	USHORT	usSelectPPSERspLen;
	INOUT	USHORT	usGotCardDataLen;
	OUT		BYTE	*pbaSelectedAID;
	OUT		BYTE	*pbaSelectAIDRsp;
	OUT		BYTE	*pbaSelectPPSERsp;
	OUT		BYTE	*pbaGotCardData;
	
}EMVCL_GET_CARD_DATA;
//------------------------------------------------------------------------------
typedef struct
{
	UINT	uiVersion;						// fixed to 0x01;
	
	BYTE	baAppLabel[33];
	BYTE	bAIDLen;
	BYTE	baAID[16];
	
}EMVCL_APP_LIST_DATA;
//------------------------------------------------------------------------------
typedef struct
{
	UINT	uiVersion;						// fixed to 0x02;
	
	BYTE	baAppLabel[33];
	BYTE	bAIDLen;
	BYTE	baAID[16];
	BOOL	bIssuerIdentificationNumberPresented;
	BYTE	baIssuerIdentificationNumber[3];
	BOOL	bIssuerCountryCodeAlpha2Presented;
	BYTE	baIssuerCountryCodeAlpha2[2];
	
}EMVCL_APP_LIST_DATA_V2;
//------------------------------------------------------------------------------
typedef struct
{
	IN	UINT	uiVersion;						// fixed to 0x01;
	
	IN	BYTE	bAIDLen;
	IN	BYTE	baAID[16];
	IN	BYTE	bASRPDLen;
	IN	BYTE	baASRPD[252];
	OUT	BYTE	bAction;
	
}EMVCL_APP_SELE_REG_PROPRI_DATA;



//Add for S1
typedef struct
{
	BYTE	bVersion;
	BYTE	bKeyType;
	USHORT	usCipherKeySet;
	USHORT	usCipherKeyIndex;
	BYTE	bCipherMethod;
	BYTE	baICV[32];
	BYTE	bICVLen;
	BYTE	baSK[32];
	BYTE	bSKLen;
	BYTE	bChecksumType;
	BYTE	bPaddingMethod;	
	BYTE	bLRCIncluded;
	BYTE	bSS_ESIncluded;		
	//add
	BYTE bIsKSNFixed;
	
}EMVCL_SECURE_DATA_INFO;

//------------------------------------------------------------------------------
//For EMVCL_SpecialEventRegister
//------------------------------------------------------------------------------
#define d_EMVCL_EVENTID_LED_PIC_SHOW						0x01
typedef ULONG (*EVENT_EMVCL_LED_PIC_SHOW)(IN BYTE bIndex, IN BYTE bOnOff);

#define d_EMVCL_EVENTID_PP30_DEK							0x02
typedef void (*EVENT_EMVCL_PP30_DEK)(BYTE *baDEK, USHORT usDEKLen);

#define d_EMVCL_EVENTID_PP30_DET							0x03
typedef USHORT (*EVENT_EMVCL_PP30_DET)(BYTE *baDETData, USHORT *pusDETDataLen, USHORT usDETMaxSize, USHORT us10MsTimeout);
#define d_EMVCL_EVENT_PP30DET_RECEIVE_ONE_DET								0x0301
#define d_EMVCL_EVENT_PP30DET_TIMEOUT										0x0302
#define d_EMVCL_EVENT_PP30DET_STOP_SIGNAL									0x0303

#define d_EMVCL_EVENTID_AUDIO_INDICATION					0x04
typedef ULONG (*EVENT_EMVCL_AUDIO_INDICATION)(IN USHORT usTone);
#define d_EMVCL_EVENT_AUDIO_TONE_CARD_READ_OK								0x0001
#define d_EMVCL_EVENT_AUDIO_TONE_ALERT										0x0002

#define d_EMVCL_EVENTID_NON_EMV_CARD						0x05
typedef void (*EVENT_EMVCL_NON_EMV_CARD)(OUT BYTE *pbNonEMVCard);
//pbNonEMVCard :
//	0x00 : EMV Card
//	0x01 : non-EMV Card

#define d_EMVCL_EVENTID_APP_LIST							0x06
typedef USHORT (*EVENT_EMVCL_APP_LIST)(IN BYTE bAppNum, IN EMVCL_APP_LIST_DATA *pstAppListExData, OUT BYTE *pbAppSelectedIndex);
#define d_EMVCL_EVENT_APP_LIST_RTN_OK										0x0000
#define d_EMVCL_EVENT_APP_LIST_RTN_CANCEL									0x0001
#define d_EMVCL_EVENT_APP_LIST_RTN_FAILURE									0x0002

#define d_EMVCL_EVENTID_APP_LIST_V2							0x07
typedef USHORT (*EVENT_EMVCL_APP_LIST_V2)(IN BYTE bAppNum, IN EMVCL_APP_LIST_DATA_V2 *pstAppListExData, OUT BYTE *pbAppSelectedIndex);

//Only apply to EUR UI Type. Inform "Read Card OK" behavior was completed
#define d_EMVCL_EVENTID_EUR_LED_READ_CARD_OK_DONE			0x08
typedef USHORT (*EVENT_EMVCL_EUR_LED_READ_CARD_OK_DONE)(void *pPara);

//This Event will be triggered in the phase of finding card and can inform
//kernel to perform specific behavior.
#define d_EMVCL_EVENTID_READY_TO_READ_BEHAVIOR				0x09
typedef USHORT (*EVENT_EMVCL_READY_TO_READ_BEHAVIOR)(BYTE *pbMode);
//pbMode :
//	0x01 : Pause "Ready to Read" LED behavior
//	Other : Normal "Ready to Read" behavior
#define d_EMVCL_EVENT_READY_TO_READ_NORMAL									0x00
#define d_EMVCL_EVENT_READY_TO_READ_NO_LED									0x01

//Application Selection Registered Proprietary Data
//This Event will be triggered if the ASRPD is presented in card with correct data format
//User set argument Action to inform kernel if transaction continues or not
#define d_EMVCL_EVENTID_APP_SELE_REG_PROPRI_DATA			0x0A
typedef USHORT (*EVENT_EMVCL_APP_SELE_REG_PROPRI_DATA)(EMVCL_APP_SELE_REG_PROPRI_DATA *pstASRPD);
#define d_EMVCL_EVENT_ASRPD_ACTION_CONTINUE									0x00
#define d_EMVCL_EVENT_ASRPD_ACTION_STOP_TXN_WITH_UI							0x01
#define d_EMVCL_EVENT_ASRPD_ACTION_STOP_TXN_WITHOUT_UI						0x02

#define d_EMVCL_EVENTID_CAPK								0x0B
typedef BOOL (*EVENT_EMVCL_CAPK)(IN BYTE *baRID, IN BYTE bKeyIndex, 
		OUT BYTE *baModulus, OUT USHORT *pusModulusLen, OUT BYTE *baExponent, OUT USHORT *pusExponentLen);

#define d_EMVCL_EVENTID_PRE_NON_EMV_CARD					0x0C
typedef void (*EVENT_EMVCL_PRE_NON_EMV_CARD)(OUT BYTE *pbNonEMVCard);
//pbNonEMVCard :
//	0x00 : EMV Card
//	0x01 : non-EMV Card

#define d_EMVCL_EVENTID_CPB_ONL_PIN							0x0D
typedef USHORT (*EVENT_EMVCL_CPB_ONL_PIN)(void *pPara);
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_OK									0x0000
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_MALFUNCTION							0x0001
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_TIMEOUT								0x0002
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_BYPASS								0x0003
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_CANCEL								0x0004
#define d_EMVCL_EVENT_CPB_ONL_PIN_RTN_TERMINATED							0x0005

//Extension of AID Tag data
//Please declare database based on EMVCL_AID_SET_TAG_DATA
//ex : EMVCL_AID_SET_TAG_DATA TagDataExt[10];
//*pulTagDataAddr = (ULONG)TagDataExt;
#define d_EMVCL_EVENTID_AID_TAG_DATA						0x0E
typedef USHORT (*EVENT_EMVCL_AID_TAG_DATA)(OUT USHORT *pusNum, OUT ULONG *pulTagDataAddr);

#define d_EMVCL_EVENTID_DEBUG_MSG							0x0F
typedef void (*EVENT_EMVCL_DEBUG_MSG)(IN BYTE *baMsg, IN USHORT usMsgLen);

//------------------------------------------------------------------------------
// EMVCL_EVENTID_xxx Only for S1
//------------------------------------------------------------------------------
//Modify for S1, Cancal and show message remove form EMVCL_Initialize, set at specail event
#define d_EMVCL_EVENTID_CANCEL_TRANSACTION					0xC0	//0x0B ,already using
typedef BOOL (*EVENT_EMVCL_CANCEL_TRANSACTION)(void);

#define d_EMVCL_EVENTID_SHOW_MESSAGE						0xC1//0x0C ,already using
typedef void (*EVENT_EMVCL_SHOW_MESSAGE)(BYTE bKernel, EMVCL_USER_INTERFACE_REQ_DATA *baUserInterfaceRequestData);
//------------------------------------------------------------------------------
// EMVCL APIs
//------------------------------------------------------------------------------

/* ==========================================================================
 * FUNCTION NAME: EMVCL_Initialize
 * DESCRIPTION:   Initiate EMVCL environment.
 * 
 * [IN] pEvent : point to struct EMVCL_INIT_DATA
 * ========================================================================== */
ULONG EMVCL_Initialize(EMVCL_INIT_DATA *pInitData);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetAPIVersion
 * DESCRIPTION:   Get Version
 * 
 * [out]cVer : Version srting
 * ========================================================================== */
void EMVCL_GetAPIVersion(char *cVer);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_ShowContactlessSymbol
 * DESCRIPTION:   Show contactless symbol on the screen.
 * 
 * [IN] pPara : RFU. Please set to NULL
 * ========================================================================== */
ULONG EMVCL_ShowContactlessSymbol(void *pPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_ShowVirtualLED
 * DESCRIPTION:   Show Virtual LED on screen.
 *
 * [IN] pPara : RFU. Please set to NULL
 * ========================================================================== */
ULONG EMVCL_ShowVirtualLED(void *pPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetLED
 * DESCRIPTION:   Set LED
 * 
 * [in]bIndex : LED Index (Bit0:LED1 RED, Bit1:LED2 GREEN, Bit2:LED3 YELLOW, Bit3:LED4 BLUE, Bit4-Bit7:RFU)
 *		Bitx = 1: LED(x+1) on/off indicated in bOnOff Bitx
 *		Bitx = 0: LED(x+1) with the current state 
 * [in]bOnOff : Turn on or off the LEDs
 *		Bitx = 1: control LED(x+1) on if LED index Bitx is set.
 *		Bitx = 0: control LED(x+1) off if LED index Bitx is set.
 * ========================================================================== */
ULONG EMVCL_SetLED(BYTE bIndex, BYTE bOnOff);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_StartIdleLEDBehavior
 * DESCRIPTION:   Start the idle behavior on LED (visual status indicators).
 *		Normal UI : First indicator(blue led) is always on
 *		EUR UI : First indicator blink on for approximately 200ms every five seconds.
 *
 * [IN] pPara : RFU. Please set to NULL
 * ========================================================================== */
ULONG EMVCL_StartIdleLEDBehavior(void *pPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_StopIdleLEDBehavior
 * DESCRIPTION:   Stop the idle behavior on LED (visual status indicators).
 *
 * [IN] pPara : RFU. Please set to NULL
 * ========================================================================== */
ULONG EMVCL_StopIdleLEDBehavior(void *pPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_AIDSetTagData
 * DESCRIPTION:   Set AID with TagsSetting. 
 *				EMVCL kernel can handle and storage more tags settings 
 *				for different combination list {TransType-AID-KERNEL ID}
 * 
 *				This API is used to set TagsSettingx 
 *				for one combination list {TransType-AID-KERNEL ID}
 * 
 * [IN] bAction : 
 *				= 0x00 for one TagSetting Addition
 *				= 0x01 for one TagSetting Deletion
 *				= 0x02 for All TagSetting Deletion
 * [IN] stTagData : Point to a EMVCL_AID_TAG_DATA structure containing the tag setting to be set/deleted
 * ========================================================================== */
ULONG EMVCL_AIDSetTagData(BYTE bAction, EMVCL_AID_SET_TAG_DATA *stTagData);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_AIDGetTagData
 * DESCRIPTION:   Get TagSetting of specific AID from EMVCL Contactless kernel.
 * 
 * [INOUT] stTagData : input is AID-kernelID-Transaction, output is TagsSetting
 * ========================================================================== */
ULONG EMVCL_AIDGetTagData(EMVCL_AID_GET_TAG_DATA *stTagData);

/*==========================================================================
 * FUNCTION NAME: EMVCL_AIDGetCombinationList
 * DESCRIPTION:   Get all of AID combinations list {TransType-AID-KERNEL ID}
 *
 * [INOUT] pstAIDList : Point to a EMVCL_AID_COMBINATION_LIST structure
 *						to get all of AID combinations.
==========================================================================*/ 
ULONG EMVCL_AIDGetCombinationList(EMVCL_AID_COMBINATION_LIST *pstAIDList);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetCAPK
 * DESCRIPTION:   Set CA Public Key with specified RID to EMVCL Contactless kernel. 
 *				  The key to be set will belong to the specified RID.
 * 
 * [in]baRID : Point to a buffer containing RID. RID must be 5 bytes
 * [in]stCAPubKey : Point to a EMVCL_CA_PUBLIC_KEY structure containing the public key to be set.
 * ========================================================================== */
ULONG EMVCL_SetCAPK(BYTE *baRID, EMVCL_CA_PUBLIC_KEY *stCAPubKey);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetCAPK
 * DESCRIPTION:   Get CA Public Key with specified RID from EMVCL Contactless kernel.
 * 
 * [in]baRID : Point to a buffer containing RID. RID must be 5 bytes.
 * [in]bKID : CAPK index
 * [out]stCAPubKey : Point to a EMVCL_CA_PUBLIC_KEY structure to retrieve the corresponding CA public key.
 * ========================================================================== */
ULONG EMVCL_GetCAPK(BYTE *baRID, BYTE bKID, EMVCL_CA_PUBLIC_KEY *stCAPubKey);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_ListAllCAPKID
 * DESCRIPTION:   List all CAPK indexs saved in EMVCL Contactless kernel..
 * 
 * ListCAPK_x = RIDx + RIDx CAPKI number + RIDx CAPKIx + RIDx CAPKI2 + RIDx CAPKI3 + ...
 * [out]baRBuf : format is RID number +  ListCAPK_1 +  ListCAPK_2 +  ListCAPK_3 +...
 * [out]uiRLen : length of baRBuf
 * ========================================================================== */
ULONG EMVCL_ListAllCAPKID(BYTE *baRBuf, UINT *uiRLen);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetParameter
 * DESCRIPTION:   Set parameter.
 * 
 * [in]stPara : Point to a EMVCL_PARAMETER_DATA structure. 
 * ========================================================================== */
ULONG EMVCL_SetParameter(EMVCL_PARAMETER_DATA *stPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetParameter
 * DESCRIPTION:   Get one parameter.
 * 
 * [in]uiPID : parameter index
 * [out] stPara : parameter data
 * ========================================================================== */
ULONG EMVCL_GetParameter(UINT uiPID, EMVCL_PARAMETER_DATA *stPara);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetUIType
 * DESCRIPTION:   Set UI Type.
 * 
 * [in]bType : UI Type
 *		= 0x00 for Normal UI Type
 *		= 0x01 for EUR UI Type
 * ========================================================================== */
ULONG EMVCL_SetUIType(BYTE bType);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetUIType
 * DESCRIPTION:   Get UI Type.
 * 
 * [out]bType : UI Type
 *		= 0x00 for Normal UI Type
 *		= 0x01 for EUR UI Type
 * ========================================================================== */
void EMVCL_GetUIType(BYTE *bType);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_VisaSetCapability
 * DESCRIPTION:   Set Visa Capability.
 * 
 * [in]stScheme : scheme
 * [out]stRsp : response scheme
 * ========================================================================== */
ULONG EMVCL_VisaSetCapability(EMVCL_SCHEME_DATA *stScheme, EMVCL_SCHEME_DATA *stRsp);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_VisaGetCapability
 * DESCRIPTION:   Get Visa Capability.
 * 
 * [in]stScheme.bNoS : number of scheme id to be checked
 * [in]stScheme.baID : scheme id to be checked
 * [out]stScheme.bNoS : response number of scheme id
 * [out]stScheme.baID : response scheme id
 * [out]stScheme.baActive : support or not
 * ========================================================================== */
ULONG EMVCL_VisaGetCapability(EMVCL_SCHEME_DATA *stScheme);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetDateTime
 * DESCRIPTION:   Set date and time.
 * 
 * [in]baDateTime : "YYYYMMDDHHMMSS" (14 Bytes)
 * ========================================================================== */
ULONG EMVCL_SetDateTime(BYTE *baDateTime);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetDateTime
 * DESCRIPTION:   Get date and time.
 * 
 * [out]baDateTime : "YYYYMMDDHHMMSS" (14 Bytes)
 * ========================================================================== */
ULONG EMVCL_GetDateTime(BYTE *baDateTime);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_StartTransactionEx
 * DESCRIPTION:   
 * 
 * Start an EMV contactless transaction with transaction related data. Transaction related data should be TLV format.(TLV1 + TLV2 + .. + TLVn). ex : 9F02060000000015009F0306000000000000.... .
 * 
 * Transaction Related Data includes : (M:Madatory, O:Option, X:Not Supported)
 * Tag 9F02   (Amount Authorized(Numeric)) : visa - M   , MC - M
 * Tag 9F03   (Amount Other(Numeric))      : visa - O   , MC - O	 
 * Tag 9C     (Transaction Type)           : visa - M   , MC - O
 * Tag 9F53   (Transaction Category Code)  : visa - X   , MC - O
 * Tag 5F2A   (Transaction Currency Code)  : visa - M   , MC - O
 * 
 * Special Transaction Related Data for PayPass 3.0 Balance Reading function : 
 * Tag DF8104 (Balance Read before Gen AC) : 
 * MC with no Balance Reading - Absent , MC with Balance Reading - O
 * Tag DF8105 (Balance Read after Gen AC)  : 
 * MC with no Balance Reading - Absent , MC with Balance Reading - O
 * 
 * Forced Transaction Online :
 *	Add Tag DF9F01 with value 01h (DF9F010101) to Transaction Related Data
 * 
 * Application Prefer Order : set the applications which are prefered to perform transaction.
 *	Add Tag DF9F02 with format below : (PerferAID(5-16B): Prefer AID , PerferAIDLen(1B) : Prefer AID Len)
 *  1st PerferAIDLen + 1st PerferAID + 2nd PerferAIDLen + 2nd PerferAID + ....+ nth PerferAIDLen + nth PerferAID 
 * 
 * [IN] stACTData : Point to a EMVCL_ACT_DATA structure to send transaction related data.
 * [OUT] stRCDataEx : Point to a EMVCL_RC_DATA_EX structure to get the transaction data.
 *
 * ========================================================================== */
ULONG EMVCL_StartTransactionEx(EMVCL_ACT_DATA *stACTData, EMVCL_RC_DATA_EX *stRCDataEx);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_InitTransactionEx
 * DESCRIPTION:   Start a EMV contactless L2 transaction with transaction related data.
 *				  Transaction related data should be TLV format.(TLV1 + TLV2 + .. + TLVn) 
 *				  ex : 9F02060000000015009F0306000000000000....
 *				  The transaction result should be retrieved by EMVCL_PerformTransactionEx function.
 *
 * Transaction Related Data include : (M:Madatory, O:Option, X:Not Supported)
 *	Tag 9F02   (Amount Authorized(Numeric)) 	: visa - M   , MC - M
 *	Tag 9F03   (Amount Other(Numeric))      	: visa - O   , MC - O	 
 *	Tag 9C     (Transaction Type)           	: visa - M   , MC - O
 *	Tag 9F53   (Transaction Category Code)  	: visa - X   , MC - O
 *	Tag 5F2A   (Transaction Currency Code)  	: visa - M   , MC - O
 *
 * Special Transaction Related Data for PayPass 3.0 : (Only for Balance Reading function) 
 *	Tag DF8104 (Balance Read before Gen AC) : MC with no Balance Reading - lack , MC with Balance Reading - O
 *	Tag DF8105 (Balance Read after Gen AC)  : MC with no Balance Reading - lack , MC with Balance Reading - O
 *
 * Forced Transaction Online :
 *	Add Tag Tag DF9F01 with value 01h (DF9F010101) to Transaction Related Data
 * 
 * Application Prefer Order : set the applications which are prefered to perform transaction.
 *	Add Tag DF9F02 with format below : (PerferAID(5-16B): Prefer AID , PerferAIDLen(1B) : Prefer AID Len)
 *  1st PerferAIDLen + 1st PerferAID + 2nd PerferAIDLen + 2nd PerferAID + ....+ nth PerferAIDLen + nth PerferAID 
 * 
 * [in]bTagNum : Number of transaction related data 
 * [in]baTransData : Transaction related data
 * [in]usTransDataLen : Transaction related data length
 * ========================================================================== */
ULONG EMVCL_InitTransactionEx(BYTE bTagNum, BYTE *baTransData, USHORT usTransDataLen);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_PerformTransactionEx
 * DESCRIPTION:   Perform transaction and Receive the result of transaction strated by EMVCL_InitTransactionEx. 
 * 
 * [out]stRCDataEx : transaction response data
 * ========================================================================== */
ULONG EMVCL_PerformTransactionEx(EMVCL_RC_DATA_EX *stRCDataEx);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_PollTransactionEx
 * DESCRIPTION:  This is a backward compatible function. It works as EMVCL_PerformTransactionEx. 
 *				 Receive the result of transaction strated by EMVCL_InitTransactionEx.
 * 
 * [out]stRCData : transaction response data
 * [in]ulMS : RFU
 * ========================================================================== */
ULONG EMVCL_PollTransactionEx(EMVCL_RC_DATA_EX *stRCDataEx,ULONG ulMS);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_CompleteEx
 * DESCRIPTION:   Perform Complete/Issuer Update Processing process.
 * 
 * [in]bAction : 
 * [in]baARC : authorization Response Code (2 Bytes)
 * [in]uiIADLen : length of IAD 
 * [in]baIAD : issuer authentication data
 * [in]uiScriptLen : length og script
 * [in]baScript : pointer for the emitter scripts
 * [out]stRCData : transaction response data
 * ========================================================================== */
ULONG EMVCL_CompleteEx(BYTE bAction, BYTE *baARC, UINT uiIADLen, BYTE *baIAD, UINT uiScriptLen, BYTE *baScript, EMVCL_RC_DATA_EX *stRCDataEx);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_DetectTransactionEx
 * DESCRIPTION:   This API is used to detect card application which is selected to perform transaction. 
 *				  When calling this API, emvcl kernel will request to present card.
 *				  After card detected, application information(Select PPSE Response, Selected AID
 *				  , Select AID Response) will be returned. In this phase, the transaction has not been finished yet.
 *                To continue transaction, please call EMVCL_InitTransactionEx + EMVCL_PerformTransactionEx.
 *				  To Stop transaction, please call EMVCL_CancelTransaction
 *				 
 * 
 * [INOUT] pstDetectTxnData : Point to EMVCL_DETECT_TXN_DATA structure.
 * ========================================================================== */         
ULONG EMVCL_DetectTransactionEx(EMVCL_DETECT_TXN_DATA *pstDetectTxnData);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_CancelTransaction
 * DESCRIPTION:   Abort the current transaction. 
    This API is used with EMVCL_InitTransactionEx and EMVCL_PerformTransactionEx.
    After send EMVCL_CancelTransaction API, please check return code 
    of EMVCL_PerformTransactionEx. If the return code is d_EMVCL_TX_CANCEL, 
    this transaction was canceled.
 * ========================================================================== */
ULONG EMVCL_CancelTransaction(void);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_AnalyzeTransactionEx
 * DESCRIPTION:   Analyze transaction Response Data EMV_RC_DATA_EX
 * 
 * [in]stRCDataEx : transaction response data
 * [out]stRCDataAnalyze : Point to struct EMVCL_RC_DATA_ANALYZ to show analyzed result.
 * ========================================================================== */
void EMVCL_AnalyzeTransactionEx(EMVCL_RC_DATA_EX *stRCDataEx, EMVCL_RC_DATA_ANALYZE *stRCDataAnalyze);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetCardData
 * DESCRIPTION:   This API is used to get requested card data.
 *				  When calling this API, emvcl kernel will request to present card.
 *				  After card detected, The below data will be returned.
 *					Selected AID				  
 *					Select PPSE Response
 *					Select AID Response
 *					Requested Card Data
 * 
 * [INOUT] pstGetCardData : Point to EMVCL_GET_CARD_DATA structure.
 * ========================================================================== */
ULONG EMVCL_GetCardData(EMVCL_GET_CARD_DATA *pstGetCardData);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetDebug
 * DESCRIPTION: Enable/Disable castles debug log. If enable, the debug log will be output by bDebugPort
 *			     For comport, the baudrate is 115200, N, 8, 1	 
 * 
 * [in]fDebufEnable : 
 			TRUE - Enable
 			FALSE - Disable
 * [in]bDebugPort : 
 			d_EMVCL_DEBUG_PORT_USB 
 			d_EMVCL_DEBUG_PORT_COM1 
			d_EMVCL_DEBUG_PORT_COM2 
			d_EMVCL_DEBUG_PORT_COM3
			d_EMVCL_DEBUG_PORT_SP_EVENT
 * ========================================================================== */ 
void EMVCL_SetDebug(BYTE fDebufEnable, BYTE bDebugPort);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SpecialEventRegister
 * DESCRIPTION: Register special event (callback) function to EMVCL library,
 *					provided by the user application using 
 * 
 * [in]bEventID : 
 * 			Identifier for registering special event
 * [in]pEventFunc : 
 *			Register event (callback) function
 * ========================================================================== */ 
ULONG EMVCL_SpecialEventRegister(BYTE bEventID, void *pEventFunc);


//------------------------------------------------------------------------------
// EMVCL Backward Compatible APIs
//------------------------------------------------------------------------------

/* ==========================================================================
 * FUNCTION NAME: EMVCL_Open
 * DESCRIPTION:   Initialize EMVCL
 * ========================================================================== */
ULONG EMVCL_Open(void);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_Close
 * DESCRIPTION:   Close EMVCL
 * ========================================================================== */
void EMVCL_Close(void);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetTimeOut
 * DESCRIPTION:   Set timeout
 * 
 * [in]ulMS : RFU
 * ========================================================================== */
ULONG EMVCL_SetTimeOut(ULONG ulMS);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_StartTransaction
 * DESCRIPTION:   Start a transaction.
 * 
 * [in]ulAmount : sale amount
 * [out]stRCData : transaction response data
 * ========================================================================== */
ULONG EMVCL_StartTransaction(ULONG ulAmount, EMVCL_RC_DATA *stRCData); 
 
/* ==========================================================================
 * FUNCTION NAME: EMVCL_Reset
 * DESCRIPTION:   Abort the current transaction.
 * ========================================================================== */
ULONG EMVCL_Reset(void);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_InitTransaction
 * DESCRIPTION:   Start a transaction. 
 *				  This API just start a transaction
 *				  To check transaction result,please use EMVCL_PollTransaction API
 * 
 * [in]ulAmount : sale amount
 * ========================================================================== */
ULONG EMVCL_InitTransaction(ULONG ulAmount);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_PollTransaction
 * DESCRIPTION:   Receive Init transaction response.
 * 
 * [out]stRCData : transaction response data
 * [in]ulMS : RFU
 * ========================================================================== */
ULONG EMVCL_PollTransaction(EMVCL_RC_DATA *stRCData, ULONG ulMS);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_Complete
 * DESCRIPTION:   Perform complete process.
 * 
 * [in]bAction : 
 * [in]baARC : authorization Response Code (2 Bytes)
 * [in]uiIADLen : length of IAD 
 * [in]baIAD : issuer authentication data
 * [in]uiScriptLen : length og script
 * [in]baScript : pointer for the emitter scripts
 * [out]stRCData : transaction response data
 * ========================================================================== */
ULONG EMVCL_Complete(BYTE bAction, BYTE *baARC, UINT uiIADLen, BYTE *baIAD, UINT uiScriptLen, BYTE *baScript, EMVCL_RC_DATA *stRCData);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_AnalyzeTransaction
 * DESCRIPTION:   Analyze transaction Response Data EMV_RC_DATA
 * 
 * [in]stRCData : transaction response data
 * [out]stRCDataAnalyze : analyzed result
 * ========================================================================== */
void EMVCL_AnalyzeTransaction(EMVCL_RC_DATA *stRCData, EMVCL_RC_DATA_ANALYZE *stRCDataAnalyze);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_SetEMVTags
 * DESCRIPTION:   Set Tag.
 * 
 * [in]baTagsData : tag data (format: tag number (1B) + TLV1 + TLV2 + TLV3 + ....)
 * [in]uiTagLen
 * [out]baRBuf : response tag data
 * [out]uiRLen
 * ========================================================================== */
ULONG EMVCL_SetEMVTags(BYTE *baTagsData, UINT uiTagLen, BYTE *baRBuf, UINT *uiRLen);
/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetEMVTags
 * DESCRIPTION:   Get Tag.
 * 
 * [in]bTagNo : number of getting tag
 * [in]baTagsData : tag data (format : Tag1 + Tag2)
 * [in]uiTagLen : length of baTagsData
 * [out]baRBuf : response data (format: tag number (1B) + TLV1 + TLV2 + TLV3 + ....)
 * [out]uiRLen : length of baRBuf
 * ========================================================================== */
ULONG EMVCL_GetEMVTags(BYTE bTagNo, BYTE *baTagsData, UINT uiTagLen,BYTE *baRBuf, UINT *uiRLen);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_DetectTransaction(ULONG ulAmount, BYTE *baAID, BYTE *bAIDLen, BYTE *baFCIData, ULONG *ulFCIDataLen, BYTE *baPSE_FCI, ULONG *ulPSE_FCILen)
 * DESCRIPTION:   Return the election AID and its FCI data after card detected. The transaction has not started yet.
 *                To continue the transaction, it still needs to use EMVCL_StartTransaction or EMVCL_InitTransaction.
 *			
 * [in]ulAmount : sale amount
 * [out]baAID 	: Election AID
 * [out]baFCIData : Point a buffer to get the responded FCI template data
 * [inout]ulFCIDataLen : 
 			in - receive buffer's size
 			out- receive data lens. 

 * [out]baPSE_FCI : Point a buffer to get the PSE responded FCI template data
 * [inout]ulPSE_FCILen : 
 			in - receive buffer's size
 			out- receive data lens.
 * ========================================================================== */
ULONG EMVCL_DetectTransaction(ULONG ulAmount, BYTE *baAID, BYTE *bAIDLen, BYTE *baFCIData, ULONG *ulFCIDataLen, BYTE *baPSE_FCI, ULONG *ulPSE_FCILen);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetKernelCoreVersion
 * DESCRIPTION:   Get kernel core version.
 *                The core version matches the fw/sw/kernel version indicated
 *                in LOA
 *			
 * [in]baKernelNameStr : kernel name
 *                 d_EMVCL_KERNEL_NAME_CLENTRY
 *                 d_EMVCL_KERNEL_NAME_MPP
 *                 d_EMVCL_KERNEL_NAME_VPW
 *                 d_EMVCL_KERNEL_NAME_AEP
 *                 d_EMVCL_KERNEL_NAME_JCT
 *                 d_EMVCL_KERNEL_NAME_VAP
 *                 d_EMVCL_KERNEL_NAME_CQP
 *                 d_EMVCL_KERNEL_NAME_IFH
 *                 d_EMVCL_KERNEL_NAME_DDP
 *                 d_EMVCL_KERNEL_NAME_MMS
 * [in]usVerStrBufSize : Buffer size
 * [out]baVerStrBuf    : Buffer to get version string
 * ========================================================================== */
ULONG EMVCL_GetKernelCoreVersion(BYTE *baKernelNameStr, BYTE* baVerStrBuf, USHORT usVerStrBufSize);

/* ==========================================================================
 * FUNCTION NAME: EMVCL_GetLastError
 * DESCRIPTION:   Get detail error code
 * ========================================================================== */
ULONG EMVCL_GetLastError(void);

ULONG EMVCL_SecureDataEncryptInfoSet(EMVCL_SECURE_DATA_INFO *pstSecureInfo);
ULONG EMVCL_SecureDataWhitelistSet(BYTE bAction, BYTE *pbListData, USHORT usListDataLen);
ULONG EMVCL_SecureDataMaskCharSet(IN BYTE bMaskChar);
ULONG EMVCL_SecureDataMaskSet(IN BYTE bType, IN BYTE *pbMaskBitsData, IN USHORT usMaskBitsDataLen);
#ifdef	__cplusplus
}
#endif

#endif	/* EMV_CL_H */

