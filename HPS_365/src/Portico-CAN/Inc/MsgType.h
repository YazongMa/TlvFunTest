/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MsgType.h
 * Author: Louis Lu
 *
 * Created on 2018å¹?01æœ?11æ—?, ä¸‹åˆ17:00
 */

#ifndef _H_MSGTYPE_H_
#define _H_MSGTYPE_H_




typedef enum enum_MSGTYPE
{
    MSG_SYS_MIN = 0,
	  
    /*********COMM_BEGIN*******************/
    MSG_COM1_SEND, 
    MSG_COM1_SEND_RESULT,
    MSG_COM1_RCV,
    MSG_COM1_UPDATE_REQ,
    MSG_COM1_UPDATE_RSP,
    MSG_COM1_CONN_STATUS_REQ,
    MSG_COM1_CONN_STATUS_RSP,
                   
    MSG_COM2_SEND, 
    MSG_COM2_SEND_RESULT,
    MSG_COM2_RCV,
    MSG_COM2_UPDATE_REQ,
    MSG_COM2_UPDATE_RSP,
    MSG_COM2_CONN_STATUS_REQ,
    MSG_COM2_CONN_STATUS_RSP,
    	                
    MSG_COM3_SEND,
    MSG_COM3_SEND_RESULT,            
    MSG_COM3_RCV, 
    MSG_COM3_UPDATE_REQ,
    MSG_COM3_UPDATE_RSP,
    MSG_COM3_CONN_STATUS_REQ,
    MSG_COM3_CONN_STATUS_RSP,	
                   
    MSG_ETH1_SEND, 
    MSG_ETH1_SEND_RESULT,
    MSG_ETH1_RCV,
    MSG_ETH1_UPDATE_REQ,
    MSG_ETH1_UPDATE_RSP,
    MSG_ETH1_CONN_STATUS_REQ,
    MSG_ETH1_CONN_STATUS_RSP,	
                   
    MSG_ETH2_SEND, 
    MSG_ETH2_SEND_RESULT,            
    MSG_ETH2_RCV,
    MSG_ETH2_UPDATE_REQ,
    MSG_ETH2_UPDATE_RSP,
    MSG_ETH2_CONN_STATUS_REQ,
    MSG_ETH2_CONN_STATUS_RSP,	
                   
    MSG_ETH3_SEND,
    MSG_ETH3_SEND_RESULT,            
    MSG_ETH3_RCV, 
    MSG_ETH3_UPDATE_REQ,
    MSG_ETH3_UPDATE_RSP,
    MSG_ETH3_CONN_STATUS_REQ,
    MSG_ETH3_CONN_STATUS_RSP,		
                   
    MSG_ETH4_SEND,
    MSG_ETH4_SEND_RESULT,            
    MSG_ETH4_RCV,	
    MSG_ETH4_UPDATE_REQ,
    MSG_ETH4_UPDATE_RSP,
    MSG_ETH4_CONN_STATUS_REQ,
    MSG_ETH4_CONN_STATUS_RSP,	
            
    MSG_ETH5_SEND,
    MSG_ETH5_SEND_RESULT,            
    MSG_ETH5_RCV,
    MSG_ETH5_UPDATE_REQ,
    MSG_ETH5_UPDATE_RSP,
    MSG_ETH5_CONN_STATUS_REQ,
    MSG_ETH5_CONN_STATUS_RSP,	
            
    MSG_ETH6_SEND, 
    MSG_ETH6_SEND_RESULT,
    MSG_ETH6_RCV,
    MSG_ETH6_UPDATE_REQ,
    MSG_ETH6_UPDATE_RSP,    
    MSG_ETH6_CONN_STATUS_REQ,
    MSG_ETH6_CONN_STATUS_RSP,	    
                   
    MSG_ETH7_SEND, 
    MSG_ETH7_SEND_RESULT,
    MSG_ETH7_RCV,
    MSG_ETH7_UPDATE_REQ,
    MSG_ETH7_UPDATE_RSP,
    MSG_ETH7_CONN_STATUS_REQ,
    MSG_ETH7_CONN_STATUS_RSP,	
                   
    MSG_ETH8_SEND, 
    MSG_ETH8_SEND_RESULT,            
    MSG_ETH8_RCV,
    MSG_ETH8_UPDATE_REQ,
    MSG_ETH8_UPDATE_RSP,
    MSG_ETH8_CONN_STATUS_REQ,
    MSG_ETH8_CONN_STATUS_RSP,	
                   
    MSG_ETH9_SEND,
    MSG_ETH9_SEND_RESULT,            
    MSG_ETH9_RCV, 
    MSG_ETH9_UPDATE_REQ,
    MSG_ETH9_UPDATE_RSP,
    MSG_ETH9_CONN_STATUS_REQ,
    MSG_ETH9_CONN_STATUS_RSP,		
                   
    MSG_ETH10_SEND,
    MSG_ETH10_SEND_RESULT,            
    MSG_ETH10_RCV,	
    MSG_ETH10_UPDATE_REQ,
    MSG_ETH10_UPDATE_RSP,
    MSG_ETH10_CONN_STATUS_REQ,
    MSG_ETH10_CONN_STATUS_RSP,	
            
    MSG_ETH11_SEND,
    MSG_ETH11_SEND_RESULT,            
    MSG_ETH11_RCV,
    MSG_ETH11_UPDATE_REQ,
    MSG_ETH11_UPDATE_RSP,
    MSG_ETH11_CONN_STATUS_REQ,
    MSG_ETH11_CONN_STATUS_RSP,	
            
    MSG_ETH12_SEND, 
    MSG_ETH12_SEND_RESULT,
    MSG_ETH12_RCV,
    MSG_ETH12_UPDATE_REQ,
    MSG_ETH12_UPDATE_RSP,    
    MSG_ETH12_CONN_STATUS_REQ,
    MSG_ETH12_CONN_STATUS_RSP,	   
                   
    MSG_WIFI1_SEND,
    MSG_WIFI1_SEND_RESULT,
    MSG_WIFI1_RCV,
    MSG_WIFI1_UPDATE_REQ,
    MSG_WIFI1_UPDATE_RSP,   
    MSG_WIFI1_CONN_STATUS_REQ,
    MSG_WIFI1_CONN_STATUS_RSP,	     
	
    MSG_WIFI2_SEND,
    MSG_WIFI2_SEND_RESULT,            
    MSG_WIFI2_RCV,	
    MSG_WIFI2_UPDATE_REQ,
    MSG_WIFI2_UPDATE_RSP,
    MSG_WIFI2_CONN_STATUS_REQ,
    MSG_WIFI2_CONN_STATUS_RSP,	  
            
    MSG_WIFI3_SEND, 
    MSG_WIFI3_SEND_RESULT,            
    MSG_WIFI3_RCV,	
    MSG_WIFI3_UPDATE_REQ,
    MSG_WIFI3_UPDATE_RSP,
    MSG_WIFI3_CONN_STATUS_REQ,
    MSG_WIFI3_CONN_STATUS_RSP,	  
            
    MSG_WIFI4_SEND,
    MSG_WIFI4_SEND_RESULT,            
    MSG_WIFI4_RCV,	
    MSG_WIFI4_UPDATE_REQ,
    MSG_WIFI4_UPDATE_RSP,
    MSG_WIFI4_CONN_STATUS_REQ,
    MSG_WIFI4_CONN_STATUS_RSP,	  
            
    MSG_WIFI5_SEND,
    MSG_WIFI5_SEND_RESULT,            
    MSG_WIFI5_RCV,	
    MSG_WIFI5_UPDATE_REQ,
    MSG_WIFI5_UPDATE_RSP,
    MSG_WIFI5_CONN_STATUS_REQ,
    MSG_WIFI5_CONN_STATUS_RSP,	  
            
    MSG_WIFI6_SEND,
    MSG_WIFI6_SEND_RESULT,            
    MSG_WIFI6_RCV,	
    MSG_WIFI6_UPDATE_REQ,
    MSG_WIFI6_UPDATE_RSP, 
    MSG_WIFI6_CONN_STATUS_REQ,
    MSG_WIFI6_CONN_STATUS_RSP,	    
                   
    MSG_WIFI7_SEND,
    MSG_WIFI7_SEND_RESULT,
    MSG_WIFI7_RCV,
    MSG_WIFI7_UPDATE_REQ,
    MSG_WIFI7_UPDATE_RSP,   
    MSG_WIFI7_CONN_STATUS_REQ,
    MSG_WIFI7_CONN_STATUS_RSP,	     
	
    MSG_WIFI8_SEND,
    MSG_WIFI8_SEND_RESULT,            
    MSG_WIFI8_RCV,	
    MSG_WIFI8_UPDATE_REQ,
    MSG_WIFI8_UPDATE_RSP,
    MSG_WIFI8_CONN_STATUS_REQ,
    MSG_WIFI8_CONN_STATUS_RSP,	  
            
    MSG_WIFI9_SEND, 
    MSG_WIFI9_SEND_RESULT,            
    MSG_WIFI9_RCV,	
    MSG_WIFI9_UPDATE_REQ,
    MSG_WIFI9_UPDATE_RSP,
    MSG_WIFI9_CONN_STATUS_REQ,
    MSG_WIFI9_CONN_STATUS_RSP,	  
            
    MSG_WIFI10_SEND,
    MSG_WIFI10_SEND_RESULT,            
    MSG_WIFI10_RCV,	
    MSG_WIFI10_UPDATE_REQ,
    MSG_WIFI10_UPDATE_RSP,
    MSG_WIFI10_CONN_STATUS_REQ,
    MSG_WIFI10_CONN_STATUS_RSP,	  
            
    MSG_WIFI11_SEND,
    MSG_WIFI11_SEND_RESULT,            
    MSG_WIFI11_RCV,	
    MSG_WIFI11_UPDATE_REQ,
    MSG_WIFI11_UPDATE_RSP,
    MSG_WIFI11_CONN_STATUS_REQ,
    MSG_WIFI11_CONN_STATUS_RSP,	  
            
    MSG_WIFI12_SEND,
    MSG_WIFI12_SEND_RESULT,            
    MSG_WIFI12_RCV,	
    MSG_WIFI12_UPDATE_REQ,
    MSG_WIFI12_UPDATE_RSP, 
    MSG_WIFI12_CONN_STATUS_REQ,
    MSG_WIFI12_CONN_STATUS_RSP,	  
            
    MSG_GPRS1_SEND,
    MSG_GPRS1_SEND_RESULT,            
    MSG_GPRS1_RCV,	
    MSG_GPRS1_UPDATE_REQ,
    MSG_GPRS1_UPDATE_RSP,
    MSG_GPRS1_CONN_STATUS_REQ,
    MSG_GPRS1_CONN_STATUS_RSP,	  
            
    MSG_GPRS2_SEND,
    MSG_GPRS2_SEND_RESULT,            
    MSG_GPRS2_RCV,	
    MSG_GPRS2_UPDATE_REQ,
    MSG_GPRS2_UPDATE_RSP,  
    MSG_GPRS2_CONN_STATUS_REQ,
    MSG_GPRS2_CONN_STATUS_RSP,	             
    
    MSG_GPRS3_SEND,
    MSG_GPRS3_SEND_RESULT,            
    MSG_GPRS3_RCV,	
    MSG_GPRS3_UPDATE_REQ,
    MSG_GPRS3_UPDATE_RSP,
    MSG_GPRS3_CONN_STATUS_REQ,
    MSG_GPRS3_CONN_STATUS_RSP,	
            
    MSG_GPRS4_SEND, 
    MSG_GPRS4_SEND_RESULT,            
    MSG_GPRS4_RCV,	
    MSG_GPRS4_UPDATE_REQ,
    MSG_GPRS4_UPDATE_RSP,    
    MSG_GPRS4_CONN_STATUS_REQ,
    MSG_GPRS4_CONN_STATUS_RSP,	           
    
    MSG_GPRS5_SEND,
    MSG_GPRS5_SEND_RESULT,
    MSG_GPRS5_RCV,	
    MSG_GPRS5_UPDATE_REQ,
    MSG_GPRS5_UPDATE_RSP,
    MSG_GPRS5_CONN_STATUS_REQ,
    MSG_GPRS5_CONN_STATUS_RSP,	
            
    MSG_GPRS6_SEND,
    MSG_GPRS6_SEND_RESULT,
    MSG_GPRS6_RCV,
    MSG_GPRS6_UPDATE_REQ,
    MSG_GPRS6_UPDATE_RSP,
    MSG_GPRS6_CONN_STATUS_REQ,
    MSG_GPRS6_CONN_STATUS_RSP,	
            
    MSG_GPRS7_SEND,
    MSG_GPRS7_SEND_RESULT,            
    MSG_GPRS7_RCV,	
    MSG_GPRS7_UPDATE_REQ,
    MSG_GPRS7_UPDATE_RSP,
    MSG_GPRS7_CONN_STATUS_REQ,
    MSG_GPRS7_CONN_STATUS_RSP,	  
            
    MSG_GPRS8_SEND,
    MSG_GPRS8_SEND_RESULT,            
    MSG_GPRS8_RCV,	
    MSG_GPRS8_UPDATE_REQ,
    MSG_GPRS8_UPDATE_RSP,  
    MSG_GPRS8_CONN_STATUS_REQ,
    MSG_GPRS8_CONN_STATUS_RSP,	             
    
    MSG_GPRS9_SEND,
    MSG_GPRS9_SEND_RESULT,            
    MSG_GPRS9_RCV,	
    MSG_GPRS9_UPDATE_REQ,
    MSG_GPRS9_UPDATE_RSP,
    MSG_GPRS9_CONN_STATUS_REQ,
    MSG_GPRS9_CONN_STATUS_RSP,	
            
    MSG_GPRS10_SEND, 
    MSG_GPRS10_SEND_RESULT,            
    MSG_GPRS10_RCV,	
    MSG_GPRS10_UPDATE_REQ,
    MSG_GPRS10_UPDATE_RSP,    
    MSG_GPRS10_CONN_STATUS_REQ,
    MSG_GPRS10_CONN_STATUS_RSP,	           
    
    MSG_GPRS11_SEND,
    MSG_GPRS11_SEND_RESULT,
    MSG_GPRS11_RCV,	
    MSG_GPRS11_UPDATE_REQ,
    MSG_GPRS11_UPDATE_RSP,
    MSG_GPRS11_CONN_STATUS_REQ,
    MSG_GPRS11_CONN_STATUS_RSP,	
            
    MSG_GPRS12_SEND,
    MSG_GPRS12_SEND_RESULT,
    MSG_GPRS12_RCV,
    MSG_GPRS12_UPDATE_REQ,
    MSG_GPRS12_UPDATE_RSP,
    MSG_GPRS12_CONN_STATUS_REQ,
    MSG_GPRS12_CONN_STATUS_RSP,	
    				                           
    				
    /*********CTMS_BEGIN*******************/
    MSG_CTMS_CLIENT_STOP_REQ,               /* CTMS->MAIN */
    MSG_CTMS_CLIENT_STOP_RSP,               /* MAIN->CTMS */

    MSG_CTMS_PRM_DOWNLOAD,                  /* CLOUD Modlue would send this msg if PRM downloaded */
    MSG_CTMS_DATA_CHCEK,                    /* CTMS has two detection mechanisms:
                                                1. automatic detection, the detection interval is TimeSpan (CastlesCloud.xml) designated time.
                                                2. detect when receiving the message.*/
    MSG_CTMS_DATA_CHCEK_DONE,               /* The value of wParam for MSG_CTMS_DATA_CHCEK_DONE:
                                                0 : ctms check OK
                                                1 : ctms no update content
                                                2 : ctms connect failed	*/
      
    /*******Merchant Portal MsgType*******/      
    MSG_CTIO_DATA_UPLOAD,
    
    /*********MANAGE_BEGIN*******************/
    MSG_STOP_COMM,              
    MSG_STOP_CLOUD,
    
    
    /*********ERR_BEGIN***************************/  
    MSG_ERR,                     
    MSG_WARNING, 
            
    /***************GPRS common******************/
    MSG_GPRS_APN_SWITCH_REQ,
    MSG_GPRS_APN_SWITCH_RSP,
    MSG_SYS_MAX = 2048,  /* please add system message type before MSG_SYS_MAX */
    
    /**********USER_BEGIN*****************/
    MSG_USER_MIN = 2049,
    MSG_UI_REQ,
    MSG_UI_RSP,
    MSG_STOP_UI,
    MSG_STOP_TXN,
    MSG_STOP_PROTOCOL,
    MSG_USER_TXN_REQ,
    MSG_USER_TXN_RSP,
    MSG_USER_CURL_REQ,
    MSG_USER_CURL_RSP,
            
    MSG_USER_MAX /* please add user message type before MSG_USER_MAX */
    /**********USER_END*****************/
    
} MSG_TYPE;




#endif