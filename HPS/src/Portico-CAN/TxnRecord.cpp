
/*
 * File Name: TxnRecord.cpp
 * Author: Alan.Ren
 * 
 * Crate Date: 2019.4.3
 */

#include <ctosapi.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "AppMain.h"
#include "TxnRecord.h"
#include "dbghlp.h"

static const char defTxnRecordFileName_[] = "./de36a656eff5492182d9fe96146de54f.txnRecord";
static pthread_mutex_t cTxnRecordLocker_ = PTHREAD_MUTEX_INITIALIZER;

#define LockTxnRecord() { \
    pthread_mutex_lock(&cTxnRecordLocker_); \
}

#define UnlockTxnRecord() { \
    pthread_mutex_unlock(&cTxnRecordLocker_); \
}

/*
 * getLacalDateString()
 * 
 * Return:
 *        const char*: "YYYYMMDDhhmmss"
 */
static const char *getLocalDateString(void)
{
    static char szDateBuf_[32] = "19000101000000";
    time_t cTm = {0};
    struct tm *pcNewTm = NULL;

    time(&cTm);
    pcNewTm = localtime(&cTm);
    if (NULL != pcNewTm)
    {
        sprintf(szDateBuf_, "%04d%02d%02d%02d%02d%02d",
                (pcNewTm->tm_year + 1900),
                pcNewTm->tm_mon,
                pcNewTm->tm_mday,
                pcNewTm->tm_hour,
                pcNewTm->tm_min,
                pcNewTm->tm_sec);
    }
    return szDateBuf_;
}

/*
 * ClearTxn
 */
void ClearTxnRecord(void)
{
    LockTxnRecord();
    FILE *pcFile = NULL;
    do
    {
        pcFile = fopen(defTxnRecordFileName_, "w");
        if (NULL == pcFile)
            break;
        
        fsync(fileno(pcFile));
        fclose(pcFile);
        pcFile = NULL;
    } while (0);
    UnlockTxnRecord();
}


/*
 * SaveOneTxnRecord
 * 
 * Txn record format:
 * [PAN], [Date], [Amount]
 */
void SaveOneTxnRecord(IN const TxnRecord *pcTxnRecord)
{
    LockTxnRecord();
    FILE *pcFile = NULL;
    int nWriteLen = 0;
    const char *pszTxnDateStr = getLocalDateString();
    
    do
    {
        if (NULL == pcTxnRecord || 0 == strlen(pcTxnRecord->szPAN))
        {
            TraceMsg("Error: Function parameter error.");
            break;
        }

        pcFile = fopen(defTxnRecordFileName_, "a");
        if (NULL == pcFile)
        {
            TraceMsg("Error: Open %s file failed, error info [%s]",
                     defTxnRecordFileName_, strerror(errno));
            break;
        }

        // Write txn record date to record file.
        nWriteLen = fprintf(pcFile, "%s %s %llu\r\n",
                            pcTxnRecord->szPAN,
                            pszTxnDateStr,
                            pcTxnRecord->llTxnAmt);
        TraceMsg("Info: Txn record, PAN=%s, Date=%s, Amt=%llu",
                 pcTxnRecord->szPAN,
                 pszTxnDateStr,
                 pcTxnRecord->llTxnAmt);
        if (nWriteLen < 0)
        {
            TraceMsg("Error: Write txn record failed, error info %s",
                     strerror(errno));
        }

        // Flush and save txn record.
        fsync(fileno(pcFile));
        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    UnlockTxnRecord();
}

/*
 * GetTotalTxnAmount.
 */
unsigned long long GetTotalTxnAmount(IN const char *pszPAN)
{
    LockTxnRecord();
    unsigned long long ulTotalAmt = 0;
    FILE *pcFile = NULL;
    
    do
    {
        if (NULL == pszPAN || 0 == strlen(pszPAN))
        {
            TraceMsg("Error: Parameter error.");
            break;
        }
        
        pcFile = fopen(defTxnRecordFileName_, "r");
        if (NULL == pcFile)
        {
            TraceMsg("Error: Open %s file failed, error info %s", 
                    defTxnRecordFileName_, strerror(errno));
            break;
        }
        
        while (!feof(pcFile))
        {
            long long llAmt = 0;
            char szTxnDateBuf[32] = {0};
            char szPAN[32] = {0};
            int nRet;

            nRet = fscanf(pcFile, "%s %s %llu\r\n", szPAN,
                          szTxnDateBuf, &llAmt);
            TraceMsg("Info: fcanf ret=%d, szPan=%s, Date=%s, Amt=%lld",
                     nRet, szPAN, szTxnDateBuf, llAmt);
            if (nRet != 3)
            {
                TraceMsg("Error: Txn record format error.");
                break;
            }
            if (0 == strcmp(szPAN, pszPAN))
            {
                ulTotalAmt += llAmt;
            }
        }
        
        fclose(pcFile);
        pcFile = NULL;
    } while (0);

    UnlockTxnRecord();
    return ulTotalAmt;
}
