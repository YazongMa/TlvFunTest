
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: ByteStream.H
 * 
 * Author: Sword.Zhang
 * 
 * Create Date: 2017.06.08
 * 
 * History:
 *           #2018.06.08, Alan.Ren
 *           1. Modify and improve.
 **/

#ifndef _H_H_MISC_20180608_H_H_
#define _H_H_MISC_20180608_H_H_


#include "deftype.h"
#include "export.h"

class PRIVATE_EXPORT CByteStream
{
public:
    explicit CByteStream();
    explicit CByteStream(IN PCSTR lpsz);
    explicit CByteStream(IN PCBYTE pbyBuffer, IN WORD wBufLen);
    CByteStream(IN const CByteStream &ByteStream);
    virtual ~CByteStream();

public:
    void Write(IN const void* lpBuf, IN UINT nLen);
    int Compare(IN PCBYTE pData, IN int nLen);
    int Compare(IN PCSTR pStr);
    int Compare(IN const CByteStream *pData);
    void Empty();
    BOOL IsEmpty();
    int GetLength() const;
    BYTE* GetBuffer(IN int nOffSet = 0) const;
    void CopyToStr(OUT PSTR pszBuf, IN int nBufLen);
    PCSTR PushZero();
    void PopTailByte(int nByte = 1);
    CByteStream& operator<<(IN BYTE by);
    CByteStream& operator>>(OUT BYTE * pby);
    CByteStream& operator<<(IN char ch);
    CByteStream& operator>>(OUT char * pch);
    CByteStream& operator<<(IN PCSTR psz);
    CByteStream& operator<<(IN const CByteStream &ByteStream);
    CByteStream& operator<<(IN DWORD dw);
    CByteStream& operator>>(OUT PDWORD pDw);
    CByteStream& operator<<(IN WORD w);
    CByteStream& operator>>(OUT PWORD pW);
    CByteStream& operator=(const CByteStream &ByteStream);
    CByteStream& operator=(PCSTR psz);
    CByteStream& operator+=(IN const CByteStream &ByteStream);
    CByteStream& operator+=(IN PCSTR psz);
    CByteStream& TrimLeft(IN const char ch);
    CByteStream& TrimRight(IN char ch);
    CByteStream& Format(PCSTR pFormat, ...);
    
private:
    BYTE* m_pData;    // the actual array of data
    int m_nSize;      // # of elements (upperBound - 1)
    int m_nMaxSize;
    void SetSize(IN int nNewSize);
};

#endif // !defined(AFX_BYTESTREAM_H__7B0E6B8B_36D7_42D0_AFA2_F6C5E43536B2__INCLUDED_)
