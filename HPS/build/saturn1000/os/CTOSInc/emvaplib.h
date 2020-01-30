#ifndef EMVAPLIB_H
#define	EMVAPLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

//EMV Tag Definition
#define d_TAG_AID                                                       0x004F
#define d_TAG_TRACK2_EQUIVALENT_DATA                                    0x0057
#define d_TAG_PAN                                                       0x005A
#define d_TAG_AMOUNT_AUTHORIZED_B                                       0x0081
#define d_TAG_ARC                                                       0x008A
#define d_TAG_TVR                                                       0x0095
#define d_TAG_TRAN_DATE                                                 0x009A
#define d_TAG_TX_TYPE                                                   0x009C
#define d_TAG_TERM_CURRENCY_CODE                                        0x5F2A
#define d_TAG_TERM_CURRENCY_EXP                                         0x5F36
#define d_TAG_AMOUNT_AUTHORIZED                                         0x9F02
#define d_TAG_AMOUNT_OTHER                                              0x9F03
#define d_TAG_APP_VERSION                                               0x9F09
#define d_TAG_TERM_COUNTRY_CODE                                         0x9F1A
#define d_TAG_FLOOR_LIMIT                                               0x9F1B
#define d_TAG_IFD_SN                                                    0x9F1E
#define d_TAG_TRAN_TIME                                                 0x9F21
#define d_TAG_TERM_CAP                                                  0x9F33
#define d_TAG_TERM_TYPE                                                 0x9F35
#define d_TAG_UNPREDICATE_NUMBER                                        0x9F37
#define d_TAG_POS_ENTRY_MODE                                            0x9F39
#define d_TAG_ADD_TERM_CAP                                              0x9F40
#define d_TAG_TERM_SEQ_COUNTER                                          0x9F41

//Propriegary Tag Definition
#define d_TAG_ISSUER_SCRIPT_RESULT                                      0x9F5B
#define d_TAG_DEFAULT_TDOL                                              0xDFC0
#define d_TAG_DEFAULT_DDOL                                              0xDFC1
#define d_TAG_TARGET_PERCENT                                            0xDFC2
#define d_TAG_MAX_TARGET_PERCENT                                        0xDFC3
#define d_TAG_THRESHHOLD_VALUE                                          0xDFC4
#define d_TAG_TAC_DENIAL                                                0xDFC6
#define d_TAG_TAC_ONLINE                                                0xDFC7
#define d_TAG_TAC_DEFAULT                                               0xDFC8

#define d_LABEL_STR_SIZE                                                16
#define d_AID_SIZE                                                      16

#define d_TXN_RESULT_APPROVAL                                           0x01
#define d_TXN_RESULT_DECLINE                                            0x02
#define d_TXN_RESULT_GO_ONLINE                                          0x03

//Online result
#define d_ONLINE_ACTION_APPROVAL                                        1
#define d_ONLINE_ACTION_DECLINE                                         2
#define d_ONLINE_ACTION_UNABLE                                          3
#define d_ONLINE_ACTION_ISSUER_REFERRAL_APPR                            4
#define d_ONLINE_ACTION_ISSUER_REFERRAL_DENY                            5

//PIN Verify Result
#define d_PIN_RESULT_OK                                                 1
#define d_PIN_RESULT_FAIL                                               2
#define d_PIN_RESULT_BLOCKED                                            3
#define d_PIN_RESULT_FAILBLOCKED                                        4

// bPINType
#define d_NOTIFY_ONLINE_PIN                                             0
#define d_NOTIFY_OFFLINE_PIN                                            1

//CDA Mode
#define d_CDA_MODE_1                                                    0x01
#define d_CDA_MODE_3                                                    0x03

//pbAIDNoMatchOption for MCCS Dynamic Terminal Supported AID
#define d_MCCS_DYNAMIC_AID_OPTION_USE_OTHER_AID	                        0x00
#define d_MCCS_DYNAMIC_AID_OPTION_TERMINATED	                        0x01

//for EMV_TerminalTagsSet()
#define d_TERM_TAG_ACTION_ADD	                	                    0x00
#define d_TERM_TAG_ACTION_DELETE	                	                0x01
#define d_TERM_TAG_ACTION_DELETE_ALL	                   	            0x02

//for EMV_AppTagsSet()
#define d_APP_TAG_ACTION_ADD               	                            0x00
#define d_APP_TAG_ACTION_DELETE                	                        0x01
#define d_APP_TAG_ACTION_DELETE_ALL               	                    0x02

//for EMV_CAPKSet()
#define d_CAPK_ACTION_ADD               	                            0x00
#define d_CAPK_ACTION_DELETE               	                            0x01
#define d_CAPK_ACTION_DELETE_ALL                                        0x02
	
//for EMV_SecureDataWhiteListSet()
#define d_WHITE_LIST_ACTION_NEW            	                            0x00
#define d_WHITE_LIST_ACTION_DELETE_ALL                                  0x01

//return code from pp function    
#define d_EMVAPLIB_OK                                                   0x0000
#define d_EMVAPLIB_ERR_DATA_BUFFER_EXCEEDED                             0x0001
#define d_EMVAPLIB_ERR_INVALID_PARA                                     0x0002
#define d_EMVAPLIB_ERR_CRITICAL_MISTAKES                                0x0003
#define d_EMVAPLIB_ERR_SELECTION_FAIL                                   0x0004
#define d_EMVAPLIB_ERR_PIN_BY_PASS                                      0x0005
#define d_EMVAPLIB_ERR_TERM_DATA_MISSING                                0x0006
#define d_EMVAPLIB_ERR_DATA_NOT_FOUND                                   0x0007
#define d_EMVAPLIB_ERR_GAC1_6985_FALLBACK                               0x0008  //TIP
#define d_EMVAPLIB_ERR_ONLY_1_AP_NO_FALLBACK                            0x0009  //JIT
#define d_EMVAPLIB_ERR_IAP_9481_FALLBACK                                0x000A  //ADVT
#define d_EMVAPLIB_ERR_FORCE_ACCEPTANCE                                 0x000B
#define d_EMVAPLIB_ERR_FUNCTION_NOT_SUPPORTED                           0x000C  //return 6A81
#define d_EMVAPLIB_ERR_ENTER_KMS_OFFLINEPIN                             0x000D
#define d_EMVAPLIB_ERR_TXN_PROCESSING                                   0x000E
#define d_EMVAPLIB_ERR_TXN_NOT_PROCESSING                               0x000F
#define d_EMVAPLIB_ERR_SECURE_DATA                                      0x0010
	

#define d_EMVAPLIB_ERR_ON_APP_CONFIRM_FAIL								0x0031
#define d_EMVAPLIB_ERR_ON_TXN_DATA_GET_FAIL								0x0032
#define d_EMVAPLIB_ERR_ON_APP_LIST_FAIL									0x0033

#define d_MEVAPLIB_ERR_LASTERR_MISSING_TERMINAL_DATA                    0x0075
#define d_EMVAPLIB_ERR_MCCS_SELECT_PREFER_AID_FAIL                      0x007A

 //return code from app define 
#define d_EMVAPLIB_ERR_AMOUNT_FORMAT_ERROR                              0x0101
#define d_EMVAPLIB_ERR_NO_AP_FOUND                                      0x0102
#define d_EMVAPLIB_ERR_ABORT_TX                                         0x0103
#define d_EMVAPLIB_ERR_USER_ABORT                                       0x0104
#define d_EMVAPLIB_ERR_CARD_PROBLEM                                     0x0105
#define d_EMVAPLIB_ERR_SETTING_ERROR                                    0x0106
#define d_EMVAPLIB_ERR_NO_CAPK                                          0x0107
#define d_EMVAPLIB_ERR_NO_DATA                                          0x0108
#define d_EMVAPLIB_ERR_DATA_BUFFER_ERROR                                0x0109
#define d_EMVAPLIB_ERR_DATA_BUF_OVERFLOW                                0x010A
#define d_EMVAPLIB_ERR_SET_CDA_MODE_BUFFER_ERROR                        0x010B
#define d_EMVAPLIB_ERR_SET_CDA_MODE_FAIL                                0x010C
#define d_EMVAPLIB_ERR_SUBSEQUENT_PIN_BYPASS_BUFFER_ERROR               0x010D
#define d_EMVAPLIB_ERR_SUBSEQUENT_PIN_BYPASS_FAIL                       0x010E
#define d_EMVAPLIB_ERR_OFFLINE_PIN_TIMEOUT                              0x010F
#define d_EMVAPLIB_ERR_OFFLINE_PIN_CANCEL                               0x0110

//return conde from config setting
#define d_EMVAPLIB_ERR_CONFIG_READ_FAIL                                 0x0201
#define d_EMVAPLIB_ERR_NO_ACTIVE_INDEX                                  0x0202
#define d_EMVAPLIB_ERR_SET_TERMINAL_DATA_ERROR                          0x0203
#define d_EMVAPLIB_ERR_SET_APPLIST_ERROR                                0x0204
#define d_EMVAPLIB_ERR_SET_APP_TAG_DATA_ERROR                           0x0205
#define d_EMVAPLIB_ERR_SET_CAPK_ERROR                                   0x0206
#define d_EMVAPLIB_ERR_SET_DEFAULT_DATA_ERROR                           0x0207
#define d_EMVAPLIB_ERR_DLL_LOAD_ERROR                                   0x0208
#define d_EMVAPLIB_ERR_CAPK_HASH_ERROR                                  0x0209
#define d_EMVAPLIB_ERR_INVALID_INPUT_PARA                               0x020A
#define d_EMVAPLIB_ERR_SET_PARAMETER_ERROR                              0x020B	
#define d_EMVAPLIB_ERR_GET_PARAMETER_ERROR                              0x020C
#define d_EMVAPLIB_ERR_STORAGE_ERROR                                    0x020D	
#define d_EMVAPLIB_ERR_SET_SECURE_DATA_WHITELIST_ERROR                  0x020E

#define d_EMVAPLIB_ERR_SET_CALLER_ERROR                                 0x0251	
#define d_EMVAPLIB_ERR_GET_CALLER_ERROR                                 0x0252	
	
#define d_EMVAPLIB_ERR_EVENT_CONFIRMED                                  0x0301
#define d_EMVAPLIB_ERR_EVENT_SELECTED                                   0x0302
#define d_EMVAPLIB_ERR_EVENT_GET_TXNDATA                                0x0303
#define d_EMVAPLIB_ERR_EVENT_ONLINE                                     0x0304
#define d_EMVAPLIB_ERR_EVENT_ISSUER_RESULT                              0x0305
#define d_EMVAPLIB_ERR_EVENT_TXN_RESULT                                 0x0306
#define d_EMVAPLIB_ERR_EVENT_BUFFER_DEFINE                              0x0307
#define d_EMVAPLIB_ERR_EVENT_VERSION                                    0x0308
#define d_EMVAPLIB_ERR_EVENT_GETPIN_NOTIFY                              0x0309
#define d_EMVAPLIB_ERR_EVENT_SETAPPDATA                                 0x030A
#define d_EMVAPLIB_ERR_EVENT_NON_EMV_CARD                               0x030B
#define d_EMVAPLIB_ERR_EVENT_APP_LIST_EX                                0x030C
#define d_EMVAPLIB_ERR_EVENT_SHOW_PIN_DIGIT                             0x030D
#define d_EMVAPLIB_ERR_EVENT_SHOW_PIN_BYPASS                            0x030E
	
	
#define d_EMVAPLIB_LASTERR_                                             0x1000

#define d_EMVAPLIB_ERR_CANDIDATE_LIST_FULL                              0x1002
#define d_EMVAPLIB_ERR_SEND_APDU_CMD_FAIL                               0x1004
#define d_EMVAPLIB_ERR_APPLICATION_NOT_ALLOW                            0x1007
#define d_EMVAPLIB_ERR_IAP_AIP_AFL_ERROR                                0x1008
#define d_EMVAPLIB_ERR_IAP_UNKNOW_SW12                                  0x1009
#define d_EMVAPLIB_ERR_IAP_MISS_PAN                                     0x100A
#define d_EMVAPLIB_ERR_IAP_MISS_CDOL1                                   0x100B
#define d_EMVAPLIB_ERR_IAP_MISS_CDOL2                                   0x100C
#define d_EMVAPLIB_ERR_IAP_APPLICATION_NOT_ALLOW                        0x100D
#define d_EMVAPLIB_ERR_ERROR_9F4A_RULE                                  0x100E
#define d_EMVAPLIB_ERR_KEY_NO_FOUND                                     0x1010
#define d_EMVAPLIB_ERR_PAN_NOT_SAME                                     0x1011
#define d_EMVAPLIB_ERR_DDOL_MISS                                        0x1012
#define d_EMVAPLIB_ERR_INTERNAL_AUTHENTICATE_FAIL                       0x1013
#define d_EMVAPLIB_ERR_SDA_LENGTH_NOT_MATCH                             0x1014
#define d_EMVAPLIB_ERR_NO_OFFLINE_DATA_AUTH_MATCH                       0x1015
#define d_EMVAPLIB_ERR_READ_DATA_TAG_NOT_70                             0x1016
#define d_EMVAPLIB_ERR_MISS_APP_EXPIRATION_DATE                         0x1017
#define d_EMVAPLIB_ERR_CARDHOLDER_VER_NOT_SUPP                          0x1018
#define d_EMVAPLIB_ERR_ERR_CVM_LIST_MISSING                             0x1019
#define d_EMVAPLIB_ERR_CVM_FINISH                                       0x101A
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT                                   0x101B
#define d_EMVAPLIB_ERR_CVM_ENC_PIN_ONLINE                               0x101C
#define d_EMVAPLIB_ERR_CVM_PLAINTEXT_PIN_SIGNATURE                      0x101D
#define d_EMVAPLIB_ERR_CVM_ENC_PIN                                      0x101E
#define d_EMVAPLIB_ERR_CVM_ENC_PIN_SIGNATURE_OFFLINE                    0x101F
#define d_EMVAPLIB_ERR_CVM_SIGNATURE                                    0x1020
#define d_EMVAPLIB_ERR_GET_DATA_CMD_ERROR                               0x1021
#define d_EMVAPLIB_ERR_CAS_GAC1_FAIL                                    0x1023
#define d_EMVAPLIB_ERR_CAS_MERCHANT_FORCE_APPROVED                      0x1024
#define d_EMVAPLIB_ERR_CVM_IDCARD_VERIFY                                0x1025   //PBOC
#define d_EMVAPLIB_ERR_ONLY_1_AP_BLOCKED                                0x1026   //JIT FT-18
#define d_EMVAPLIB_ERR_CAS_NEED_ONLINE                                  0x1027
#define d_EMVAPLIB_ERR_CAS_CARD_INIT_REFERRAL                           0x1028
#define d_EMVAPLIB_ERR_CAS_DDAAC_FAIL                                   0x1029
#define d_EMVAPLIB_ERR_CAS_ONLINE_FAIL_APPROVAL                         0x102A
#define d_EMVAPLIB_ERR_CAS_GAC1_WRONG_CID                               0x102C
#define d_EMVAPLIB_ERR_CAS_GAC2_FAIL                                    0x102E

#define d_EMVAPLIB_ERR_DIS_ADF_UNKNOW_TAG                               0x102F
#define d_EMVAPLIB_ERR_DIS_ADF_DATA_MISSING                             0x1030
#define d_EMVAPLIB_ERR_DIS_FCI_DATA_ERROR                               0x1031
#define d_EMVAPLIB_ERR_DIS_TLV_TAG_ZERO                                 0x1032
#define d_EMVAPLIB_ERR_DIS_TLV_FAIL                                     0x1033
#define d_EMVAPLIB_ERR_DIS_TLV_EXCEED_MAX_LEN                           0x1035
#define d_EMVAPLIB_ERR_CAS_GAC1_FORMAT_1_PADDING                        0x1036
#define d_EMVAPLIB_ERR_MISS_AIP_AFL                                     0x1037
#define d_EMVAPLIB_ERR_DDOL_NOT_HAVE_9F37                               0x1038
#define d_EMVAPLIB_ERR_SDA_DATA_ERROR                                   0x103C
#define d_EMVAPLIB_ERR_SDA_ALGORITHM_NOT_SUPPORT                        0x103D
#define d_EMVAPLIB_ERR_DDA_DATA_ERROR                                   0x103E
#define d_EMVAPLIB_ERR_DDA_ALGORITHM_NOT_SUPPORT                        0x103F
#define d_EMVAPLIB_ERR_DDAAC_DATA_ERROR                                 0x1040
#define d_EMVAPLIB_ERR_DDAAC_ALGORITHM_NOT_SUPPORT                      0x1041
#define d_EMVAPLIB_ERR_KEY_LENGTH_ERROR                                 0x1043
#define d_EMVAPLIB_ERR_DDAAC_HASH1_ERROR                                0x1045
#define d_EMVAPLIB_ERR_DDAAC_HASH2_ERROR                                0x1046
#define d_EMVAPLIB_ERR_ISSUER_CERT_NOT_EXIST                            0x1047
#define d_EMVAPLIB_ERR_ISSUER_CERT_FORMAT_ERROR                         0x1048
#define d_EMVAPLIB_ERR_ISSUER_CERT_IIN_PAN_NOT_SAME                     0x1049
#define d_EMVAPLIB_ERR_ISSUER_CERT_REVOCATION_FOUND                     0x104A
#define d_EMVAPLIB_ERR_ISSUER_CERT_ALGORITHM_NOT_SUPPORT                0x104B
#define d_EMVAPLIB_ERR_ISSUER_CERT_LENGTH_ERROR                         0x104C
#define d_EMVAPLIB_ERR_ISSUER_CERT_EXPIRATION_DATE                      0x104D
#define d_EMVAPLIB_ERR_ISSUER_CERT_HASH_NOT_MATCH                       0x104E
#define d_EMVAPLIB_ERR_ISSUER_CERT_EXPONENT_NOT_EXIST                   0x104F
#define d_EMVAPLIB_ERR_ISSUER_CERT_REMAINDER_MISSING                    0x1050
#define d_EMVAPLIB_ERR_ICC_CERT_NOT_EXIST                               0x1051
#define d_EMVAPLIB_ERR_ICC_CERT_FORMAT_ERROR                            0x1052
#define d_EMVAPLIB_ERR_ICC_CERT_ALGORITHM_NOT_SUPPORT                   0x1053
#define d_EMVAPLIB_ERR_ICC_CERT_LENGTH_ERROR                            0x1054
#define d_EMVAPLIB_ERR_ICC_CERT_HASH_NOT_MATCH                          0x1055
#define d_EMVAPLIB_ERR_ICC_CERT_EXPIRATION_DATE                         0x1056
#define d_EMVAPLIB_ERR_ICC_CERT_EXPONENT_NOT_EXIST                      0x1057
#define d_EMVAPLIB_ERR_ICC_ISSUER_PK_NOT_EXIST                          0x1058
#define d_EMVAPLIB_ERR_ICC_CERT_REMAINDER_MISSING                       0x1059

#define d_EMVAPLIB_ERR_PIN_CERT_NOT_EXIST                               0x105A
#define d_EMVAPLIB_ERR_PIN_CERT_LENGTH_ERROR                            0x105B
#define d_EMVAPLIB_ERR_PIN_CERT_FORMAT_ERROR                            0x105C
#define d_EMVAPLIB_ERR_PIN_CERT_ALGORITHM_NOT_SUPPORT                   0x105D
#define d_EMVAPLIB_ERR_PIN_CERT_HASH_NOT_MATCH                          0x105E
#define d_EMVAPLIB_ERR_PIN_CERT_EXPIRATION_DATE                         0x105F
#define d_EMVAPLIB_ERR_PIN_CERT_KEY_LENGTH_ERROR                        0x1060
#define d_EMVAPLIB_ERR_READ_PIN_TRY_COUNT_FAIL                          0x1061
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_PIN_NOT_KEYIN                     0x1062
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_PIN_TRY_LIMIT_EXCEEDED            0x1063
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_PIN_OK                            0x1064
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_PIN_WRONG                         0x1066
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_UNKNOW_SW12                       0x1067
#define d_EMVAPLIB_ERR_CVM_TERMINAL_NOT_SUPPORT_SPECIFY_CVM             0x1068  //No pin pad or terminal cap indicates not support!
#define d_EMVAPLIB_ERR_CVM_ENC_PIN_ONLINE_PIN_NOT_KEYIN                 0x1069
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_NOT_KEYIN                     0x106C
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_TRY_LIMIT_EXCEEDED            0x106D
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_UNKNOW_SW12                   0x106E
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_OK                            0x106F
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_WORNG                         0x1070
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_GET_RN_UNKNOW_SW12            0x1071
#define d_EMVAPLIB_ERR_DIS_GAC_UNKNOW_TAG                               0x1072
#define d_EMVAPLIB_ERR_MISSING_TERMINAL_DATA                            0x1075
#define d_EMVAPLIB_ERR_CARD_DATA_MULTIPLE                               0x1076
#define d_EMVAPLIB_ERR_CARD_DATA_BUF_OVERFLOW                           0x1077
#define d_EMVAPLIB_ERR_CAS_ONLINE_FAIL_DECLINED                         0x1078
#define d_EMVAPLIB_ERR_ISSUER_CERT_CAPKI_NOT_EXIST                      0x1079

#define d_EMVAPLIB_ERR_CAS_DDAAC_FAIL_NEED_GAC2                         0x107B
#define d_EMVAPLIB_ERR_SFI_TEMPLATE_RULE_WRONG                          0x107C
#define d_EMVAPLIB_ERR_CVM_PLAIN_TEXT_PIN_BLOCK                         0x107D
#define d_EMVAPLIB_ERR_CVM_ENCIPHERED_PIN_BLOCK                         0x107E
#define d_EMVAPLIB_ERR_CAS_GAC1_6985_FALLBACK                           0x107F   //TIP request
#define d_EMVAPLIB_ERR_IAP_BAD_PADDING                                  0x1081   //Paypass MChip test------------------
#define d_EMVAPLIB_ERR_DIS_GAC_ATC_LEN_WRONG                            0x1083
#define d_EMVAPLIB_ERR_DIS_GAC_DATA_MISSING                             0x1084
#define d_EMVAPLIB_ERR_PIN_CERT_EXP_NOT_EXIST                           0x1085
#define d_EMVAPLIB_ERR_DIS_GAC_AC_LEN_WRONG                             0x1086
#define d_EMVAPLIB_ERR_DIS_GAC_CID_LEN_WRONG                            0x1087
#define d_EMVAPLIB_ERR_IAP_FALLBACK_9481                                0x1088   //ADVT-------------------------------
#define d_EMVAPLIB_ERR_CAS_OFFLINE_APPROVED                             0x10C8
#define d_EMVAPLIB_ERR_CAS_OFFLINE_DECLINED                             0x10C9
#define d_EMVAPLIB_ERR_CAS_ONLINE_APPROVED                              0x10CA
#define d_EMVAPLIB_ERR_CAS_ONLINE_DECLINED                              0x10CB
#define d_EMVAPLIB_ERR_CAS_ONLINE_DECLINED_REVERSAL                     0x10CC
#define d_EMVAPLIB_ERR_CAS_REFERRAL_APPROVED                            0x10CD
#define d_EMVAPLIB_ERR_CAS_REFERRAL_DECLINED                            0x10CE
#define d_EMVAPLIB_ERR_UP_APPLICATION_UNKNOW_SW12                       0x1228

#define d_EMVAPLIB_ERR_CRITICAL_ERROR                                   0x7FFF

#define d_EN_MSG_PROCESSING		"Processing..."
#define d_EN_MSG_SELECT_FAIL	"Select Fail\nPlz Try Again"

typedef struct
{
	// Should be 0x01;
	BYTE Version;
	
	ULONG ulAmount;
	
	BYTE bPOSEntryMode;  //default 0x00
	
	BYTE bTxnType;
	
	// "YYMMDD"
	BYTE TxnDate[6];
	
	// "HHMMSS"
	BYTE TxnTime[6];
	
	BYTE isForceOnline;
	
}EMV_TXNDATA;

typedef struct
{
	// Should be 0x02;
	BYTE Version;
	
	ULONG ulAmount;
	
	BYTE bPOSEntryMode;  //default 0x00
	
	BYTE bTxnType;
	
	// "YYMMDD"
	BYTE TxnDate[6];
	
	// "HHMMSS"
	BYTE TxnTime[6];
	
	BYTE isForceOnline;
	
	ULONG ulAmountOther;
	
}EMV_TXNDATA_2;

typedef struct
{
	// Should be 0x03;
	BYTE Version;
	
	ULONG ulAmount;			//ignored after version 3			
	
	BYTE bPOSEntryMode;  	//default 0x00
	
	BYTE bTxnType;
	
	// "YYMMDD"
	BYTE TxnDate[6];
	
	// "HHMMSS"
	BYTE TxnTime[6];
	
	BYTE isForceOnline;
	
	ULONG ulAmountOther;	//ignored after version 3
	
	BYTE baAmount[6];		//BCD format
	BYTE baAmountOther[6];	//BCD format
	
}EMV_TXNDATA_3;

typedef struct
{
	BYTE bAction; 
	
	// ARC
	BYTE *pAuthorizationCode; 
	
	BYTE *pIssuerAuthenticationData;
	USHORT IssuerAuthenticationDataLen; 
	
	BYTE *pIssuerScript; 
	USHORT IssuerScriptLen; 
			
}EMV_ONLINE_RESPONSE_DATA;


typedef struct
{
	BOOL isOnlinePINRequired;
	
	BYTE *pPIN;
	
	BYTE bPINLen;
	
}ONLINE_PIN_DATA;

typedef struct
{
	// Should be 0x00 or 0x01
	BYTE Version;
			
	USHORT usLineLeft_X;
	USHORT usLineRight_X;
	USHORT usLinePosition_Y;
	ULONG ulTimeout;
	BYTE bPINDigitMaxLength;
	BYTE bPINDigitMinLength;	
	BOOL IsRightToLeft;
	BOOL IsReverseLine;	
	
	//This is used for entering online pin (bPINType = d_NOTIFY_ONLINE_PIN).
	struct
	{
		USHORT CipherKeySet;
		USHORT CipherKeyIndex;
		BYTE bPANLen;
		BYTE baPAN[32];
	}ONLINEPIN_PARA;
}DEFAULT_GETPIN_FUNC_PARA;

typedef struct {
	BYTE *pbAID;
	BYTE bAIDLen;
	BYTE bApplication_Selection_Indicator;
} EMV_APPLICATION_PARA;

typedef struct
{
    BYTE    bVersion;           // Should be 0x01;
    BYTE    bAction;
    BYTE    bIndex;
    UINT    uiModulusLen;
    BYTE    baModulus[248];
    UINT    uiExponentLen;      // Length of Extension
    BYTE    baExponent[3];      // Extension
    BYTE    baHash[20];         // Key Hash (SHA-1) Result
}EMV_CA_PUBLIC_KEY;


typedef struct
{
	BYTE Version;
	
	void (*OnDisplayShow)(IN char *pStrMsg);  //add '\n' break line
	
	void (*OnErrorMsg)(IN char *pStrMsg);
	
	// This function will be called during EMV_Initialize;
	void (*OnEMVConfigActive)(INOUT BYTE* pActiveIndex);
	
	//default hash calculate = SHA-1(RID(5)+CAPKI(1)+MoudlesLen(1)+Modules(var)+ExponentLen(1)+Exponent(var), length is 20bytes
	//This function will be called, if hash verify error for default calculate.
	BOOL (*OnHashVerify)(IN BYTE *pRID, IN BYTE bKeyIndex, IN BYTE *pModulus, IN USHORT ModulusLen, IN BYTE *pExponent, IN USHORT ExponentLen, IN BYTE *pHash, IN USHORT HashLen);
	
	USHORT (*OnTxnDataGet)(OUT EMV_TXNDATA *pTxnData);
	
	// Range of pAppSelectedIndex value is 0 to (AppNum-1)    
	USHORT (*OnAppList)(IN BYTE AppNum, IN char AppLabel[][d_LABEL_STR_SIZE+1], OUT BYTE *pAppSelectedIndex);
	
	// Return d_OK to indicate CONFIRMED        
	USHORT (*OnAppSelectedConfirm)(IN BOOL IsRequiredbyCard, IN BYTE *pLabel, IN BYTE bLabelLen);
	
	BOOL (*OnTerminalDataGet)(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
	
	BOOL (*OnCAPKGet)(IN BYTE *pRID, IN BYTE bKeyIndex, OUT BYTE *pModulus, OUT USHORT *pModulusLen, OUT BYTE *pExponent, OUT USHORT *pExponentLen);
	
	//if bPINType is d_NOTIFY_OFFLINE_PIN, bRemainingCounter is effective.
	//if pIsInternalPINPad is TRUE, DEFAULT_GETPIN_FUNC_PARA can't be NULL.
	void (*OnGetPINNotify)(IN BYTE bPINType, IN USHORT bRemainingCounter, OUT BOOL* pIsInternalPINPad, OUT DEFAULT_GETPIN_FUNC_PARA *pGetPINFuncPara);
	
	// Return d_OK to indicate Online PIN block is ready for application
	USHORT (*OnOnlinePINBlockGet)(OUT ONLINE_PIN_DATA *pOnlinePINData);
	
	// Return d_OK to indicate Offline PIN block is ready for Kernel
	// If this function uses KMS_GetEncOfflinePIN function to get offline pin, return d_EMV_ENTER_KMS_OFFLINEPIN to indicate enciphed offline PIN is ready for Kernel    
	//USHORT (*OnOfflinePINBlockGet)(IN BYTE bRemainingCounter, OUT BYTE *pszPIN);
	USHORT (*OnOfflinePINBlockGet)(void);
	
	void (*OnOfflinePINVerifyResult)(IN USHORT usResult);
	
	void (*OnTxnOnline)(IN ONLINE_PIN_DATA *pOnlinePINData, OUT EMV_ONLINE_RESPONSE_DATA* pOnlineResponseData);
	
	void (*OnTxnIssuerScriptResult)(IN BYTE* pScriptResult, IN USHORT pScriptResultLen);
	
	void (*OnTxnResult)(IN BYTE bResult, IN BOOL IsSignatureRequired);
	
	void (*OnTotalAmountGet)(IN BYTE *pPAN, IN BYTE bPANLen, OUT ULONG *pAmount);
	
	void (*OnExceptionFileCheck)(IN BYTE *pPAN, IN BYTE bPANLen, OUT BOOL *isException);       
	
	BOOL (*OnCAPKRevocationCheck)(IN BYTE *pbRID, IN BYTE bCAPKIdx, IN BYTE *pbSerialNumuber);    
	
}EMV_EVENT;

typedef struct
{
	IN BYTE bVersion;		// Should be 0x01;
	
	IN char cAppLabel[d_LABEL_STR_SIZE+1];
	IN BYTE baAIDLen;
	IN BYTE baAID[d_AID_SIZE+1];
	
}EMV_APP_LIST_EX_DATA;

typedef struct
{
	IN	BYTE bVersion;		// Should be 0x01;
	
	OUT BYTE bAppLabelLen;
	OUT char cAppLabel[33];
	OUT BYTE bAIDLen;
	OUT BYTE baAID[d_AID_SIZE+1];
	
}EMV_CANDIDATE_DATA;

typedef struct
{
	IN	BYTE bVersion;		// Should be 0x02;
	
	IN BYTE bKeyType;
	IN USHORT usCipherKeySet;
	IN USHORT usCipherKeyIndex;
	IN BYTE bCipherMethod;
	IN BYTE *pbICV;
	IN BYTE bICVLen;
	IN BYTE bChecksumType;
	
	IN BYTE bPaddingMethod;	
	IN BYTE bLRCIncluded;
	IN BYTE bSS_ESIncluded;		
	
	IN BYTE bIsKSNFixed; 
	IN BYTE bMSREncFlow;	// Should be 0x01 now;
	
}EMV_SECURE_DATA_INFO;

typedef struct 
{
	OUT BYTE bVersion; // Should be 0x01;

	OUT USHORT usEncryptedTrack1Len;
	OUT BYTE *pbEncryptedTrack1;
	OUT USHORT usHashTrack1Len;
	OUT BYTE *pHashTrack1; //Add KSN 

	OUT USHORT usEncryptedTrack2Len;
	OUT BYTE *pbEncryptedTrack2;
	OUT USHORT usHashTrack2Len;
	OUT BYTE *pHashTrack2; //Add KSN 

	OUT USHORT usEncryptedTrack3Len;
	OUT BYTE *pbEncryptedTrack3;
	OUT USHORT usHashTrack3Len;
	OUT BYTE *pHashTrack3; //Add KSN 

	OUT USHORT usTrack1KSNLen;
	OUT BYTE *pTrack1KSN;
	OUT USHORT usTrack2KSNLen;
	OUT BYTE *pTrack2KSN;
	OUT USHORT usTrack3KSNLen;
	OUT BYTE *pTrack3KSN;

}MSR_ENCRYPTED_TRACK_DATA;


typedef struct
{
	OUT	BYTE bVersion;		// Should be 0x01;

	OUT USHORT	usMaskTrack1Len;
	OUT BYTE	*pbMaskTrack1;
	
	OUT USHORT	usMaskTrack2Len;
	OUT BYTE	*pbMaskTrack2;
	
	OUT USHORT	usMaskTrack3Len;
	OUT BYTE	*pbMaskTrack3;
}
MSR_MASK_TRACK_DATA;	

typedef struct
{
	OUT	BYTE bVersion;		// Should be 0x01;

	OUT USHORT	usEncryptedDataLen;
	OUT BYTE	*pbEncryptedData;
	OUT USHORT	usHashLen;
	OUT BYTE	*pHash;

	OUT USHORT	usKSNLen;
	OUT BYTE	*pKSN;
	
}MANUALENTRY_ENCRYPTED_DATA;	

#define d_MANUAL_ENTRY_EVENTID_ON_SHOW_PAN	0
#define d_MANUAL_ENTRY_EVENTID_ON_SHOW_EXP	1
#define d_MANUAL_ENTRY_EVENTID_ON_SHOW_CSC	2
#define d_MANUAL_ENTRY_EVENTID_ON_SHOW_ADR	3
#define d_MANUAL_ENTRY_EVENTID_ON_SHOW_ZIP	4

//--------------------------------------------------------------------------------------------------------------------------

USHORT EMV_Initialize(IN EMV_EVENT *pEvent, IN char* pEMVConfigFileName);

USHORT EMV_TxnAppSelect(OUT BYTE* pSelectedAID, OUT BYTE* pSelectedAIDLen, OUT BYTE *pSelectedAppLabel, OUT BYTE *pSelectedAppLabelLen);

USHORT EMV_TxnAppSpecificSelect(IN BYTE* pSpecificAppLabel, IN BYTE bSpecificAppLabelLen);

USHORT EMV_TxnPerform(void);

USHORT EMV_TxnCompletion(IN EMV_ONLINE_RESPONSE_DATA* pOnlineResponseData);

USHORT EMV_DataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
USHORT EMV_DataGetEx(IN BYTE *baTagList, IN USHORT usTagListLen,
				OUT BYTE *baTLVBuf, OUT USHORT *pusTLVLen, IN USHORT usMaxTLVBufSize);

//set/get from terminal buffer
USHORT EMV_TerminalDataSet(IN BYTE* pAID, IN BYTE bAIDLen, IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue);
USHORT EMV_TerminalDataGet(IN BYTE* pAID, IN BYTE bAIDLen, IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
USHORT EMV_TerminalDataDelete(IN BYTE* pAID, IN BYTE bAIDLen, IN USHORT usTag);

//set during doing txn
USHORT EMV_TxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue);
//Pre setting before txn, the tags will be add to kernel before GPO
USHORT EMV_TxnPreDataSet(IN BYTE *baTLVBuf, IN USHORT usTLVLen);

/******************************************************
 * OnSetAppData is callback function, 
 * when return FALSE, express the last AID data, 
 * if return TRUE, express have the next AID data.
 ******************************************************/
USHORT EMV_ApplicationListSet(BOOL (*OnSetAppData)(OUT EMV_APPLICATION_PARA *pAppData));

//Set CDA Mode
USHORT EMV_CDAModeSet(BYTE bMode);

USHORT EMV_TxnKernelVersionGet(BYTE *baVerStr);

//Set Subsequent PIN Bypass
USHORT EMV_SubsequentPINBypassSupport(BOOL isSupported);

void EMV_SetDebug(BYTE fDebufEnable, BYTE bDebugPort);

USHORT EMV_ApplicationSet(IN BYTE bAppNum, IN EMV_APPLICATION_PARA *pAppData);
USHORT EMV_CAPKSet(IN BYTE *pRID, IN EMV_CA_PUBLIC_KEY *stCAPubKey);
USHORT EMV_TerminalTagsSet(IN BYTE bAction, IN BYTE *pTagsData, USHORT usTagsDataLen);
USHORT EMV_AppTagsSet(IN BYTE bAction, IN BYTE *pAID, IN BYTE bAIDLen, IN BYTE *pTagsData, USHORT usTagsDataLen);

USHORT EMV_TxnCandidateList(IN BYTE bNewSel, OUT USHORT *pusCandidateNum, OUT EMV_CANDIDATE_DATA *pstCandidateBuf, IN USHORT usMaxCandidateNum, OUT BOOL *pIsCardholderConfirmation);
USHORT EMV_TxnFinalSelection(IN USHORT usAppIdx);
USHORT EMV_TxnCandidateDirEntry(IN USHORT usAppIdx, OUT BYTE *baDirEntry, OUT USHORT *pusDirEntryLen, IN USHORT usMaxDirEntrySize);
USHORT EMV_TxnPSERsp(OUT BYTE *baRspBuf, OUT USHORT *pusRspBufLen, IN USHORT usMaxRspBufSize);
USHORT EMV_SecureDataEncryptInfoSet(EMV_SECURE_DATA_INFO *pstSecureInfo);
USHORT EMV_SecureDataWhitelistSet(BYTE bAction, BYTE *pbListData, USHORT usListDataLen);


//EventID
#define d_EVENTID_OUTPUT_CARDAPDU									0
#define d_EVENTID_DISABLE_PINNULL									1
#define d_EVENTID_FIRSTGENAC_TERMDECISION							2
#define d_EVENTID_DISPLAY_APPLICATION_LIST							3
#define d_EVENTID_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS			4
#define d_EVENTID_APP_LIST_EX										5
#define d_EVENTID_NON_EMV_CARD										6
#define d_EVENTID_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID				7
#define d_EVENTID_NSICC_PAYMENT										8
#define d_EVENTID_TXN_FORCED_ONLINE									9
#define d_EVENTID_INTERAC_ASF_MODE									10
#define d_EVENTID_SHOW_PIN_BYPASS									11
#define d_EVENTID_SHOW_VIRTUAL_PIN									12
#define d_EVENTID_GET_PIN_DONE										13
//#define d_EVENTID_SHOW_VIRTUAL_PIN_EX								14 //used in Jar
#define d_EVENTID_SHOW_PIN_DIGIT									15
#define d_EVENTID_ONLINE_PINBLOCK_GET								16
#define d_EVENTID_OFFLINE_PINBLOCK_GET								17
#define d_EVENTID_APPLICATION_SELECTION_METHOD						30

#define d_EVENTID_OFFLINE_PIN_VERIFY_RESULT							0xC0
#define d_EVENTID_DEBUG_MSG											0xC1

//variable for d_EVENTID_OUTPUT_CARDAPDU
typedef void (*EVENT_OUTPUTCARDAPDU)(IN BYTE *pTxAPDU, IN USHORT TxAPUDLen, IN BYTE *pRxAPDU, IN USHORT RxAPDULen);

//variable for d_EVENTID_DISABLE_PINNULL
typedef void (*EVENT_DISABLE_PINNULL)(OUT BOOL *IsDisablePINNull);

//variable for d_EVENTID_FIRSTGENAC_TERMDECISION
typedef void (*EVENT_FIRSTGENAC_TERMDECISION)(INOUT BYTE *TermDecision);

//variable for d_EVENTID_CONTINUE_APP_SELECT
//using EMV_TxnAppSpecificSelect function, when specified application label is not match, call this callback function and return true to display application list to select. default return d_EMVAPLIB_ERR_NO_AP_FOUND when is not match.
typedef BOOL (*EVENT_DISPLAY_APPLICATION_LIST)(void);

//variable for d_EVENTID_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS
typedef void (*EVENT_ENABLE_INTERNAL_GETPIN_CANCEL_KEY_BYPASS)(OUT BYTE *isEnableGetPINBypassByCancelKey);

//variable for d_EVENTID_APP_LIST_EX
typedef USHORT (*EVENT_APP_LIST_EX)(IN BYTE bAppNum, IN EMV_APP_LIST_EX_DATA *pstAppListExData, OUT BYTE *pbAppSelectedIndex);

//variable for d_EVENTID_NON_EMV_CARD
typedef void (*EVENT_NON_EMV_CARD)(OUT BOOL *IsNonEMVCard);

//variable for d_EVENTID_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID
typedef void (*EVENT_MCCS_DYNAMIC_TERMIANL_SUPPORTED_AID)(OUT BYTE *pbAID, OUT BYTE *pbAIDLen, OUT BYTE *pbAIDNoMatchOption);

//variable for d_EVENTID_NSICC_PAYMENT
typedef void (*EVENT_NSICCPayment)(IN BYTE *pbAID, IN BYTE bAIDLen, OUT BYTE *pbNSICCPayment);

//variable for d_EVENTID_TXN_FORCED_ONLINE
typedef void (*EVENT_TxnForcedOnline)(OUT BYTE *pbForcedONL);
//pbForcedONL :
// 0x01 : Txn Forced Online

//variable for d_EVENTID_SHOW_PIN_DIGIT
typedef void (*EVENT_SHOW_PIN_DIGIT)(IN BYTE bNoDigits);

//variable for d_EVENTID_SHOW_PIN_BYPASS
typedef void (*EVENT_SHOW_PIN_BYPASS)(void);

//variable for d_EVENTID_SHOW_VIRTUAL_PIN
typedef USHORT (*EVENT_SHOW_VIRTUAL_PIN)(OUT USHORT *pusPINPadButtonNum, OUT BYTE *pbPINPadButtonInfo, OUT USHORT *pusPINPadButtonInfoLen);

//variable for d_EVENTID_GET_PIN_DONE
typedef USHORT (*EVENT_GET_PIN_DONE)(void);

//variable for d_EVENTID_OFFLINE_PIN_VERIFY_RESULT
typedef void (*EVENT_OFFLINE_PIN_VERIFY_RESULT)(IN USHORT usResult);

//variable for EVENT_DEBUG_MSG
typedef void (*EVENT_DEBUG_MSG)(IN BYTE *baMsg, IN USHORT usMsgLen);


void EMV_SpecialEventRegister(IN BYTE EventID, IN void *EventFunc);

#ifdef	__cplusplus
}
#endif

#endif	/* EMVAPLIB_H */

