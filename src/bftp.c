#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "connection_handler.h"
#include "client_handler.h"
#include "util.h"

int *status;

/**
 * sigint_handler()
 */
void sigint_handler()
{
	if (*status == CONNECTION) stop_connection_handler();
	else if (*status == CLIENT) stop_client_handler();
}

/**
 * main()
 */
int
main()
{
	status = ecalloc(sizeof(int));
	signal(SIGINT, sigint_handler);
	
	start_connection_handler(status);
	
	return 0;
}
