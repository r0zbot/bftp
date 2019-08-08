#include "client_handler.h"
#define NOME_DO_SERVIDOR bftp
#define BUFFER_SIZE 1024

void
start_client_handler(Socket *s, int connfd)
{
	char buffer[BUFFER_SIZE];
	socket_write(s, "220 %s", NOME_DO_SERVIDOR);
	
	while (socket_read(s, buffer) > 0) printf("%s", buffer);
}
