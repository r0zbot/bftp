#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "../ext/socket.h"
#include "../hdr/error.h"
#include "util.h"
#include "control_handler.h"

void start_connection_handler(int *status, int port);
void stop_connection_handler(void);
