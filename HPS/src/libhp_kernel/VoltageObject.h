
/* 
 * File:   VoltageObject.h
 * Author: Tim.Ma
 *
 * Created on 2019.03.12
 */

#ifndef VOLTAGEOBJECT_H
#define VOLTAGEOBJECT_H

#include "BasicTypeDef.h"
#include "Inc\IParameter.h"
#include "Inc\DefParamTypeId.h"
#include "TxnDef.h"

#include "..\libvoltage\voltage.h"

class PUBLIC_EXPORT CVoltageObject : public CVoltageEncrypt
{
public:
    CVoltageObject(IN IParameter *pParamPtr);
    virtual ~CVoltageObject();
    
    virtual int EncryptData(IN int nDataType, 
            IN const char *szData, 
            IN unsigned int nLength,
            OUT char *szEncryptBuffer, 
            IN_OUT unsigned int *pnEncryptBuffLength,
            OUT char *szBase64EtbBuffer, 
            IN_OUT unsigned int *pnBase64EtbBufferLength);
    
    virtual int GetFlag(void);
    
private:
    int Initialize(void);
    
protected:
    IParameter*     m_pParamPtr;
    int             m_nInitFlag;
    int             m_nVoltageFlag;
};

#endif /* VOLTAGEOBJECT_H */

