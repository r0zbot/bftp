#include "../ext/socket.h"
#define BUFFER_SIZE 1398

void start_data_handler(Socket *ds, int *status);
int data_handler_send_file(char *file, void *buffer, int type);
void data_handler_send(void *msg);
int data_handler_receive_file(char *file, void *buffer, int type);
void stop_data_handler();
