
/*
 * File Name: IDataPack.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.9
 */

#ifndef _H_H_DATA_PACK_H_H_
#define _H_H_DATA_PACK_H_H_

#ifndef IN
#define IN
#endif 

#ifndef OUT
#define OUT
#endif 

class CByteStream;

class IDataPack
{
public:
    virtual int DoPack(OUT CByteStream *pcData) = 0;
    virtual int DoUnpack(IN const CByteStream &cData) = 0;
};

#endif 