// Necssary Includes
#include <vector>
#include <string.h>

#include "client.h"

Client::Client(const char *host, const char *port, const char *cid) {
    this->host = host;
    this->port = port;
    this->cid = cid;
}

Client::~Client() {}

void Client::publish(const char *topic, const char *message, size_t length){
    int sockfd = socket ();
    connect(sockfd, )
}

void Client::subscribe(const char *topic, Callback *callback) {

}

void Client::unsubscribe(const char *topic) {

}

void Client::disconnect() {

}

void Client::run() {

}

bool Client::shutdown() {

}
