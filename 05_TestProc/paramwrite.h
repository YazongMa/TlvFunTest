
/* 
 * Copyright (c) Castles Technology Co., Ltd. 
 * 
 * File:   paramwrite.h
 * Author: Amber
 *
 * Created: 2018/07/13
 */

#ifndef PARAMWRITE_H
#define PARAMWRITE_H


#define BUF_LEN 128


#include <Windows.h>

class ParamWrite {
public:

    ParamWrite();
    virtual ~ParamWrite();
       
    BOOL Hex2Str(BYTE* pbyStrData, BYTE* pbyData, int nSize);
    BOOL Str2Hex(const char* pStr, BYTE* cOut);
};

#endif /* PARAMWRITE_H */

