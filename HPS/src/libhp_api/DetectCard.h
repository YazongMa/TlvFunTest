
/*
 * File Name: DetectCard.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.2.28
 */

#ifndef _H_H_DETECT_CARD_H_H_
#define _H_H_DETECT_CARD_H_H_

#include "./Inc/BasicInc.h"
#include "../libhp_kernel/dbghlp.h"
#include "../libhp_kernel/Inc/DefParamTypeId.h"

class CDetectCard
{
public:
    CDetectCard() { }
    virtual ~CDetectCard() { }
    
public:
    void Detect(OUT int *pCardType);
    void Detect(OUT int *pCardType, IN int nCardSet);
    void ClearLastDetectInfo(void);

protected:
    virtual BOOL DetectMSR(void); 
    virtual BOOL DetectEMV(void); 
    virtual BOOL DetectEMVCL(void);

    virtual void ClearLastMSRInfo(void);
    virtual void ClearLastEMVCLInfo(void);
    virtual void ClearLastEMVInfo(void);
};

#endif 
