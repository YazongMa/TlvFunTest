
/**
 * Copyright(c) Castles Technology Co,. Ltd
 * 
 * File Name: ByteStream.cpp
 * Author: Sword.Zhang
 * 
 * Create Date: 2017.06.08
 * 
 * History:
 *           #2018.06.08, Alan.Ren
 *           1. Modify and improve.
 **/

#include "ByteStream.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>


#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


CByteStream::CByteStream()
{
    m_pData = NULL;
    m_nSize = 0;
    m_nMaxSize = 0;
}

CByteStream::CByteStream(IN PCSTR lpsz)
{
    m_pData = NULL;
    m_nSize = 0;
    m_nMaxSize = 0;

    const int nSrcLen = ((lpsz != NULL) ? strlen(lpsz) : 0);
    if (nSrcLen != 0)
    {
        Empty();
        Write(lpsz, nSrcLen);
    }
}

CByteStream::CByteStream(IN PCBYTE pbyBuffer, IN WORD wBufLen)
{
    m_pData = NULL;
    m_nSize = 0;
    m_nMaxSize = 0;

    if (wBufLen != 0)
    {
        Empty();
        Write(reinterpret_cast<const void *> (pbyBuffer), wBufLen);
    }
}

CByteStream::CByteStream(IN const CByteStream &ByteStream)
{
    m_pData = NULL;
    m_nSize = 0;
    m_nMaxSize = 0;

    if (this != &ByteStream)
    {
        Empty();
        Write(ByteStream.GetBuffer(), ByteStream.GetLength());
    }
}

CByteStream::~CByteStream()
{
    Empty();
}

void CByteStream::Write(IN const void* lpBuf, IN UINT nLen)
{
    if (nLen <= 0)
        return;
    if (NULL == lpBuf)
        return;

    int nIndex = 0;
    if (m_nSize > 0)
        nIndex = m_nSize;

    SetSize(m_nSize + nLen);
    memcpy(&m_pData[nIndex], lpBuf, nLen);
}

int CByteStream::Compare(IN PCBYTE pData, IN int nLen)
{
    return memcmp(pData, m_pData, nLen);
}

int CByteStream::Compare(IN PCSTR pStr)
{
    return memcmp(pStr, m_pData, strlen(pStr));
}

int CByteStream::Compare(IN const CByteStream *pData)
{
    if (NULL == pData)
        return 1;

    if (m_nSize != pData->GetLength())
        return 1;

    return Compare(pData->GetBuffer(), m_nSize);
}

void CByteStream::Empty()
{
    SetSize(0);
}

BOOL CByteStream::IsEmpty()
{
    if (m_nSize > 0)
        return FALSE;
    else
        return TRUE;
}

int CByteStream::GetLength() const
{
    return m_nSize;
}

BYTE* CByteStream::GetBuffer(IN int nOffSet) const
{
    if (nOffSet > m_nSize)
        return NULL;

    if (nOffSet == 0)
        return m_pData;
    else
        return &m_pData[nOffSet];
}

void CByteStream::CopyToStr(OUT PSTR pszBuf, IN int nBufLen)
{
    if (NULL == pszBuf)
        return;

    memset(pszBuf, 0, nBufLen);
    if (nBufLen <= m_nSize)
        memcpy(pszBuf, m_pData, nBufLen - 1);
    else
        memcpy(pszBuf, m_pData, m_nSize);
}

PCSTR CByteStream::PushZero()
{
    if (0 == m_nSize)
        return NULL;

    BYTE byTemp[4] = {0};
    byTemp[0] = 0x00;

    Write(byTemp, 1);

    return (PCSTR)this->m_pData;
}

void CByteStream::PopTailByte(IN int nByte)
{
    if (m_nSize < nByte)
        return;

    SetSize(m_nSize - nByte);
}

// insertion operations

CByteStream& CByteStream::operator<<(IN BYTE by)
{
    BYTE byTemp[4] = {0};
    byTemp[0] = by;

    Write(byTemp, 1);

    return *this;
}

CByteStream& CByteStream::operator>>(OUT BYTE * pby)
{
    if (m_nSize < 1)
        return *this;

    memcpy(pby, &m_pData[m_nSize - 1], 1);
    SetSize(m_nSize - 1);

    return *this;
}

CByteStream& CByteStream::operator<<(IN char ch)
{
    BYTE byTemp[4] = {0};
    byTemp[0] = ch;

    Write(byTemp, 1);

    return *this;
}

CByteStream& CByteStream::operator>>(OUT char * pch)
{
    if (m_nSize < 1)
        return *this;

    memcpy(pch, &m_pData[m_nSize - 1], 1);
    SetSize(m_nSize - 1);

    return *this;
}

CByteStream& CByteStream::operator<<(IN PCSTR psz)
{
    Write(psz, strlen(psz));

    return *this;
}

CByteStream& CByteStream::operator<<(IN const CByteStream &ByteStream)
{
    Write(ByteStream.GetBuffer(), ByteStream.GetLength());

    return *this;
}

CByteStream& CByteStream::operator<<(IN DWORD dw)
{
    int nIndex = 0;
    if (m_nSize > 0)
        nIndex = m_nSize;

    SetSize(m_nSize + sizeof (dw));

    memcpy(&m_pData[nIndex], &dw, sizeof (DWORD));

    return *this;
}

CByteStream& CByteStream::operator>>(OUT PDWORD pDw)
{
    int nLen = sizeof (DWORD);
    if (m_nSize < nLen)
        return *this;

    memcpy(pDw, &m_pData[m_nSize - nLen], nLen);
    SetSize(m_nSize - nLen);

    return *this;
}

CByteStream& CByteStream::operator<<(IN WORD w)
{
    int nIndex = 0;
    if (m_nSize > 0)
        nIndex = m_nSize;

    SetSize(m_nSize + sizeof (WORD));

    memcpy(&m_pData[nIndex], &w, sizeof (WORD));

    return *this;
}

CByteStream& CByteStream::operator>>(OUT PWORD pW)
{
    int nLen = sizeof (WORD);
    if (m_nSize < nLen)
        return *this;

    memcpy(pW, &m_pData[m_nSize - nLen], nLen);
    SetSize(m_nSize - nLen);

    return *this;
}

CByteStream& CByteStream::operator=(IN const CByteStream &ByteStream)
{
    if (this != &ByteStream)
    {
        Empty();
        Write(ByteStream.GetBuffer(), ByteStream.GetLength());
    }

    return *this;
}

CByteStream& CByteStream::operator=(IN PCSTR psz)
{
    Empty();
    Write(psz, strlen(psz));

    return *this;
}

CByteStream& CByteStream::operator+=(IN const CByteStream &ByteStream)
{
    Write(ByteStream.GetBuffer(), ByteStream.GetLength());

    return *this;
}

CByteStream& CByteStream::operator+=(IN PCSTR psz)
{
    Write(psz, strlen(psz));
    return (*this);
}

CByteStream& CByteStream::TrimLeft(IN char ch)
{
    if (0 == m_nSize)
        return *this;

    BYTE *pbyTemp = m_pData;
    while (*pbyTemp == ch)
    {
        pbyTemp++;
    }

    int iFirst = int(pbyTemp - m_pData);
    memmove(m_pData, pbyTemp, m_nSize - iFirst);
    SetSize(m_nSize - iFirst);

    return *this;
}

CByteStream& CByteStream::TrimRight(IN char ch)
{
    if (0 == m_nSize)
        return *this;

    PBYTE pbyTemp = m_pData + m_nSize - 1;

    while (*pbyTemp == ch)
    {
        pbyTemp--;
    }

    int nLen = int(pbyTemp - m_pData) + 1;
    SetSize(nLen);

    return ( *this);
}

CByteStream& CByteStream::Format(IN PCSTR pFormat, ...)
{
    char szBuf[4096] = {0};

    SetSize(0);
    va_list argList;
    va_start(argList, pFormat);
    vsprintf(szBuf, pFormat, argList);
    va_end(argList);

    int nLen = strlen(szBuf);
    if (nLen > 1024)
        nLen = 1024;
    Write(szBuf, nLen);

    return ( *this);
}

void CByteStream::SetSize(IN int nNewSize)
{
    if (nNewSize < 0)
    {
        if (m_pData != NULL)
        {
            delete [] m_pData;
            m_pData = NULL;
        }
        return;
    }

    int m_nGrowBy = 0;

    if (nNewSize == 0)
    {
        // shrink to nothing
        if (m_pData != NULL)
        {
            delete[] (BYTE*) m_pData;
            m_pData = NULL;
        }
        m_nSize = m_nMaxSize = 0;
    }
    else if (m_pData == NULL)
    {
        // create one with exact size
        m_pData = (BYTE*) new BYTE[nNewSize * sizeof (BYTE)];
        memset(m_pData, 0, nNewSize * sizeof (BYTE)); // zero fill
        m_nSize = m_nMaxSize = nNewSize;
    }
    else if (nNewSize <= m_nMaxSize)
    {
        // it fits
        if (nNewSize > m_nSize)
        {
            // initialize the new elements
            memset(&m_pData[m_nSize], 0, (m_nMaxSize - m_nSize) * sizeof (BYTE));
        }
        else
        {
            memset(&m_pData[nNewSize], 0, (m_nMaxSize - nNewSize) * sizeof (BYTE));
        }
        m_nSize = nNewSize;
    }
    else
    {
        // otherwise, grow array
        int nGrowBy = m_nGrowBy;
        if (nGrowBy == 0)
        {
            // heuristically determine growth when nGrowBy == 0
            //  (this avoids heap fragmentation in many situations)
            nGrowBy = min(1024, max(4, m_nSize / 8));
        }
        int nNewMax = 0;
        if (nNewSize < (m_nMaxSize + nGrowBy))
        {
            nNewMax = m_nMaxSize + nGrowBy; // granularity
        }
        else
        {
            nNewMax = nNewSize; // no slush
        }

        BYTE* pNewData = (BYTE*) new BYTE[nNewMax * sizeof (BYTE)];

        // copy new data from old
        // WCE_FCTN(memcpy)(pNewData, m_pData, m_nSize * sizeof(BYTE));
        memcpy(pNewData, m_pData, m_nSize * sizeof (BYTE));
        // construct remaining elements
        memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof (BYTE));
        // get rid of old stuff (note: no destructors called)
        if (m_pData != NULL)
        {
            delete[] (BYTE*) m_pData;
            m_pData = NULL;
        }

        m_pData = pNewData;
        m_nSize = nNewSize;
        m_nMaxSize = nNewMax;
    }
}
