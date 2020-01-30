#ifndef _LIBVKBD_H
#define	_LIBVKBD_H

#ifdef	__cplusplus
extern "C" {
#endif

// key value define  
#define d_KBD_a                                                         'a'
#define d_KBD_b                                                         'b'
#define d_KBD_c                                                         'c'
#define d_KBD_d                                                         'd'
#define d_KBD_e                                                         'e'
#define d_KBD_f                                                         'f'
#define d_KBD_g                                                         'g'
#define d_KBD_h                                                         'h'
#define d_KBD_i                                                         'i'
#define d_KBD_j                                                         'j'
#define d_KBD_k                                                         'k'
#define d_KBD_l                                                         'l'
#define d_KBD_m                                                         'm'
#define d_KBD_n                                                         'n'
#define d_KBD_o                                                         'o'
#define d_KBD_p                                                         'p'
#define d_KBD_q                                                         'q'
#define d_KBD_r                                                         'r'
#define d_KBD_s                                                         's'
#define d_KBD_t                                                         't'
#define d_KBD_u                                                         'u'
#define d_KBD_v                                                         'v'
#define d_KBD_w                                                         'w'
#define d_KBD_x                                                         'x'
#define d_KBD_y                                                         'y'
#define d_KBD_z                                                         'z'
    
#define d_KBD_A                                                         0x00
#define d_KBD_B                                                         0x01
#define d_KBD_C                                                         0x02
#define d_KBD_D                                                         0x03
#define d_KBD_E                                                         0x04
#define d_KBD_F                                                         0x05
#define d_KBD_G                                                         0x06
#define d_KBD_H                                                         0x07
#define d_KBD_I                                                         0x08
#define d_KBD_J                                                         0x09
#define d_KBD_K                                                         0x0a
#define d_KBD_L                                                         0x0b
#define d_KBD_M                                                         0x0c
#define d_KBD_N                                                         0x0d
#define d_KBD_O                                                         0x0e
#define d_KBD_P                                                         0x0f
#define d_KBD_Q                                                         0x10
#define d_KBD_R                                                         0x11
#define d_KBD_S                                                         0x12
#define d_KBD_T                                                         0x13
#define d_KBD_U                                                         0x14
#define d_KBD_V                                                         0x15
#define d_KBD_W                                                         0x16
//#define d_KBD_X                                                       refer to ctosapi.h
#define d_KBD_Y                                                         0x18
#define d_KBD_Z                                                         0x19    
   
#define d_KBD_TILDE                                                     0x20//~
#define d_KBD_EXCLAMATION                                               0x21//!
#define d_KBD_AT                                                        0x22//@
#define d_KBD_POUND                                                     0x23//#
#define d_KBD_DOLLAR                                                    0x24//$
#define d_KBD_PER                                                       0x25//%
#define d_KBD_CARET                                                     0x26//^
#define d_KBD_AMPERSAND                                                 0x27//&
#define d_KBD_STAR                                                      0x28//*
#define d_KBD_OPEN_PAREN                                                0x29//(
#define d_KBD_CLOSE_PAREN                                               0x2a//)
#define d_KBD_OPEN_BRACKET                                              0x2b//[
#define d_KBD_CLOSE_BRACKET                                             0x2c//]
#define d_KBD_OPEN_BRACE                                                0x2d//{
#define d_KBD_CLOSE_BRACE                                               0x2e//}
#define d_KBD_PLUS                                                      0x2f//+
#define d_KBD_MINUS                                                     0x80//-
#define d_KBD_EQUAL                                                     0x81//=
#define d_KBD_UNDERLINE                                                 0x82//_
#define d_KBD_VERTICAL_BAR                                              0x83//|
#define d_KBD_BACKSLASH                                                 0x84//\                           .
#define d_KBD_DOUBLE_QUOTATION                                          0x85//"
#define d_KBD_QUOTATION                                                 0x86//'
#define d_KBD_APOSTROPHE                                                0x87//`
#define d_KBD_COLON                                                     0x88//:
#define d_KBD_SEMICOLON                                                 0x89//;
#define d_KBD_QUESTION                                                  0x8a//?
#define d_KBD_LESS_THAN                                                 0x8b//<
#define d_KBD_GREATER_THAN                                              0x8c//>
#define d_KBD_PERIOD                                                    0x8d//.
//#define d_KBD_COMMA//,                                                refer to ctosapi.h
#define d_KBD_SLASH                                                     0x8e///
    
#define d_KBD_SPACE                                                     0x8f
    

// virtual keyboard error code
#define ERROR_VKBD_INVALID_KEY                                      (0xFA01)
#define ERROR_VKBD_ENABLE_FAIL                                      (0xFA02)
#define ERROR_VKBD_THREAD_DEAD                                      (0xFA03)
#define ERROR_VKBD_WITHOUT_UI                                       (0xFA04)
#define ERROR_VKBD_TOUCH_INIT_FAIL                                  (0xFA05)
#define ERROR_VKBD_TOUCH_TIMEOUT                                    (0xFA06)
#define ERROR_VKBD_PARAM_NULL                                       (0xFA07)
#define ERROR_VKBD_INVALID_MODE                                     (0xFA08)
    
#define VKBD_NUMBER_MODE                                            1
#define VKBD_LETTER_MODE                                            2
#define VKBD_CAP_LETTER_MODE                                        3
#define VKBD_SPECIAL_CHARACTER_MODE                                 4    
    
WORD CTAP_VirtualKBDOpen(void);
WORD CTAP_VirtualKBDClose(void);
WORD CTAP_VirtualKBDShow(void);
WORD CTAP_VirtualKBDHide(void);
WORD CTAP_VirtualKBDBufFlush(void);
WORD CTAP_VirtualKBDGet(BYTE* pbyKey);
WORD CTAP_VirtualKBDHit(BYTE* pbyKey);
WORD CTAP_VirtualKBDSetSound(BOOL  bOnOff);
WORD CTAP_VirtualKBDSetDefaultMode(BYTE byDefMode);
    

#ifdef	__cplusplus
}
#endif



#endif	/* _DYNAMICLIB_H */

