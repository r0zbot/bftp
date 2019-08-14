#include <signal.h>
#include "connection_handler.h"

/**
 * sigint_handler()
 */
void sigint_handler()
{
	stop_connection_handler();
}

/**
 * main()
 */
int
main()
{
	signal(SIGINT, sigint_handler);
	start_connection_handler();
	return 0;
}
