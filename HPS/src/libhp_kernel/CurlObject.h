
/*
 * File Name: CurlObject.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.3.12
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_CURLPROC_H__
#define __HEARTLAND_PAYMENT_KERNEL_CURLPROC_H__

#include "BasicTypeDef.h"

#include "Inc/IBaseCurl.h"
#include "Inc/IParameter.h"
#include "Inc/DefParamTypeId.h"

class PUBLIC_EXPORT CurlObject: public IBaseCurl
{
public:
    CurlObject(IN IParameter *pParamPtr);
    virtual ~CurlObject();
    
    virtual int CurlProcess(IN CByteStream *pcInData, OUT CByteStream& cOutData);

    virtual int CurlCheckConnectivity(IN unsigned long ulTimeout = 0);
    
protected:
    IParameter*     m_pParamPtr;
};

#endif 