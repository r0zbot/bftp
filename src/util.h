/*******************************************************************************
 * Utility functions & macros
 ******************************************************************************/
#ifndef UTIL_H
#define UTIL_H

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

#define UNDEF  0
#define BINARY 1
#define ASCII  2

char * concatf(const char* fmt, ...);

int strncmpi(const char* s1, const char* s2, size_t n);

int stripln(char* str, size_t n);

void * emalloc(size_t size);

void * ecalloc(size_t size);

char * pasv(char *ip);

void _mkdir(const char *dir);

#endif /* UTIL_H */
