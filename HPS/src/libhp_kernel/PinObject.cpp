
/* 
 * File:   PinObject.cpp
 * Author: Administrator
 * 
 * Created on 2019年3月13日, 上午10:06
 */

#include "PinObject.h"
#include "TxnDef.h"
#include "dbghlp.h"

static void PrintKMSParam(CTOS_KMS2PINGET_PARA *pPinGetPara);


CPinObject::CPinObject(IParameter* pParamPtr)
:m_pParamPtr(pParamPtr)
{
}

CPinObject::~CPinObject() 
{
}


/**
* Function:
*      GetPinBlock, get pin block from kms module
*
* @param
*      [out]pszPinBlock: pin block data
*      [in]pwPinBlockSize: buffer size of the first parameter
*      [out]pwPinBlockSize: length of the real pin block
*
* @return
*      0:       success
*      other:   fail, please refer to ctosapi.h
* Author: Tim.Ma
*/
unsigned short CPinObject::GetPinBlock(
    OUT unsigned char *pszPinBlock,
    IN_OUT unsigned short *pwPinBlockSize)
{
    WORD wErr;
    tagParam cTagParam = { 0 };
    HEARTLAND_PINENTRY_PARAM pinParam = { 0 };
    WORD wBufferSize = 0;

    if (NULL != pszPinBlock && NULL != pwPinBlockSize)
    {
        wBufferSize = (*pwPinBlockSize);
        bzero(pszPinBlock, wBufferSize);
        (*pwPinBlockSize) = 0;

        do
        {
            InitTagParam(cTagParam, kPinParam, pinParam);
            int nRet = m_pParamPtr->GetParameter(&cTagParam);
            if (nRet != 0)
            {
                wErr = 1;
                TraceMsg("Error: Get PIN parameter failed.");
                break;
            }

            pinParam.cKms2PinObj.AdditionalData.pInData = pinParam.byPanData;
            pinParam.cKms2PinObj.AdditionalData.InLength = pinParam.byPanLength;
            pinParam.cKms2PinObj.PINOutput.pEncryptedBlock = pszPinBlock;
            pinParam.cKms2PinObj.PINOutput.EncryptedBlockLength = wBufferSize;

            wErr = CTOS_KMS2PINGet(&pinParam.cKms2PinObj);
            if (wErr != d_OK)
            {
                if (d_KMS2_GET_PIN_NULL_PIN == wErr)
                {
                    TraceMsg("Error: KMS2PINGet is GET_PIN_NULL_PIN.");
                    break;
                }
                TraceMsg("Error: Call KMS2PINGet failed, ErrorCode=0x%04X", wErr);
                break;
            }
            (*pwPinBlockSize) = pinParam.cKms2PinObj.PINOutput.EncryptedBlockLength;
        } while (0);
    }
    else
    {
        wErr = d_SYSTEM_INVALID_PARA;
        TraceMsg("Error: Pointer is NULL.");
    }
    return wErr;
}


/**
* Function:
*      GetKSNString, get key SN from kms module
*
* @param
*      [out]pszKSNString: ken sn data
*      [in]pbyKSNStringSize: buffer size of the first parameter
*      [out]pbyKSNStringSize: length of the real key sn
*
* @return
*      0:       success
*      other:   fail, please refer to ctosapi.h
* Author: Tim.Ma
*/
unsigned short CPinObject::GetKSNString(
        OUT unsigned char* pszKSNString, 
        IN_OUT unsigned char* pbyKSNStringSize)
{
    WORD wErr;
    tagParam cTagParam = { 0 };
    HEARTLAND_PINENTRY_PARAM pinParam = { 0 };
	
#ifndef PLATFORM_ANDROID
        BYTE bPinKsnLen = 32;
#else
        UINT bPinKsnLen = 32;
#endif

    if (NULL != pszKSNString && NULL != pbyKSNStringSize)
    {
        bPinKsnLen = (*pbyKSNStringSize);
        bzero(pszKSNString, bPinKsnLen);
        (*pbyKSNStringSize) = 0;

        do
        {
            InitTagParam(cTagParam, kPinParam, pinParam);
            int nRet = m_pParamPtr->GetParameter(&cTagParam);
            if (0 != nRet)
            {
                TraceMsg("Error: Get PIN parameter failed.");
                wErr = 1;
                break;
            }

        wErr = CTOS_KMS2DUKPTGetKSN(
                pinParam.cKms2PinObj.Protection.CipherKeySet,
                pinParam.cKms2PinObj.Protection.CipherKeyIndex,
                pszKSNString,
                &bPinKsnLen);
            if (d_OK != wErr)
            {
                TraceMsg("Error: CTOS_KMS2DUPKPTGetKSN falied, ErrorCode=0x%04X",
                         wErr);
                break;
            }
            (*pbyKSNStringSize) = bPinKsnLen;
            wErr = d_OK;
        } while (0);
    }
    else
    {
        wErr = d_SYSTEM_INVALID_PARA;
        TraceMsg("Error: Pointer is NULL.");
    }
    return wErr;
}


/**
* Function:
*      PrintKMSParam, print the value of pPinGetPara
*
* @param
*      [in]pPinGetPara: data need to be debugged
*
* @return
*      [none]
* Author: Tim.Ma
*/
void PrintKMSParam(CTOS_KMS2PINGET_PARA *pPinGetPara)
{
    TraceMsg("Version:%d", pPinGetPara->Version);
    
    TraceMsg("PIN_Info.BlockType:%d", pPinGetPara->PIN_Info.BlockType);
    TraceMsg("PIN_Info.PINDigitMaxLength:%d", pPinGetPara->PIN_Info.PINDigitMaxLength);
    TraceMsg("PIN_Info.PINDigitMinLength:%d", pPinGetPara->PIN_Info.PINDigitMinLength);
    
    TraceMsg("Protection.CipherKeySet:%X", pPinGetPara->Protection.CipherKeySet);
    TraceMsg("Protection.CipherKeyIndex:%X", pPinGetPara->Protection.CipherKeyIndex);
    TraceMsg("Protection.CipherMethod:%X", pPinGetPara->Protection.CipherMethod);
    TraceMsg("Protection.SK_Length:%d", pPinGetPara->Protection.SK_Length);
    TraceMsg("Protection.pSK:%p", pPinGetPara->Protection.pSK);
    
    TraceMsg("AdditionalData.InLength:%d", pPinGetPara->AdditionalData.InLength);
    TraceMsg("AdditionalData.pInData:%s", pPinGetPara->AdditionalData.pInData);
    
    TraceMsg("DUKPT_PARA.IsUseCurrentKey:%d", pPinGetPara->DUKPT_PARA.IsUseCurrentKey);
    
    TraceMsg("PINOutput.EncryptedBlockLength:%d", pPinGetPara->PINOutput.EncryptedBlockLength);
    TraceMsg("PINOutput.pEncryptedBlock:%p", pPinGetPara->PINOutput.pEncryptedBlock);
    TraceMsg("PINOutput.PINDigitActualLength:%d", pPinGetPara->PINOutput.PINDigitActualLength);
    
    TraceMsg("Control.Timeout:%d", pPinGetPara->Control.Timeout);
    TraceMsg("Control.AsteriskPositionX:%d", pPinGetPara->Control.AsteriskPositionX);
    TraceMsg("Control.AsteriskPositionY:%d", pPinGetPara->Control.AsteriskPositionY);
    TraceMsg("Control.NULLPIN:%d", pPinGetPara->Control.NULLPIN);
    TraceMsg("Control.piTestCancel:%p", pPinGetPara->Control.piTestCancel);
}