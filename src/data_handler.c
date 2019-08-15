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

Socket *ds_copy;

void
start_data_handler(Socket *ds, int *status)
{
	ds_copy = ds;
	*status = DATA;
}

void
stop_data_handler() {
	socket_fin(ds_copy);
	socket_close(ds_copy);
	exit(0);
}
