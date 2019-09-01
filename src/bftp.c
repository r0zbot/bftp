#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "connection_handler.h"
#include "control_handler.h"
#include "data_handler.h"
#include "util.h"

void sigint_handler(int sig);
static int *status;

/**
 * Recebe o SIGINT e chama a função apropriada de acordo com o status
 * do processo.
 */
void
sigint_handler(int sig)
{
    (void) sig;
    if (*status == CONNECTION) stop_connection_handler();
    else if (*status == CONTROL) stop_control_handler();
    free(status);
}

/**
 * Inicia o servidor na porta 21 caso nenhum argumento tenha sido passado.
 */
int
main(int argc, char **argv)
{
    status = ecalloc(sizeof(int));

    signal(SIGINT, &sigint_handler);

    int port = 21;
    if (argc > 1) port = atoi(argv[1]);
    start_connection_handler(status, port);

    return 0;
}
