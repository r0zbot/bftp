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
		int connfd = socket_listen(s);
		if (!fork()) start_client_handler(s, connfd);
	}
}

