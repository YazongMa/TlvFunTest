
/*
 * File Name: IUserAction.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.13
 */

#ifndef _H_H_USER_ACTION_H_H_ 
#define _H_H_USER_ACTION_H_H_

/* 
 * IUserAction
 * 
 * provide interface to caller. 
 *
 */
class IUserAction
{
public:
  virtual void RequsetUserAction(const unsigned int &nActionType,
                                 const void *pVoidData,
                                 const size_t &nDataSize) = 0;

  virtual void WaitUserResponse(const unsigned int &nActionType,
                                void *pBuffer,
                                const size_t &nBufferSize) = 0;
};

#endif