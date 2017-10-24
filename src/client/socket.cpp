// socket.cpp: socket class

#include <iostream>
#include <utility>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string>

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
    //struct addrinfo *rp = results;

    for (struct addrinfo *rp = results; rp != NULL && socket_fd < 0; rp = rp->ai_next) {

        if ((socket_fd = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol)) < 0) {
            fprintf (stderr, "Client: Unable to make socket: %s\n",
                strerror(errno));
            continue;
        }

        if ( connect(socket_fd, rp->ai_addr, rp->ai_addrlen) < 0 ) {

    //        fprintf(stderr, "Client: Unable to connect: %s\n",
    //            strerror(errno));
            socket_fd = -1;
            continue;
        }
    }

    freeaddrinfo(results);

    this->sfd = socket_fd;
    std::cout << "Connected successfully. socket_fd: " << socket_fd << std::endl;
    return socket_fd;
}   

void Socket::sock_disconnect() {
    if (sfd >= 0) {
        close(sfd);
        sfd = -1;
    }
}
