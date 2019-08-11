/*******************************************************************************
 * Utility macros
 * Include this header in your file
 ******************************************************************************/
#ifndef _UTIL_H_
#define _UTIL_H_

#include "error.h"

#define check_null(ptr, msg) { if (!ptr) exit(msg) }

char* concatf(const char* fmt, ...);

int strncmpi(const char* s1, const char* s2, size_t n);

int stripln(char* str, size_t n);

#endif /* _UTIL_H_ */
