#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include "../hdr/error.h"
#include "util.h"
#include "client_handler.h"

#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

// Helper for simplified socket writing, freeing the buffer after usage
char *socket_tmp;
#define socket_writef(x, ...) \
socket_tmp = concatf(__VA_ARGS__);\
socket_write(x, socket_tmp);\
free(socket_tmp)

char *buffer,
	 *user,
	 *pass;

Socket *s_copy;

void
start_client_handler(Socket *s, int *status)
{
	s_copy = s;
	buffer = emalloc(sizeof(char) * BUFFER_SIZE);
	user = emalloc(sizeof(char) * MAX_USER_LENGTH);
	pass = emalloc(sizeof(char) * MAX_PASS_LENGTH);
	*status = CLIENT;
	
	socket_write(s, "220 BFTP - Batista's FTP Server [IP_ADDR]\n"); //TODO: Colocar endereço de IP
	
	bool logged = false;
	while (socket_read(s, buffer) > 0) {
	    stripln(buffer, BUFFER_SIZE); //remove os \r e \n
		
		/* input do usuário */
		if (strncmpi(buffer, "USER", 4) == 0) {
		    // TODO: consertar quando usuario vazio
			// TODO: USERxxx ou PASSxxx aceitos como comandos válidos
			// TODO: o que acontece se um usuário já logado tenta rodar USER?
			// TODO: tirar \n das responses?
			strncpy(user, buffer + 5, MAX_USER_LENGTH);
			pass[0] = '\0';
			socket_writef(s, "331 Password required for %s\n", user);
		}
		else if (strncmpi(buffer, "PASS", 4) == 0) {
			if (!strlen(user))
				socket_write(s, "503 Login with USER first\n");
			else if (strncmp(buffer + 5, "teste123", MAX_PASS_LENGTH))
				socket_write(s, "530 Login incorrect.\n");
			else {
				strncpy(pass, buffer + 5, MAX_PASS_LENGTH);
				socket_writef(s, "230 User %s logged in\n", user);
			}
		}
		// TODO: todos outros comandos possíveis
		else if (strncmpi(buffer, "QUIT", 4) == 0) {
			socket_write(s, "221 Goodbye.\n");
			socket_fin(s);
		}
        else if (strncmpi(buffer, "DEBUG", 5) == 0) {
            socket_writef(s, "User: %s\n", user);
            socket_writef(s, "Pass: %s\n", pass);
            socket_writef(s, "Buffer: %s\n", buffer);
        }
        else if (strncmpi(buffer, "PWD", 3) == 0) {
			if (logged) {
				char cwd[PATH_MAX];
				getcwd(cwd, sizeof(char) * PATH_MAX);
				socket_writef(s, "257 \"%s\" is the current directory\n", cwd);
			}
			else {
				; //TODO: o que responder qd nao houver credenciais validas?
			}
        }
        else if (strncmpi(buffer, "LIST", 4) == 0) {
			if (logged) {
				char cwd[PATH_MAX];
				getcwd(cwd, sizeof(cwd));
				char *list = listdir(cwd);
				socket_write(s, list);
				free(list);
			}
			else {
				; //TODO: o que responder qd nao houver credenciais validas?
			}
        }
        else {
            socket_writef(s, "500 %s not understood\n", buffer);
        }

		/* estado do login */
		if (strlen(user) && strlen(pass)) logged = true;
		else logged = false;

	}
	/* TODO - Missing commands:
	    USER r0zbot
	        331 Password required for r0zbot
	    PASS senha
	        230 User r0zbot logged in
	    SYST
	        215 UNIX Type: L8
	    FEAT
	        211-Features:
            211-CLNT
            211-EPRT
            211-EPSV
            211-HOST
            211-LANG en-US.UTF-8*;en-US
            211-MDTM
            211-MFF modify;UNIX.group;UNIX.mode;
            211-MFMT
            211-MLST modify*;perm*;size*;type*;unique*;UNIX.group*;UNIX.groupname*;UNIX.mode*;UNIX.owner*;UNIX.ownername*;
            211-REST STREAM
            211-SITE COPY
            211-SITE MKDIR
            211-SITE RMDIR
            211-SITE SYMLINK
            211-SITE UTIME
            211-SIZE
            211-TVFS
            211-UTF8
            211 End
	    CLNT Cliente_de_FTP 1.0.0
	        200 OK
	    OPTS UTF8 ON
	        200 UTF8 set to on
	    PWD
	        257 "/home/r0zbot" is the current directory
	    TYPE A
	        200 Type set to A
	    PASV
	        227 Entering Passive Mode (192,168,195,129,131,35).
	    MLSD
	        150 Opening ASCII mode data connection for MLSD
	        226 Transfer complete
	    TYPE A
            200 Type set to A
	    REST 0
	        350 Restarting at 0. Send STORE or RETRIEVE to initiate transfer
	        421 No transfer timeout (600 seconds): closing control connection
	 */
}

void
stop_client_handler() {
	socket_fin(s_copy);
	socket_close(s_copy);
	free(buffer);
	free(user);
	free(pass);
}
