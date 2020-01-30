
#ifndef __HEARTLAND_UTIL_TLV_H__
#define __HEARTLAND_UTIL_TLV_H__

#include "BasicTypeDef.h"

class PUBLIC_EXPORT CTlvFun
{
public:
    static unsigned short TLVGetTagLength(
                unsigned char *pbTag);
    
    static unsigned short TLVGetLengthLength(
                unsigned char *pbLength, 
                unsigned short *pusValueLen);
    
    static unsigned short TLVListCheckFormat(
                unsigned char *baTlvListBuf, 
                unsigned short usTlvListLen);
    
    static unsigned short TLVListReadTagPkt(
                unsigned char *baTagName, 
                unsigned char *baTlvListBuf, 
                unsigned short usTlvListLen, 
                unsigned char **pbTlvPkt, 
                unsigned short *pusTlvPktLen);
    
    static unsigned short TLVListReadTagValue(
                char *caTagName, 
                unsigned char *baTlvListBuf, 
                unsigned short usTlvListLen, 
                unsigned char **pbTlvValue, 
                unsigned short *pusTlvValueLen);
    
    static unsigned short TLVListAddTag(
                unsigned char *baTlvListBuf, 
                unsigned short *pusTlvListLen, 
                unsigned char *baTlvPkt, 
                unsigned short usTlvPktLen);
    
    static unsigned short TLVFormatTlv(
                char *caTagName, 
                unsigned short usTagLen, 
                unsigned char *baTagData, 
                unsigned short *pusTlvPktLen, 
                unsigned char *baTlvPktBuf);
    
    static unsigned short TLVFormatAndUpdateList(
                char *caTagName, 
                unsigned short usValueLen, 
                unsigned char *baValueData, 
                unsigned short *pusTlvListLen, 
                unsigned char *baTlvListBuf);
    
    static unsigned long TLVFormatAndUpdateListFromAnotherList(
                char caTagName[],
                unsigned short *pusTlvListDstLen, 
                unsigned char ucaTlvListDst[],
                unsigned short usTlvListSrcLen, 
                unsigned char ucaTlvListSrc[]);
    
    static unsigned long TLVReadAndUpdateListFromAnotherList(
                char caTagName[],
                unsigned char **ppucTlvValue, 
                unsigned short *pusTlvValueLen,
                unsigned short *pusTlvListDstLen, 
                unsigned char ucaTlvListDst[],
                unsigned short usTlvListSrcLen, 
                unsigned char ucaTlvListSrc[]);
};

#endif
