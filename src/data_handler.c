#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"


Socket *ds_copy;

void
start_data_handler(Socket *ds, int *status, void *msg)
{
    printf("a");
	ds_copy = ds;
	*status = DATA;
	printf("a");
	if (socket_listen(ds) < 0) return;
    printf("b");
	socket_write(ds, msg);
    printf("c");
	socket_fin(ds_copy);
	socket_close(ds_copy);
}

void
stop_data_handler() {
//	if (ds_copy) {
//		socket_fin(ds_copy);
//		socket_close(ds_copy);
//	}
	exit(0);
}
