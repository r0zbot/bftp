#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

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