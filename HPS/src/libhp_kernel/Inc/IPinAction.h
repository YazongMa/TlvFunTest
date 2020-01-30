/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PinAction.h
 * Author: Tim Ma
 *
 * Created on 2019-02-26
 */

#ifndef __HEARTLAND_PINENTRY_H__
#define __HEARTLAND_PINENTRY_H__

#include "../BasicTypeDef.h"


/* 
 * IPinAction
 * 
 * provide interface to caller. 
 *
 */
class IPinAction
{
public:
    virtual unsigned short GetPinBlock(
                OUT unsigned char* pszPinBlock, 
                IN_OUT unsigned short* pwPinBlockSize) = 0;
    
    virtual unsigned short GetKSNString(
                OUT unsigned char* pszKSNString, 
                IN_OUT unsigned char* pbyKSNStringSize) = 0;
};


#endif /* __HEARTLAND_PINENTRY_H__ */

