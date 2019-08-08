#include <signal.h>
#include "connection_handler.h"

/* Signal Handler for SIGINT */
void sigint_handler()
{
	stop_connection_handler();
}

/**
 * main()
 */
int
main(int argc, char **argv)
{
	signal(SIGINT, sigint_handler);
	start_connection_handler();
	return 0;
}
