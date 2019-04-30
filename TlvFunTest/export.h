
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

#ifndef _WIN32
#define PUBLIC_EXPORT __attribute__((visibility("default")))
#else
#define PUBLIC_EXPORT
#endif

#ifdef PRIVATE_EXPORT
#undef PRIVATE_EXPORT
#endif

#ifndef _WIN32
#define PRIVATE_EXPORT __attribute__((visibility("hidden")))
#else
#define PUBLIC_EXPORT
#endif

#endif
