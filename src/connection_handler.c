#include "connection_handler.h"

bool sigint_sent = false;
Socket *s;
/**
 * start_connection_handler()
 */
void
start_connection_handler()
{
	s = socket_open(2124);
	check_null(listen, "connection_handler: não foi possível criar a socket");
	printf("bftp escutando na porta %d...\n", socket_port(s));
	
	while (!sigint_sent) {
		socket_listen(s);
		if (!fork()) start_client_handler(s);
	}
}

/**
 * stop_connection_handler()
 */
void
stop_connection_handler()
{
	sigint_sent = true;
	//TODO: kill todas as childs
	socket_close(s);
}

