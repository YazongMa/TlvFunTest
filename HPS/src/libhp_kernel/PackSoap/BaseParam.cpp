
/*
 * File Name: BaseParam.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.8
 */

#include "BaseParam.h"
#include "../dbghlp.h"


CBaseParam::CBaseParam()
{
    bzero(&m_cTransParam, sizeof(m_cTransParam));
    // bzero(&m_cSoapRequest, sizeof(m_cSoapRequest));
    // bzero(&m_cSoapResponse, sizeof(m_cSoapResponse));
    bzero(&m_cCurlParam, sizeof(m_cCurlParam));
    bzero(&m_cEmvclParam, sizeof(m_cEmvclParam));
    bzero(&m_cEmvParam, sizeof(m_cEmvParam));
    bzero(&m_cGeneralParam, sizeof(m_cGeneralParam));
    bzero(&m_cPinParam, sizeof(m_cPinParam));
    bzero(&m_cTxnAmountData, sizeof(m_cTxnAmountData));
    bzero(&m_cPollingCardCache, sizeof(m_cPollingCardCache));
}

CBaseParam::~CBaseParam()
{
}

/*
 * Public.
 */
#define SetParam(data, dataType, Length, inObj) { \
    if ((dataType) == (inObj).nType && (inObj).nParamSize == (Length)) { \
        memcpy(&(data), cParam.pParam, (Length)); \
        return 0; \
    } \
    else if ((dataType) == (inObj).nType && (inObj).nParamSize != (Length)) { \
        TraceMsg("Error: call SetParameter failed, nType=%d", (dataType)); \
        return -1; \
    } \
}
int CBaseParam::SetParameter(IN const tagParam &cParam)
{
    if (NULL != cParam.pParam)
    {
        SetParam(m_cTransParam, kHLTransParam, sizeof(m_cTransParam), cParam);
        // SetParam(m_cSoapRequest, kSoapRequest, sizeof(m_cSoapRequest), cParam);
        // SetParam(m_cSoapResponse, kSoapResponse, sizeof(m_cSoapResponse), cParam);
        SetParam(m_cCurlParam, kCurlParam, sizeof(m_cCurlParam), cParam);
        SetParam(m_cEmvclParam, kEmvclParam, sizeof(m_cEmvclParam), cParam);
        SetParam(m_cEmvParam, kEmvParam, sizeof(m_cEmvParam), cParam);
        SetParam(m_cGeneralParam, kGeneralParam, sizeof(m_cGeneralParam), cParam);
        SetParam(m_cTxnAmountData, kTxnAmountData, sizeof(m_cTxnAmountData), cParam);
        SetParam(m_cPinParam, kPinParam, sizeof(m_cPinParam), cParam);
        SetParam(m_cPollingCardCache, kPollingCardCache, sizeof(m_cPollingCardCache), cParam);
    }
    return -1;
}

/*
 * Public.
 */
#define GetParam(data, dataType, Length, outPtr) { \
    if ((dataType) == (outPtr)->nType && (Length) == (outPtr)->nParamSize) { \
        memcpy((outPtr)->pParam, &(data), (Length)); \
        return 0; \
    } \
    else if ((dataType) == (outPtr)->nType && (Length) != (outPtr)->nParamSize) { \
        TraceMsg("Error: Call GetParameter failed, nType=%d, %d", (dataType), (outPtr)->nType); \
        return -1; \
    } \
}
int CBaseParam::GetParameter(OUT tagParam *pcParam)
{
    if (NULL != pcParam)
    {
        GetParam(m_cTransParam, kHLTransParam, sizeof(m_cTransParam), pcParam);
        // GetParam(m_cSoapRequest, kSoapRequest, sizeof(m_cSoapRequest), pcParam);
        // GetParam(m_cSoapResponse, kSoapResponse, sizeof(m_cSoapResponse), pcParam);
        GetParam(m_cCurlParam, kCurlParam, sizeof(m_cCurlParam), pcParam);
        GetParam(m_cEmvclParam, kEmvclParam, sizeof(m_cEmvclParam), pcParam);
        GetParam(m_cEmvParam, kEmvParam, sizeof(m_cEmvParam), pcParam);
        GetParam(m_cGeneralParam, kGeneralParam, sizeof(m_cGeneralParam), pcParam);
        GetParam(m_cTxnAmountData, kTxnAmountData, sizeof(m_cTxnAmountData), pcParam);
        GetParam(m_cPinParam, kPinParam, sizeof(m_cPinParam), pcParam);
        GetParam(m_cPollingCardCache, kPollingCardCache, sizeof(m_cPollingCardCache), pcParam);
    }
    return -1;
}