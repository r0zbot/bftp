#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../ext/socket.h"
#include "../hdr/error.h"
#include "../hdr/util.h"
#include "connection_handler.h"

/**
 * start_connection_handler()
 */
int
start_connection_handler()
{
	Socket *s = socket_open(21);
	check_null(listen, "connection_handler: não foi possível criar a socket");
	printf("bftp escutando na porta %d...\n", socket_port(s));
	
	while (true) {
		socket_listen(s);
	}
}

