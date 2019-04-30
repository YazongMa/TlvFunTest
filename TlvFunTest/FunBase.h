
#ifndef H_FUNBASE
#define H_FUNBASE

#define DATATYPE_CN        0x00000100   // binary coded decimal
#define JUSTIFIED_OPEN     0x00000200   // justified open
#define JUSTIFIED_LEFT     0x00000400   // left-justified
#define JUSTIFIED_RIGHT    0x00000800   // right-justified
#define FMT_SPACE          0x0001       // Space separate
#define FMT_16NEWLINE      0x0002       // 16 bytes and one line
#define FMT_32NEWLINE      0x0004       // 32 bytes and one line


#include "deftype.h"
#include "export.h"
#include "ByteStream.h"


class CFunBase
{
public:
    CFunBase(void);
    virtual ~CFunBase(void);

    static void        SetByteBit(BYTE& by, int idx, BOOL b);
    static BOOL        GetByteBit(const BYTE by, int idx);
    static void        Byte2Bit(BYTE byData, CByteStream &cRet);
    static DWORD       FormatDataLength(CByteStream &cSrc, int nFormatLen, char ch, DWORD dwJustifiedType);
    static DWORD       ASCIIToDec(BYTE* buffer, BYTE Len);
    static DWORD       ASCIIToDec(char * pszStr);
    static void        DecToASCII(DWORD dwValue, BYTE* buffer, BYTE Digit);
    static void        DecToASCII(DWORD dwValue, CByteStream &cRet);
    static BYTE        Toupper(BYTE ch);
    static BYTE        PackByte(BYTE h, BYTE l);
    static CByteStream Hex2Str(PBYTE pbyData, int nSize, WORD  dwFormat=0);
    static DWORD       Hex2Str(CByteStream *pcSrc, CByteStream &cOut, WORD  dwFormat=0);
    static void        Str2Hex(PCSTR pStr, CByteStream & cOut);

    static int         Str2BCD(unsigned char *str, unsigned char *hex, unsigned int nLen);
    static DWORD       Str2BCD(CByteStream *pSrc, CByteStream &cOutData);
    static DWORD       Bcd2Str(CByteStream *pcSrc, CByteStream &cOut);
    
    static void        Dec2Bcd(DWORD dwValue, CByteStream &cOut);
    static DWORD       Bcd2Dec(CByteStream *pcSrc);
    
};

#endif