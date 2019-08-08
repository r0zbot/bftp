#include "client_handler.h"
#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

void
start_client_handler(Socket *s)
{
	char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
	check_null(buffer, "client_handler: não foi possível alocar buffer");
	socket_write(s, "220 *** servidor bftp ***\n");
	
	char *user = malloc(sizeof(char) * MAX_USER_LENGTH);
	char *pass = malloc(sizeof(char) * MAX_PASS_LENGTH);
	
	// TODO: aceitar maiúsculas e minúsculas?
	while (socket_read(s, buffer) > 0) {
		if 		(strncmp(buffer, "USER", 4) == 0) {
			strncpy(user, buffer + 4, MAX_USER_LENGTH);
			socket_write(s, "331 usuário ok, envie senha\n");
		}
		else if (strncmp(buffer, "PASS", 4) == 0) {
			if (!strlen(user)) socket_write(s, "faça login\n");
			else {
				strncpy(pass, buffer + 4, MAX_PASS_LENGTH);
				socket_write(s, "230 usuário logado\n");
			}
		}
		else if (strncmp(buffer, "QUIT", 4) == 0) {
			socket_write(s, "221 até a próxima...\n");
			socket_fin(s);
			break;
		}
	}
	exit(0);
}
