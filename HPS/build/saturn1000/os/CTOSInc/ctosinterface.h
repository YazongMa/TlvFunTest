#ifndef CTOS_INTERFACE_H
#define CTOS_INTERFACE_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "typedef.h"
#include "font_def.h"

#define d_ERR_PRINTER						0x1600
#define d_NOT_SUPPORT						0xFF70
// Printer Error
#define d_PRINTER_HEAD_OVERHEAT				0x1602
#define d_PRINTER_PAPER_OUT					0x1603
#define d_PRINTER_BARCODE_GENERATE_ERR		0x1604
#define d_PRINTER_BARCODE_CONTENT_ERR		0x1605
#define d_PRINTER_BARCODE_CONTENT_LEN_ERR	0x1606
#define d_PRINTER_BARCODE_OUTSIDE_PAPER		0x1607
#define d_PRINTER_PARA_ERR					0x1608
#define d_PRINTER_PIC_FORMAT_NOT_SUPPORT	0x1609
#define d_PRINTER_PIC_OPEN_FAILED			0x160A
#define d_PRINTER_PIC_OVER_SIZE				0x160B
#define d_PRINTER_HAL_FAULT					0x160C
#define d_PRINTER_NOT_SUPPORT				0x1612
#define d_PRINTER_LANGUAGE_NOT_SUPPORT		0x1613

// Printer Buffer Error
#define d_PRTBUF_NOT_INIT					0x1701
#define d_PRTBUF_PARA_ERR 					0x1702
#define d_PRTBUF_CANVAS_OVERFLOW			0x1703
#define d_PRTBUF_PIC_FORMAT_NOT_SUPPORT		0x1704
#define d_PRTBUF_PIC_OPEN_FAILED			0x1705
#define d_PRTBUF_PIC_OVER_SIZE				0x1706
#define d_PRTBUF_LANGUAGE_NOT_SUPPORT		0x1707

#define d_SYSTEM_INVALID_PARA				0x0201


//--------For Aligned functions using----for internal use
#define d_NORMAL_CASE		  0
#define d_ALIGNLEFT_CASE	  1
#define d_ALIGNCENTER_CASE	  2
#define d_ALIGNRIGHT_CASE	  3
//--------------------------------------------------------
#define d_PRINTER_ALIGNLEFT	0
#define d_PRINTER_ALIGNCENTER	1
#define d_PRINTER_ALIGNRIGHT	2
// for Printer function
#define PAPER_X_SIZE			384
#define K_PRT_DOT_PER_LINE					384
#define K_PRT_MAX_Y8_SIZE					60

#define PB_CANVAS_X_SIZE			384
#define PB_CANVAS_Y_SIZE 			80
#define MAX_PB_CANVAS_NUM			2

// PRT Error (k_status.h)
#define K_PRT_SUCCESS					0
#define K_PRT_OUT_OF_RANGE				0x01
#define K_PRT_HEAD_TOO_HOT				0x02
#define K_PRT_OUT_OF_PAPAER				0x03
#define K_PRT_OVER_ROLL					0x04
#define K_PRT_HAL_FAULT					0x0C
#define K_PRT_NOT_SUPPORT				0x12

USHORT CTOS_FontSelectMode(BYTE bDevice,BYTE bMode);

USHORT CTOS_FontFNTNum(USHORT* pusIndex);
USHORT CTOS_FontFNTInfo(USHORT usIndex,USHORT* pusFontID,USHORT* pusFontSize);
USHORT CTOS_LibCAFontGetVersion(BYTE* verion_str);
USHORT CTOS_FontFNTSelectLanguage(BYTE bDevice,USHORT usLanguage);
USHORT CTOS_FontFNTSelectFont(BYTE bDevice,USHORT usFontID);
USHORT CTOS_FontFNTSelectASCIIFont(BYTE bDevice,USHORT usFontID);
USHORT CTOS_FontFNTSelectStyle(BYTE bDevice,USHORT usStyle);
USHORT CTOS_FontFNTSelectSize(BYTE bDevice,USHORT usFontSize);
USHORT CTOS_FontTTFCheckSupport(ULONG ulEncoding, BOOL *fSupported );
USHORT CTOS_FontTTFSelectFontFile(BYTE bDevice,BYTE *baFontFileName, BYTE bIndex);
USHORT CTOS_FontTTFSelectStyle(BYTE bDevice,USHORT usStyle);
USHORT CTOS_FontTTFSelectSize(BYTE bDevice,USHORT usFontSize);
USHORT CTOS_FontTTFSwitchDisplayMode(BYTE bDevice, USHORT usMode);
USHORT CTOS_FontTTFSwichDisplayMode(BYTE bDevice, USHORT usMode);

USHORT CTOS_LanguageConfig(USHORT usLanguage,USHORT usFontSize,USHORT usFontStyle, BOOL boSetDefault);
USHORT CTOS_LanguageLCDFontSize(USHORT usFontSize,USHORT usFontStyle);
USHORT CTOS_LanguagePrinterFontSize(USHORT usFontSize,USHORT usFontStyle, USHORT usSetType);
USHORT CTOS_LanguageInfo(USHORT usIndex,USHORT* pusLanguage,USHORT* pusFontSize,USHORT* pusFontStyle);
USHORT CTOS_LanguageNum(USHORT* pusIndex);
USHORT CTOS_LanguagePrinterSelectASCII(USHORT usASCIIFontID);
USHORT CTOS_LanguageLCDSelectASCII(USHORT usASCIIFontID);
USHORT CTOS_LanguagePrinterGetFontInfo(USHORT* pusASCIIFontID,USHORT* pusFontSize,USHORT* pusFontStyle);
USHORT CTOS_LanguageLCDGetFontInfo(USHORT* pusASCIIFontID,USHORT* pusFontSize,USHORT* pusFontStyle);
//-------------------------------------------------------------------------
//    Printer
//-------------------------------------------------------------------------
USHORT CTOS_PrinterSetDefaultASCIIStyle( UCHAR bStyle);
USHORT CTOS_PrinterBufferInit(BYTE *pPtr,USHORT usHeight);
USHORT CTOS_PrinterLogo(IN BYTE* baLogo,IN USHORT usXstart,IN USHORT usXsize,IN USHORT usY8Size);
USHORT CTOS_PrinterBufferPutString(BYTE* pbPtr, USHORT usXPos, USHORT usYPos, BYTE *baStr, CTOS_FONT_ATTRIB* ATTRIB);
USHORT CTOS_PrinterBufferOutput(BYTE* pbPtr, USHORT usY8Len);
USHORT CTOS_PrinterFline(IN USHORT usLines);
USHORT CTOS_PrinterPutString(UCHAR* baBuf);
//-------------------------------------------------------------------------

USHORT CTOS_PrinterFontSelectMode(BYTE bMode);
USHORT CTOS_PrinterTTFSelect(BYTE *baFilename, BYTE bIndex);
USHORT CTOS_PrinterTTFSwitchDisplayMode(USHORT usMode);
USHORT CTOS_PrinterTTFSwichDisplayMode(USHORT usMode);
USHORT CTOS_PrinterStatus(void);
USHORT CTOS_PrinterBMPPic(USHORT usX, BYTE *baFilename);
USHORT CTOS_PrinterBufferFill(USHORT usXStart, USHORT usYStart, USHORT usXEnd, USHORT usYEnd, BOOL fPat);
USHORT CTOS_PrinterBufferHLine(USHORT usXStart, USHORT usYStart, USHORT usXEnd, BOOL fPat);
USHORT CTOS_PrinterBufferVLine(USHORT usXStart, USHORT usYStart, USHORT usYEnd, BOOL fPat);
USHORT CTOS_PrinterBufferLogo(USHORT usXPos, USHORT usYPos, USHORT usWidth, USHORT usHeight, BYTE *baPat);
USHORT CTOS_PrinterBufferPixel(USHORT usXPos, USHORT usYPos,BOOL fPat);
USHORT CTOS_PrinterBufferBMPPic(BYTE* pbPtr, USHORT usXPos, USHORT usYPos, BYTE *baFilename);
void CTOS_PrinterBufferEnable(void);
USHORT CTOS_PrinterBufferSelectActiveAddress (BYTE *pPtr);
USHORT CTOS_PrinterPutStringAligned (UCHAR* pbBuf, BYTE bMode);
USHORT CTOS_PrinterBufferPutStringAligned(BYTE* pbPtr,  USHORT usYPos, BYTE *baStr, CTOS_FONT_ATTRIB* ATTRIB, BYTE bMode);
USHORT CTOS_PrinterSetHeatLevel(UCHAR bHeatLevel);


//-------------------------------------------------------------------------
//    LCD Function
//-------------------------------------------------------------------------
// Graphic Mode
USHORT CTOS_LCDGShowBMPPic(USHORT usX, USHORT usY, BYTE *baFilename);

// Text Mode
USHORT CTOS_LCDTClearDisplay(void);
USHORT CTOS_LCDTPrintXY(USHORT usX, USHORT usY, UCHAR* pbBuf);
USHORT CTOS_LCDTSetReverse(BOOL boReverse);

// User Mode
BYTE* CTOS_GETGBBuffer();

//----------------------------------------------------------------------------
USHORT CTOS_SCStatus(BYTE bID, BYTE* baStatus);
#ifdef __cplusplus
}
#endif

#endif
