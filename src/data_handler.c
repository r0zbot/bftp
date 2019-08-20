#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"


Socket *ds;

void
start_data_handler(Socket *ds_arg, int *status)
{
    ds = ds_arg;
	*status = DATA;
    socket_listen(ds);
}

void
data_handler_send(void *msg)
{
    socket_write(ds, msg);
}

void
stop_data_handler() {
    if (ds) {
        socket_fin(ds);
        socket_close(ds);
    }
	exit(0);
}
