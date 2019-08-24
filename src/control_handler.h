#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include "../hdr/error.h"
#include "util.h"
#include "data_handler.h"

void
start_control_handler(Socket *s, int *status);

void
stop_control_handler();
