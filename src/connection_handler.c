#include "connection_handler.h"

Socket *s;
/**
 * start_connection_handler()
 */
void
start_connection_handler()
{
	s = socket_open(213);
	check_null(s, "connection_handler: não foi possível criar a socket\n");
	
	printf("bftp escutando na porta %d...\n", socket_port(s));
	
	while (true) {
		if (socket_listen(s) < 0) break;
		if (!fork()) start_client_handler(s);
	}
	puts("");
}

/**
 * stop_connection_handler()
 */
void
stop_connection_handler()
{
	// TODO: kill all children muahsuhauahuah
	// OBS: todas as children executam essa parte do código
	printf("pid %d\n", getpid());
	socket_close(s);
}
