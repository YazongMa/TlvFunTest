
/*
 * File Name: TxnMsr.h
 * Author: Tim.Ma
 * 
 * Create Date: 2019.2.28
 */

#ifndef __HEARTLAND_PAYMENT_KERNEL_TXNMSR_H__
#define __HEARTLAND_PAYMENT_KERNEL_TXNMSR_H__


#include "TxnContext.h"

class PUBLIC_EXPORT CTxnMsr : public CTxnContext
{
public:
    CTxnMsr(IN IParameter *pParamPtr, 
            IN IDataPack *pDataPackPtr, 
            IN IBaseCurl *pBaseCurlPtr,
            IN IPinAction *pPinEntryPtr,
            IN IUserAction *pUserAction);
            
    virtual ~CTxnMsr();

    virtual int     PerformTxn(void);

protected:
    void            InitTransaction(void);

    void            ClearAllTrackData(void);

    void            ReadTrackData(void);

    void            DumpTrackData(void);
    
    WORD            RetrieveCardData(void);

    bool            CheckTrackData(void);

    bool            Track1IsEmv(const char *pszTrack1, size_t nTrack1Len);
    
    bool            Track2IsEmv(const char *pszTrack2, size_t nTrack2Len);
    
    void            SendReversal(void);

    WORD            ProcessMatchedTxnSeqNbr(void);

protected:
    struct TrackData 
    {
        BYTE byErr;
        BYTE byDataBuf[128];
        WORD wDataLen;
    };
    struct TrackData    m_cTrack1;
    struct TrackData    m_cTrack2;
    struct TrackData    m_cTrack3;
};

#endif
