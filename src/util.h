/*******************************************************************************
 * Utility functions & macros
 ******************************************************************************/
#ifndef _UTIL_H_
#define _UTIL_H_

#define DEBUG 1

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#include <string.h>

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

#ifdef DEBUG
char* print_tmp;
#define dprint(...){\
    print_tmp = concatf(__VA_ARGS__);\
    strcat(print_tmp, "\n");\
    printf("%s", print_tmp);\
    fflush(stdout);\
    free(print_tmp);\
}
#else
#define dprint(x,...) ;
#endif

#endif /* _UTIL_H_ */
