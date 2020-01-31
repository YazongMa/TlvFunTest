
/*
 * File Name: voltage.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.2.21
 */

#ifndef _H_H_VOLTAGE_H_ 
#define	_H_H_VOLTAGE_H_ 

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif 

#ifndef OUT
#define OUT
#endif 

#ifndef IN_OUT
#define IN_OUT
#endif 

/*
 * Voltage Object.
 */
class CVoltageEncrypt
{
public:
    virtual int Voltage_Init(IN const char *pszFileName);
    virtual int Voltage_Destory(void);
    virtual int Voltage_Encrypt(IN int a, IN const char * b, IN unsigned int c,
                           OUT char *g, IN_OUT unsigned int *d,
                           OUT char *f, IN_OUT unsigned int *e);
};


#ifdef	__cplusplus
}
#endif

#endif

