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

// Code from https://stackoverflow.com/questions/24442459/returning-formatted-string-from-c-function
char *
concatf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char* buf = NULL;
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (n >= 0) {
        va_start(args, fmt);
        buf = malloc(n+1);
        if (buf) vsnprintf(buf, n+1, fmt, args);
        va_end(args);
    }
    return buf;
}

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

char *
listdir(const char *path)
{
    DIR *dir;
    struct dirent *ent;
    unsigned long entries = 0;
    char *out;
    if ((dir = opendir (path)) != NULL) {
        /* count files in directory for string size allocation */
        while ((ent = readdir (dir)) != NULL) {
            ++entries;
        }
        out = malloc(sizeof(char) * entries * FILENAME_MAX);
        strcpy(out, "XXX Directory contents");
        /* print all the files and directories within directory */
        dir = opendir (path);
        while ((ent = readdir (dir)) != NULL) {
            strcat(out, "\nXXX ");
            strcat(out, ent->d_name);
        }
        strcat(out, "\n");
        return (out);
        closedir (dir);
    }
    else {
        /* could not open directory */
        return "XXX Could not open directory\n";
    }
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
