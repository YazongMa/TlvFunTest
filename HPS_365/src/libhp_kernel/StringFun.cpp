/**
 **    A Template for developing new terminal shared application
 **/

#include <string.h>
#include <stdio.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "FunBase.h"
#include "ByteStream.h"

#include "StringFun.h"

static CFunBase sFunBase;


//static unsigned char UTIL_Hex2Ascii(unsigned char chHex);
//static void UTIL_StrAppendByteHex(unsigned char *str, unsigned char ch);
//static unsigned int UTIL_FindStrEndPt(unsigned char *str);
static BOOL isCompressedTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen);

unsigned long CStringFun::CopyStringIfExist(char* src, char* dst)
{
    if (src == NULL || dst == NULL)
    {
        return 1;
    }

    unsigned short usLen = strlen(src);
    if (usLen)
    {
        memcpy(dst, src, usLen);
        dst[usLen] = '\0';
        return 0;
    }
    else
    {
        dst[0] = '\0';
        return 1;
    }
}


bool CStringFun::MaskString(char caString[], char cFirstPlainCharacterCounts, char cLastPlainCharacterCounts, char cMaskChar)
{
    if (caString == NULL)
    {
        return false;
    }

    int iPanLength = strlen(caString);
    if (iPanLength < cFirstPlainCharacterCounts + cLastPlainCharacterCounts)
    {
        return false;
    }

    int iLastMaskedPos = iPanLength - cLastPlainCharacterCounts - 1;
    int i = 0;
    for (i = cFirstPlainCharacterCounts; i <= iLastMaskedPos && i < iPanLength; ++i)
    {
        caString[i] = cMaskChar;
    }

    return true;
}


void CStringFun::Track2ToFormatedStr(unsigned char *baHexData, unsigned short usHexDataLen, unsigned char *baFormatedStr, unsigned short *usFormatedStrLen)
{
    if (baHexData == NULL || usHexDataLen == 0 || baFormatedStr == NULL || usFormatedStrLen == NULL)
    {
        return;
    }
 
    if (baHexData[0] == ';')
    {
//        memcpy(baFormatedStr, baHexData, usHexDataLen - 1);
//        baFormatedStr[usHexDataLen] = '\0';
//        *usFormatedStrLen = usHexDataLen - 1;
        memcpy(baFormatedStr, baHexData, usHexDataLen);
        if (baFormatedStr[usHexDataLen - 2] == '?')
        {
            baFormatedStr[usHexDataLen-1] = '\0';
            *usFormatedStrLen = usHexDataLen-1;
        }
        else
        {
            baFormatedStr[usHexDataLen] = '\0';
            *usFormatedStrLen = usHexDataLen;
        }
    }
    else if(!isCompressedTrack2Data(baHexData, usHexDataLen))
    {
        baFormatedStr[0] = ';';
        memcpy(&baFormatedStr[1], baHexData, usHexDataLen);
        ++usHexDataLen;

        if (baFormatedStr[usHexDataLen - 2] == '?')
        {
            baFormatedStr[usHexDataLen-1] = '\0';
            *usFormatedStrLen = usHexDataLen-1;
        }
        else
        {
            baFormatedStr[usHexDataLen] = '\0';
            *usFormatedStrLen = usHexDataLen;
        }
    }
    else
    {
        //*usFormatedStrLen = UTIL_Hex2Str(baHexData, &baFormatedStr[1], usHexDataLen);

        CByteStream cTmpData = sFunBase.Hex2Str(baHexData, (int)usHexDataLen);
        cTmpData.CopyToStr((char *)&baFormatedStr[1], cTmpData.GetLength()+1);
        *usFormatedStrLen = cTmpData.GetLength();
        baFormatedStr[0] = ';';

        unsigned int i = 1;
        for (; i <= *usFormatedStrLen; ++i)
        {
            switch (baFormatedStr[i])
            {
                case 'D':
                    baFormatedStr[i] = '=';
                    break;
                case 'F':
                    baFormatedStr[i] = '0';
                    --(*usFormatedStrLen);
                    break;
                default:
                    break;
            }
        }
        baFormatedStr[++(*usFormatedStrLen)] = '?';
        baFormatedStr[++(*usFormatedStrLen)] = '\0';
    }
}


//unsigned int CStringFun::Hex2Str(unsigned char *hex, unsigned char *str, unsigned int nLen)
//{
//    unsigned int i;
//    str[0] = 0;
//    for (i = 0; i < nLen; i++)
//    {
//        UTIL_StrAppendByteHex(str, hex[i]);
//    }
//    return nLen * 2;
//}
//
//
//unsigned int CStringFun::Str2Hex(unsigned char *str, unsigned char *hex, unsigned int nLen)
//{
//    unsigned int i;
//    for (i = 0; i < nLen / 2; i++)
//    {
//        hex[i] = CStringFun::PackByte(str[i * 2], str[i * 2 + 1]);
//    }
//    return nLen / 2;
//}
//
//
//unsigned int CStringFun::Str2Dec(unsigned char* str, unsigned char* dec, unsigned int nLen)
//{
//    if ((str == NULL) || (dec == NULL))
//        return -1;
//
//    ULONG ulResult = 0;
//    for (int i = 0; i < nLen; i++)
//    {
//        ulResult = ulResult * 10 + (str[i] - '0');
//    }
//
//    if (ulResult > 255)
//        return -1;
//
//    dec[0] = (BYTE)ulResult;
//
//    return 0;
//}
//
//
//unsigned long CStringFun::Str2Long(unsigned char *str, unsigned int nLen)
//{
//    unsigned char i;
//    unsigned long k;
//    i = 0;
//    k = 0;
//    //while (str[i] != 0x00) {
//    while (i < nLen)
//    {
//        k += str[i++] - '0';
//        //if (str[i] != 0x00) {
//        if (i != nLen)
//        {
//            k *= 10;
//        }
//    }
//
//    return k;
//}
//
//
//unsigned char CStringFun::Toupper(unsigned char ch)
//{
//    //a - z = 0x61 - 0x7A
//    //A - Z = 0x41 - 0x5A
//    if ((ch >= 'a') && (ch <= 'z'))
//    {
//        ch -= 0x20;
//    }
//    return ch;
//}
//
//
//unsigned char CStringFun::PackByte(unsigned char h, unsigned char l)
//{
//    unsigned char i, j, k;
//    short z;
//
//    j = CStringFun::Toupper(h);
//    k = CStringFun::Toupper(l);
//
//    if ((j >= '0' && j <= '9') || (j >= 'A' && j <= 'Z'))
//        ;
//    else
//        j = '0';
//
//    if ((k >= '0' && k <= '9') || (k >= 'A' && k <= 'Z'))
//        ;
//    else
//        k = '0';
//
//    if ((j >= 'A') && (j <= 'F'))
//    {
//        z = 10;
//        z += (short)j;
//        z -= 65;
//        i = (unsigned char)(0xFF & z);
//    }
//    else
//    {
//        i = j - '0';
//    }
//
//    i <<= 4;
//
//    if ((k >= 'A') && (k <= 'F'))
//    {
//        z = 10;
//        z += (short)k;
//        z -= 65;
//        i |= (unsigned char)(0xFF & z);
//    }
//    else
//    {
//        i |= (k - '0');
//    }
//    return i;
//}
//
//
///*
// * @param[IN]      pkucOriginData         Origin data you want to encode.
// * @param[IN]      kuiOriginDataLength    Origin data length.
// * @param[OUT]     pucEncodeResult        Encoded result, should have allocated memory.
// * @param[IN,OUT]  puiEncodeResultLength  Input length of pucEncodeResult. Output the length of encode result.
// *
// * @return true for success or false for fail.
// */
//bool CStringFun::Base64Encode(const unsigned char *pkucOriginData, const unsigned int kuiOriginDataLength,
//                              unsigned char *pucEncodeResult, unsigned int *puiEncodeResultLength)
//{
//    if (pkucOriginData == NULL ||
//        kuiOriginDataLength == 0 ||
//        pucEncodeResult == NULL ||
//        puiEncodeResultLength == NULL ||
//        *puiEncodeResultLength == 0)
//    {
//
//        if (puiEncodeResultLength)
//        {
//            *puiEncodeResultLength = 0;
//        }
//        return false;
//    }
//
//    unsigned int uiOutputLength = 4 * ((kuiOriginDataLength + 2) / 3);
//    if (*puiEncodeResultLength < uiOutputLength)
//    {
//        *puiEncodeResultLength = 0;
//        return false;
//    }
//    *puiEncodeResultLength = uiOutputLength;
//
//    static const char kcaBase64EncodeTable[64] = {
//                                                  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
//                                                  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
//                                                  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
//                                                  '+', '/'
//    };
//    static int iaModeTable[] = {0, 2, 1};
//
//    int i = 0, j = 0;
//    for (; i < kuiOriginDataLength;)
//    {
//        uint32_t octet_a = i < kuiOriginDataLength ? (unsigned char)pkucOriginData[i++] : 0;
//        uint32_t octet_b = i < kuiOriginDataLength ? (unsigned char)pkucOriginData[i++] : 0;
//        uint32_t octet_c = i < kuiOriginDataLength ? (unsigned char)pkucOriginData[i++] : 0;
//
//        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
//
//        pucEncodeResult[j++] = kcaBase64EncodeTable[(triple >> 3 * 6) & 0x3F];
//        pucEncodeResult[j++] = kcaBase64EncodeTable[(triple >> 2 * 6) & 0x3F];
//        pucEncodeResult[j++] = kcaBase64EncodeTable[(triple >> 1 * 6) & 0x3F];
//        pucEncodeResult[j++] = kcaBase64EncodeTable[(triple >> 0 * 6) & 0x3F];
//    }
//
//    for (i = 0; i < iaModeTable[kuiOriginDataLength % 3]; i++)
//    {
//        pucEncodeResult[*puiEncodeResultLength - 1 - i] = '=';
//    }
//
//    return true;
//}
//
//
///*
// * @param[IN]      pkucEncodedData         Encoded data you want to decode.
// * @param[IN]      kuiEncodedDataLength    Encoded data length.
// * @param[OUT]     pucOriginData           Decoded result, should have allocated memory.
// * @param[IN,OUT]  puiOriginDataLength     Input length of pucOutputData. Output the length of encoded result.
// *
// * @return true for success or false for fail.
// */
//bool CStringFun::Base64Decode(const unsigned char *pkucEncodedData, const unsigned int kuiEncodedDataLength,
//                              unsigned char *pucDecodeResult, unsigned int *puiDecodeResultLength)
//{
//    if (pkucEncodedData == NULL ||
//        kuiEncodedDataLength == 0 ||
//        kuiEncodedDataLength % 4 != 0 ||
//        pucDecodeResult == NULL ||
//        puiDecodeResultLength == NULL ||
//        *puiDecodeResultLength == 0)
//    {
//        if (puiDecodeResultLength)
//        {
//            *puiDecodeResultLength = 0;
//        }
//        return false;
//    }
//
//    unsigned int uiOutputLength = kuiEncodedDataLength / 4 * 3;
//    if (pkucEncodedData[kuiEncodedDataLength - 1] == '=') uiOutputLength--;
//    if (pkucEncodedData[kuiEncodedDataLength - 2] == '=') uiOutputLength--;
//
//    if (*puiDecodeResultLength < uiOutputLength)
//    {
//        *puiDecodeResultLength = 0;
//        return false;
//    }
//
//    static unsigned char kcaBaseDecodeTable[128] = {
//                                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//                                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
//                                                    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
//                                                    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0
//    };
//
//    int i = 0, j = 0;
//    for (; i < kuiEncodedDataLength;)
//    {
//        uint32_t sextet_a = pkucEncodedData[i] == '=' ? 0 & i++ : kcaBaseDecodeTable[pkucEncodedData[i++]];
//        uint32_t sextet_b = pkucEncodedData[i] == '=' ? 0 & i++ : kcaBaseDecodeTable[pkucEncodedData[i++]];
//        uint32_t sextet_c = pkucEncodedData[i] == '=' ? 0 & i++ : kcaBaseDecodeTable[pkucEncodedData[i++]];
//        uint32_t sextet_d = pkucEncodedData[i] == '=' ? 0 & i++ : kcaBaseDecodeTable[pkucEncodedData[i++]];
//
//        uint32_t triple = (sextet_a << 3 * 6)
//                + (sextet_b << 2 * 6)
//                + (sextet_c << 1 * 6)
//                + (sextet_d << 0 * 6);
//
//        if (j < *puiDecodeResultLength) pucDecodeResult[j++] = (triple >> 2 * 8) & 0xFF;
//        if (j < *puiDecodeResultLength) pucDecodeResult[j++] = (triple >> 1 * 8) & 0xFF;
//        if (j < *puiDecodeResultLength) pucDecodeResult[j++] = (triple >> 0 * 8) & 0xFF;
//    }
//
//    return true;
//}
//
//
//unsigned int UTIL_FindStrEndPt(unsigned char *str)
//{
//    unsigned int i;
//
//    i = 0;
//    while (str[i] != 0x00)
//    {
//        i++;
//    }
//    return i;
//}
//
//
//unsigned char UTIL_Hex2Ascii(unsigned char chHex)
//{
//    if (chHex <= 9)
//    {
//        return chHex + '0';
//    }
//    else
//    {
//        return chHex - 10 + 'A';
//    }
//}
//
//
//void UTIL_StrAppendByteHex(unsigned char *str, unsigned char ch)
//{
//    unsigned int i;
//    i = UTIL_FindStrEndPt(str);
//    str[i++] = UTIL_Hex2Ascii(ch / 16);
//    str[i++] = UTIL_Hex2Ascii(ch % 16);
//    ;
//    str[i] = 0x00;
//}


BOOL isCompressedTrack2Data(const BYTE* pbyTrack2Data, const BYTE bTrack2DataLen)
{
    int nLoop = 0;
    char* pTmp = NULL;
    BOOL bRet = FALSE;
    do
    {
        if (!pbyTrack2Data)
        {
            break;
        }

        if (*pbyTrack2Data == ';')
        {
            break;
        }

        if (*pbyTrack2Data < '0' || *pbyTrack2Data > '9')
        {
            bRet = TRUE;
            break;
        }

        while(nLoop < bTrack2DataLen)
        {
            if (pbyTrack2Data[nLoop] == '=')
            {
                pTmp = (char *)&pbyTrack2Data[nLoop];
                break;
            }
            ++nLoop;
        }

        if (!pTmp)
        {
            bRet = TRUE;
            break;
        }

        while ((char *)pbyTrack2Data != pTmp)
        {
            if (*pbyTrack2Data < '0' || *pbyTrack2Data > '9')
            {
                bRet = TRUE;
            }
            pbyTrack2Data++;
        }
    }while (0);

    return bRet;
}