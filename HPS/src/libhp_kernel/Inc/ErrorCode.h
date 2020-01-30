
/* 
 * File Name: ErrorCode.h
 * Author: Tim.Ma & Alan.Ren
 * 
 * Create Date: 2019.03.01
 */

//
#ifndef _H_ERROR_CODE_H_
#define _H_ERROR_CODE_H_

// Common status
#define OK                                          0x0000

#define ERROR_BASE                                  ( 0xF000 )

// Error Code
#define ERROR_INVALID_PARAM                         ( ERROR_BASE + 0x01 )
#define ERROR_GET_TAGDATA_FAIL                      ( ERROR_BASE + 0XC6 )
#define d_FILE_ERR_INVALID_PARAM                    ( ERROR_BASE + 0xD0 )
#define d_FILE_ERR_OPEN_FAIL                        ( ERROR_BASE + 0xD1 )
#define d_FILE_ERR_WRITE_FAIL                       ( ERROR_BASE + 0xD2 )
#define d_FILE_ERR_SYNC_FAIL                        ( ERROR_BASE + 0xD3 )
#define d_FILE_ERR_FILENAME_TOO_LONG                ( ERROR_BASE + 0xD4 )
#define ERROR_CODE_GETPARAM_FAIL                    ( 0xEA00 )
#define ERROR_CODE_AIDLIST_FAIL                     ( 0xEA01 )
#define ERROR_CODE_AIDCONFIRM_FAIL                  ( 0xEA02 )
#define ERROR_CODE_AIDCONFIRM_REFUSE                ( 0xEA03 )
#define ERROR_CODE_EXCEPTFILE_CHECK_FAIL            ( 0xEA04 )
#define ERROR_CODE_INTERAC_POS_SEQUENCE_NBR         ( 0xEA05 )
#define ERROR_CODE_INTERAC_ACCOUNT_TYPE             ( 0xEA06 )
//#define ERROR_CODE_SELECT_LANG_FAIL                 ( 0XEA07 )
#define ERROR_CODE_INTERAC_RECEIPT_WARNING          ( 0xEA08 )

#endif
