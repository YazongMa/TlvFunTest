
/*
 * File Name: IKernelCtrl.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.14
 * Description: Define public interface.
 */

#ifndef _H_H_KERNEL_CTRL_H_H_ 
#define _H_H_KERNEL_CTRL_H_H_

/*
 * Define interface.
 */
class IKernelCtrl
{
public:
    /*
     * Public.
     */
    virtual void PostRequest(IN const char *pszJsonCmd) = 0;
    
    /*
     * Public.
     */
    virtual void GetResponse(OUT char *pszBuffer, IN const int nBufSize) = 0;
};

#endif