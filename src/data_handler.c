#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"


static Socket *ds;

/**
 * Bloqueia e espera uma conexão TCP ser iniciada.
 */
void
start_data_handler(Socket *ds_arg, int *status)
{
    ds = ds_arg;
    socket_accept(ds);
    *status = DATA;
}

/**
 * Lê um arquivo e envia em chunks de BUFFER_SIZE para o cliente.
 */
int
data_handler_send_file(char *file, void *buffer, int type)
{
    FILE *fp;
    if (type == BINARY) fp = fopen(file, "rb");
    else fp = fopen(file, "r");

    if (fp) {
        struct stat st;
        stat(file, &st);
        ssize_t bytes_read = 0;
        do {
            bytes_read = read(fileno(fp), buffer, BUFFER_SIZE);
            if (bytes_read > 0) socket_write(ds, buffer, bytes_read);
        } while (bytes_read > 0);
        
        fclose(fp);
        return 0;
    }
    else return 1;
}

/**
 * Recebe chunks de tamanho BUFFER_SIZE e escreve em um arquivo.
 */
int
data_handler_receive_file(char *file, void *buffer, int type)
{
    FILE *fp;
    if (type == BINARY) fp = fopen(file, "wb");
    else fp = fopen(file, "w");
    if (fp) {
        ssize_t bytes_rcvd = 0;
        do {
            bytes_rcvd = socket_read(ds, buffer, BUFFER_SIZE);
            if (bytes_rcvd > 0) write(fileno(fp), buffer, bytes_rcvd);
        } while (bytes_rcvd > 0);
        fclose(fp);
        return 0;
    }
    else return 1;
}

void
stop_data_handler(char *buffer, Socket *s, Socket *ds) {
    free(buffer);
    free(s);
    socket_finish(ds);
    free(ds);
    exit(0);
}
