// Necssary Includes
#include <vector>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"

Client::Client(const char *host, const char *port, const char *cid) {
    this->host = host;
    this->port = port;
    this->cid = cid;
}

Client::~Client() {}

void Client::publish(const char *topic, const char *message, size_t length){
    std::string str_topic(topic, strnlen(topic, MAXLEN));
    std::string str_message(message, strnlen(message, MAXLEN));
    std::string str_sender(name, strnlen(name, MAXLEN));
    Message temp_message = {
        "MESSAGE",        
        str_topic,
        str_sender,
        atoi(cid),
        str_message
    };
}

void Client::subscribe(const char *topic, Callback *callback) {
    Message subMessage;
    message.type = "SUBSCRIBE";
    message.topic = topic;
    message.sender = name;
    topicCallbacks.insert (std::pair<string, Callback> (topic, callback));
    outMessages.push_back(subMessage);
}

void Client::unsubscribe(const char *topic) {
    Message unsubMessage;
    unsubMessage.type  = "UNSUBSCRIBE";
    unsubMessage.topic = topic;
    message.sender = name;
    topicCallbacks.erase (topicCallbacks.find(topic));
    outMessages.push_back(unsubMessage);
}

int Client::connect() {

    struct addrinfo hints = {
        .ai_family      = AF_UNSPEC,    // return IPv4 and IPv6 choices
        .ai_socktype    = SOCK_STREAM   // Use TCP
        .ai_flags       = AI_PASSIVE    // Use all interfaces
    };
    
    struct addrinfo* results;
    int status;
    
    if ((status = getaddrinfo(host, port, &hints, &results)) != 0) {
        
        fprintf(stderr, "Client: getaddrinfo failed: %s\n",
            gai_strerror(status));
        return -1;
    }
    
    socket_fd = -1;
    struct addrinfo *rp = results;

    if ((socket_fd = socket(rp->ai_family, rp->ai_socktype, 
        rp->ai_protocol)) < 0) {
        
        fprintf (stderr, "Client: Unable to make socket: %s\n", 
            strerror(errno));
        return -1;
    }

    if ( connect(s, rp->ai_addr, rp->ai_addrlen) < 0 ) {
        
        fprintf(stderr, "Client: Unable to connect: %s\n",
            strerror(errno));
        return -1;
    }

    return socket_fd;

}

void Client::disconnect() {
    closesocket(socket_fd);
}

void Client::run() {
    if (this->connect() < 0) {
        fprintf(stderr, "Client: Unable to connect to server\n
            Shutting down...\n");
        this->shutdown();
    }
    // Identify client

}

bool Client::shutdown() {

}
