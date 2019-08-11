/*******************************************************************************
 * Utility macros
 * Include this header in your file
 ******************************************************************************/
#ifndef _UTIL_H_
#define _UTIL_H_

#include "error.h"

#define check_null(ptr, msg) { if (!ptr) exit(msg) }

char* concatf(const char* fmt, ...);

#endif /* _UTIL_H_ */
