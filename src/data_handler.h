#include "../ext/socket.h"
#define BUFFER_SIZE 1398

void start_data_handler(Socket *ds, int *status);
int data_handler_send_file(char *file, void *buffer);
void data_handler_send(void *msg);
void data_handler_write_file(void *buffer);
void stop_data_handler();
