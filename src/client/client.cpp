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
#include <semaphore.h>

#include "ps_client/client.h"

#define MAXLEN 1000

Client::Client(const char *host, const char *port, const char *cid) {
    this->host = host;
    this->port = port;
    this->cid = cid;
    Message iMessage;
    iMessage.topic = "IDENTIFY";
    iMessage.sender = cid;
    iMessage.nonce = rand()%1000;
    outMessages.push_back(iMessage);
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
    outMessages.push_back(temp_message);
}

void Client::subscribe(const char *topic, Callback *callback) {
    Message subMessage;
    subMessage.type = "SUBSCRIBE";
    subMessage.topic = topic;
    subMessage.sender = cid;
    std::pair <std::string, Callback*> newTopicCallback;
    newTopicCallback = std::make_pair(topic, callback);
    topicCallbacks.insert (newTopicCallback);
    outMessages.push_back(subMessage);
}

void Client::unsubscribe(const char *topic) {
    Message unsubMessage;
    unsubMessage.type  = "UNSUBSCRIBE";
    unsubMessage.topic = topic;
    unsubMessage.sender = cid;
    topicCallbacks.erase (topicCallbacks.find(topic));
    outMessages.push_back(unsubMessage);
}

void Client::disconnect() {
    Message dMessage;
    dMessage.type  = "DISCONNECT";
    dMessage.sender = this->cid;
    dMessage.nonce = this->nonce;
    outMessages.push_back(dMessage);
}

void Client::run() {
    sem_t out_lock;
    sem_t callback_lock;
    sem_init (&out_lock, 0, 0);
    sem_init (&callback_lock, 0, 0);
    thread_args arg = {
        host,
        port,
        cid,
        nonce,
        &outMessages,
        &topicCallbacks,
        &out_lock,
        &callback_lock
    };
    Thread  publisher;
    Thread  receiver;
    Thread  callbacks;
    publisher.start(publishing_thread,(void*)arg);
    publisher.detach();
    receiver.start(receiving_thread, (void*)arg);
    receiver.detatch();
    callbacks_thread((void*)arg);
}

bool Client::shutdown() {
    return true;
}

void* publish(void * arg) {
    queue<Message> *outQueue = (queue<Message> *) arg;
}
