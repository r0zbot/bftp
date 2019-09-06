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
        int bytes_read = 0;
        while (bytes_read < st.st_size) {
            bytes_read += read(fileno(fp), buffer, BUFFER_SIZE);
            socket_write(ds, buffer, BUFFER_SIZE);
        }
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
        while (socket_read(ds, buffer, BUFFER_SIZE) > 0) {
            write(fileno(fp), buffer, BUFFER_SIZE);
        }
        fclose(fp);
        return 0;
    }
    else return 1;
}

void
data_handler_send(void *msg)
{
    socket_write(ds, msg);
}

void
data_handler_read(void *buffer)
{
    socket_read(ds, buffer);
}

void
stop_data_handler() {
    socket_finish(ds);
    exit(0);
}
