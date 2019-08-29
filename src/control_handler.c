#include "control_handler.h"

#define MAX_PASS_LENGTH 32
#define MAX_USER_LENGTH 32

// Helper for simplified command checking with authentication
#define CMD(str) (strncmpi(buffer, str, strlen(str)) == 0)
#define AUTH_CMD(str) (CMD(str) && (denied = true) && logged)

char *buffer;
Socket *s = NULL;
Socket *data_s = NULL;

/**
 * start_control_handler():
 *     Conversa com o cliente na conexão fornecida pelo processo principal.
 *
 * @s_arg: o socket no qual vamos conversar com o cliente
 * @status: indica em que fase o processo está
 */
void
start_control_handler(Socket *s_arg, int *status)
{
    s = s_arg;
    buffer = emalloc(sizeof(char) * BUFFER_SIZE);
    *status = CONTROL;
    char user[MAX_USER_LENGTH+1];
    char pass[MAX_PASS_LENGTH+1];
    char cwd[PATH_MAX];
    int type = UNDEF;
    socket_printf(s, "220 BFTP - Batista's FTP Server [%s]\r\n", socket_server_ip(s));

    bool logged = false;
    bool denied = false; // Set to true when user is denied access to a command
    while (socket_read(s, buffer) > 0) {
        stripln(buffer, BUFFER_SIZE); //remove os \r e \r\n
        char *cmd, *arg;
        cmd = strtok_r(buffer, " ", &arg);
        getcwd(cwd, sizeof(char) * PATH_MAX);
        /******************************* USER *********************************/
        if (CMD("USER")) {
            if (!arg) socket_printf(s, "500 %s: command requires a parameter\r\n", cmd);
            else {
                strncpy(user, arg, MAX_USER_LENGTH);
                pass[0] = '\0';
                socket_printf(s, "331 Password required for %s\r\n", user);
            }
        }
        /******************************* PASS *********************************/
        else if (CMD("PASS")) {
            if (!strlen(user)) socket_printf(s, "503 Login with USER first\r\n");
            else {
                strncpy(pass, arg, MAX_PASS_LENGTH);
                socket_printf(s, "230 User %s logged in\r\n", user);
            }
        }
        /******************************* QUIT *********************************/
        else if (CMD("QUIT")) {
            socket_printf(s, "221 Goodbye.\r\n");
            stop_control_handler();
        }
        /******************************** PWD *********************************/
        else if (AUTH_CMD("PWD")) {
            socket_printf(s, "257 \"%s\" is the current directory\r\n", cwd);
        }
        /******************************** CWD *********************************/
        else if (AUTH_CMD("CWD")) {
            if (!chdir(arg)) socket_printf(s, "250 CWD command successful\r\n");
            else socket_printf(s, "550 %s: No such file or directory\r\n", arg);
        }
        /******************************* CDUP *********************************/
        else if (AUTH_CMD("CDUP")) {
            if (!chdir("..")) socket_printf(s, "250 CDUP command successful\r\n");
            else socket_printf(s, "550 %s: No such file or directory\r\n", arg);
        }
        /******************************* LIST *********************************/
        else if (AUTH_CMD("LIST")) {
            if (data_s) {
                if (type == BINARY)
                    socket_printf(s, "150 Opening BINARY mode data connection for file list\r\n");
                else
                    socket_printf(s, "150 Opening ASCII mode data connection for file list\r\n");
                
                if (!fork()) {
                    FILE *fp;
                    char command[PATH_MAX];
                    char entry[PATH_MAX];
                    bzero(&command, PATH_MAX);
                    bzero(&entry, PATH_MAX);
                    strcat(command, "/bin/ls -la \"");
                    strcat(command, cwd);
                    strcat(command, "\"");
                    fp = popen(command, "r");
                    read(fileno(fp), buffer, BUFFER_SIZE);
                    pclose(fp);
                    start_data_handler(data_s, status);
                    data_handler_send((void *) buffer);
                    socket_printf(s, "226 Transfer complete\r\n");
                    stop_data_handler();
                }
            }
        }
        /******************************* RETR *********************************/
        else if (AUTH_CMD("RETR")) {
            if (data_s) {
                if (type == BINARY)
                    socket_printf(s, "150 Opening BINARY mode data connection for %s\r\n", arg);
                else
                    socket_printf(s, "150 Opening ASCII mode data connection for %s\r\n", arg);
                
                if (!fork()) {
                    start_data_handler(data_s, status);
                    
                    if (!data_handler_send_file(arg, (void *) buffer, type))
                        socket_printf(s, "226 Transfer complete\r\n");
                    else
                        socket_printf(s, "550 %s: No such file or directory\r\n", arg);
                    
                    stop_data_handler();
                }
            }
        }
        /******************************* STOR *********************************/
        else if (AUTH_CMD("STOR")) {
            if (type == BINARY)
                socket_printf(s, "150 Opening BINARY mode data connection for %s\r\n", arg);
            else
                socket_printf(s, "150 Opening ASCII mode data connection for %s\r\n", arg);
            
            if (!fork()) {
                start_data_handler(data_s, status);
                
                if (!data_handler_receive_file(arg, (void *) buffer, type))
                    socket_printf(s, "226 Transfer complete\r\n");
                else
                    socket_printf(s, "550 %s: No such file or directory\r\n", arg);
                
                stop_data_handler();
            }
        }
        /******************************* RMD *********************************/
        else if (AUTH_CMD("RMD")) {
            if (!rmdir(arg)) socket_printf(s,"250 RMD command successfull\r\n");
            else socket_printf(s, "550 %s: Could not remove\r\n", arg);
            // TODO: more informative error messages
        }
        /******************************* DELE *********************************/
        else if (AUTH_CMD("DELE")) {
            if (!remove(arg)) socket_printf(s,"250 DELE command successfull\r\n");
            else socket_printf(s, "550 %s: Could not remove\r\n", arg);
            // TODO: more informative error messages
        }
        /******************************* MKD *********************************/
        else if (AUTH_CMD("MKD")) {
            if (!mkdir(arg,S_IRWXU)) socket_printf(s,"257 \"%s\" - Directory successfully created\r\n", arg);
            else socket_printf(s, "550 %s: Could not create\r\n", arg);
            // TODO: more informative error messages
        }
        /******************************* TYPE *********************************/
        else if (CMD("TYPE")) {
            if (strcmp(arg, "I") == 0) {
                type = BINARY;
                socket_printf(s, "200 Type set to I\r\n");
            }
            else if (strcmp(arg, "A") == 0) {
                type = ASCII;
                socket_printf(s, "200 Type set to A\r\n");
            }
            else
                socket_printf(s, "504 TYPE not implemented for %s parameter\r\n", arg);

        }
        /******************************* PASV *********************************/
        else if (AUTH_CMD("PASV")) {
            if (!data_s) data_s = socket_open(0);
            socket_printf(s, "227 Entering Passive Mode (%s,%d,%d).\r\n",
                          pasv(socket_server_ip(s)),
                          socket_port(data_s) / 256,
                          socket_port(data_s) % 256);
        }
        /******************************* PORT *********************************/
        else if (AUTH_CMD("PORT")) {
            socket_printf(s, "501 Server cannot accept argument.\r\n");
        }
        /******************************* SYST *********************************/
        else if (AUTH_CMD("SYST")) {
            socket_printf(s, "215 UNIX Type: L8.\r\n");
        }
        /******************************* FEAT *********************************/
        else if (AUTH_CMD("FEAT")) {
            socket_printf(s, "211-Features:\r\n");
            socket_printf(s, "211 End\r\n");
        }
        /******************************* SIZE *********************************/
        else if (AUTH_CMD("SIZE")) {
            struct stat st;

            if (stat(arg, &st) == 0) socket_printf(s, "213 %d\r\n", st.st_size);
            else socket_printf(s, "550 %s: Could not get size of %s\r\n", arg);
        }
        /****************************** DEBUG *********************************/
        else if (CMD("DEBUG")) {
            socket_printf(s, "User: %s\r\n", user);
            socket_printf(s, "Pass: %s\r\n", pass);
            socket_printf(s, "Buffer: %s\r\n", arg);
            socket_printf(s, "PID: %lu\r\n", getpid());
            socket_printf(s, "IP Cliente: %s\r\n", socket_ip_client(s));
            socket_printf(s, "IP Server: %s\r\n", socket_ip_server(s));
            socket_printf(s, "Port: %d\r\n", socket_port(s));
            if (data_s) socket_printf(s, "Data port: %d\r\n", socket_port(data_s));
        }
        /******************************* ETC **********************************/
        else {
            if (denied)
                socket_printf(s, "530 Please login with USER and PASS\r\n");
            else
                socket_printf(s, "500 %s not understood\r\n", arg);
        }

        /* estado do login */
        if (strlen(user) && strlen(pass)) logged = true;
        else logged = false;
        denied = false;

    }
}

/**
 * stop_control_handler():
 *     Fecha o socket e as conexões remanescentes, liberando a memória alocada.
 */
void
stop_control_handler() {
    socket_finish(s);
    //socket_close(s); TODO: close just connfd socket on QUIT ?
    free(buffer);
    exit(0);
}
