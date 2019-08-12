#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>

// Code from https://stackoverflow.com/questions/24442459/returning-formatted-string-from-c-function
char* concatf(const char* fmt, ...) {
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
