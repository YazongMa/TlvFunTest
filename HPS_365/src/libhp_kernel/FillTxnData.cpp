
/*
 * File Name: FillTxnData.cpp
 * Author: Time.Ma 
 * 
 * Crate Date: 2019.3.13
 */

#include "FillTxnData.h"
#include "TxnTag.h"
#include "TlvFun.h"
#include "FileFun.h"
#include "dbghlp.h"
#include "Inc/ErrorCode.h"
#include "FunBase.h"


#define MANDATORY_EMV_CONTACT                       0x01
#define MANDATORY_EMV_CTLS                          0x02
#define MANDATORY_MSD                               0x04

#define isEMVContactCondition(tagCondition)         (tagCondition & MANDATORY_EMV_CONTACT)
#define isEMVContactlessCondition(tagCondition)     (tagCondition & MANDATORY_EMV_CTLS)
#define isMSDCondition(tagCondition)                (tagCondition & MANDATORY_MSD)

static BYTE s_prevTransactionType   = 0x00;
static bool s_prevSuccessChipRead   = false;

static bool IsTagNecessary(const BYTE tagCondition, const BYTE cardSource, const BYTE posEntry)
{
    return(isEMVContactCondition(tagCondition) && d_ICC_ARQC == cardSource) ||
            (isEMVContactlessCondition(tagCondition) && d_CTLS_ARQC == cardSource && POS_ENTRY_MODE_CTLS_CHIP == posEntry) ||
            (isMSDCondition(tagCondition) && d_CTLS_ARQC == cardSource && POS_ENTRY_MODE_CTLS_MSR == posEntry);
}



CFillTxnData::CFillTxnData(HeartlandSoap *pcSoapKernel, CVoltageObject* pcVoltageObj)
:
m_pcSoapKernel(pcSoapKernel),
m_pcVoltageObj(pcVoltageObj)
{
}


CFillTxnData::~CFillTxnData()
{
}
    

/**
* SetTrackData
*
* Function:
*      Set pcHsoapRequest->szTrackData from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetTrackData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam, BOOL bCardType)
{       
    int     nRet = 1;
    WORD    wRet = 0;
    WORD    wTrack2Len = 0;
    BYTE*   pbyTrack2Data = NULL;
    
    // Voltage encryption
    DWORD dwETBLen = 512;
    DWORD dwEncryptTK2Len = 64;
    char szEncryptTK2Data[64] = { 0 };
    char szETBData[512] = { 0 };
    char szTK2Data[64] = {0};
    do
    {
        if (pcTransParam == NULL)
        {
            TraceMsg("Invalid Param");
            return ERROR_INVALID_PARAM;
        }
        
        wRet = CTlvFun::TLVListReadTagValue(TAG_TK2, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pbyTrack2Data, &wTrack2Len);
        if (wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_TK2>");
            return wRet;
        }
        
        TraceMsg("VoltageFlag:%d", m_pcVoltageObj->GetFlag());
        if (m_pcVoltageObj->GetFlag() == 1)
        {            
            // Generate encrypted data and KTB
            memcpy(szTK2Data, pbyTrack2Data, wTrack2Len);
            for (int i=0; i<strlen(szTK2Data); i++)
            {
                if (szTK2Data[i] == 0x3F)
                {
                    szTK2Data[i + 1] = '\0';
                    break;
                }
            }

            nRet = m_pcVoltageObj->EncryptData(4, szTK2Data, strlen(szTK2Data),
                    szEncryptTK2Data, &dwEncryptTK2Len, szETBData, &dwETBLen);
            if(nRet == 0)
            {
                memcpy(pcHsoapRequest->cCardData.cEncrypData.szVersion, "04", 2);
                memcpy(pcHsoapRequest->cCardData.cEncrypData.szEncryptedTrackNumber, "2", 1);
                memcpy(pcHsoapRequest->cCardData.cEncrypData.szKTB, szETBData, dwETBLen);                
                
                if (bCardType == d_CREDIT_CARD_TYPE)
                {
                    memset(pcHsoapRequest->cCardData.szTrackData, 0, sizeof(pcHsoapRequest->cCardData.szTrackData));
                    memcpy(pcHsoapRequest->cCardData.szTrackData, szEncryptTK2Data + 1, dwEncryptTK2Len - 2);
                }
                else if(bCardType == d_DEBIT_CARD_TYPE)
                {
                    memset(pcHsoapRequest->szTrackData, 0, sizeof(pcHsoapRequest->cCardData.szTrackData));
                    memcpy(pcHsoapRequest->szTrackData, szEncryptTK2Data + 1, dwEncryptTK2Len - 2);
                }

                break;
            }
            
            TraceMsg("Voltage_EncryptData failure nRet: %d, Would not use VOLTAGE ENCRYPTION", nRet);
        }
        
        if (bCardType == d_CREDIT_CARD_TYPE && nRet != 0)
        {
            memcpy(pcHsoapRequest->cCardData.szTrackData, pbyTrack2Data, wTrack2Len);
            //TraceMsg("track2data:%s", pcHsoapRequest->cCardData.szTrackData);
        }
        else if(bCardType == d_DEBIT_CARD_TYPE && nRet != 0)
        {
            memcpy(pcHsoapRequest->szTrackData, pbyTrack2Data, wTrack2Len);
            //TraceMsg("track2data:%s", pcHsoapRequest->szTrackData);
        }
    }while (0);
    
    return wRet;
}


/**
* SetAmt
*
* Function:
*      Set pcHsoapRequest->szAmt from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetAmt(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    WORD    wRet = 1;
    int     iOffset = 0;
    WORD    wTrack2Len = 0;
    BYTE*   pbyTrack2Data = NULL;
    BYTE    tmpAmtSting[16] = { 0 };
    long long int llAmt = 0;
    
    do
    {
        if (pcTransParam == NULL)
        {
            TraceMsg("pcTransParam == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        wRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_QUICKCHIP_ACTURAL_AMT, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pbyTrack2Data, &wTrack2Len);
        if (!wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <d_TAG_TRANS_QUICKCHIP_ACTURAL_AMT> OK");
            break;
        }
        
        wRet = CTlvFun::TLVListReadTagValue(TAG_AMT_AUTH, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pbyTrack2Data, &wTrack2Len);
        if (!wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_AMT_AUTH> OK");
            break;
        }
    } while (0);
    
    if (wRet == 1)
    {
        TraceMsg("ERROR: tag Amt is missing");
        return 0;
    }
    
    static CFunBase cFunBase;
    CByteStream cAmount = cFunBase.Hex2Str(pbyTrack2Data, wTrack2Len);
    memcpy(tmpAmtSting, cAmount.GetBuffer(), cAmount.GetLength());

    llAmt = atoll((char *)tmpAmtSting);
    sprintf(pcHsoapRequest->szAmt, "%lld.%02lld", llAmt / 100, llAmt % 100);

    TraceMsg("pcHsoapRequest->szAmt :%s, llAmt:%lld", pcHsoapRequest->szAmt, llAmt);
    
    
    return OK;
}


/**
* SetCashbackAmt
*
* Function:
*      Set pcHsoapRequest->szCashBackAmt from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetCashBackAmt(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    WORD    wRet = 0;
    int     iOffset = 0;
    WORD    wTrack2Len = 0;
    BYTE*   pbyTrack2Data = NULL;
    BYTE    tmpAmtSting[16] = { 0 };
    long long int llAmt = 0;
    
    do
    {
        if (pcTransParam == NULL)
        {
            TraceMsg("pcTransParam == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        wRet = CTlvFun::TLVListReadTagValue(TAG_AMT_OTHER, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pbyTrack2Data, &wTrack2Len);
        if (wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_AMT_OTHER>");
            break;
        }
        
        static CFunBase cFunBase;
        CByteStream cAmount = cFunBase.Hex2Str(pbyTrack2Data, wTrack2Len);
        memcpy(tmpAmtSting, cAmount.GetBuffer(), cAmount.GetLength());

        if (memcmp(tmpAmtSting, "000000000000", 12) == 0)
        {
            TraceMsg("Cashback or AmountOther is zero");
            break;
        }

        llAmt = atoll((char *)tmpAmtSting);
        sprintf(pcHsoapRequest->szCashbackAmtInfo, "%lld.%02lld", llAmt / 100, llAmt % 100);

        // memcpy(&pcHsoapRequest->szCashbackAmtInfo[iOffset], tmpAmtSting, strlen((char *)tmpAmtSting) - 2);
        // iOffset += (strlen((char *)tmpAmtSting) - 2);
        // pcHsoapRequest->szCashbackAmtInfo[iOffset++] = '.';
        // memcpy(&pcHsoapRequest->szCashbackAmtInfo[iOffset], &tmpAmtSting[strlen((char *)tmpAmtSting) - 2], 2);
        TraceMsg("pcHsoapRequest->szCashbackAmtInfo :%s, llAmt:%lld", pcHsoapRequest->szCashbackAmtInfo, llAmt);
    } while (0);
    
    
    return OK;
}

/**
* SetGratuityAmtInfo
*
* Function:
*      Set pcHsoapRequest->GratuityAmtInfo from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetGratuityAmtInfo(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    TraceMsg("Do nothing");
    return OK;
}


/**
* SetAllowDupInfo
*
* Function:
*      Set pcHsoapRequest->AllowDupInfo from pcTransParam.
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point.
*
* @return:
*      OK  - Successful.
*      other - Please refer to ErrorCode.h.
**/
WORD CFillTxnData::SetAllowDupInfo(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    bool allow = true;
    WORD wRet = 0;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        if (allow)
        {
            pcHsoapRequest->szAllowDup[0] = 'Y';
            pcHsoapRequest->szAllowDup[1] = '\0';
        }
    }
    while (0);
    return (wRet);
}


/**
* SetAllowPartialAuth
*
* Function:
*      Set pcHsoapRequest->szAllowPartialAuth from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetAllowPartialAuth(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    bool allow = true;
    WORD wRet = 0;

    WORD    wLen = 0;
    BYTE*   pbyData = NULL;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        pcHsoapRequest->szAllowPartialAuth[0] = 'Y';
        pcHsoapRequest->szAllowPartialAuth[1] = '\0';

        wRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_ALLOW_PARTIAL_AUTH, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pbyData, &wLen);
        if (wRet || wLen==0)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <d_TAG_TRANS_ALLOW_PARTIAL_AUTH>");
            return wRet;
        }

        if (*pbyData == '0')
        {
            pcHsoapRequest->szAllowPartialAuth[0] = '\0';
        }
    }
    while (0);
    return (wRet);
}


/**
* SetEMVData
*
* Function:
*      Set pcHsoapRequest->EMVData from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetEMVData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    // TODO: This method should be called only when current chip read failed
    ULONG ulRet = 0;
    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }

        if (pcTransParam == NULL)
        {
            TraceMsg("pcTransParam == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        
        BYTE * const baTlvListPtr = &pcTransParam->ucaRequestData[0];
        const USHORT usTlvListLen = pcTransParam->usRequestDataLen;
        USHORT posEntryLen = 0;
        BYTE *posEntry = NULL;
        ulRet = CTlvFun::TLVListReadTagValue(TAG_POS_ENTRY_MODE, baTlvListPtr, usTlvListLen, &posEntry, &posEntryLen);
        if (ulRet)
        {
            TraceMsg("No TAG_POS_ENTRY_MODE Data.");
            ulRet = 0;
            s_prevTransactionType = pcTransParam->bARQCType;
            break;
        }

        USHORT i = 0;
        printf("TAG_POS_ENTRY_MODE:");
        for (; i < posEntryLen; ++i)
        {
            printf("%02X ", posEntry[i]);
        }
        printf("\n");

        if (1 == posEntryLen && (POS_ENTRY_MODE_ICC_MSR == posEntry[0] || POS_ENTRY_MODE_ICC_MSR_SER_101 == posEntry[0]))
        {
            // ICC Fallback (Chip read failed)
            USHORT len;
            if (s_prevTransactionType != d_ICC_ARQC || s_prevSuccessChipRead)
            {
                // Use this condition type when the current chip read failed
                // but the previous transaction on the same device was either a successful chip read or was not a chip transaction.
                const char successString[] = "CHIP_FAILED_PREV_SUCCESS";
                len = strlen(successString);

                if (pcTransParam->bCardType == d_CREDIT_CARD_TYPE)
                {
                    memcpy(pcHsoapRequest->cEmvData.szEMVChipCondition, successString, len);
                    pcHsoapRequest->cEmvData.szEMVChipCondition[len] = '\0';
                }
                else if(pcTransParam->bCardType == d_DEBIT_CARD_TYPE)
                {
                    memcpy(pcHsoapRequest->szEMVChipCondition, successString, len);
                    pcHsoapRequest->szEMVChipCondition[len] = '\0';
                }
            }
            else
            {
                // Use this condition type when the current chip read failed and
                // the previous transaction on the same device was also an unsuccessful chip read.
                const char failedString[] = "CHIP_FAILED_PREV_FAILED";
                len = strlen(failedString);

                if (pcTransParam->bCardType == d_CREDIT_CARD_TYPE)
                {
                    memcpy(pcHsoapRequest->cEmvData.szEMVChipCondition, failedString, len);
                    pcHsoapRequest->cEmvData.szEMVChipCondition[len] = '\0';
                }
                else if(pcTransParam->bCardType == d_DEBIT_CARD_TYPE)
                {
                    memcpy(pcHsoapRequest->szEMVChipCondition, failedString, len);
                    pcHsoapRequest->szEMVChipCondition[len] = '\0';
                }
            }

            s_prevSuccessChipRead = false;
            s_prevTransactionType = d_ICC_ARQC;
        }
        else if (d_ICC_ARQC == pcTransParam->bARQCType)
        {
            // Chipe read successed
            s_prevSuccessChipRead = true;
            s_prevTransactionType = pcTransParam->bARQCType;
        }
        else
        {
            s_prevTransactionType = pcTransParam->bARQCType;
        }
        
        if(pcTransParam->bCardType == d_DEBIT_CARD_TYPE)
        {
            TraceMsg("INOF: Debit transaction");
            break;
        }

        WORD wRet = 0;
        WORD wValueLen = 0, wOffset = 0;
        unsigned char *pucValue = NULL;
        memset(pcHsoapRequest->cEmvData.szPINBlock, 0x00, sizeof(pcHsoapRequest->cEmvData.szPINBlock));
        wRet = CTlvFun::TLVListReadTagValue(d_TAG_EPB, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pucValue, &wValueLen);
        if (wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_EPB>");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
                
        CFunBase    cFun;
        CByteStream cData = cFun.Hex2Str(pucValue, wValueLen);
        wOffset = cData.GetLength();
        memcpy(pcHsoapRequest->cEmvData.szPINBlock, cData.GetBuffer(), wOffset);
        
        wRet = CTlvFun::TLVListReadTagValue(d_TAG_EPB_KSN, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pucValue, &wValueLen);
        if (!wRet)
        {
            cData = cFun.Hex2Str(&pucValue[2], wValueLen - 2);
            memcpy(&pcHsoapRequest->cEmvData.szPINBlock[wOffset], cData.GetBuffer(), cData.GetLength());
            //CStringFun::Hex2Str(&pucValue[2], (BYTE *) & pcHsoapRequest->cEmvData.szPINBlock[wOffset], wValueLen - 2);
        }
        wRet = OK;
        TraceMsg("pin block %s", pcHsoapRequest->cEmvData.szPINBlock);
    }
    while (0);
    TraceMsg("%s exit, exit code:%ld", __FUNCTION__, ulRet);

    return ulRet;
}


/**
* SetTagData
*
* Function:
*      Set pcHsoapRequest->TagData from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetTagData(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    ULONG ulRet = 0;
    TraceMsg("entry");
    //return 0;
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        if (pcTransParam == NULL)
        {
            TraceMsg("pcTransParam == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }

        BYTE cardSource = pcTransParam->bARQCType;
        if (d_ICC_ARQC != cardSource && d_CTLS_ARQC != cardSource)
        {
            TraceMsg("cardSource:%x, neither d_ICC_ARQC(%x) nor d_CTLS_ARQC(%x)", cardSource, d_ICC_ARQC, d_CTLS_ARQC);
            break;
        }

        BYTE * const baTlvListPtr = &pcTransParam->ucaRequestData[0];
        const USHORT usTlvListLen = pcTransParam->usRequestDataLen;

        USHORT posEntryLen = 4;
        BYTE posEntryArray[4] = { 0 };
        BYTE posEntry;
        ulRet = CTlvFun::TLVListReadTagValue(TAG_POS_ENTRY_MODE, baTlvListPtr, usTlvListLen, (unsigned char **)&posEntryArray, &posEntryLen);
        if (ulRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_POS_ENTRY_MODE>");
            ulRet = ERROR_INVALID_PARAM;
            //break;
        }

        posEntry = posEntryArray[0];

        if (d_ICC_ARQC == cardSource)
        {
            pcHsoapRequest->cTagData.nTagValueType = HSOAP_TAG_VALUE_TYPE_CHIP;
        }
        else if (d_CTLS_ARQC == cardSource && POS_ENTRY_MODE_CTLS_MSR == posEntry)
        {
            pcHsoapRequest->cTagData.nTagValueType = HSOAP_TAG_VALUE_TYPE_MSD;
        }

        const bool NOT_NECESSARY = false;
        bool isNecessary;

        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_POS_ENTRY_MODE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }

        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        //BYTE condition = MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS;
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ADDITIONAL_TM_CAP, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // POS Terminal / Mandatory for EMV contact, EMV contactless, and MSD contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS | MANDATORY_MSD, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_AMT_AUTH, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // POS Terminal / Mandatory for EMV contact, EMV contactless, and MSD contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS | MANDATORY_MSD, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_AMT_OTHER, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_AC, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ADF, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // POS Terminal / Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_APP_ID, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // Chip Card / Mandatory for EMV contact and EMV contactless transactions.
        //             Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_AIP, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional
        // ulRet = SetTagDataValue(pcHsoapRequest, TAG_APP_LABEL, NULL, 0, NOT_NECESSARY);
        // if (ulRet) {
        //     break;
        // }
        // // Chip Card / Conditional
        // ulRet = SetTagDataValue(pcHsoapRequest, TAG_APP_PREFER_NAME, NULL, 0, NOT_NECESSARY);
        // if (ulRet) {
        //     break;
        // }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        //              Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_PAN_SEQ_NUM, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Mandatory for EMV contact and EMV contactless transactions.
        //             Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_COUNTER, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_APP_USAGE_CTRL, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)"9F08", baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_APP_VER, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Optional field. Mandatory for offline decline advice transactions and offline approvals.
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ARC, NULL, 0, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions
        //                             when the result of the last CVM performed is available.
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_CVM_RESULT, baTlvListPtr, usTlvListLen, NOT_NECESSARY);
        if (ulRet)
        {
          //  break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_CID, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // Chip Card / Conditional: Mandatory for EMV contactless transactions if available.
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)"9F7C", NULL, 0, NOT_NECESSARY);
        if (ulRet)
        {
          //  break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        //                          Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_FD_NAME, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for VISA contactless transactions when available.
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_PAYPASS_3RD_DATA, NULL, 0, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contactless transactions.
        // isNecessary = IsTagNecessary(MANDATORY_EMV_CTLS, cardSource, posEntry);
        // ulRet = SetTagDataValue(pcHsoapRequest, TAG_ICC_DYNAMIC_NUM, NULL, 0, isNecessary);
        // if (ulRet) {
        //     break;
        // }
        // POS Terminal (from the chip card reader) / Conditional: Mandatory for EMV contact transactions if available.
        // ulRet = SetTagDataValue(pcHsoapRequest, TAG_IFD_SN, NULL, 0, NOT_NECESSARY);
        // if (ulRet) {
        //     break;
        // }
        // Chip Card / Conditional: Mandatory for EMV contact transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ISSUER_ACTION_CODE_DEFAULT, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ISSUER_ACTION_CODE_DENAIL, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ISSUER_ACTION_CODE_ONLINE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // Chip Card / Mandatory for EMV contact and EMV contactless transactions.
        //             Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ISSUER_APP_DATA, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // Chip Card / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        //                          Mandatory for MSD contactless transactions when available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_ISSUER_COUNTRY_CODE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact transactions
        //                             when an issuer script was returned in the authorization response message.

        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)"9F5B", NULL, 0, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contactless transactions and MSD contactless transactions.
        // isNecessary = IsTagNecessary(MANDATORY_EMV_CTLS | MANDATORY_MSD, cardSource, posEntry);
        // ulRet = SetTagDataValue(pcHsoapRequest, TAG_ISSUER_COUNTRY_CODE, NULL, 0, isNecessary);
        // if (ulRet) {
        //     break;
        // }
        // POS Terminal / Conditional: Mandatory for EMV contact transactions declined offline.

        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)d_TAG_TAC_DEFAULT_1, baTlvListPtr, usTlvListLen, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact transactions declined offline.
        isNecessary = (d_ICC_ARQC == cardSource);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)d_TAG_TAC_DECLINE, baTlvListPtr, usTlvListLen, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact transactions declined offline.
        isNecessary = (d_ICC_ARQC == cardSource);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)d_TAG_TAC_ONLINE_1, baTlvListPtr, usTlvListLen, NOT_NECESSARY);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        //                             Mandatory for MSD contactless transactions if available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TM_CAP, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact, EMV contactless transactions.
        //                             Mandatory for MSD contactless transactions if available.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TM_COUNTRY_CODE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TM_TYPE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // POS Terminal / Mandatory for chip card transactions (contact and contactless)
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TVR, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // POS Terminal / Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_COUNTRY_CODE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // POS Terminal / Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_DATE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_SEQ_COUNTER, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
         //   break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TSI, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Conditional: Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_TIME, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
          //  break;
        }
        // POS Terminal / Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_TXN_TYPE, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
           // break;
        }
        // POS Terminal / Mandatory for EMV contact and EMV contactless transactions.
        isNecessary = IsTagNecessary(MANDATORY_EMV_CONTACT | MANDATORY_EMV_CTLS, cardSource, posEntry);
        ulRet = SetTagDataValue(pcHsoapRequest, (unsigned char*)TAG_UN_NUM, baTlvListPtr, usTlvListLen, isNecessary);
        if (ulRet)
        {
            //break;
        }

    }
    while (0);
    TraceMsg("%s exit, exit code:%ld", __FUNCTION__, ulRet);

    return ulRet;
}


/**
* SetTransactionDescriptor
*
* Function:
*      Set pcHsoapRequest->TransactionDescriptor from pcGeneralParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetTransactionDescriptor(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_GENERAL_PARAM* pcGeneralParam)
{
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        if (pcGeneralParam == NULL)
        {
            TraceMsg("pcGeneralParam == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        
        usValueLen = strlen(pcGeneralParam->szTxnDescriptor);
        if (usValueLen)
        {
            memcpy(pcHsoapRequest->szTxnDescriptor, pcGeneralParam->szTxnDescriptor, usValueLen);
        }
        ulRet = 0;
    }
    while (0);
    TraceMsg("%s exit, exit code:%ld", __FUNCTION__, ulRet);
    
    return ulRet;
}


/**
* szUniqueDeviceId
*
* Function:
*      Set pcHsoapRequest->szUniqueDeviceId from pcGeneralParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
WORD CFillTxnData::SetUniqueDeviceId(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_GENERAL_PARAM* pcGeneralParam)
{
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;
    unsigned char *pValue = NULL;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        if (pcGeneralParam == NULL)
        {
            TraceMsg("pcGeneralParam == NULL");
            ulRet = ERROR_INVALID_PARAM;
            break;
        }
        
        usValueLen = strlen(pcGeneralParam->szTxnUniqueDebiceId);
        if (usValueLen)
        {
            ulRet = m_pcSoapKernel->HeartlandSOAP_SetHeaderOptionalFields(
                                            d_HEADER_OPTIONAL_FIELD_UNIQUE_DEVICE_ID,
                                            (unsigned char*)pcGeneralParam->szTxnUniqueDebiceId,
                                            usValueLen);
        }
        else
        {
            ulRet = 0;
        }
    }
    while (0);
    TraceMsg("%s exit, exit code:%ld", __FUNCTION__, ulRet);
    return ulRet;
}



/**
 * SetTokenRequest
 *
 * Function:
 *      Set pcHsoapRequest->szGatewayTxnId from pcTransParam
 *
 * @param:
 *      [OUT]pcHsoapRequest - The request soap structure point
 *
 * @return:
 *      OK -  Successful
 *      Other - Please refer to ErrorCode.h
 */
WORD CFillTxnData::SetTokenRequest(ST_HSOAP_REQUEST* pcHsoapRequest, int nTokenFlag)
{   
    TraceMsg("SetTokenRequest entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            break;
        }
        
        memset(pcHsoapRequest->cCardData.szTokenRequest, 0, sizeof(pcHsoapRequest->cCardData.szTokenRequest));
        if(nTokenFlag == 1)
        {
            pcHsoapRequest->cCardData.szTokenRequest[0] = 'Y';
            TraceMsg("SetTokenRequest: use token request");
        }
        else
        {
            pcHsoapRequest->cCardData.szTokenRequest[0] = '\0';
            TraceMsg("SetTokenRequest: unuse token request");
        }
        pcHsoapRequest->cCardData.szTokenRequest[1] = '\0';
        break;
        
        
        TraceMsg("SetTokenRequest: unuse token request");
    }
    while (0);
    
    TraceMsg("SetTokenRequest exit");
    return (0);
}



WORD CFillTxnData::SetPINBlock(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{    
    TraceMsg("entry");
    WORD wRet = 0;
    WORD wValueLen = 0, wOffset = 0;
    unsigned char *pucValue = NULL;
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            wRet = ERROR_INVALID_PARAM;
            return wRet;
        }

        memset(pcHsoapRequest->szPinBlock, 0x00, sizeof(pcHsoapRequest->szPinBlock));
        wRet = CTlvFun::TLVListReadTagValue(d_TAG_EPB, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pucValue, &wValueLen);
        if (wRet)
        {
            TraceMsg("CTlvFun::TLVListReadTagValue <TAG_EPB>");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
                
        //wOffset = CStringFun::Hex2Str(pucValue, (BYTE *)pcHsoapRequest->szPinBlock, wValueLen);
        
        CFunBase    cFun;
        CByteStream cData = cFun.Hex2Str(pucValue, wValueLen);
        wOffset = cData.GetLength();
        memcpy(pcHsoapRequest->szPinBlock, cData.GetBuffer(), wOffset);
        wRet = CTlvFun::TLVListReadTagValue(d_TAG_EPB_KSN, pcTransParam->ucaRequestData, pcTransParam->usRequestDataLen, &pucValue, &wValueLen);
        if (!wRet)
        {
            //CStringFun::Hex2Str(&pucValue[2], (BYTE *) & pcHsoapRequest->szPinBlock[wOffset], wValueLen - 2);
            
            cData = cFun.Hex2Str(&pucValue[2], wValueLen - 2);
            memcpy(&pcHsoapRequest->szPinBlock[wOffset], cData.GetBuffer(), cData.GetLength());
        }
        wRet = OK;
        TraceMsg("pin block %s", pcHsoapRequest->szPinBlock);
    }
    while (0);

    if (wRet)
    {
        memset(pcHsoapRequest->szPinBlock, 0x00, sizeof(pcHsoapRequest->szPinBlock));
    }
    
    return (wRet);
}


/**
 * SetGatewayTxnId
 *
 * Function:
 *      Set pcHsoapRequest->szGatewayTxnId from pcTransParam
 *
 * @param:
 *      [OUT]pcHsoapRequest - The request soap structure point
 *
 * @return:
 *      OK -  Successful
 *      Other - Please refer to ErrorCode.h
 */
WORD CFillTxnData::SetGatewayTxnId(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{   
    WORD wRet = 0;
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;
    unsigned char *pValue = NULL;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        if (pcTransParam == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        ulRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANSACTION_IDENTITY,
                        &pcTransParam->ucaRequestData[0],
                        pcTransParam->usRequestDataLen,
                        &pValue,
                        &usValueLen);
        if(!ulRet)
        {
            memcpy(pcHsoapRequest->szGatewayTxnId, pValue, usValueLen);
            break;
        }
        wRet = ERROR_INVALID_PARAM;
        TraceMsg("GatewayTxnId == NULL, ulRet=0x%08lX", ulRet);
    }
    while (0);
    return (wRet);
}


/**
 * SetTokenValue
 *
 * Function:
 *      Set pcHsoapRequest->szTokenValue from pcTransParam
 *
 * @param:
 *      [OUT]pcHsoapRequest - The request soap structure point
 *
 * @return:
 *      OK -  Successful
 *      Other - Please refer to ErrorCode.h
 */
WORD CFillTxnData::SetTokenValue(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam, BOOL bCardType)
{   
    WORD wRet = 0;
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;
    unsigned char *pValue = NULL;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        if (pcTransParam == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        ulRet = CTlvFun::TLVListReadTagValue(d_TAG_TRANS_TOKEN_VALUE,
                        &pcTransParam->ucaRequestData[0],
                        pcTransParam->usRequestDataLen,
                        &pValue,
                        &usValueLen);
        if(!ulRet)
        {
            pcHsoapRequest->cCardData.nChoice = HSOAP_TOKEN_DATA;
            if (bCardType == d_CREDIT_CARD_TYPE)
            {
                memset(&pcHsoapRequest->cCardData.cTokenData, 0, sizeof(pcHsoapRequest->cCardData.cTokenData));
                memcpy(pcHsoapRequest->cCardData.cTokenData.szTokenValue, pValue, usValueLen);
                pcHsoapRequest->cCardData.cTokenData.cCommData.szCardPresent[0] = 'N';
                pcHsoapRequest->cCardData.cTokenData.cCommData.szReaderPresent[0] = 'N';
            }
            else if(bCardType == d_DEBIT_CARD_TYPE)
            {
                memset(&pcHsoapRequest->szTokenValue, 0, sizeof(pcHsoapRequest->szTokenValue));
                memcpy(pcHsoapRequest->szTokenValue, pValue, usValueLen);
            }
            TraceMsg("TokenValue:%s", pcHsoapRequest->cCardData.cTokenData.szTokenValue);
            memset(&pcHsoapRequest->cCardData.szTokenRequest, 0, sizeof(pcHsoapRequest->cCardData.szTokenRequest));
            break;
        }
        wRet = ERROR_INVALID_PARAM;
        TraceMsg("GatewayTxnId == NULL, ulRet=0x%08lX", ulRet);
    }
    while (0);
    return (wRet);
}


WORD CFillTxnData::SetTokenValue(ST_HSOAP_REQUEST* pcHsoapRequest, char* pszTokenValue, BOOL bCardType)
{
    WORD wRet = 0;
    unsigned long ulRet = 0;

    TraceMsg("entry");
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        if (pszTokenValue == NULL || strlen(pszTokenValue)==0)
        {
            TraceMsg("pszTokenValue == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        pcHsoapRequest->cCardData.nChoice = HSOAP_TOKEN_DATA;
        if (bCardType == d_CREDIT_CARD_TYPE)
        {
            memset(&pcHsoapRequest->cCardData.cTokenData, 0, sizeof(pcHsoapRequest->cCardData.cTokenData));
            memcpy(pcHsoapRequest->cCardData.cTokenData.szTokenValue, pszTokenValue, strlen(pszTokenValue));
            pcHsoapRequest->cCardData.cTokenData.cCommData.szCardPresent[0] = 'N';
            pcHsoapRequest->cCardData.cTokenData.cCommData.szReaderPresent[0] = 'N';
        }
        else if(bCardType == d_DEBIT_CARD_TYPE)
        {
            memset(&pcHsoapRequest->szTokenValue, 0, sizeof(pcHsoapRequest->szTokenValue));
            memcpy(pcHsoapRequest->szTokenValue, pszTokenValue, strlen(pszTokenValue));
        }
        TraceMsg("TokenValue:%s", pcHsoapRequest->cCardData.cTokenData.szTokenValue);
        memset(&pcHsoapRequest->cCardData.szTokenRequest, 0, sizeof(pcHsoapRequest->cCardData.szTokenRequest));
    }
    while (0);
    return (wRet);    
}


WORD CFillTxnData::SetInteracPOSSeqNbr(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    WORD wRet = 0;
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;
    unsigned char *pValue = NULL;
    
    TraceMsg("%s Entry", __FUNCTION__);
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        if (pcTransParam == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        ulRet = CTlvFun::TLVListReadTagValue(d_TAG_HEARTLAND_POS_SEQUENCE_NBR,
                        &pcTransParam->ucaRequestData[0],
                        pcTransParam->usRequestDataLen,
                        &pValue,
                        &usValueLen);
        if(!ulRet)
        {
            memcpy(pcHsoapRequest->szPosSequenceNbr, pValue, usValueLen);
            TraceMsg("PosSequenceNbr:%s", pcHsoapRequest->szPosSequenceNbr);
            break;
        }
    }
    while (0);
    
    TraceMsg("%s Exit", __FUNCTION__);
    return (wRet);
}

WORD CFillTxnData::SetInteracAccountType(ST_HSOAP_REQUEST* pcHsoapRequest, HEARTLAND_TRANS_PARAM* pcTransParam)
{
    WORD wRet = 0;
    unsigned long ulRet = 0;
    unsigned short usValueLen = 0;
    unsigned char *pValue = NULL;

    TraceMsg("%s Entry", __FUNCTION__);
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        
        if (pcTransParam == NULL)
        {
            TraceMsg("hsoapData == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        ulRet = CTlvFun::TLVListReadTagValue(d_TAG_HEARTLAND_ACCOUNT_TYPE,
                        &pcTransParam->ucaRequestData[0],
                        pcTransParam->usRequestDataLen,
                        &pValue,
                        &usValueLen);
        if(!ulRet)
        {
            memcpy(pcHsoapRequest->szAccountType, pValue, usValueLen);
            TraceMsg("AccountType:%s", pcHsoapRequest->szAccountType);
            break;
        }
    }
    while (0);
    
    TraceMsg("%s Exit", __FUNCTION__);
    return (wRet);
}

/**
* SetReversalData
*
* Function:
*      Set SetReversalData from pcHSoapRequest
*
* @param:
*      [OUT]pcHSoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
**/
WORD CFillTxnData::SetReversalData(ST_HSOAP_REQUEST* pcHSoapRequest)
{
    do 
    {
        // Set Trans type
        if (tt_CREDIT_SALE == pcHSoapRequest->nTransactionType || 
            tt_CREDIT_AUTH == pcHSoapRequest->nTransactionType)
        {
            pcHSoapRequest->nTransactionType = tt_CREDIT_REVERSAL;
        }
        else if (tt_DEBIT_SALE == pcHSoapRequest->nTransactionType)
        {
            pcHSoapRequest->nTransactionType = tt_DEBIT_REVERSAL;
        }
        else
        {
            TraceMsg("Original TransactionType:%d doesn't support reversal", pcHSoapRequest->nTransactionType);
            break;
        }
        
        // Set Allow PartialAuth
        pcHSoapRequest->szAllowPartialAuth[0] = '\0';

        // Set Allow Dup
        pcHSoapRequest->szAllowDup[0] = '\0';
        
        // Reset Trans Descriptor
        memset(pcHSoapRequest->szTxnDescriptor, 0, sizeof(pcHSoapRequest->szTxnDescriptor));
        
        // Reset Header Optional Fields
        m_pcSoapKernel->HeartlandSOAP_ResetHeaderOptionalFields();
        
        FILE *pFile = fopen(HEARTLAND_REVERSAL_FILE_NAME, "wb");
        if (NULL == pFile)
        {
            TraceMsg("Open reversal for write failed");
            break;
        }

        int writeFileRtn = CFileFun::WriteFile(pcHSoapRequest, 1, sizeof(ST_HSOAP_REQUEST), pFile);
        fclose(pFile);
        
        TraceMsg("WriteFile %s, return: %d\n", writeFileRtn==0 ? "successful" : "fail", writeFileRtn);
    }while (0);

    return 0;
}


/**
* SetTagDataValue
*
* Function:
*      Set pcHsoapRequest->TagDataValue from pcTransParam
*
* @param:
*      [OUT]pcHsoapRequest - The request soap structure point
*
* @return:
*      OK  - Successful
*      other - Please refer to ErrorCode.h
**/
ULONG CFillTxnData::SetTagDataValue(ST_HSOAP_REQUEST *pcHsoapRequest, BYTE *baTagName, BYTE *baTlvListPtr, USHORT usTlvListLen, bool necessary)
{
    static BYTE *_baTlvListPtr = NULL;
    static USHORT _usTlvListLen = 0;

    WORD wRet = 0;
    do
    {
        if (pcHsoapRequest == NULL)
        {
            TraceMsg("pcHsoapRequest == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }
        if (baTagName == NULL)
        {
            TraceMsg("baTagName == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        if (baTlvListPtr != NULL)
        {
            _baTlvListPtr = baTlvListPtr;
            _usTlvListLen = usTlvListLen;
        }

        if (_baTlvListPtr == NULL)
        {
            TraceMsg("_baTlvListPtr == NULL");
            wRet = ERROR_INVALID_PARAM;
            break;
        }

        BYTE *pbTlvPkt;
        USHORT usTlvPktLen;
        BYTE baTagNameHex[100];

        memset(baTagNameHex, 0, sizeof(baTagNameHex));
        USHORT tagNameLen = strlen((char *)baTagName);

        //UTIL_Str2Hex(baTagName, baTagNameHex, tagNameLen);
        //cTmpData.CopyToStr((char *)baTagNameHex, cTmpData.GetLength()+1);
        
        CFunBase cFunBase;
        CByteStream cTmpData;
        
        cFunBase.Str2Hex((char *)baTagName, cTmpData);
        memcpy(baTagNameHex, cTmpData.GetBuffer(), cTmpData.GetLength());

        if (CTlvFun::TLVListReadTagPkt(baTagNameHex, _baTlvListPtr, _usTlvListLen, (unsigned char **)&pbTlvPkt, &usTlvPktLen) == 0 &&
            pbTlvPkt != NULL && usTlvPktLen != 0)
        {
            BYTE tlvPktString[100];
            unsigned short tagValuesLen = 0;

            //int stringLength = CStringFun::Hex2Str(pbTlvPkt, tlvPktString, usTlvPktLen);
            cTmpData = cFunBase.Hex2Str(pbTlvPkt, usTlvPktLen);
            int stringLength = cTmpData.GetLength(); //CStringFun::Hex2Str(pbTlvPkt, tlvPktString, usTlvPktLen);
            memcpy(tlvPktString, cTmpData.GetBuffer(), stringLength);
            tlvPktString[stringLength] = 0x00;
            // TraceMsg("Get Tag Pkt %s: %s", baTagName, tlvPktString);

            if (memcmp(tlvPktString, d_TAG_TAC_DECLINE, strlen(d_TAG_TAC_DECLINE)) == 0)
            {
                memcpy(tlvPktString, "FFC7", strlen("FFC7"));
                memcpy(&tlvPktString[4], &tlvPktString[6], stringLength - 6);
                stringLength -= 2;
            }
            else if (memcmp(tlvPktString, d_TAG_TAC_ONLINE_1, strlen(d_TAG_TAC_ONLINE_1)) == 0)
            {
                memcpy(tlvPktString, "FFC8", strlen("FFC8"));
                memcpy(&tlvPktString[4], &tlvPktString[6], stringLength - 6);
                stringLength -= 2;
            }
            else if (memcmp(tlvPktString, d_TAG_TAC_DEFAULT_1, strlen(d_TAG_TAC_DEFAULT_1)) == 0)
            {
                memcpy(tlvPktString, "FFC6", strlen("FFC6"));
                memcpy(&tlvPktString[4], &tlvPktString[6], stringLength - 6);
                stringLength -= 2;
            }

            tagValuesLen = strlen(pcHsoapRequest->cTagData.szTagValues);
            memcpy(&pcHsoapRequest->cTagData.szTagValues[tagValuesLen], tlvPktString, stringLength);
        }
        else if (necessary)
        {
            TraceMsg("Get Tag Pkt %4s Failed", baTagName);
            wRet = ERROR_GET_TAGDATA_FAIL;
            break;
        }
    }
    while (0);

    return wRet;
}


WORD CFillTxnData::SetTxnResult(HEARTLAND_TRANS_PARAM* pTransParam, char* pszTag, char* pszValue)
{
    if (NULL == pTransParam || NULL == pszTag || NULL == pszValue)
    {
        TraceMsg("ERROR: SetTxnResult Invalid Parameter, Tag [%s] Failed", pszTag);
        return 1;
    }
    
    if(strlen(pszValue) == 0)
    {
        TraceMsg("ERROR: SetTxnResult \"pszValue\" is empty, Tag [%s] Failed", pszTag);
        return 1;
    }
    
    WORD wRet = CTlvFun::TLVFormatAndUpdateList(pszTag, strlen(pszValue), (BYTE *)pszValue, &pTransParam->bResultLen, pTransParam->bResult);
    if (wRet != 0) 
    {
        TraceMsg("ERROR: SetTxnResult TLVFormatAndUpdateList, Tag [%s] Failed, wRet:%d", pszTag, wRet);
    }
    return wRet;
}


WORD CFillTxnData::SetTxnResult(HEARTLAND_TRANS_PARAM* pTransParam, char* pszTag, char* pszValue, int nLength)
{
    if (NULL == pTransParam || NULL == pszTag || NULL == pszValue)
    {
        TraceMsg("ERROR: SetTxnResult Invalid Parameter, Tag [%s] Failed", pszTag);
        return 1;
    }
    
    WORD wRet = CTlvFun::TLVFormatAndUpdateList(pszTag, nLength, (BYTE *)pszValue, &pTransParam->bResultLen, pTransParam->bResult);
    if (wRet != 0) 
    {
        TraceMsg("ERROR: SetTxnResult TLVFormatAndUpdateList, Tag [%s] Failed, wRet:%d", pszTag, wRet);
    }
    return wRet;
}