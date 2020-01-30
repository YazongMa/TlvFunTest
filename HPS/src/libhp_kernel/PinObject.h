
/* 
 * File:   PinAction.h
 * Author: Tim Ma
 *
 * Created on 2019-03-13
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_PINOBJECT_H__
#define __HEARTLAND_PAYMENT_KERNEL_PINOBJECT_H__

#include "BasicTypeDef.h"
#include "Inc/IParameter.h"
#include "Inc/DefParamTypeId.h"

#include "Inc/IPinAction.h"

class PUBLIC_EXPORT CPinObject : public IPinAction
{
public:
    CPinObject(IParameter *pParamPtr);
    virtual ~CPinObject();

    virtual unsigned short GetPinBlock(
        OUT unsigned char *pszPinBlock,
        IN_OUT unsigned short *pwPinBlockSize);

    virtual unsigned short GetKSNString(
        OUT unsigned char *pszKSNString,
        IN_OUT unsigned char *pbyKSNStringSize);

protected:
    IParameter*     m_pParamPtr;
};

#endif /* __HEARTLAND_PAYMENT_KERNEL_PINOBJECT_H__ */

