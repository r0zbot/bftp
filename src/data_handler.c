#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"

// Helper for simplified socket writing, freeing the buffer after usage
char *socket_tmp;
#define socket_writef(x, ...) \
socket_tmp = concatf(__VA_ARGS__);\
socket_write(x, socket_tmp);

Socket *ds;

void
start_data_handler(Socket *ds_arg, int *status, void *msg)
{
    ds = ds_arg;
	*status = DATA;
    printf("data_handler: msg %s\n", (char *) msg);
	if (socket_listen(ds) < 0) return;
    printf("data_handler: accepting...");
	socket_write(ds, msg);
	socket_fin(ds);
	socket_close(ds);
}

void
stop_data_handler() {
//	if (ds_copy) {
//		socket_fin(ds_copy);
//		socket_close(ds_copy);
//	}
	exit(0);
}
