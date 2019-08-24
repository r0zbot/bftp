#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket.h"

#undef socket_open
#undef socket_connect
#undef socket_read
#undef socket_write
#undef socket_writef

#ifdef _WIN32
    #include <alloc.h>
	#define MALLOC_SIZE(x) _msize(x)
#elif __APPLE__
    #include <malloc/malloc.h>
	#define MALLOC_SIZE(x) malloc_size(x)
#else
    /* Both linux and cygwin have the same path */
    #include <malloc.h>
    #define MALLOC_SIZE(x) malloc_usable_size(x)
#endif

struct Socket {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    int type, protocol;
};

/**
 * socket_open(): opens a port and returns its Socket pointer
 * @port: # of the port we wish to open
 * @type: socket type (TCP, UDP, ...)
 * @protocol: protocol used
 * @family: socket domain (AF_INET, ...)
 * @queue: max number of clients
 *
 * @return: a pointer to the newly created Socket.
 */
Socket *
socket_open(int vargc, ...)
{
    /* parse va args */
    va_list vargp;
    va_start(vargp, vargc);
    /* initialize with default values */
    int port = 0,
            type = SOCK_STREAM,
            protocol = 0,
            family = AF_INET,
            queue = 16;
    /* assign custom values */
    for (int i = 0; i < vargc; i++) {
        if (i == 0) port = va_arg(vargp, int);
        else if (i == 1) type = va_arg(vargp, int);
        else if (i == 2) protocol = va_arg(vargp, int);
        else if (i == 3) family = va_arg(vargp, int);
        else if (i == 4) queue = va_arg(vargp, int);
    }
    va_end(vargp);

    /* init */
    Socket *s = (Socket *) malloc(sizeof(Socket));
    if (!s) {
        fprintf(stderr, "socket_open: malloc error\n");
        return NULL;
    }
    s -> type = type;
    s -> protocol = protocol;

    if ((s -> sockfd = socket(family, type, protocol)) == -1) {
        fprintf(stderr, "socket_open: cannot create new socket\n");
        // TODO: quit
        return NULL;
    }

    bzero(&s -> servaddr, sizeof(struct sockaddr_in));
    s -> servaddr.sin_family      = family;
    s -> servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    s -> servaddr.sin_port        = htons(port);

    if (bind(s -> sockfd, (struct sockaddr *) &s -> servaddr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "socket_open: cannot bind new socket\n");
        // TODO: quit
        return NULL;
    }

    /* connection-oriented protocols need to call listen() */
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(s -> sockfd, queue) == -1) {
            fprintf(stderr, "socket_open: cannot listen new socket\n");
            return NULL;
        }
    }

//    /* prints out random port */
//    if (port == 0) printf("listening at port %d...\n", socket_port(s));

    return s;
}

/**
 * socket_listen(): waits for client connection - blocking call
 * @s: pointer to the Socket which will be used
 *
 * @return: file descriptor int for the recently accepted connection
 */
int
socket_listen(Socket *s)
{
    /* message-oriented error handling */
    if (!(s -> type == SOCK_STREAM || s -> type == SOCK_SEQPACKET)) return -1;
    /* connection-oriented */
    if (s -> type == SOCK_STREAM || s -> type == SOCK_SEQPACKET) {
        unsigned int len = sizeof(s -> cliaddr);
        if ((s -> connfd = accept(s -> sockfd, (struct sockaddr *) &s -> cliaddr, &len)) == -1) {
//            fprintf(stderr, "socket_listen: cannot accept on new socket\n");
            // TODO: fazer direito
            return -1;
        }
        else return s -> connfd;
    }
    else return -1;
}

/**
 * socket_read(): tries to read the message and copy it to given buffer
 * @s: pointer for the Socket which will be read
 * @buffer: memory space where the message will be written
 * @buffersize: max message length, if any
 * @flags: special flags for recv() call
 *
 * @return: number of bytes read.
 */
int
socket_read(int vargc, ...)
{
    if (vargc < 2) fprintf(stderr, "socket_read(): not enough args!");
    /* parse va args */
    va_list vargp;
    va_start(vargp, vargc);
    /* initialize with default values */
    struct Socket *s = NULL;
    void *buffer = NULL;
    size_t buffersize = 0;
    int flags = 0;
    /* assign custom values */
    for (int i = 0; i < vargc; i++) {
        if (i == 0) s = va_arg(vargp, struct Socket *);
        else if (i == 1) buffer = va_arg(vargp, void *);
        else if (i == 2) buffersize = va_arg(vargp, size_t);
        else if (i == 3) flags = va_arg(vargp, int);
    }
    va_end(vargp);
    /* attempts to determine buffersize */
    if (!buffersize) buffersize = MALLOC_SIZE(buffer);
    // TODO: how do we handle buffersize still being == 0?

    /* wipes buffer clean */
    memset(buffer, 0, buffersize);
//	/* server -> client */
//	if (s -> connfd) return send(s -> connfd, buffer, buffersize, flags);
//	/* client -> server */
//	else return send(s -> sockfd, buffer, buffersize, flags);

    return recv(s -> connfd, buffer, buffersize, flags);
}

/**
 * socket_port(): returns Socket's port number
 * @s: socket which will be used
 *
 * @return: port number
 */
int
socket_port(Socket *s)
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(s -> sockfd, (struct sockaddr *)&sin, &len) != -1)
        return ntohs(sin.sin_port);
    return 0;
}

/**
 * socket_connect(): attempts to connect to a remote server
 * @address: IP address which we wish to connect to
 * @port: port number of the remote process
 * @type: socket type (TCP, UDP, ...)
 * @protocol: protocol used
 * @family: socket domain (AF_INET, ...)
 *
 * @return: pointer to the newly created Socket
 */
Socket *
socket_connect(int vargc, ...)
{
    if (vargc < 2) fprintf(stderr, "socket_connect(): not enough args!");
    /* parse va args */
    va_list vargp;
    va_start(vargp, vargc);
    /* initialize with default values */
    char *address = "127.0.0.1";
    int port = 0,
            type = SOCK_STREAM,
            protocol = 0,
            family = AF_INET;
    /* assign custom values */
    for (int i = 0; i < vargc; i++) {
        if (i == 0) address = va_arg(vargp, char *);
        else if (i == 1) port = va_arg(vargp, int);
        else if (i == 2) type = va_arg(vargp, int);
        else if (i == 3) protocol = va_arg(vargp, int);
        else if (i == 4) family = va_arg(vargp, int);
    }
    va_end(vargp);

    /* init */
    Socket *s = (Socket *) malloc(sizeof(Socket));
    if (!s) {
        fprintf(stderr, "socket_connect: malloc error\n");
        return NULL;
    }
    s -> type = type;
    s -> protocol = protocol;

    if ((s -> sockfd = socket(family, type, protocol)) == -1) {
        fprintf(stderr, "socket_connect: cannot create new socket\n");
        return NULL;
    }

    bzero(&s -> servaddr, sizeof(struct sockaddr_in));
    s -> servaddr.sin_family      = family;
    s -> servaddr.sin_addr.s_addr = inet_addr(address);
    s -> servaddr.sin_port        = htons(port);

    /* attempts to connect to remote server */
    if (connect(s -> sockfd, (struct sockaddr *) &s -> servaddr, sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "socket_connect: cannot connect to server\n");
        return NULL;
    }

    return s;
}

/**
 * socket_write(): writes given message on given socket
 * @s: pointer to the socket which will be written
 * @buffer: message we wish to write to socket
 * @buffersize: message length
 * @flags: optional flags for send() call
 *
 * @return: number of bytes written
 */
int
socket_write(int vargc, ...)
{
    if (vargc < 2) fprintf(stderr, "socket_write(): not enough args!");
    /* parse va args */
    va_list vargp;
    va_start(vargp, vargc);
    /* initialize with default values */
    struct Socket *s = NULL;
    void *buffer = NULL;
    size_t buffersize = 0;
    int flags = 0;
    /* assign custom values */
    for (int i = 0; i < vargc; i++) {
        if (i == 0) s = va_arg(vargp, struct Socket *);
        else if (i == 1) buffer = va_arg(vargp, void *);
        else if (i == 2) buffersize = va_arg(vargp, size_t);
        else if (i == 3) flags = va_arg(vargp, int);
    }
    va_end(vargp);
    /* attempts to determine buffersize */
    if (!buffersize) buffersize = strlen(buffer);
    // TODO: dá pra inferir tamanho de struct?
    if (!buffersize) buffersize = MALLOC_SIZE(buffer);
    // TODO: how do we handle buffersize still being == 0?
    /* server -> client */
    if (s -> connfd) return send(s -> connfd, buffer, buffersize, flags);
    /* client -> server */
    else return send(s -> sockfd, buffer, buffersize, flags);
}

/**
 * socket_writef(): writes formatted message on given socket
 * @s: pointer to the socket which will be written
 * @fmsg: formatted message which we wish to write to socket
 *
 * @return: number of bytes written
 */
int
socket_writef(Socket *s, char *fmsg, ...)
{
    /* parse va args */
    va_list vargp;
    va_start(vargp, fmsg);
    int n = vsnprintf(NULL, 0, fmsg, vargp);
    va_end(vargp);
    if (n >= 0) {
        va_start(vargp, fmsg);
        char buffer[n + 1];
        vsnprintf(buffer, n + 1, fmsg, vargp);
        va_end(vargp);
        return socket_write(2, s, buffer);
    }
    return socket_write(2, s, fmsg);
}

/**
 * socket_ip(): returns ip address of client in printable format
 * @s: socket which will be read
 */
char *
socket_ip_client(Socket *s)
{
    return inet_ntoa(s -> cliaddr.sin_addr);
}

/**
 * socket_ip(): returns ip address of server in printable format
 * @s: socket which will be read
 */
char *
socket_ip_server(Socket *s)
{
    struct sockaddr_in localAddress;
    socklen_t addressLength = sizeof(localAddress);;
    getsockname(s -> connfd, (struct sockaddr*) &localAddress, &addressLength);

    return inet_ntoa(localAddress.sin_addr);
}

/**
 * socket_close(): closes socket and frees malloc'd memory
 * @s: socket which will be closed
 */
void
socket_close(Socket *s)
{
    shutdown(s -> sockfd, SHUT_RDWR);
    close(s -> sockfd);
    free(s);
}

/**
 * socket_fin(): closes connfd
 * @s: socket which will finish connection with client
 */
void
socket_fin(Socket *s)
{
    shutdown(s -> connfd, SHUT_RDWR);
    close(s -> connfd);
}

/**
 * socket_test(): unit test
 * @return: 0 for success and 1 for error
 */
int
socket_test()
{
    // TODO
    return 0;
}
