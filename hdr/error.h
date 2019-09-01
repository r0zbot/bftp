/*******************************************************************************
 * Error macros
 * Include this header in your file
 ******************************************************************************/
#ifndef ERROR_H
#define ERROR_H

#define check_null(ptr, msg) { if (!ptr) fatal(msg) }
#define error(msg) fprintf(stderr, msg)
#define fatal(msg) { fprintf(stderr, msg); exit(1); }

#endif /* ERROR_H */
