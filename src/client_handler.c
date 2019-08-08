#include "client_handler.h"
#define BUFFER_SIZE 1024

void
start_client_handler(Socket *s, int connfd)
{
	char buffer[BUFFER_SIZE];
	socket_write(s, "220 bftp bem vindo");
	
	while (socket_read(s, buffer) > 0) printf("%s", buffer);
}
