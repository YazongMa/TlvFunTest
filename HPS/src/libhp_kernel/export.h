
/*
 * File Name: export.h
 * Author: Alan.Ren
 * 
 * Create Date: 2019.3.29
 */

#ifndef _H_H_EXPORTS_H_H_
#define _H_H_EXPORTS_H_H_

#ifdef PUBLIC_EXPORT
#undef PUBLIC_EXPORT
#endif

#define PUBLIC_EXPORT __attribute__((visibility("default")))

#ifdef PRIVATE_EXPORT
#undef PRIVATE_EXPORT
#endif

#define PRIVATE_EXPORT __attribute__((visibility("hidden")))

#endif
