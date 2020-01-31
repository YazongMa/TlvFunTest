
/* 
 * File Name: JsonReqCmd.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.15
 */

#ifndef _H_H_json_req_cmd_H_H_
#define _H_H_json_req_cmd_H_H_

#include "Inc\BasicInc.h"
#include "Inc\JsonCmdProp.h"
#include "cJSON.h"

/*
 * Parse request json command.
 */
class CJsonReqCmd
{
public:
    CJsonReqCmd();
    virtual ~CJsonReqCmd();

    virtual BOOL ParseCmd(IN const char *pszJsonCmd);

    const int GetTxnType(void) const
    {
        return m_nTxnType;
    }

    const int GetCardType(void) const
    {
        return m_nCardType;
    }

    const unsigned long long GetAmt(void) const
    {
        return m_nAmt;
    }

    const unsigned long long GetOtherAmt(void) const
    {
        return m_nOtherAmt;
    }

    const int GetActType() const
    {
        return m_nActType;
    }
    
    const char* GetTxnGatewayID(void) const
    {
        return m_szTxnGatewayID;
    }
    
    const char* GetTxnTokenValue(void) const
    {
        return m_szTxnTokenValue;
    }

    const cJSON* GetJsonObjPtr(void) const;

    BOOL IsRequestCmdValid(void) const;

    void ClearReqCmd(void);

    static int StringToTxnType(IN const char *pszTxnTypeStr);
    static const char *TxnTypeToString(IN const int &nType);

    static int StringToCardType(IN const char *pszStr);
    static const char *CardTypeToString(IN const int &nCardType);

    static int StringToActType(IN const char *pszStr);

    inline void DetachJsonObj(void)
    {
        this->m_pReqJsonCmd = NULL;
    }

    inline CJsonReqCmd& operator=(IN const CJsonReqCmd &cObj)
    {
        if (this != &cObj)
        {
            this->ClearReqCmd();
            this->m_pReqJsonCmd = cObj.m_pReqJsonCmd;
            this->m_nTxnType = cObj.m_nTxnType;
            this->m_nCardType = cObj.m_nCardType;
            this->m_nActType = cObj.m_nActType;
            this->m_nAmt = cObj.m_nAmt;
            this->m_nOtherAmt = cObj.m_nOtherAmt;
            this->m_bIsRequestCmdValid = cObj.m_bIsRequestCmdValid;
            memcpy(this->m_szTxnGatewayID, cObj.m_szTxnGatewayID, sizeof(m_szTxnGatewayID));
            memcpy(this->m_szTxnTokenValue, cObj.m_szTxnTokenValue, sizeof(m_szTxnTokenValue));
        }
        return (*this);
    }

private:
    void FreeResource(void);
    void ResetCmdProp(void);
    BOOL CheckRequestCmd(IN const cJSON *pcJsonObj);

protected:
    cJSON *m_pReqJsonCmd;
    int m_nTxnType;
    int m_nCardType;
    int m_nActType;
    unsigned long long m_nAmt;
    unsigned long long m_nOtherAmt;
    BOOL m_bIsRequestCmdValid;
    char m_szTxnGatewayID[32];
    char m_szTxnTokenValue[64];
};

#endif