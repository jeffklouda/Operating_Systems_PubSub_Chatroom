// Necssary Includes
#include <vector>
#include <utility>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>

#include "ps_client/client.h"

#define MAXLEN 1000

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
    int iCid = atoi(cid);
    Message temp_message = {
        "MESSAGE",        
        str_topic,
        str_sender,
        iCid,
        str_message
    };
}

void Client::subscribe(const char *topic, Callback *callback) {
    Message subMessage;
    subMessage.type = "SUBSCRIBE";
    subMessage.topic = topic;
    subMessage.sender = name;
    std::pair <std::string, Callback*> newTopicCallback;
    newTopicCallback = std::make_pair(topic, callback);
    topicCallbacks.insert (newTopicCallback);
    outMessages.push_back(subMessage);
}

void Client::unsubscribe(const char *topic) {
    Message unsubMessage;
    unsubMessage.type  = "UNSUBSCRIBE";
    unsubMessage.topic = topic;
    unsubMessage.sender = name;
    topicCallbacks.erase (topicCallbacks.find(topic));
    outMessages.push_back(unsubMessage);
}

void Client::disconnect() {
}

void Client::run() {
    if (this->serverConnect() < 0) {
        fprintf(stderr, "Client: Unable to connect to server\n"
            "Shutting down...\n");
        this->shutdown();
    }
    // Identify client

}

bool Client::shutdown() {
    return true;
}
