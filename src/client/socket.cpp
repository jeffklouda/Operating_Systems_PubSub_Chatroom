// socket.cpp: socket class

#include <utility>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "ps_client/client.h"

Socket::Socket() {
    sfd = -1;
}
Socket::~Socket() {}

int Socket::sock_connect(const char* host, const char* port) {

    struct addrinfo hints;
    hints.ai_family      = AF_UNSPEC;       // return IPv4 and IPv6 choices
    hints.ai_socktype    = SOCK_STREAM;     // Use TCP
    hints.ai_flags       = AI_PASSIVE;      // Use all interfaces

    struct addrinfo* results;
    int status;

    if ((status = getaddrinfo(host, port, &hints, &results)) != 0) {

        fprintf(stderr, "Client: getaddrinfo failed: %s\n",
            gai_strerror(status));
        return -1;
    }

    int socket_fd = -1;
    struct addrinfo *rp = results;

    if ((socket_fd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol)) < 0) {
        fprintf (stderr, "Client: Unable to make socket: %s\n",
            strerror(errno));
        return -1;
    }

    if ( connect(socket_fd, rp->ai_addr, rp->ai_addrlen) < 0 ) {

        fprintf(stderr, "Client: Unable to connect: %s\n",
            strerror(errno));
        return -1;
    }
    
    this->sfd = socket_fd;
    return socket_fd;
}     
