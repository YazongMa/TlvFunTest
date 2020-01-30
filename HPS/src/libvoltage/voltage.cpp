
/*
 * File Name: voltage.cpp
 * Author: Alan.Ren
 * 
 * Create Date: 2019.2.21
 */
  
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifndef PLATFORM_ANDROID
#include <openssl/crypto.h>
#endif

#include "voltage.h"
#include "vppos.h"
#include "base64.h"

static void Init(void);

static VpPosContext posContext_ = NULL;
static VpPosKey posKey_ = NULL;
static unsigned int initFlag_ = 0;
// static unsigned int bbParamsSize_ = 0;
// static unsigned int bbIdentitySize_ = 0;
// static unsigned char bbParamsBuffer_[1024] = {0};
// static unsigned char bbIdentityBuffer_[1024] = {0};



/* remove invalid character: newline, space, etc */
static unsigned int InValidCharFilter(char string[], unsigned int length)
{
    unsigned int i, j = 0;
    char *buff = NULL;

    buff = (char *) malloc(length + 1);
    if (buff == NULL)
    {
        return 0;
    }

    for (i = 0, j = 0; i < length; i++)
    {

        switch (string[i])
        {
            case '\n':
            case ' ':
                break;

            default:
                buff[j++] = string[i];
                break;
        }
    }
    buff[j] = '\0';

    memset(string, 0x00, length);
    if (j != length)
    {
        memcpy(string, buff, j);
    }

    free(buff);

    return j;
}

static int ReadBBParamsByXml(char* bbParamsXmlFile,
                      unsigned char* bbParamsBuffer,
                      unsigned int* bbParamsBufLen,
                      unsigned char* bbIdentityName,
                      unsigned int* bbIdentityNameLen)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr cur = NULL;
    xmlChar *key = NULL;

    doc = xmlParseFile(bbParamsXmlFile);
    if (doc == NULL)
    {
        fprintf(stderr, "Document not parsed successfully. \n");
        return -1;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return -2;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "voltage_settings"))
    {
        fprintf(stderr, "document of the wrong type, root node != voltage_settings");
        cur = cur->xmlChildrenNode;
        xmlFreeDoc(doc);
        return -3;
    }

    /* parsing voltage settings */
    printf("\nVoltage Settings Parsing...\n\n");
    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
        /* identity name */
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "identity_name")))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            strcpy((char *)bbIdentityName, (char *)key);
            *bbIdentityNameLen = InValidCharFilter((char *)bbIdentityName, strlen((char *)key));
            xmlFree(key);
        }
        /* public parameters */
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "public_parameters")))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            *bbParamsBufLen = InValidCharFilter((char *)key, strlen((char *)key));
            *bbParamsBufLen = base64_decode((char *)key, *bbParamsBufLen, bbParamsBuffer);
            xmlFree(key);
        }
        cur = cur->next;
    }

    xmlFreeDoc(doc);

    return 0;
}
/* end of function ReadBBParamsByXml() */



/* Stand-alone function to Base64-encode the ETBs. */
/* Not related to the Terminal SDK, per se.        */
#define  ZERO  0
#define  ONE   1


/* Base64 alphabet: */
static const char base64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/* Stand-alone function to Base64-encode binary input.              */
/* Returns zero on success.                                         */
/* Returns -1 to indicate an output buffer too small.               */

/* Returns -2 to indicate an inability to allocate required memory. */
static int Base64Encode(unsigned char* inputBuffer,
                 size_t inputSize,
                 char* outputBuffer,
                 size_t outputSize)
{
    unsigned int inputByteIndex, bitInByteIndex, bitBufIndex;
    unsigned char* bitBuf;
    unsigned int bitBufLen, bitInSixBitsIndex;
    unsigned char curByte;
    unsigned int base64BufIndex, base64Value, equalsSignCount = 0;
    /* Allocate the "byte-sized bit" buffer. */
    bitBuf = (unsigned char*) malloc(inputSize * 8); /* more than enough */
    if (bitBuf == NULL)
    {
        return (-2);
    }
    /* Convert the input to a buffer of zero- or one-valued bytes. */
    for (inputByteIndex = 0, bitBufIndex = 0;
            inputByteIndex < inputSize;
            inputByteIndex++)
    {
        curByte = inputBuffer[inputByteIndex]; /* byte to be converted to its bits */
        for (bitInByteIndex = 0; bitInByteIndex < 8; bitInByteIndex++)
        {
            if ((curByte & 0x80) != 0)
            {
                bitBuf[bitBufIndex++] = ONE;
            }
            else
            {
                bitBuf[bitBufIndex++] = ZERO;
            }
            curByte = curByte << 1;
        }
    }
    bitBufLen = bitBufIndex; /* record the number of "bits" saved */
    /* Convert the byte-sized "bits" array to Base64. */
    for (bitBufIndex = 0, base64BufIndex = 0;
            bitBufIndex < bitBufLen;
            base64BufIndex++)
    {
        /* Compute a Base64 value. */
        base64Value = 0; /* reset for next 6-bit iteration */
        for (bitInSixBitsIndex = 0;
                (bitInSixBitsIndex < 6) && (bitBufIndex < bitBufLen);
                bitInSixBitsIndex++, bitBufIndex++)
        {
            base64Value = base64Value << 1; /* first time is a no-op */
            base64Value = base64Value | bitBuf[bitBufIndex];
        }
        /* Add extra zeros as required and track the number of equals signs to append. */
        for (equalsSignCount = 0; bitInSixBitsIndex < 6; bitInSixBitsIndex++)
        {
            base64Value = base64Value << 1;
            equalsSignCount++;
        }
        /* Add the Base64 character to the output buffer, checking for overflow. */
        if (base64BufIndex < outputSize)
        {
            outputBuffer[base64BufIndex] = base64Alphabet[base64Value];
        }
        else
        {
            free(bitBuf);
            return (-1); /* output buffer too small */
        }
    }
    /* Add the required number of equals signs, checking for overflow. */
    for (; equalsSignCount > 0; equalsSignCount -= 2)
    {
        if (base64BufIndex < outputSize)
        {
            outputBuffer[base64BufIndex++] = '=';
        }
        else
        {
            free(bitBuf);
            return (-1); /* output buffer too small */
        }
    }
    /* Add the NULL-terminator, checking for overflow. */
    if (base64BufIndex < outputSize)
    {
        outputBuffer[base64BufIndex] = '\0';
    }
    else
    {
        free(bitBuf);
        return (-1); /* output buffer too small */
    }
    free(bitBuf);
    return (0); /* success */
} /* end of function Base64Encode() */

int CVoltageEncrypt::Voltage_Init(const char *pszFileName)
{
#if 0
    if (NULL == pszFileName)
    {
        printf("Voltage_Initialize invalid param\n");
        return 1;
    }

    // SSL version is matched ?
    unsigned long ulVersion = SSLeay();
    printf("Voltage_Initialize SSL VERSON :%lX\n", ulVersion);

    if (ulVersion != 0x100010AF)
    {
        printf("Voltage_Initialize SSL VERSON is not matched\n");
        return 2;
    }
 
    int nRet = ReadBBParamsByXml((char* )pszFileName, bbParamsBuffer_, &bbParamsSize_,
            bbIdentityBuffer_, &bbIdentitySize_);
    if (0 != nRet)
    {
        return 3;
    }

    printf("Voltage_Initialize bbIdentityBuffer:%s\n", bbIdentityBuffer_);
    printf("Voltage_Initialize bbParamsSize:%d, bbParamsBuffer:\n", bbParamsSize_);
    for(int i=0; i<bbParamsSize_; ++i)
    {
        printf("%02X", bbParamsBuffer_[i]);
    }
    printf("\n");
#endif

    int nRet = VpPosCreateContext(VP_POS_FLAGS_ENCRYPT_420 | VP_POS_FLAGS_ENCRYPT_SPE, &posContext_);
	if (0 != nRet)
    {
        printf("Voltage_Initialize VpPosCreateContext failure, nRet:%d\n", nRet);
        return 4;
    }

    nRet = VpPosCreateKey(posContext_, 1, &posKey_);
    if (0 != nRet)
    {
        printf("Voltage_Initialize VpPosCreateKey failure, nRet:%d\n", nRet);
        return 5;
    }

    initFlag_ = 1;
    return 0;
}

int CVoltageEncrypt::Voltage_Destory(void)
{
        /* Securely destroy the random symmetric key. */
    if (posKey_ != NULL)
    {
        VpPosDestroyKey(&posKey_);
        posKey_ = NULL;
    }

    /* Always destroy the HostContext after it is no longer needed. */
    if (posContext_ != NULL)
    {
        VpPosDestroyContext(&posContext_);
        posContext_ = NULL;
    }
    return 0;
}

int CVoltageEncrypt::Voltage_Encrypt(IN int nDataType, IN const char *szData, IN unsigned int nLength,
                    OUT char *szEncryptBuffer, IN_OUT unsigned int *pnEncryptBuffLength,
                    OUT char *szBase64EtbBuffer, IN_OUT unsigned int *pnBase64EtbBufferLength)
{
    if (initFlag_ == 0)
    {
        printf("Voltage_EncryptData initFlag: %d is invalid\n", initFlag_);
        return 0xFFFF;
    }
    
    unsigned char etbBuffer[500];
    unsigned int etbSize = 0;

    VpPosDate identityDate = {0};
    time_t now;
    struct tm* timeInfo;

    /* Get the current year, month, and day to use in the new ETB generation. */
    /* Note that function localtime() is not thread-safe.                     */
    time(&now);
    timeInfo = localtime(&now);
    identityDate.year = timeInfo->tm_year + 1900;
    identityDate.month = timeInfo->tm_mon + 1;
    identityDate.day = timeInfo->tm_mday;

#if 0
    /* Generate the matching ETB */
    int nRet = VpPosGenerateEtb(posKey_, (const char*)bbIdentityBuffer_, &identityDate,
            bbParamsBuffer_, bbParamsSize_,
            etbBuffer, sizeof (etbBuffer), &etbSize);
#endif
    int nRet = SecureVpPosGenerateEtb(posKey_, VP_POS_ENC_ALG_TEP1x_TEP2x_TEP4, &identityDate,
            etbBuffer, sizeof (etbBuffer), &etbSize);
    if (nRet != 0)
    {
        printf("Voltage_EncryptData Call to function VpPosGenerateEtb failed: %d\n", nRet);
        return nRet;
    }

    int nEncryptBuffLength = *pnEncryptBuffLength;

    nRet = VpPosEncryptCardData(posKey_, nDataType, szData, nLength,
            szEncryptBuffer, nEncryptBuffLength, pnEncryptBuffLength);
    if (nRet != 0)
    {
        printf("Voltage_EncryptData Call to function VpPosEncryptCardData failed: %d\n", nRet);
        return nRet;
    }

    szEncryptBuffer[*pnEncryptBuffLength] = '\0';

    /* Create the Base64-encoded version of the ETB. */
    nRet = Base64Encode(etbBuffer, (size_t) etbSize, szBase64EtbBuffer, *pnBase64EtbBufferLength);
    if (nRet != 0)
    {
        printf("Voltage_EncryptData Failed to Base64-encode the ETB.\n");
        return nRet;
    }
    return 0;
}

