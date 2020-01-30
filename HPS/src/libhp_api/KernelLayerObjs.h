
/*
 * File Name: KernelLayerObjs.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.20
 */

#ifndef _H_H_Kernel_Layer_Objs_H_H_
#define _H_H_Kernel_Layer_Objs_H_H_

#include "KernelLayerInc.h"
#include "DetectCard.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 */
IParameter* HLKE_GetParamPtr(void);

/*
 */
CTxnBase* HLKE_GetMSRTxnPtr(void);

/*
 */
CTxnBase* HLKE_GetEMVTxnPtr(void);

/*
 */
CTxnBase* HLKE_GetEMVCLTxnPtr(void);

/*
 */
CTxnBase* HLKE_GetNoneCardTxnPtr(void);

/*
 */
CDetectCard* HLKE_GetDetectedCardPtr(void);

/*
 */
HeartlandSoap* HLKE_GetSoapKernelPtr(void);

/*
 */
IBaseCurl* HLKE_GetCurlObjPtr(void);

#ifdef __cplusplus
}
#endif

#endif