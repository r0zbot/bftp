#include "connection_handler.h"

Socket *s;
bool is_client;
bool sigint_not_sent;

/**
 * start_connection_handler()
 */
void
start_connection_handler()
{
	sigint_not_sent = true;
	s = socket_open(213);
	check_null(s, "connection_handler: não foi possível criar a socket\n");
	
	printf("bftp escutando na porta %d...\n", socket_port(s));
	
	while (sigint_not_sent) {
		socket_listen(s);
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
