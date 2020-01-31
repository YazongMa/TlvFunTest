/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   xmlnodeNets.h
 * Author: Administrator
 *
 * Created on 2018年7月20日, 上午11:26
 */

#ifndef XMLNODENETS_H
#define XMLNODENETS_H

#ifdef __cplusplus
extern "C" {
#endif

/*Parameter Config XML*/
//Parameter Config XML file Node & Name
#define CONFIG_XML_NAME            "emvcl_config.xml"
#define CONFIG_XML_ROOT_NODE       "configurationDescriptor"
#define CONFIG_XML_CLConfig        "CLConfig"
#define CONFIG_XML_TAG_NODE        "TagCombination"
#define CONFIG_XML_CAPK_NODE       "CAPKConfig"
#define CONFIG_XML_PARAMETER       "ParametersConfig"
    
    
#define CONFIG_EMV_XML_NAME        "emvcl_config.xml"
#define CONFIG_XML_Config          "Config"   
#define CONFIG_XML_APPLIST_NODE    "AppList"
#define CONFIG_XML_TERM_NODE       "TerminalConfig"
#define CONFIG_XML_APPCONFIG_NODE  "AppConfig"
    

//CAPKS
#define CAPK_GROUP               "Group"
#define CRID                     "RID"
#define ITEM                     "Item"
#define INDEX                    "index"
#define MODULES                  "modules"
#define EXPONENT                 "exponent"
#define HASH                     "hash"

//TermConfig
#define TERM_GROUP               "Group"
#define DAID                     "AID"
#define KERNEL_ID                "KernelID"
#define TXN_TYPE                 "TxnType"
#define ATTR                     "attribute"

#define EXPIRYDATA               "expirydata"
    
//AppList
#define ASI                      "ASI"
    
#define EMV_TAG                  "tag"
#define APPCONFIG_GROUP          "Group"

#define PARAINDEX               "ParaIndex"


#ifdef __cplusplus
}
#endif

#endif /* XMLNODEGP_H */

