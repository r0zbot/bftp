#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include "client_handler.h"
#define BUFFER_SIZE 1024
#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

#define checkcmd(str) (strncmpi(buffer, str, strlen(str)) == 0)
#define authcheckcmd(str) (checkcmd(str) && (denied = true) && logged) //yes, its an assignment

char*
listdir(const char* path){
    DIR *dir;
    struct dirent *ent;
    unsigned long entries = 0;
    char* out;
    if ((dir = opendir (path)) != NULL) {
        /*count files in directory for string size allocation*/
        while ((ent = readdir (dir)) != NULL) {
            ++entries;
        }
        out = malloc(sizeof(char) * entries * FILENAME_MAX);
        strcpy(out, "XXX Directory contents");
        /* print all the files and directories within directory */
        entries = 0;
        dir = opendir (path);
        while ((ent = readdir (dir)) != NULL) {
            strcat(out, "\nXXX ");
            strcat(out, ent->d_name);
        }
        strcat(out, "\n");
        return (out);
        closedir (dir);
    } else {
        /* could not open directory */
        return "XXX Could not open directory";
    }
}


void
start_client_handler(Socket *s)
{
	char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
	check_null(buffer, "client_handler: não foi possível alocar buffer");
	socket_write(s, "220 BFTP - Batista's FTP Server [IP_ADDR]\n"); //TODO: Colocar endereço de IP

	char *user = malloc(sizeof(char) * MAX_USER_LENGTH);
	char *pass = malloc(sizeof(char) * MAX_PASS_LENGTH);
	bool logged = false;
    bool denied = false;

	while (socket_read(s, buffer) > 0) {
	    stripln(buffer, BUFFER_SIZE); //remove os \r e \n
		/* input do usuário */
		if (checkcmd("USER")) {
		    // TODO: consertar quando usuario vazio
			strncpy(user, buffer + 5, MAX_USER_LENGTH);
			pass[0] = '\0';
			fsocket_write(s, "331 Password required for %s\n", user);
		}
		else if (checkcmd("PASS")) {
			if (!strlen(user)) socket_write(s, "503 Login with USER first\n");
			else if (strncmp(buffer+5, "teste123", MAX_PASS_LENGTH) != 0) socket_write(s, "530 Login incorrect.\n");
			else {
				strncpy(pass, buffer + 5, MAX_PASS_LENGTH);
				fsocket_write(s, "230 User %s logged in\n", user); // TODO: descobrir pq da pau aqui
			}
		}
		// TODO: todos outros comandos possíveis
		else if (checkcmd("QUIT")) {
			socket_write(s, "221 Goodbye.\n");
			socket_fin(s);
		}
        else if (checkcmd("DEBUG")) {
            fsocket_write(s, "User: %s\n", user);
            fsocket_write(s, "Pass: %s\n", pass);
            fsocket_write(s, "Buffer: %s\n", buffer);
        }
        else if (authcheckcmd("PWD")) {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            fsocket_write(s, "257 \"%s\" is the current directory\n", cwd);
        }
        else if (authcheckcmd("LIST")) {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            char* list = listdir(cwd);
            socket_write(s, list);
            free(list);
        }
        else{
            if(denied){
                socket_write(s, "530 Please login with USER and PASS\n");
                denied = false;
            }
            else{
                fsocket_write(s, "500 %s not understood\n", buffer);
            }
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
	free(user);
	free(pass);
	exit(0);
}