#include "../ext/socket.h"
#define BUFFER_SIZE 1398

void start_data_handler(Socket *ds, int *status);
void stop_data_handler(void) __attribute__((noreturn));
int data_handler_send_file(char *file, void *buffer, int type);
int data_handler_receive_file(char *file, void *buffer, int type);
