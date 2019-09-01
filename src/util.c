#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../hdr/error.h"
#include "util.h"

int
strncmpi(const char* s1, const char* s2, size_t n)
{
    while (n--) {
        if (*s1 == tolower(*s2) || *s1 == toupper(*s2)) {
            s1++;
            s2++;
        }
        else return 1;
    }
    return 0;
}

int
stripln(char* str, size_t n)
{
    for (unsigned long i = 0; i < n && str[i] != '\0'; ++i) {
        if(str[i] == '\n' || str[i] == '\r'){
            str[i] = '\0';
            return 1;
        }
    }
    return 0;
}

void *
emalloc(size_t size) {
    void *mem = malloc(size);
    check_null(mem, "emalloc: no more mem!");
    return mem;
}

void *
ecalloc(size_t size) {
    void *mem = calloc(1, size);
    check_null(mem, "ecalloc: no more mem!");
    return mem;
}

char *
pasv(char *ip)
{
    char *str = strdup(ip);
    char *begin = str;
    while (*str++ != '\0'){
        if (*str == '.') {
            *str = ',';
        }
    }
    return begin;
}

void
_mkdir(const char *dir) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}
