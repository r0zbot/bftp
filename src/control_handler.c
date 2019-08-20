#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include "../hdr/error.h"
#include "util.h"
#include "control_handler.h"
#include "data_handler.h"


#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

// TODO: token com strtok para parse do cmd e arg
// Helper for simplified command checking with authentication
#define checkcmd(str) (strncmpi(buffer, str, strlen(str)) == 0 && \
                      (cmd_arg = buffer + (strlen(str) + 1)))
#define authcheckcmd(str) (checkcmd(str) && (denied = true) && logged)

char *buffer,
	 *user,
	 *pass,
	 *cmd_arg;

Socket *s = NULL;
Socket *data_s = NULL;

void
start_control_handler(Socket *s_arg, int *status)
{
	s = s_arg;
	buffer = emalloc(sizeof(char) * BUFFER_SIZE);
	user = emalloc(sizeof(char) * MAX_USER_LENGTH);
	pass = emalloc(sizeof(char) * MAX_PASS_LENGTH);
	*status = CONTROL;
	
	char cwd_init[PATH_MAX];
	getcwd(cwd_init, sizeof(char) * PATH_MAX);
	strcat(cwd_init, "/ftp/");
    char cwd[PATH_MAX];
    strncpy(cwd, cwd_init, PATH_MAX);

	socket_writef(s, "220 BFTP - Batista's FTP Server [%s]\r\n", socket_ip_client(s));
	
	bool logged = false;
	bool denied = false; // Set to true when user is denied access to a command
	while (socket_read(s, buffer) > 0) {
        getcwd(cwd, sizeof(char) * PATH_MAX);
	    stripln(buffer, BUFFER_SIZE); //remove os \r e \r\n
		// TODO: USERxxx ou PASSxxx aceitos como comandos válidos
		// TODO: tirar \r\n das responses? RESPOSTA: \r\n é obrigatorio em todas as respostas
		
		/******************************* USER *********************************/
		// TODO: consertar quando usuario vazio
		// TODO: o que acontece se um usuário já logado tenta rodar USER?
		// TODO: anon login?: 331 Anonymous login ok, send your complete email address as your password
		if (checkcmd("USER")) {
			strncpy(user, cmd_arg, MAX_USER_LENGTH);
			pass[0] = '\0';
			socket_writef(s, "331 Password required for %s\r\n", user);
		}
		/******************************* PASS *********************************/
		else if (checkcmd("PASS")) {
			if (!strlen(user)) socket_write(s, "503 Login with USER first\r\n");
			else {
				strncpy(pass, cmd_arg, MAX_PASS_LENGTH);
				socket_writef(s, "230 User %s logged in\r\n", user);
                
                strncpy(cwd, cwd_init, PATH_MAX);
                strcat(cwd, user);
                _mkdir(cwd);
                chdir(cwd);
			}
		}
		/******************************* QUIT *********************************/
		else if (checkcmd("QUIT")) {
			socket_write(s, "221 Goodbye.\r\n");
//			stop_data_handler();
			stop_control_handler();
		}
		/******************************** PWD *********************************/
        else if (authcheckcmd("PWD")) {
            socket_writef(s, "257 \"%s\" is the current directory\r\n", cwd);
        }
        /******************************** CWD *********************************/
        else if (authcheckcmd("CWD")) {
            if (!chdir(cmd_arg)) socket_write(s, "250 CWD command successful\r\n");
            else socket_writef(s, "550 %s: No such file or directory\r\n", cmd_arg);
        }
        /******************************* CDUP *********************************/
        else if (authcheckcmd("CDUP")) {
            if (!chdir("..")) socket_write(s, "250 CDUP command successful\r\n");
            else socket_writef(s, "550 %s: No such file or directory\r\n", cmd_arg);
        }
		/******************************* LIST *********************************/
        else if (authcheckcmd("LIST")) {
			if (data_s) {
				socket_write(s, "150 Opening BINARY mode data connection for for file list\r\n");
				// TODO: deixar bunitin e prever casos de erro
                if (!fork()) {
                    FILE *fp;
                    char command[PATH_MAX];
                    char entry[PATH_MAX];
                    char list[PATH_MAX];
                    bzero(&command, PATH_MAX);
                    bzero(&entry, PATH_MAX);
                    bzero(&list, PATH_MAX);
                    strcat(command, "/bin/ls -la \"");
                    strcat(command, cwd);
                    strcat(command, "\"");
                    fp = popen(command, "r");
                    if (fp == NULL) exit("Failed to run command\n");

                    while (fgets(entry, sizeof(entry) - 1, fp) != NULL)
                        strcat(list, entry);
                    
                    pclose(fp);
                    start_data_handler(data_s, status, list);
                    socket_write(s, "226 Transfer complete\r\n");
                    stop_data_handler();
                }
			}
			//TODO: o que responder com LIST antes de PASV?
        }
		/******************************* MLSD *********************************/
		else if (authcheckcmd("MLSD")) {
		    //Desnecessario, é só não falar que tem essa feature
			if (data_s) {
				if (!fork()) {
                    char cwd[PATH_MAX];
                    getcwd(cwd, sizeof(char) * PATH_MAX);
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
			if (strcmp(cmd_arg, "I") == 0){
                socket_write(s, "200 Type set to I\r\n");
			}
			else if (strcmp(cmd_arg, "A") == 0){
                socket_write(s, "200 Type set to A\r\n");
			}
			// TODO: setar alguma variável do type?
			else{
                socket_writef(s, "504 TYPE not implemented for %s parameter\r\n", cmd_arg);
			}

		}
		/******************************* PASV *********************************/
		else if (authcheckcmd("PASV")) {
            if (!data_s) data_s = socket_open(0);
            socket_writef(s, "227 Entering Passive Mode (%s,%d,%d).\r\n",
						  pasv(socket_ip_server(s)),
						  socket_port(data_s) / 256,
						  socket_port(data_s) % 256);
		}
		/******************************* PORT *********************************/
		else if (authcheckcmd("PORT")) {
			socket_write(s, "501 Server cannot accept argument.\r\n");
		}
        /******************************* SYST *********************************/
		else if (authcheckcmd("SYST")) {
			socket_write(s, "215 UNIX Type: L8.\r\n");
		}
        /******************************* FEAT *********************************/
		else if (authcheckcmd("FEAT")) {
			socket_write(s, "211-Features:\r\n");
            socket_write(s, "211 End\r\n");
		}
        /******************************* OPTS *********************************/
		else if (authcheckcmd("OPTS UTF8 ON")) {
			socket_write(s, "200 UTF8 set to on\r\n");
		}
		/****************************** DEBUG *********************************/
		else if (checkcmd("DEBUG")) {
			socket_writef(s, "User: %s\r\n", user);
			socket_writef(s, "Pass: %s\r\n", pass);
			socket_writef(s, "Buffer: %s\r\n", cmd_arg);
			socket_writef(s, "PID: %lu\r\n", getpid());
            socket_writef(s, "IP Cliente: %s\r\n", socket_ip_client(s));
            socket_writef(s, "IP Server: %s\r\n", socket_ip_server(s));
            socket_writef(s, "Port: %d\r\n", socket_port(s));
            if (data_s) {
                socket_writef(s, "Port data: %d\r\n", socket_port(data_s));
            }
        }
		/******************************* ETC **********************************/
        else {
            if (denied) {
                socket_write(s, "530 Please login with USER and PASS\r\n");
            }
            else {
                socket_writef(s, "500 %s not understood\r\n", cmd_arg);
            }
        }

		/* estado do login */
		if (strlen(user) && strlen(pass)) logged = true;
		else logged = false;
		denied = false;
		
	}
    stop_control_handler();
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
