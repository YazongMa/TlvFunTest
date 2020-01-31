
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: DebugInfo.H
 * 
 * Author: Sword.Zhang
 * 
 * Create Date: 2017.06.08
 * 
 * History:
 *           #2018.06.11, Alan.Ren
 *           1. Modify and improve.
 **/

#ifndef _H_H_DEBUG_H_H_
#define _H_H_DEBUG_H_H_

#define SRCFILE     __FILE__
#define CODELINE    __LINE__
#define FUNCNAME    __FUNCTION__
#define DEBUG_MSG   0x0401

#define DEBUGINFO_NORMAL   0x00   //Debug info normal
#define DEBUGINFO_WARNING  0x01   //Debug info warning
#define DEBUGINFO_ERROR    0x02   //Debug info error

#define  FMT_SPACE      0x0001  //Space separate
#define  FMT_16NEWLINE  0x0002  //16 bytes and one line
#define  FMT_32NEWLINE  0x0004  //32 bytes and one line

class CDebugInfo
{
public:
    CDebugInfo(void);
    ~CDebugInfo(void);

    WORD            DebugEnable(BYTE byDbg, BYTE byType, DWORD dwLevel, WORD (*pFun)(WORD wCBType, WPARAM wParam, LPARAM lParam));
    virtual void    DebugEntry(PCSTR lpFileName, int iLine, PCSTR lpStr);
    virtual void    DebugInfo(PCSTR pSourceFileName, int iLineNo, DWORD dwLevel, PCSTR pFormat, ...);
    virtual WORD    DebugExit(PCSTR lpFileName, int iLine, PCSTR lpStr, WORD wRet);
    virtual WORD    DebugRetCode(PCSTR lpFileName, int iLine, PCSTR lpStr, WORD wRet);
    virtual void    DebugByteStream(PCSTR lpFileName, int iLine, DWORD dwLevel, CByteStream *pCmd, PCSTR pszStr);
    virtual void    DebugByteStream(PCSTR lpFileName, int iLine, DWORD dwLevel, BYTE *pbyData, int nLen, PCSTR  pszStr);
    virtual WORD    DebugCBRegister(WORD (*pFun)(WORD wCBType, WPARAM wParam, LPARAM lParam));

private:        
    BYTE m_byDebug;
    BYTE m_byDebugType;
    DWORD m_dwDebugLevel;
    WORD (*pFunCB)(WORD wCBType, WPARAM wParam, LPARAM lParam);
    DWORD   Hex2ASC(CByteStream *pcSrc, CByteStream &cOut, WORD  dwFormat=0);
    void    GetTime(CByteStream &cTime, PCSTR lpFileName, int iLine);

};

#endif