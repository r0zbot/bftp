#include "connection_handler.h"

static Socket *s;

/**
 * Inicia o servidor usando a estrutura Socket e entra num loop. Caso uma
 * conexão TCP seja aceita, inicia a conexão de controle num outro processo.
 */
void
start_connection_handler(int *status, int port)
{
    *status = CONNECTION;
    s = socket_open(port);
    if (!s) fatal("bftp: Erro ao criar socket\n");

    printf("bftp escutando na porta %d...\n", socket_port(s));

    while (true) {
        if (socket_accept(s) < 0) break;
        if (!fork()) start_control_handler(s, status);
    }
}

/**
 * Fecha o socket e as conexões remanescentes, liberando a memória alocada.
 */
void
stop_connection_handler()
{
    socket_close(s);
}
