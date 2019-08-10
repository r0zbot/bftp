#include "client_handler.h"
#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

int
strncmpi(const char* s1, const char* s2, size_t n)
{
	while (n--) {
		if (*s1 == tolower(*s2) || *s1 == toupper(*s2)) {
			s1++;
			s2++;
		}
		else return 1;
	}
	return 0;
}

void
start_client_handler(Socket *s)
{
	char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
	check_null(buffer, "client_handler: não foi possível alocar buffer");
	socket_write(s, "220 *** servidor bftp ***\n");
	
	char *user = malloc(sizeof(char) * MAX_USER_LENGTH);
	char *pass = malloc(sizeof(char) * MAX_PASS_LENGTH);
	bool logged = false;
	
	while (socket_read(s, buffer) > 0) {
		/* input do usuário */
		if 		(strncmpi(buffer, "USER", 4) == 0) {
			strncpy(user, buffer + 4, MAX_USER_LENGTH);
			pass[0] = '\0';
			socket_write(s, "331 usuário ok, envie senha\n");
		}
		else if (strncmpi(buffer, "PASS", 4) == 0) {
			if (!strlen(user)) socket_write(s, "envie usuário primeiro\n");
			else {
				strncpy(pass, buffer + 4, MAX_PASS_LENGTH);
				socket_write(s, "230 usuário logado\n");
			}
		}
		// TODO: todos outros comandos possíveis
		else if (strncmpi(buffer, "QUIT", 4) == 0) {
			socket_write(s, "221 até a próxima...\n");
			socket_fin(s);
			break;
		}
		
		/* estado do login */
		if (strlen(user) && strlen(pass)) logged = true;
		else logged = false;
		
	}
	free(user);
	free(pass);
	exit(0);
}
