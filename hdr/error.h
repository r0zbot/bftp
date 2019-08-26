/*******************************************************************************
 * Error macros
 * Include this header in your file
 ******************************************************************************/
#ifndef _ERROR_H_
#define _ERROR_H_

#define check_null(ptr, msg) { if (!ptr) fatal(msg) }
#define error(msg) fprintf(stderr, msg)
#define fatal(msg) { fprintf(stderr, msg); exit(1); }

#endif /* _ERROR_H_ */
