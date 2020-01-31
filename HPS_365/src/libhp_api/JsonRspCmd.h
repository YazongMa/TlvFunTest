
/*
 * File Name: JsonRspCmd.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.03.15
 */

#ifndef _H_H_json_rsp_cmd_H_H_
#define _H_H_json_rsp_cmd_H_H_

#include "Inc\BasicInc.h"

#include "Inc\JsonCmdProp.h"
#include "cJSON.h"
#include "..\libhp_kernel\Inc\IParameter.h"

class CJsonReqCmd;
class CHPApiContext;
class CJsonRspCmd
{
public:
    CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj);
    CJsonRspCmd(IN const char *pszValue, IN unsigned long dwStatus);
    CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj, IN unsigned long dwStatus);
    CJsonRspCmd(IN CJsonReqCmd *pcReqCmdObj, IN CHPApiContext *pcApiContext);
    virtual ~CJsonRspCmd();

    const char *PackResponse(void);

    void SetUiIndex(IN const int &nIndex);
    void SetTxnField(IN IParameter* pcParamObj);

protected:
    virtual void DoPackResponse(void);
    virtual void DoPackUserInputResponse(void);
    virtual void DoExtensionPack(IN cJSON *pcRoot);
    
private:
    void AddItemToObject(IN cJSON* pcJsonRoot, 
            IN const char *pszTag, 
            IN const char *pszValue);
    
    unsigned short _PackTransResult(cJSON *jsonRoot, char *tag, BYTE *tlvBuff, USHORT tlvBuffSize);

protected:
    const char *m_pszRspJsonStr;
    CJsonReqCmd *m_pcReqCmdObj;
    IParameter *m_pcParamPtr;
    int m_nUiIndex;
    DWORD m_dwStatus;
    const char *m_pszUserJsonData;
    CHPApiContext *m_pcApiContext;
};

#endif
