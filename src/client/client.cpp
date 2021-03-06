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
#include <signal.h>

#include "ps_client/client.h"

#define MAXLEN 1000

Client::Client(const char *host, const char *port, const char *cid) {
    sem_init (&out_lock, 0, 1);
    sem_init (&callback_lock, 0, 1);
    sem_init (&sock_lock, 0, 1);
    sem_init (&io_lock, 0, 1);
	this->disconnect_msg = false;
    this->host = host;
    this->port = port;
    this->cid = cid;
    this->name = cid;
}

Client::~Client() {}

void Client::publish(const char *topic, const char *message, size_t length){
    std::string str_topic(topic, strlen(topic));
    std::string str_message(message, length);
    std::string str_sender(name, strlen(name));
    Message temp_message = {
        "MESSAGE",        
        str_topic,
        str_sender,
        nonce,
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
    sem_wait(&out_lock);
    disconnect_msg = true;
    sem_post(&out_lock);
}

void Client::run() {
    thread_args arg = {
        host,
        port,
        cid,
        nonce,
        &outMessages,
        &inbox,
        &topicCallbacks,
        &out_lock,
        &callback_lock,
        &sock_lock,
        &io_lock,
        this
    };
    Thread  publisher;
    Thread  receiver;
    Thread  callbacks;
    publisher.start(publishing_thread,(void*)&arg);
    publisher.detach();
    receiver.start(receiving_thread, (void*)&arg);
    receiver.detach();
    while (!shutdown()){
        callbacks_thread((void*)&arg);
    }
    pthread_cancel(publisher.get_thread_var());
    pthread_cancel(receiver.get_thread_var());
}

bool Client::shutdown() {
    sem_wait(&out_lock);
    bool result = disconnect_msg;
    sem_post(&out_lock);
    return result;
}

const char* Client::get_name() {
    return name;
}

const char* Client::get_host() {
    return host;
}

const char* Client::get_cid() {
    return cid;
}

size_t Client::get_nonce() {
    return nonce;
}

bool Client::get_disconnect_msg() {
    return disconnect_msg;
}

int Client::get_socket_fd() {
    return socket_fd;
}

std::deque<Message>* Client::get_outMessages() {
    return &outMessages;
}

std::deque<std::string>* Client::get_inbox() {
    return &inbox;
}

std::map<std::string, Callback*>* Client::get_topicCallbacks() {
    return &topicCallbacks;
}

sem_t* Client::get_out_lock() {
    return &out_lock;
}

sem_t* Client::get_callback_lock() {
    return &callback_lock;
}

sem_t* Client::get_sock_lock() {
    return &sock_lock;
}

sem_t* Client::get_io_lock() {
    return &io_lock;
}
