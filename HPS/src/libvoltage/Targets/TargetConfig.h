/* Copyright 2015, Voltage Security, all rights reserved.
 */
#if 0
	#ifndef TARGET_CONFIG_H
	#define TARGET_CONFIG_H


	#undef INCLUDE_POS232
	#undef SUPPORT_STRINGS
	#undef SUPPORT_TEP4_ALPHANUMERIC


	#endif //TARGET_CONFIG_H
#endif


//max modify
#define d_420_SUPPORT						0
#define d_420_232_SUPPORT					0
#define d_420_232_STRING_SUPPORT			0
#define d_420_232_STRING_TEP4_SUPPORT		1
#define d_420_232_TEP4_SUPPORT				0	
#define d_420_STRING_SUPPORT				0
#define d_420_STRING_TEP4_SUPPORT			0
#define d_420_TEP4_SUPPORT					0

#if	(d_420_SUPPORT == 1)				
	#include "./420/TargetConfig.h"		
#elif (d_420_232_SUPPORT == 1)	
	#include "./420-232/TargetConfig.h"		
#elif (d_420_232_STRING_SUPPORT == 1)				
	#include "./420-232-String/TargetConfig.h"		
#elif (d_420_232_STRING_TEP4_SUPPORT == 1)			
	#include "./420-232-String-TEP4/TargetConfig.h"		
#elif (d_420_232_TEP4_SUPPORT == 1)				
	#include "./420-232-TEP4/TargetConfig.h"		
#elif (d_420_STRING_SUPPORT == 1)				
	#include "./420-String/TargetConfig.h"		
#elif (d_420_STRING_TEP4_SUPPORT == 1)		
	#include "./420-String-TEP4/TargetConfig.h"		
#elif (d_420_TEP4_SUPPORT == 1)			
	#include "./420-TEP4/TargetConfig.h"		
#else	
	#ifndef TARGET_CONFIG_H
	#define TARGET_CONFIG_H	
		#undef INCLUDE_POS232
		#undef SUPPORT_STRINGS
		#undef SUPPORT_TEP4_ALPHANUMERIC
	#endif //TARGET_CONFIG_H
#endif



