
/*
 * File Name: HPApiContext.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.4.1
 */

#ifndef _H_H_HP_API_CONTEXT_H_H_
#define _H_H_HP_API_CONTEXT_H_H_

#include "Inc/BasicInc.h"
#include "JsonReqCmd.h"
#include "JsonRspCmd.h"

typedef struct
{
    int         m_nActType;
    char        m_szActData[512];
} HL_UserAction;

enum _TxnCard_EntryMode_
{
    kUnknown = -1,
    kSwipe,
    kTap,
    kInsert,
    kFallback, 
    kNone
};

#define STATE_DELAY_TIME (300)

class CHPApiContext
{
public:
    virtual ~CHPApiContext();

protected: // Protected access right function member.
    CHPApiContext();

    inline void LockDataAccess(void)
    {
        pthread_mutex_lock(&cMutex_);
    }

    inline void UnlockDataAccess(void)
    {
        pthread_mutex_unlock(&cMutex_);
    }

    inline void EnterCallApi(void)
    {
        pthread_mutex_lock(&cApiCallMutex_);
    }
    inline void LeaveCallApi(void)
    {
        pthread_mutex_unlock(&cApiCallMutex_);
    }

    inline void EnableCancel(IN const BOOL &bEnable)
    {
        LockDataAccess();
        m_bEnableCancel = bEnable;
        m_cCurCancelCmd.ClearReqCmd();
        UnlockDataAccess();
    }

    inline BOOL IsEnableCancel(void)
    {
        LockDataAccess();
        const BOOL bEnable = m_bEnableCancel;
        UnlockDataAccess();
        return bEnable;
    }

    inline void SetCurKeState(IN const int &nState)
    {
        LockDataAccess();
        if (nState != m_nCurState)
        {
            m_nCurState = nState;
        }
        // Update timeout value.
        m_ulStateTimeout = CTOS_TickGet() + STATE_DELAY_TIME;
        UnlockDataAccess();
    }
    
    inline int GetCurKeState(void)
    {
        LockDataAccess();
        const int nState = m_nCurState;
        UnlockDataAccess();
        return nState;
    }
    
    inline int GetCurApiMode(void)
    {
        LockDataAccess();
        const int nApiMode = m_nCurApiMode;
        UnlockDataAccess();
        return nApiMode;
    }

    inline void SetCurApiMode(IN const int &nApiMode)
    {
        LockDataAccess();
        if (nApiMode != m_nCurApiMode)
        {
            m_nLastApiMode = m_nCurApiMode;
            m_nCurApiMode = nApiMode;
        }
        UnlockDataAccess();
    }
    
    inline int GetLastApiMode(void)
    {
        LockDataAccess();
        const int nApiMode = m_nLastApiMode;
        UnlockDataAccess();
        return nApiMode;
    }

    inline BOOL IsEnableResponse(void)
    {
        LockDataAccess();
        const BOOL bEnable = m_bEnableResponse;
        UnlockDataAccess();
        return bEnable;
    }

    inline void EnableResponse(IN const BOOL &bEnable)
    {
        LockDataAccess();
        m_bEnableResponse = bEnable;
        UnlockDataAccess();
    }

    inline void SetCurStateTimeout(IN const ULONG &ulTimeout)
    {
        LockDataAccess();
        m_ulStateTimeout = ulTimeout;
        UnlockDataAccess();
    }

    inline ULONG GetCurStateTimeout(void)
    {
        LockDataAccess();
        const ULONG ulTimeout = m_ulStateTimeout;
        UnlockDataAccess();
        return ulTimeout;
    }

    inline void AddProcessingStatus(IN const DWORD &dwFlag)
    {
        LockDataAccess();
        m_dwProcessingStatus |= dwFlag;
        UnlockDataAccess();
    }

    inline void ResetProcessingStatus(void)
    {
        LockDataAccess();
        m_dwProcessingStatus &= 0x01;
        UnlockDataAccess();
    }

    inline void SetTxnEntryMode(IN const int &nEntryMode)
    {
        LockDataAccess();
        m_nDetectedCardType = nEntryMode;
        UnlockDataAccess();
    }


    inline void ResetTxnEntryMode(void)
    {
        LockDataAccess();
        m_nDetectedCardType = kUnknown;
        UnlockDataAccess();
    }

public:
    inline DWORD GetProcessingStatus(void)
    {
        LockDataAccess();
        const DWORD dwStatus = m_dwProcessingStatus;
        UnlockDataAccess();
        return dwStatus;
    }

    inline int GetTxnEntryMode(void)
    {
        LockDataAccess();
        const int nTxnEntryMode = m_nDetectedCardType;
        UnlockDataAccess();
        return nTxnEntryMode;
    }

protected: // Procetected access right data member.
    // Current Json command data.
    CJsonReqCmd m_cCurReqCmd;
    int m_nCurState;
    ULONG m_ulStateTimeout;
    HL_UserAction m_cActReqData;
    HL_UserAction m_cActRspData;

    // Cancel Cmd
    BOOL m_bEnableCancel;
    CJsonReqCmd m_cCurCancelCmd;

    // Current Api Mode 
    int m_nCurApiMode;
    int m_nLastApiMode;
    IParameter *m_pParamPtr;

    // Response
    BOOL m_bEnableResponse;

    // API kernel processing status
    DWORD m_dwProcessingStatus;

    // Locker
    pthread_mutex_t cMutex_;         // Data Mutex.
    pthread_mutex_t cApiCallMutex_;  // For PostRequest and GetResponse.

    // Api Kernel therad Id.
    pthread_t cApiThreadId_;

    // Detected card type
    int m_nDetectedCardType;
};

#endif
