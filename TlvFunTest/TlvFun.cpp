#include "deftype.h"
#include <stdio.h>
#include <string.h>

#include "TlvFun.h"

#define d_UTIL_READ_TAG_VALUE_FAILED   1
#define d_UTIL_UPDATE_TLV_LIST_FAILED  2


static unsigned char baTempTlvList[0x800];
static unsigned char Toupper(unsigned char ch);
static unsigned char PackByte(unsigned char h, unsigned char l);
static unsigned int Str2Hex(unsigned char *str, unsigned char *hex, unsigned int nLen);

unsigned short CTlvFun::TLVGetTagLength(unsigned char *pbTag)
{
    unsigned short tagLen = 0, cnt = 0;

    if ((pbTag == NULL) || (*pbTag == 0))
    {
        return 0;
    }
    tagLen = 1;
    cnt = 0;
    if ((pbTag[cnt]&0x1F) == 0x1F)
    {
        tagLen++;
        cnt++;
        while (pbTag[cnt] & 0x80)
        {
            tagLen++;
            cnt++;
            if (tagLen > 4)
            { //limit the tag len max is 4
                return 0;
            }
        }
    }
    return tagLen;
}


unsigned short CTlvFun::TLVGetLengthLength(
        unsigned char *pbLength, 
        unsigned short *pusValueLen)
{
    unsigned short lengthLen = 0;

    if (*pbLength < 0x80)
    {
        lengthLen = 1;
        *pusValueLen = *pbLength;
    }
    else if (*pbLength == 0x81)
    {
        lengthLen = 2;
        *pusValueLen = pbLength[1];
    }
    else if (*pbLength == 0x82)
    {
        lengthLen = 3;
        *pusValueLen = ((pbLength[1] << 8) + pbLength[2]);
    }
    else if (*pbLength == 0x83)
    {
        lengthLen = 4;
        *pusValueLen = ((pbLength[1] << 16) + (pbLength[2] << 8) + pbLength[3]);
    }
    else
    {
        lengthLen = 0;
    }
    return lengthLen;
}


unsigned short CTlvFun::TLVListCheckFormat(
        unsigned char *baTlvListBuf, 
        unsigned short usTlvListLen)
{
    unsigned short tagLen = 0, lenLen = 0, valueLen = 0, totalLen = 0;
    unsigned char *pTlv = NULL;

    pTlv = baTlvListBuf;
    totalLen = 0;
    while (totalLen < usTlvListLen)
    {
        tagLen = TLVGetTagLength(&pTlv[totalLen]);
        if (tagLen == 0)
        {
            return 1;
        }
        totalLen += tagLen;
        lenLen = TLVGetLengthLength(&pTlv[totalLen], &valueLen);
        if (lenLen == 0)
        {
            return 1;
        }
        totalLen += lenLen;
        totalLen += valueLen;
    }
    if (totalLen == usTlvListLen)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}


unsigned short CTlvFun::TLVListReadTagPkt(
        unsigned char *baTagName, 
        unsigned char *baTlvListBuf, 
        unsigned short usTlvListLen, 
        unsigned char **pbTlvPkt, 
        unsigned short *pusTlvPktLen)
{
    unsigned short tagLen, cnt, tempTagLen, tempLenLen, tempValueLen;

    //handle tag
    tagLen = TLVGetTagLength(baTagName);
    cnt = 0;
    while (memcmp(&baTlvListBuf[cnt], baTagName, tagLen) != 0)
    {
        if (cnt >= usTlvListLen)
        {
            *pusTlvPktLen = 0;
            return 1;
        }
        tempTagLen = TLVGetTagLength(&baTlvListBuf[cnt]);
        cnt += tempTagLen;
        tempLenLen = TLVGetLengthLength(&baTlvListBuf[cnt], &tempValueLen);
        cnt += tempLenLen;
        cnt += tempValueLen;
    }
    *pbTlvPkt = &baTlvListBuf[cnt];
    tempTagLen = TLVGetTagLength(&baTlvListBuf[cnt]);
    cnt += tempTagLen;
    tempLenLen = TLVGetLengthLength(&baTlvListBuf[cnt], &tempValueLen);
    *pusTlvPktLen = (tempTagLen + tempLenLen + tempValueLen);
    return 0;
}


unsigned short CTlvFun::TLVListReadTagValue(
        char *caTagName, 
        unsigned char *baTlvListBuf,
        unsigned short usTlvListLen, 
        unsigned char **pbTlvValue, 
        unsigned short *pusTlvValueLen)
{
    unsigned short tagLen, cnt, tempTagLen, tempLenLen, tempValueLen;
    unsigned char baTagName[4];

    if (strlen(caTagName) < 2)
    {
        *pusTlvValueLen = 0;
        return 1;
    }
    //handle tag
    tagLen = Str2Hex((unsigned char *)caTagName, baTagName, strlen(caTagName));
    //handle tag
    //tagLen = TLV_GetTagLength(baTagName);
    cnt = 0;
    while (memcmp(&baTlvListBuf[cnt], baTagName, tagLen) != 0)
    {
        tempTagLen = TLVGetTagLength(&baTlvListBuf[cnt]);
        cnt += tempTagLen;
        tempLenLen = TLVGetLengthLength(&baTlvListBuf[cnt], &tempValueLen);
        cnt += tempLenLen;
        cnt += tempValueLen;
        if (cnt >= usTlvListLen)
        {
            *pusTlvValueLen = 0;
            return 1;
        }
    }
    tempTagLen = TLVGetTagLength(&baTlvListBuf[cnt]);
    cnt += tempTagLen;
    tempLenLen = TLVGetLengthLength(&baTlvListBuf[cnt], &tempValueLen);
    cnt += tempLenLen;
    *pbTlvValue = &baTlvListBuf[cnt];
    *pusTlvValueLen = tempValueLen;
    return 0;
}


unsigned short CTlvFun::TLVListAddTag(
        unsigned char *baTlvListBuf, 
        unsigned short *pusTlvListLen,
        unsigned char *baTlvPkt, 
        unsigned short usTlvPktLen)
{
    unsigned short rtn, usPktLen, usTagLen, usListLen, cnt, idx;
    unsigned char baTagName[4], *pbTlv;

    if (usTlvPktLen == 0)
    {
        return 0;
    }
    usListLen = *pusTlvListLen;
    usTagLen = TLVGetTagLength(baTlvPkt);
    memcpy(baTagName, baTlvPkt, usTagLen);
    rtn = TLVListReadTagPkt(baTagName, baTlvListBuf, usListLen, &pbTlv, &usPktLen);
    if (rtn == 0)
    {//found in original list
        // replace if pkt length is the same
        if (usPktLen == usTlvPktLen)
        {
            memcpy(pbTlv, baTlvPkt, usTlvPktLen);
        }
        else
        {
            //copy to temp list
            memset(baTempTlvList, 0, sizeof(baTempTlvList));
            cnt = 0;
            memcpy(&baTempTlvList[cnt], baTlvPkt, usTlvPktLen);
            cnt += usTlvPktLen;

            idx = 0;
            while (idx < usListLen)
            {
                TLVListReadTagPkt(&baTlvListBuf[idx], baTlvListBuf, usListLen, &pbTlv, &usPktLen);
                if (memcmp(baTagName, &baTlvListBuf[idx], usTagLen))
                {
                    memcpy(&baTempTlvList[cnt], pbTlv, usPktLen);
                    cnt += usPktLen;
                    idx += usPktLen;
                }
                else
                {
                    idx += usPktLen;
                }
            } /* while (idx < usListLen)*/
            //copy back from temp
            memset(baTlvListBuf, 0, usListLen);
            memcpy(baTlvListBuf, baTempTlvList, cnt);
            *pusTlvListLen = cnt;
        }
    }/* if (rtn == 0) */
    else
    {
        // if not found at original list, add at the end
        memcpy(&baTlvListBuf[usListLen], baTlvPkt, usTlvPktLen);
        *pusTlvListLen = (usListLen + usTlvPktLen);
    }
    return 0;
}

// input tag is ascii, other is hex


unsigned short CTlvFun::TLVFormatTlv(
        char *caTagName,
        unsigned short usTagLen, 
        unsigned char *baTagData,
        unsigned short *pusTlvPktLen, 
        unsigned char *baTlvPktBuf)
{
    unsigned short tempLen, cnt;

    if (strlen(caTagName) < 2)
    {
        *pusTlvPktLen = 0;
        return 0;
    }
    //handle tag
    cnt = 0;
    tempLen = Str2Hex((unsigned char *)caTagName, &baTlvPktBuf[cnt], strlen(caTagName));
    cnt += tempLen;

    //handle length
    tempLen = usTagLen;
    if (tempLen > 0xFF)
    {
        baTlvPktBuf[cnt++] = 0x82;
        baTlvPktBuf[cnt++] = (unsigned char)(tempLen >> 8);
        baTlvPktBuf[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    else if (tempLen > 0x7F)
    {
        baTlvPktBuf[cnt++] = 0x81;
        baTlvPktBuf[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    else
    {
        baTlvPktBuf[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    //handle value
    memcpy(&baTlvPktBuf[cnt], baTagData, usTagLen);
    cnt += usTagLen;
    *pusTlvPktLen = cnt;
    return 0;
}


unsigned short CTlvFun::TLVFormatAndUpdateList(
        char *caTagName,
        unsigned short usValueLen, 
        unsigned char *baValueData,
        unsigned short *pusTlvListLen, 
        unsigned char *baTlvListBuf)
{
    unsigned short tempLen, cnt;
    unsigned short usTlvPktLen;
    unsigned char baTlvPkt[4096];

    if (strlen(caTagName) < 2)
    {
        return 1;
    }
    if (usValueLen == 0)
    {
        return 1;
    }
    
    //handle tag
    cnt = 0;
    tempLen = Str2Hex((unsigned char *)caTagName, &baTlvPkt[cnt], strlen(caTagName));
    cnt += tempLen;
    //handle length
    tempLen = usValueLen;
    if (tempLen > 0xFF)
    {
        baTlvPkt[cnt++] = 0x82;
        baTlvPkt[cnt++] = (unsigned char)(tempLen >> 8);
        baTlvPkt[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    else if (tempLen > 0x7F)
    {
        baTlvPkt[cnt++] = 0x81;
        baTlvPkt[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    else
    {
        baTlvPkt[cnt++] = (unsigned char)(tempLen & 0x00FF);
    }
    //handle value
    memcpy(&baTlvPkt[cnt], baValueData, usValueLen);
    cnt += usValueLen;
    usTlvPktLen = cnt;
    TLVListAddTag(baTlvListBuf, pusTlvListLen, baTlvPkt, usTlvPktLen);
    return 0;
}


unsigned long CTlvFun::TLVFormatAndUpdateListFromAnotherList(
        char caTagName[],
        unsigned short *pusTlvListDstLen, 
        unsigned char ucaTlvListDst[],
        unsigned short usTlvListSrcLen, 
        unsigned char ucaTlvListSrc[])
{
    if (caTagName == NULL ||
        pusTlvListDstLen == NULL || ucaTlvListDst == NULL ||
        ucaTlvListSrc == NULL)
    {
        return 1;
    }

    unsigned char *pucValue;
    unsigned short usValueLength;

    unsigned long ulRet = TLVListReadTagValue(caTagName, ucaTlvListSrc, usTlvListSrcLen, &pucValue, &usValueLength);
    if (ulRet)
    {
        return d_UTIL_READ_TAG_VALUE_FAILED;
    }

    ulRet = TLVFormatAndUpdateList(caTagName, usValueLength, pucValue, pusTlvListDstLen, ucaTlvListDst);
    if (ulRet)
    {
        return d_UTIL_UPDATE_TLV_LIST_FAILED;
    }

    return 0;
}


unsigned long CTlvFun::TLVReadAndUpdateListFromAnotherList(
        char caTagName[],
        unsigned char **ppucTlvValue, 
        unsigned short *pusTlvValueLen,
        unsigned short *pusTlvListDstLen, 
        unsigned char ucaTlvListDst[],
        unsigned short usTlvListSrcLen, 
        unsigned char ucaTlvListSrc[])
{
    if (caTagName == NULL ||
        ppucTlvValue == NULL || pusTlvValueLen == NULL ||
        pusTlvListDstLen == NULL || ucaTlvListDst == NULL ||
        ucaTlvListSrc == NULL)
    {
        return 1;
    }

    unsigned long ulRet = TLVListReadTagValue(caTagName, ucaTlvListSrc, usTlvListSrcLen, ppucTlvValue, pusTlvValueLen);
    if (ulRet)
    {
        return 1;
    }

    ulRet = TLVFormatAndUpdateList(caTagName, *pusTlvValueLen, *ppucTlvValue, pusTlvListDstLen, ucaTlvListDst);
    if (ulRet)
    {
        return 1;
    }

    return 0;
}


unsigned int Str2Hex(unsigned char *str, unsigned char *hex, unsigned int nLen)
{
    unsigned int i;
    for (i = 0; i < nLen / 2; i++)
    {
        hex[i] = PackByte(str[i * 2], str[i * 2 + 1]);
    }
    return nLen / 2;
}


unsigned char PackByte(unsigned char h, unsigned char l)
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
        z += (short)j;
        z -= 65;
        i = (unsigned char)(0xFF & z);
    }
    else
    {
        i = j - '0';
    }

    i <<= 4;

    if ((k >= 'A') && (k <= 'F'))
    {
        z = 10;
        z += (short)k;
        z -= 65;
        i |= (unsigned char)(0xFF & z);
    }
    else
    {
        i |= (k - '0');
    }
    return i;
}


unsigned char Toupper(unsigned char ch)
{
    //a - z = 0x61 - 0x7A
    //A - Z = 0x41 - 0x5A
    if ((ch >= 'a') && (ch <= 'z'))
    {
        ch -= 0x20;
    }
    return ch;
}