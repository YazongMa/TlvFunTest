
/*
 * File Name: IBaseCurl.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.3.12
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_BASECURL_H__
#define __HEARTLAND_PAYMENT_KERNEL_BASECURL_H__

#ifndef IN
#define IN
#endif 

#ifndef OUT
#define OUT
#endif 

class CByteStream;

class IBaseCurl
{
public:
    virtual int CurlProcess(IN CByteStream *pcInData, OUT CByteStream& cOutData) = 0;

    virtual int CurlCheckConnectivity(IN unsigned long ulTimeout = 0) = 0;
};

#endif 