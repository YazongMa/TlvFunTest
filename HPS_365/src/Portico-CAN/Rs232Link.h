#ifndef __HEARTLAND_TESTAP_RS232LINK_H__
#define __HEARTLAND_TESTAP_RS232LINK_H__

#ifdef __cplusplus
extern "C"
{
#endif

void InitRs232Module(const char* pcRs232CfgFile);
void BypassRsponseData(const char *pszJsonData);

#ifdef __cplusplus
};
#endif

#endif
