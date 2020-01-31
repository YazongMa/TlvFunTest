
/*
 * File:   StringFun.h
 *
 * Created on 2018.6.26 8:57
 */

#ifndef __HEARTLAND_STRINGFUN_H__
#define __HEARTLAND_STRINGFUN_H__

#include <stdio.h>
#include <typedef.h>

class CStringFun
{
public :
    static unsigned long    CopyStringIfExist(char *src, char *dst);

//    static unsigned int     Hex2Str(unsigned char *hex, unsigned char *str, unsigned int nLen);
//    static unsigned int     Str2Hex(unsigned char *str, unsigned char *hex, unsigned int nLen);
//    static unsigned int     Str2Dec(unsigned char* str, unsigned char* dec, unsigned int nLen);
//    static unsigned long    Str2Long(unsigned char *str, unsigned int nLen);
//    
//    static unsigned char    Toupper(unsigned char ch);
//    static unsigned char    PackByte(unsigned char h, unsigned char l);
    
    static bool MaskString(char caString[], char cFirstPlainCharacterCounts, char cLastPlainCharacterCounts, char cMaskChar);
    
    static void Track2ToFormatedStr(unsigned char *baHexData, unsigned short usHexDataLen, unsigned char *baFormatedStr, unsigned short *usFormatedStrLen);

//    static bool Base64Encode(const unsigned char *pkucOriginData, const unsigned int kuiOriginDataLength,
//                           unsigned char *pucEncodeResult, unsigned int *puiEncodeResultLength);
//    static bool Base64Decode(const unsigned char *pkucEncodedData, const unsigned int kuiEncodedDataLength,
//                           unsigned char *pucDecodeResult, unsigned int *puiDecodeResultLength);
};

#endif    /* __HEARTLAND_FILEOPER_H__ */
