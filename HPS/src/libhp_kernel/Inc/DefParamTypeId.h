
#ifndef _H_H_TXN_TYPE_H_H_
#define _H_H_TXN_TYPE_H_H_


typedef enum
{
    kNoneCard       = 0x01,
    kEmv            = 0x02,
    kEmvCL          = 0x04,
    kMSR            = 0x08,
    kMSR_Fallback   = 0x10
} kCardType;


typedef enum
{
    kHLTransParam = 0,
    kSoapRequest,
    kSoapResponse,
    kCurlParam,
    kEmvParam,
    kEmvclParam,
    kTxnAmountData,
    kPinParam,
    kGeneralParam,
    kPollingCardCache
} ParamType;

typedef enum
{
    kInvalidType = -1,
    kBatchClose_SoapType = 0,
    kCreditReturn_SoapType,
    kCreditSale_SoapType,
    kCreditVoid_SoapType,
    kDebitReturn_SoapType,
    kDebitSale_SoapType,
    kPreAuth_SoapType,
    kAuthComplete_SoapType,
    kReversal_SoapType,
    kSAF_SoapType,
    kCheckConnectivity_SoapType,
    kDebitPollingCard_TxnType,
    kCreditPollingCard_TxnType,
    kManualCreditReversal_SoapType,
    kManualDebitReversal_SoapType,
    kMaxSoapTypeCount = 0xFF
} kSoapType;

#endif 