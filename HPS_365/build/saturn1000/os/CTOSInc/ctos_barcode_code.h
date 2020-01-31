#ifndef CTOS_BARCODE_H
#define CTOS_BARCODE_H

#ifdef	__cplusplus
extern "C" {
#endif


USHORT CTOS_PrinterCode128Barcode(IN USHORT x, IN USHORT y, IN BYTE *baCodeContent, IN BYTE bContentLen, IN BYTE bXExtend, IN BYTE bY8Extend, IN BOOL bShowChar);

#ifdef	__cplusplus
}
#endif

#endif
