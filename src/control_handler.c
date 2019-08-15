#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../hdr/error.h"
#include "util.h"
#include "control_handler.h"
#include "data_handler.h"
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

// Helper for simplified socket writing, freeing the buffer after usage
char *socket_tmp;
#define socket_writef(x, ...) \
socket_tmp = concatf(__VA_ARGS__);\
socket_write(x, socket_tmp);

// Helper for simplified command checking with authentication
#define checkcmd(str) (strncmpi(buffer, str, strlen(str)) == 0)
#define authcheckcmd(str) (checkcmd(str) && (denied = true) && logged) // yes, its an assignment

char *buffer,
	 *user,
	 *pass;

Socket *s;
Socket *data_s;

void
start_control_handler(Socket *s_arg, int *status)
{
	s = s_arg;
	buffer = emalloc(sizeof(char) * BUFFER_SIZE);
	user = emalloc(sizeof(char) * MAX_USER_LENGTH);
	pass = emalloc(sizeof(char) * MAX_PASS_LENGTH);
	*status = CONTROL;
	
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(char) * PATH_MAX);
	strcat(cwd, "/ftp/");
	
	socket_writef(s, "220 BFTP - Batista's FTP Server [%s]\n", socket_ip(s));
	
	bool logged = false;
	bool denied = false; // Set to true when user is denied access to a command
	while (socket_read(s, buffer) > 0) {
	    stripln(buffer, BUFFER_SIZE); //remove os \r e \n
		// TODO: USERxxx ou PASSxxx aceitos como comandos válidos
		// TODO: tirar \n das responses?
		
		/******************************* USER *********************************/
		// TODO: consertar quando usuario vazio
		// TODO: o que acontece se um usuário já logado tenta rodar USER?
		// TODO: anon login?: 331 Anonymous login ok, send your complete email address as your password
		if (checkcmd("USER")) {
			if (strlen(user)) cwd[strlen(cwd) - strlen(user)] = '\0';
			
			strncpy(user, buffer + 5, MAX_USER_LENGTH);
			pass[0] = '\0';
			socket_writef(s, "331 Password required for %s\n", user);
			
			strcat(cwd, user);
			mkdir(cwd, 0777);
		}
		/******************************* PASS *********************************/
		else if (checkcmd("PASS")) {
			if (!strlen(user))
				socket_write(s, "503 Login with USER first\n");
			else if (strncmp(buffer + 5, "ftp", MAX_PASS_LENGTH))
				socket_write(s, "530 Login incorrect.\n");
			else {
				strncpy(pass, buffer + 5, MAX_PASS_LENGTH);
				socket_writef(s, "230 User %s logged in\n", user);
			}
		}
		/******************************* QUIT *********************************/
		else if (checkcmd("QUIT")) {
			socket_write(s, "221 Goodbye.\n");
//			stop_data_handler();
			stop_control_handler();
		}
		/******************************** PWD *********************************/
        else if (authcheckcmd("PWD")) {
            socket_writef(s, "257 \"%s\" is the current directory\n", cwd);
        }
		/******************************* LIST *********************************/
        else if (authcheckcmd("LIST")) {
			if (data_s) {
				if (!fork()) {
					char *list = listdir(cwd);
					start_data_handler(data_s, status, list);
					// TODO: (child) transmitir transfer complete/error e quit
					break;
				}
			}
			//TODO: o que responder com LIST antes de PASV?
        }
		/******************************* MLSD *********************************/
		else if (authcheckcmd("MLSD")) {
			if (data_s) {
				if (!fork()) {
					char *list = listdir(cwd);
					start_data_handler(data_s, status, list);
					// TODO: (child) transmitir transfer complete/error e quit
					free(list);
					break;
				}
			}
			//TODO: o que responder com MLSD antes de PASV?
		}
		/******************************* TYPE *********************************/
		else if (checkcmd("TYPE")) {
			if (strcmp(buffer + 5, "I") == 0)
				socket_write(s, "200 Type set to I\n");
			else if (strcmp(buffer + 5, "A") == 0)
				socket_write(s, "200 Type set to A\n");
			// TODO: setar alguma variável do type?
			else
				socket_writef(s, "504 TYPE not implemented for %s parameter\n",
							  buffer + 5);
		}
		/******************************* PASV *********************************/
		else if (authcheckcmd("PASV")) {
            data_s = socket_open(0);
            socket_writef(s, "227 Entering Passive Mode (%s,%d,%d)\n",
						  pasv(socket_ip(s)),
						  socket_port(data_s) / 256,
						  socket_port(data_s) % 256);
		}
		/****************************** DEBUG *********************************/
		else if (checkcmd("DEBUG")) {
			socket_writef(s, "User: %s\n", user);
			socket_writef(s, "Pass: %s\n", pass);
			socket_writef(s, "Buffer: %s\n", buffer);
		}
		/******************************* ETC **********************************/
        else {
            if (denied) socket_write(s, "530 Please login with USER and PASS\n");
            else socket_writef(s, "500 %s not understood\n", buffer);
        }

		/* estado do login */
		if (strlen(user) && strlen(pass)) logged = true;
		else logged = false;
		denied = false;
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
stop_control_handler() {
	socket_fin(s);
	socket_close(s);
	free(buffer);
	free(user);
	free(pass);
	exit(0);
}
