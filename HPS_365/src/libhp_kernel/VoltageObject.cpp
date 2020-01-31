/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VoltageObject.cpp
 * Author: Tim.Ma
 *
 * Created on 2019.03.12
 */

#include "VoltageObject.h"
#include "dbghlp.h"


CVoltageObject::CVoltageObject(IN IParameter *pParamPtr)
:
m_pParamPtr(pParamPtr),
m_nInitFlag(0),
m_nVoltageFlag(0)
{
}


CVoltageObject::~CVoltageObject()
{
    if (m_nInitFlag == 1)
    {
        Voltage_Destory();
        m_nInitFlag = 0;
    }
}


int CVoltageObject::EncryptData(IN int nDataType, 
        IN const char *szData, 
        IN unsigned int nLength,
        OUT char *szEncryptBuffer, 
        IN_OUT unsigned int *pnEncryptBuffLength,
        OUT char *szBase64EtbBuffer, 
        IN_OUT unsigned int *pnBase64EtbBufferLength)
{    
    if (m_nVoltageFlag == 0)
    {
        printf("VoltageFlag:%d, it doesn't need voltage to encrypt data\n",  m_nVoltageFlag);
        return 1 ;
    }
    
    return Voltage_Encrypt(nDataType, szData, nLength, 
            szEncryptBuffer, pnEncryptBuffLength, 
            szBase64EtbBuffer, pnBase64EtbBufferLength);
}


int CVoltageObject::GetFlag(void)
{
    int nRet = Initialize();
    if (nRet != 0)
    {
        printf("Initialize Failed:%d\n",  nRet);
        return 0;
    }
    
    return m_nVoltageFlag;
}


int CVoltageObject::Initialize(void)
{
    HEARTLAND_GENERAL_PARAM voltageParam = { 0 };
    tagParam cTagParam = { 0 };

    InitTagParam(cTagParam, kGeneralParam, voltageParam);
    int nRet = m_pParamPtr->GetParameter(&cTagParam);
    if (nRet != 0)
    {
        printf("GetParameter Failed:%d\n",  nRet);
        return 1;
    }
    
    TraceMsg("Info: voltageParam.szVoltageFlag:%s, %p", voltageParam.szVoltageFlag, &voltageParam);
    m_nVoltageFlag = 0;

    if (0 == strcasecmp((char *)voltageParam.szVoltageFlag, "true"))
    {    
        m_nVoltageFlag = 1;
    }
        
    
    if (1 == m_nInitFlag)
    {
        printf("Voltage_Init has been already completed\n");
        return 0;
    }
    
    nRet = Voltage_Init(voltageParam.szBBParamFile);
    if (nRet != 0)
    {
        printf("Voltage_Init Failed:%d\n",  nRet);
        return 1;
    }
    
    m_nInitFlag = 1;
    return 0;
}