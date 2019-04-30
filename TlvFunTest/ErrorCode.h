/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   error_code.h
 * Author: Louis Lu
 *
 * Created on 2017�?????4�?????7�?????, 下午7:06
 */
//
#ifndef H_ERROR_CODE
#define H_ERROR_CODE

//Common status
#define OK                                          0x0000


#define ERROR_BASE                                  ( 0xF000 )

//Error Code
#define ERROR_INVALID_PARAM                         ( ERROR_BASE + 0x01 )
#define ERROR_GET_TAGDATA_FAIL                      ( ERROR_BASE + 0XC6 )


#define d_FILE_ERR_INVALID_PARAM                    ( ERROR_BASE + 0xD0 )
#define d_FILE_ERR_OPEN_FAIL                        ( ERROR_BASE + 0xD1 )
#define d_FILE_ERR_WRITE_FAIL                       ( ERROR_BASE + 0xD2 )
#define d_FILE_ERR_SYNC_FAIL                        ( ERROR_BASE + 0xD3 )
#define d_FILE_ERR_FILENAME_TOO_LONG                ( ERROR_BASE + 0xD4 )


#define ERROR_CODE_GETPARAM_FAIL                    0xEA00
#define ERROR_CODE_AIDLIST_FAIL                     0xEA01
#define ERROR_CODE_AIDCONFIRM_FAIL                  0xEA02
#define ERROR_CODE_AIDCONFIRM_REFUSE                0xEA03
#define ERROR_CODE_EXCEPTFILE_CHECK_FAIL            0xEA04

#endif /* ERROR_CODE_H */

