#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"

Socket *ds;

/**
 * start_data_handler():
 *     Bloqueia e esperar uma conexão TCP ser iniciada.
 *
 * @ds_arg: o socket no qual vamos trocar dados com o cliente
 * @status: indica em que fase o processo está
 */
void
start_data_handler(Socket *ds_arg, int *status)
{
    ds = ds_arg;
    socket_accept(ds);
    *status = DATA;
}

/**
 * data_handler_send_file():
 *     Lê um arquivo e envia em chunks de BUFFER_SIZE para o cliente.
 *
 * @file: arquivo que desejamos ler
 * @buffer: buffer no qual vamos escrever
 * @type: BINARY, ASCII ou UNDEF
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
 * data_handler_receive_file():
 *     Recebe chunks de BUFFER_SIZE e escreve em um arquivo.
 *
 * @file: arquivo que desejamos escrever
 * @buffer: buffer que será lido
 * @type: BINARY, ASCII ou UNDEF
 */
int
data_handler_receive_file(char *file, void *buffer, int type)
{
    FILE *fp;
    if (type == BINARY) fp = fopen(file, "wb");
    else fp = fopen(file, "w");
    
    if (fp) {
        while (socket_read(ds, buffer, BUFFER_SIZE) > 0) {
            UNUSED(write(fileno(fp), buffer, BUFFER_SIZE));
        }
        fclose(fp);
        return 0;
    }
    else return 1;
}

/**
 * data_handler_send():
 *     Manda uma string ao cliente
 *
 * @msg: mensagem que desejamos enviar
 */
void
data_handler_send(void *msg)
{
    socket_write(ds, msg);
}

/**
 * data_handler_read():
 *     Recebe uma string do cliente
 *
 * @buffer: buffer no qual vamos escrever a mensagem recebida
 */
void
data_handler_read(void *buffer)
{
    socket_read(ds, buffer);
}

/**
 * stop_data_handler():
 *     Fecha o socket e as conexões remanescentes, liberando a memória alocada.
 */
void
stop_data_handler() {
    socket_finish(ds);
    socket_close(ds);
    exit(0);
}
