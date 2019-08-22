#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "../ext/socket.h"
#include "../hdr/error.h"
#include "util.h"
#include "control_handler.h"
#include "connection_handler.h"

Socket *s;

/**
 * start_connection_handler()
 */
void
start_connection_handler(int *status, int port)
{
    *status = CONNECTION;
    s = socket_open(port);

    dprint("bftp escutando na porta %d...\n", socket_port(s));

    while (true) {
        if (socket_listen(s) < 0) break;
        if (!fork()) start_control_handler(s, status);
    }
}

/**
 * stop_connection_handler()
 */
void
stop_connection_handler()
{
    socket_close(s);
}
