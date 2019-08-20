/*******************************************************************************
 * Utility functions & macros
 ******************************************************************************/
#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __linux__
    #include <linux/limits.h>
#else
    #include <limits.h>
#endif

#define CONNECTION	1
#define CONTROL		2
#define DATA		3

char * concatf(const char* fmt, ...);

int strncmpi(const char* s1, const char* s2, size_t n);

int stripln(char* str, size_t n);

char * listdir(const char *path);

void * emalloc(size_t size);

void * ecalloc(size_t size);

char * pasv(char *ip);

void _mkdir(const char *dir);



#endif /* _UTIL_H_ */
