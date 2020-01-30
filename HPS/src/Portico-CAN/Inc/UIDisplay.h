
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: UIDisplay.h
 * Author: Sword.Zhang
 * 
 * Create Date: 2017.06.12
 * 
 * History:
 *           #2018.06.12, Alan.Ren
 *           1. Modify and improve.
 **/

#ifndef _H_H_UIDISPLAY_20180612_H_H_
#define _H_H_UIDISPLAY_20180612_H_H_

#include "StdafxHead.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>

#define MAXSUBLEN  3
#define DOTNUM     3

enum UiDisplay
{
    DATATYPE_N    = 0x00000001,   // numeric characters only
    DATATYPE_ANS  = 0x00000002,   // alpha, numeric, and special characters
    DATATYPE_B    = 0x00000004,   // binary data
    DATATYPE_Z    = 0x00000008,   // magnetic stripe track-2 or track-3 data
    DATATYPE_AN   = 0x00000010,   // alpha and numeric characters
    DATATYPE_NS   = 0x00000020,   // numeric and special characters
    DATATYPE_PWD  = 0x00000040,   // numeric and special characters
    DATATYPE_IP   = 0x00000080    // IP :numeric and Splitting point
};


class CUIDisplay {
public:
    CUIDisplay(void);
    ~CUIDisplay(void);

    UINT MsgBox(PSTR pszDisText, UINT nRetCode);
    WORD GetUserInputData(PSTR pszTitle, PSTR pszDisplay, BYTE byMaxLength, BYTE byDateType, CByteStream &cOutData, BOOL bIsFixed);
    void ShowUI(PSTR pszTitle, PSTR pszDisplayStr, BYTE byMaxLength, BOOL byMasked, BOOL bFixed);

    BOOL GetYesNo(PSTR pszTitle, BYTE byYN);
private:
    //WORD InPutIP(PSTR pszIN, BYTE byKeyIn, PSTR );
};
#endif