#include "connection_handler.h"

Socket *s;

/**
 * start_connection_handler():
 *     Inicia o servidor usando a estrutura Socket e entra num loop. Caso uma
 *     conexão TCP seja aceita, inicia a conexão de controle num outro processo.
 *
 * @status: indica em que fase o processo está
 * @port: a porta na qual o servidor será aberto
 */
void
start_connection_handler(int *status, int port)
{
    *status = CONNECTION;
    s = socket_open(port);

    dprint("bftp escutando na porta %d...\n", socket_port(s));

    while (true) {
        if (socket_accept(s) < 0) break;
        if (!fork()) start_control_handler(s, status);
    }
}

/**
 * stop_connection_handler():
 *     Fecha o socket e as conexões remanescentes, liberando a memória alocada.
 */
void
stop_connection_handler()
{
    socket_close(s);
}
