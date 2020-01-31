
/*
 * File Name: IParameter.h
 * Author: Alan.Ren & Time.Ma 
 * 
 * Crate Date: 2019.3.6
 */

#ifndef _H_H_IParameter_H_H_
#define _H_H_IParameter_H_H_

#ifndef OUT
#define OUT
#endif 

#ifndef IN
#define IN
#endif 


/*
 * struct tagParam is a data type that used to store data
 *
 * nType: Parameter Type
 * nParamSize: pParam Size.
 * pParam: point of the parameter
 */
typedef struct
{
  int nType;            // Parameter Type
  int nParamSize;       // pParam Size.
  void *pParam;
} tagParam;

#define InitTagParam(paramObj, paramType, param) { \
  paramObj.nType = paramType; \
  paramObj.nParamSize = sizeof(param); \
  paramObj.pParam = &param; \
}


/* 
 * IParameter
 * 
 * provide interface to caller. 
 *
 */
class IParameter
{
public:
  virtual int SetParameter(IN const tagParam &cParam) = 0;
  virtual int GetParameter(OUT tagParam* pcParam) = 0;
};

#endif 