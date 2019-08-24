#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "connection_handler.h"
#include "control_handler.h"
#include "data_handler.h"
#include "util.h"

//TODO: Fix compiler warnings

int *status;

/**
 * sigint_handler():
 *     Recebe o SIGINT e chama a função apropriada de acordo com o status
 *     do processo.
 */
void
sigint_handler()
{
    if (*status == CONNECTION) stop_connection_handler();
    else if (*status == CONTROL) stop_control_handler();
    else if (*status == DATA) stop_data_handler();
    free(status);
}

/**
 * main():
 *     Inicia o servidor na porta 21 caso nenhum argumento tenha sido passado.
 *
 * @argv[1]: número de porta desejada
 */
int
main(int argc, char **argv)
{
    status = ecalloc(sizeof(int));

    signal(SIGINT, sigint_handler);
    srand(time(0));

    int port = 21;
    if (argc > 1) port = atoi(argv[1]);
    start_connection_handler(status, port);

    return 0;
}
