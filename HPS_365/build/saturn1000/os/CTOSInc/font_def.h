#ifndef __FONT_DEF_H__
#define __FONT_DEF_H__

#include "typedef.h"
//=============================================================================================================================
//
// Font Functions
//
//=============================================================================================================================
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	USHORT FontSize;
	USHORT X_Zoom;
	USHORT Y_Zoom;
	USHORT X_Space;
	USHORT Y_Space;
} CTOS_FONT_ATTRIB;

#define d_PRT_ASCII_DEFAULT    0  	//check
#define d_PRT_ASCII_BOLD       1	//check
//----------------------------------------------------------------------------------------Mark_Bottom
#define d_UTF_16
#define d_FONT_MAX_PAT_LEN	255*32 //support maximum font size 255*255

#define d_FNT_MODE 			0
#define d_TTF_NORM_MODE 	1
#define d_TTF_HB_MODE 		2
//-----------------------------------
#define d_LANGUAGE_DEFAULT    0
#define d_LANGUAGE_KHMER 	  1
#define d_LANGUAGE_ARABIC 	  2
#define d_LANGUAGE_THAI 	  3
//-----------------------------------
// font device definition
#define d_FONT_DEVICE_LCD_0                                                 0x00
#define d_FONT_DEVICE_LCD_1                                                 0x01
#define d_FONT_DEVICE_PRINTER                                               0x80

// FNT Lauguage definition
#define d_FNT_LANGUAGE_ALPHABET                                             0x0000
#define d_FNT_LANGUAGE_CHINESE_TRADITIONAL                                  0x0001  //Chinese(Traiditional)
#define d_FNT_LANGUAGE_CHINESE_SIMPLIFIED                                   0x0002    //Chinese(Simplified)
#define d_FNT_LANGUAGE_JAPANESE                                             0x0003    //Japanese
#define d_FNT_LANGUAGE_THAI                                                 0x0004    //Thai
#define d_FNT_LANGUAGE_VIETNAMESE                                           0x0005    //Vietnamese
#define d_FNT_LANGUAGE_KOREAN                                               0x0006    //Korean
#define d_FNT_LANGUAGE_RUSSIAN                                              0x0007  //Russian
#define d_FNT_LANGUAGE_PORTUGUESE_PORTUGAL                                  d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_SPANISH                                              d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_TURKISH                                              d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_FARSI                                                d_FNT_LANGUAGE_ALPHABET

#define d_FNT_LANGUAGE_PORTUGUESE_BRAZIL                                    d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_CZECH                                                d_FNT_LANGUAGE_ALPHABET
#define d_FNT_LANGUAGE_ARABIC_QATAR                                         d_FNT_LANGUAGE_ALPHABET

// FNT FontID definition
#define d_FNT_FONTID_EMPTY                                                  0       //No font
#define d_FNT_FONTID_CHINESE_TAIWAN                                         1028    //Chinese(Taiwan)
#define d_FNT_FONTID_CZECH                                                  1029    //Czech
#define d_FNT_FONTID_JAPANESE                                               1041    //Japanese
#define d_FNT_FONTID_KOREAN                                                 1042    //Korean
#define d_FNT_FONTID_PORTUGUESE_BRAZIL                                      1046    //Portuguese(Brazil)
#define d_FNT_FONTID_RUSSIAN                                                1049    //Russian
#define d_FNT_FONTID_THAI                                                   1054    //Thai
#define d_FNT_FONTID_TURKISH                                                1055    //Turkish
#define d_FNT_FONTID_FARSI                                                  1065    //Farsi
#define d_FNT_FONTID_VIETNAMESE                                             1066    //Vietnamese
#define d_FNT_FONTID_CHINESE_PRC                                            2052    //Chinese(PRC)
#define d_FNT_FONTID_CHINESE_PRC1                                           2054    //Simple(Song)
#define d_FNT_FONTID_PORTUGUESE_PORTUGAL                                    2070    //Portuguese(Portugal)
#define d_FNT_FONTID_SPANISH                                                3082    //Spanish(Spain)
#define d_FNT_FONTID_ARABIC_QATAR                                           16385   //Arabic(Qatar)
#define d_FNT_FONTID_CHINESE_TAIWAN_PLUS                                    64507   //Chinese(Taiwan) with Symbol
#define d_FNT_FONTID_DEFAULTASCII                                           36864

// Font language
// ASCII < 0x80 is world-wide same
// ASCII >=0x80 is language dependent
#define d_TTF_C0_CONTROLS                                         0x0000
#define d_TTF_BASIC_LATIN                                         0x0020
#define d_TTF_C1_CONTROLS                                         0x0080
#define d_TTF_LATIN_1_SUPPLEMENT                                  0x00A0
#define d_TTF_LATIN_EXTENDED_A                                    0x0100
#define d_TTF_LATIN_EXTENDED_B                                    0x0180
#define d_TTF_IPA_EXTENSIONS                                      0x0250
#define d_TTF_SPACING_MODIFIERS                                   0x02B0
#define d_TTF_COMBINING_DIACRITICS_MARKS                          0x0300
#define d_TTF_GREEK_AND_COPTIC                                    0x0370
#define d_TTF_CYRILLIC                                            0x0400
#define d_TTF_CYRILLIC_SUPPLEMENT                                 0x0500
#define d_TTF_ARMENIAN                                            0x0530
#define d_TTF_HEBREW                                              0x0590
#define d_TTF_ARABIC                                              0x0600
#define d_TTF_SYRIAC                                              0x0700
#define d_TTF_ARABIC_SUPPLEMENT                                   0x0750
#define d_TTF_THAANA                                              0x0780
#define d_TTF_N_KO                                                0x07C0
#define d_TTF_SAMARITAN                                           0x0800
#define d_TTF_MANDAIC                                             0x0840
#define d_TTF_DEVANAGARI                                          0x0900
#define d_TTF_BENGALI                                             0x0980
#define d_TTF_GURMUKHI                                            0x0A00
#define d_TTF_GUJARATI                                            0x0A80
#define d_TTF_ORIYA                                               0x0B00
#define d_TTF_TAMIL                                               0x0B80
#define d_TTF_TELUGU                                              0x0C00
#define d_TTF_KANNADA                                             0x0C80
#define d_TTF_MALAYALAM                                           0x0D00
#define d_TTF_SINHALA                                             0x0D80
#define d_TTF_THAI                                                0x0E00
#define d_TTF_LAO                                                 0x0E80
#define d_TTF_TIBETAN                                             0x0F00
#define d_TTF_MYANMAR                                             0x1000
#define d_TTF_GEORGIAN                                            0x10A0
#define d_TTF_HANGUL_JAMO                                         0x1100
#define d_TTF_ETHIOPIC                                            0x1200
#define d_TTF_ETHIOPIC_SUPPLEMENT                                 0x1380
#define d_TTF_CHEROKEE                                            0x13A0
#define d_TTF_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS               0x1400
#define d_TTF_OGHAM                                               0x1680
#define d_TTF_RUNIC                                               0x16A0
#define d_TTF_TAGALOG                                             0x1700
#define d_TTF_HANUNOO                                             0x1720
#define d_TTF_BUHID                                               0x1740
#define d_TTF_TAGBANWA                                            0x1760
#define d_TTF_KHMER                                               0x1780
#define d_TTF_MONGOLIAN                                           0x1800
#define d_TTF_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED      0x18B0
#define d_TTF_LIMBU                                               0x1900
#define d_TTF_TAI_LE                                              0x1950
#define d_TTF_NEW_TAI_LUE                                         0x1980
#define d_TTF_KHMER_SYMBOLS                                       0x19E0
#define d_TTF_BUGINESE                                            0x1A00
#define d_TTF_TAI_THAM                                            0x1A20
#define d_TTF_BALINESE                                            0x1B00
#define d_TTF_SUNDANESE                                           0x1B80
#define d_TTF_BATAK                                               0x1BC0
#define d_TTF_LEPCHA                                              0x1C00
#define d_TTF_OL_CHIKI                                            0x1C50
#define d_TTF_SUDANESE_SUPPLEMENT                                 0x1CC0
#define d_TTF_VEDIC_EXTENSIONS                                    0x1CD0
#define d_TTF_PHONETIC_EXTENSIONS                                 0x1D00
#define d_TTF_PHONETIC_EXTENSIONS_SUPPLEMENT                      0x1D80
#define d_TTF_COMBINING_DIACRITICS_MARKS_SUPPLEMENT               0x1DC0
#define d_TTF_LATIN_EXTENDED_ADDITIONAL                           0x1E00
#define d_TTF_GREEK_EXTENDED                                      0x1F00
#define d_TTF_GENERAL_PUNCTUATION                                 0x2000
#define d_TTF_SUPERSCRIPTS_AND_SUBSCRIPTS                         0x2070
#define d_TTF_CURRENCY_SYMBOLS                                    0x20A0
#define d_TTF_COMBINING_DIACRITICS_MARKS_FOR_SYMBOLS              0x20D0
#define d_TTF_LETTERLIKE_SYMBOLS                                  0x2100
#define d_TTF_NUMBER_FORM                                         0x2150
#define d_TTF_ARROWS                                              0x2190
#define d_TTF_MATHEMATICAL_OPERATOR                               0x2200
#define d_TTF_MISCELLANEOUS_TECHNICAL                             0x2300
#define d_TTF_CONTROL_PICTURES                                    0x2400
#define d_TTF_OPTICAL_CHARACTER_RECOGNITION                       0x2440
#define d_TTF_ENCLOSED_ALPHANUMERICS                              0x2460
#define d_TTF_BOX_DRAWING                                         0x2500
#define d_TTF_BLOCK_ELEMENT                                       0x2580
#define d_TTF_GEOMETRIC_SHAPES                                    0x25A0
#define d_TTF_MISCELLANEOUS_SYMBOLS                               0x2600
#define d_TTF_DINGBATS                                            0x2700
#define d_TTF_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A                0x27C0
#define d_TTF_SUPPLEMENTAL_ARROWS_A                               0x27F0
#define d_TTF_BRAILLE_PATTERNS                                    0x2800
#define d_TTF_SUPPLEMENTAL_ARROWS_B                               0x2900
#define d_TTF_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B                0x2980
#define d_TTF_SUPPLEMENTAL_MATHEMATICAL_OPERATOR                  0x2A00
#define d_TTF_MISCELLANEOUS_SYMBOLS_AND_ARROWS                    0x2B00
#define d_TTF_GLAGOLITIC                                          0x2C00
#define d_TTF_LATIN_EXTENDED_C                                    0x2C60
#define d_TTF_COPTIC                                              0x2C80
#define d_TTF_GEORGIAN_SUPPLEMENT                                 0x2D00
#define d_TTF_TIFINAGH                                            0x2D30
#define d_TTF_ETHIOPIC_EXTENDED                                   0x2D80
#define d_TTF_SUPPLEMENTAL_PUNCTUATION                            0x2E00
#define d_TTF_CJK_RADICALS_SUPPLEMENT                             0x2E80
#define d_TTF_KANGXI_RADICALS                                     0x2F00
#define d_TTF_IDEOGRAPHIC_DESCRIPTION_CHARACTERS                  0x2FF0
#define d_TTF_CJK_SYMBOLS_AND_PUNCTUATION                         0x3000
#define d_TTF_HIRAGANA                                            0x3040
#define d_TTF_KATAKANA                                            0x30A0
#define d_TTF_BOPOMOFO                                            0x3100
#define d_TTF_HANGUL_COMPATIBILITY_JAMO                           0x3130
#define d_TTF_KANBUN                                              0x3190
#define d_TTF_BOPOMOFO_EXTENDED                                   0x31A0
#define d_TTF_CJK_STROKES                                         0x31C0
#define d_TTF_KATAKANA_PHONETIC_EXTENSIONS                        0x31F0
#define d_TTF_ENCLOSED_CJK_LETTERS_AND_MONTHS                     0x3200
#define d_TTF_CJK_COMPATIBILITY                                   0x3300
#define d_TTF_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A                  0x3400
#define d_TTF_YIJING_HEXAGRAMS_SYMBOLS                            0x4DC0
#define d_TTF_CJK_UNIFIED_IDEOGRAPHS                              0x4E00
#define d_TTF_YI_SYLLABLES                                        0xA000
#define d_TTF_YI_RADICALS                                         0xA490
#define d_TTF_LISU                                                0xA4D0
#define d_TTF_VAI                                                 0xA500
#define d_TTF_CYRILLIC_EXTENDED_B                                 0xA640
#define d_TTF_BAMUM                                               0xA6A0
#define d_TTF_MODIFIER_TONE_LETTERS                               0xA700
#define d_TTF_LATIN_EXTENDED_D                                    0xA720
#define d_TTF_SYLOTI_NAGRI                                        0xA800
#define d_TTF_IND_NO                                              0xA830
#define d_TTF_PHAGS_PA                                            0xA840
#define d_TTF_SAURASHTRA                                          0xA880
#define d_TTF_DEVA_EXT                                            0xA8E0
#define d_TTF_KAYAH_LI                                            0xA900
#define d_TTF_REJANG                                              0xA930
#define d_TTF_JAVANESE                                            0xA980
#define d_TTF_CHAM                                                0xAA00
#define d_TTF_MYANMAR_EXTA                                        0xAA60
#define d_TTF_TAI_VIET                                            0xAA80
#define d_TTF_MEETEI_EXT                                          0xAAE0
#define d_TTF_ETHIOPIC_EXT_A                                      0xAB00
#define d_TTF_MEETEI_MAYEK                                        0xABC0
#define d_TTF_HANGUL_SYLLABLES                                    0xAC00
#define d_TTF_HANGUL_JAMO_EXTENDED_B                              0xD7B0
#define d_TTF_HIGH_HALF_ZONE_OF_UTF_16                            0xD800
#define d_TTF_LOW_HALF_ZONE_OF_UTF_16                             0xDC00
#define d_TTF_PRIVATE_USE_ZONE                                    0xE000
#define d_TTF_CJK_COMPATIBILITY_IDEOGRAPHS                        0xF900
#define d_TTF_ALPHABETIC_PRESENTATION_FORMS                       0xFB00
#define d_TTF_ARABIC_PRESENTATION_FORMS_A                         0xFB50
#define d_TTF_VARIATION_SELECTOR                                  0xFE00
#define d_TTF_VERTICAL_FORMS                                      0xFE10
#define d_TTF_COMBINING_HALF_MARKS                                0xFE20
#define d_TTF_CJK_COMPATIBILITY_FORMS                             0xFE30
#define d_TTF_SMALL_FORM_VARIANTS                                 0xFE50
#define d_TTF_ARABIC_PRESENTATION_FORMS_B                         0xFE70
#define d_TTF_HALFWIDTH_AND_FULLWIDTH_FORMS                       0xFF00
#define d_TTF_SPECIALS                                            0xFFF0

#define d_FONT_EMPTY                                              0       //No font
#define d_FONT_CHINESE_TAIWAN                                     1028    //Chinese(Taiwan)
#define d_FONT_CZECH                                              1029    //Czech
#define d_FONT_JAPANESE                                           1041    //Japanese
#define d_FONT_KOREAN                                             1042    //Korean
#define d_FONT_PORTUGUESE_BRAZIL                                  1046    //Portuguese(Brazil)
#define d_FONT_RUSSIAN                                            1049    //Russian
#define d_FONT_THAI                                               1054    //Thai
#define d_FONT_TURKISH                                            1055    //Turkish
#define d_FONT_FARSI                                              1065    //Farsi
#define d_FONT_VIETNAMESE                                         1066    //Vietnamese
#define d_FONT_CHINESE_PRC                                        2052    //Chinese(PRC)
#define d_FONT_CHINESE_PRC1                                       2054    //Simple(Song)
#define d_FONT_PORTUGUESE_PORTUGAL                                2070    //Portuguese(Portugal)
#define d_FONT_SPANISH                                            3082    //Spanish(Spain)
#define d_FONT_ARABIC_QATAR                                       16385   //Arabic(Qatar)
#define d_FONT_CHINESE_TAIWAN_PLUS                                64507   //Chinese(Taiwan) with Symbol
#define d_FONT_DEFAULTASCII                                       36864
//SwitchDisplayMode()
#define d_TTF_MODE_DEFAULT                                        0x0000
#define d_TTF_MODE_VIETNAM                                        0xA001
#define d_TTF_MODE_BURMA                                          0xA002
#define d_TTF_MODE_KHMER                                          0xA003
#define d_TTF_MODE_SPAIN                                          0xA004

// for font usage
#define d_FONT_LCD_USE                                            0x00
#define d_FONT_PRINTER_USE                                        0x01
#define d_FONT_FNT_MODE                                           0
#define d_FONT_TTF_MODE                                           1
#define d_FONT_DEFAULT_TTF                                        "ca_default.ttf"

// font style definition
#define d_FONT_STYLE_NORMAL                                                 0x0000
#define d_FONT_STYLE_REVERSE                                                0x0001
#define d_FONT_STYLE_UNDERLINE                                              0x0002
#define d_FONT_STYLE_BOLD                                                   0x0004
#define d_FONT_STYLE_ITALIC                                                 0x0010

// Font size
#define d_FONT_8x8                                                0x0808
#define d_FONT_8x16                                               0x0810
#define d_FONT_16x16                                              0x1010
#define d_FONT_12x24                                              0x0C18
#define d_FONT_24x24                                              0x1818
#define d_FONT_9x9                                                0x0909
#define d_FONT_9x18                                               0x0912
#define d_FONT_16x30                                              0x101E
#define d_FONT_20x40                                              0x1428

// for printer font type set
#define d_FONT_NO_SET_TYPE										  0
#define d_FONT_TYPE1											  1
#define d_FONT_TYPE2											  2
#define d_FONT_TYPE3											  3
//==================================================
#ifdef __cplusplus
}
#endif
#endif
