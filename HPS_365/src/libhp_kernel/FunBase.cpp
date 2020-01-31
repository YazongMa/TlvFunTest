
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ByteStream.h"
#include "FunBase.h"
#include "Inc/ErrorCode.h"

CFunBase::CFunBase(void)
{
}

CFunBase::~CFunBase(void)
{
}

void CFunBase::SetByteBit(BYTE& by, int idx, BOOL b)
{
    BYTE tb = 0xff;
    tb >>= (8- idx - 1);
    tb <<= 7;
    tb >>= idx;
    if (b)
        by |= tb;
    else
        by &= ~tb;
}

BOOL CFunBase::GetByteBit(const BYTE by, int idx)
{
    BYTE tb = by;
    tb >>= (8- idx - 1);
    tb <<= 7;
    return (tb != 0);
}


void CFunBase::Byte2Bit(BYTE byData, CByteStream &cRet)
{
    for(int i=0; i<8; i++)
    {
        if(GetByteBit(byData, i))
            cRet += "1";
        else
            cRet += "0";
    }
}

DWORD CFunBase::ASCIIToDec(BYTE* buffer, BYTE Len)
{
    BYTE i;
    ULONG sum;

    sum = 0;
    for (i = 0; i < Len; i++)
    {
        if ( buffer[i] >= '0' && buffer[i] <= '9' )
            sum = sum * 10 + (buffer[i] - 0x30);
        else
            sum = sum * 10;
    }

    return sum;
}

DWORD CFunBase::ASCIIToDec(char * pszStr)
{
    return ASCIIToDec((PBYTE)pszStr, strlen(pszStr));
}

void CFunBase::DecToASCII(DWORD dwValue, BYTE* buffer, BYTE Digit)
{
    BYTE Mod;
    ULONG mValue;

    mValue = dwValue;

    while (Digit > 0)
    {
        Mod = mValue % 10;
        Mod = Mod + 0x30;
        mValue = mValue / 10;
        buffer[Digit - 1] = Mod;
        Digit--;
    }
}

DWORD CFunBase::FormatDataLength(CByteStream &cSrc, int nFormatLen, char ch, DWORD dwJustifiedType)
{
    CByteStream cData = cSrc;

    PBYTE pBuf = (PBYTE) new BYTE[nFormatLen];
    int nLen = cSrc.GetLength();
    memset(pBuf, ch, nFormatLen);
    if(nLen >= nFormatLen)
        return 0;

    if(dwJustifiedType == JUSTIFIED_LEFT)
    {
        cSrc.Write(pBuf, nFormatLen-nLen);
    }
    else if(dwJustifiedType == JUSTIFIED_RIGHT)
    {
        cSrc.Empty();
        cSrc.Write(pBuf, nFormatLen-nLen);
        cSrc<<cData;
    }
    else
        return  ERROR_INVALID_PARAM;

    return 0;
}

void CFunBase::DecToASCII(DWORD dwValue, CByteStream &cRet)
{
    BYTE byBuf[16]={0};

    DecToASCII(dwValue, byBuf, 8);

    cRet.Empty();
    cRet.Write(byBuf, strlen((char*)byBuf));
 }

BYTE CFunBase::Toupper(BYTE ch)
{
    //a - z = 0x61 - 0x7A
    //A - Z = 0x41 - 0x5A
    if ((ch >= 'a') && (ch <= 'z')) 
    {
        ch -= 0x20;
    }
    
    return ch;
}

BYTE CFunBase::PackByte(BYTE h, BYTE l)
{
    unsigned char i, j, k;
    short z;

    j = Toupper(h);
    k = Toupper(l);

    if ((j >= '0' && j <= '9') || (j >= 'A' && j <= 'Z'))
        ;
    else
        j = '0';

    if ((k >= '0' && k <= '9') || (k >= 'A' && k <= 'Z'))
        ;
    else
        k = '0';

    if ((j >= 'A') && (j <= 'F')) 
    {
        z = 10;
        z += (short) j;
        z -= 65;
        i = (BYTE) (0xFF & z);
    }
    else 
    {
        i = j - '0';
    }

    i <<= 4;

    if ((k >= 'A') && (k <= 'F')) 
    {
        z = 10;
        z += (short) k;
        z -= 65;
        i |= (BYTE) (0xFF & z);
    }
    else 
    {
        i |= (k - '0');
    }

    return i;
}

int CFunBase::Str2BCD(unsigned char *str, unsigned char *hex, unsigned int nLen)
{
    unsigned int i;

    for (i = 0; i < nLen / 2; i ++) 
    {
        hex[i] = PackByte(str[i * 2], str[i * 2 + 1]);
    }

    return nLen / 2;
}

DWORD CFunBase::Str2BCD(CByteStream *pcSrc, CByteStream &cOutData)
{
    if(pcSrc == NULL)
        return ERROR_INVALID_PARAM;

    int nLen = pcSrc->GetLength()+1;
    
    PBYTE pbyHex = (PBYTE)new BYTE[nLen];
    memset(pbyHex, 0, nLen);

    int nRet = Str2BCD(pcSrc->GetBuffer(), pbyHex, nLen-1);
    cOutData.Empty();
    cOutData.Write(pbyHex, nRet);

    delete [] pbyHex;

    return 0;
}

DWORD CFunBase::Bcd2Str(CByteStream *pcSrc, CByteStream &cOut)
{
    if(pcSrc == NULL)
        return ERROR_INVALID_PARAM;

    PBYTE src_ptr = pcSrc->GetBuffer();
    int nLen = pcSrc->GetLength();
    cOut.Empty();

    while(nLen--) 
    {
        cOut<<(BYTE) (((*src_ptr & 0xf0) <= 0x90 ? '0' : 'A') + ((*src_ptr >> 4)%10));
        cOut<<(BYTE) (((*src_ptr & 0x0f) <= 0x09 ? '0' : 'A') + ((*src_ptr & 0x0f)%10));
        src_ptr++;
    }

    return 0;
}


void CFunBase::Str2Hex(PCSTR pStr, CByteStream & cOut)
{
    int nLen = strlen(pStr);
    cOut.Empty();

    char szTemp[4] = {0};
    for(int i=0; i<nLen/2; i++)
    {
        szTemp[0] = pStr[i*2];
        szTemp[1] = pStr[i*2+1];
        cOut<<(BYTE)strtol(szTemp, NULL, 16);
    }
}


BOOL CFunBase::Str2Hex(const char* pStr, BYTE* cOut)
{
    int nLen = 0;
    char szTemp[4] = {0};

    if (NULL == pStr)
    {
        printf(" Line:%d, Func:%s ERROR:NULL String \n", __LINE__, __func__);
        return FALSE;
    }

    nLen = strlen(pStr);
    for(int i=0; i<nLen/2; i++)
    {
        memset(szTemp, 0, 2);
        szTemp[0] = pStr[i*2];
        szTemp[1] = pStr[i*2+1];
        cOut[i] = (BYTE)strtol(szTemp, NULL, 16);
//        memset(szTemp, 0, 2);
    }

    return TRUE;
}


DWORD CFunBase::Str2Hex(unsigned char *str, unsigned char *hex, unsigned int nLen)
{
    unsigned int i;
    for (i = 0; i < nLen / 2; i++)
    {
        hex[i] = CFunBase::PackByte(str[i * 2], str[i * 2 + 1]);
    }
    return nLen / 2;
}



CByteStream CFunBase::Hex2Str(PBYTE pbyData, int nSize, WORD  dwFormat)
{
    char szBuf[8]={0};
    CByteStream cRet;

    for(int i=0; i<nSize; i++)
    {
        memset(szBuf, 0, 8);
        sprintf(szBuf, "%02X", pbyData[i]);
        cRet += szBuf;
        if(dwFormat != 0)
        {
            if(dwFormat & FMT_SPACE)
                cRet += " ";
            if(dwFormat & FMT_16NEWLINE)
            {
                if((((i+1)%16)==0) && (i>0))
                {
                    cRet<<(BYTE)0x0D;
                    cRet<<(BYTE)0x0A;
                }
            }
            else if(dwFormat & FMT_32NEWLINE)
            {
                if((((i+1)%32)==0) && (i>0))
                {
                    cRet<<(BYTE)0x0D;
                    cRet<<(BYTE)0x0A;
                }
            }
        }
    }

    return cRet;
}

DWORD CFunBase::Hex2Str(CByteStream *pcSrc, CByteStream &cOut, WORD  dwFormat)
{
    if(pcSrc == NULL)
        return ERROR_INVALID_PARAM;

    PBYTE pbyData = pcSrc->GetBuffer();
    int nSize = pcSrc->GetLength();
    char szBuf[4];
    cOut.Empty();

    for(int i=0; i<nSize; i++)
    {
        memset(szBuf, 0, 4);
        sprintf(szBuf, "%02X", pbyData[i]);
        cOut += szBuf;
        if(dwFormat != 0)
        {
            if(dwFormat & FMT_SPACE)
                cOut += " ";
            if(dwFormat & FMT_16NEWLINE)
            {
                if((((i+1)%16)==0) && (i>0))
                {
                    cOut<<(BYTE)0x0D;
                    cOut<<(BYTE)0x0A;
                }
            }
            else if(dwFormat & FMT_32NEWLINE)
            {
                if((((i+1)%32)==0) && (i>0))
                {
                    cOut<<(BYTE)0x0D;
                    cOut<<(BYTE)0x0A;
                }
            }

        }
    }

    return 0;
}


BOOL CFunBase::Hex2Str(BYTE* pbyStrData, BYTE* pbyHexData, int nSize)
{
    char szBuf[8 + 1] = {0};

    if (NULL == pbyHexData || nSize < 0)
    {
        printf(" Line:%d, Func:%s ERROR:Bad Parameter \n", __LINE__, __func__);
        return FALSE;
    }

    for(int i=0; i<nSize; i++)
    {
        memset(szBuf, 0, sizeof(szBuf));
        sprintf(szBuf, "%02X", pbyHexData[i]);
        memcpy(pbyStrData + i*2, szBuf, 2);
    }

    return TRUE;
}


void CFunBase::Dec2Bcd(DWORD dwValue, CByteStream &cOut)
{
    cOut.Empty();
    CByteStream cTemp;
    
    DecToASCII(dwValue, cTemp);
    cTemp.TrimLeft('0');
    int nLen = cTemp.GetLength();

    if((nLen%2) != 0)
        FormatDataLength(cTemp, nLen+1, '0', JUSTIFIED_RIGHT);

    Str2BCD(&cTemp, cOut);
}

DWORD CFunBase::Bcd2Dec(CByteStream *pcSrc)
{
    CByteStream cOut;
    Bcd2Str(pcSrc, cOut);
    return ASCIIToDec((PSTR)cOut.PushZero());

//     if(pcSrc == NULL)
//         return ERROR_INVALID_PARAM;
// 
//     CString csRet = Hex2Str(pcSrc->GetBuffer(), pcSrc->GetLength());
//     return ASCIIToDec(csRet);
}
