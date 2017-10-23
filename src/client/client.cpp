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
	this->disconnect_msg = false;
    this->host = host;
    this->port = port;
    this->cid = cid;
    //Message iMessage;
    //iMessage.type = "IDENTIFY";
    //iMessage.sender = cid;
    //iMessage.nonce = rand()%1000;
    //this->nonce = iMessage.nonce;
    //outMessages.push_front(iMessage);
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
        nonce,
        str_message
    };

    //sem_wait(&out_lock);
	//std::cout << "temp_message: " << temp_message.type << std::endl;
    outMessages.push_back(temp_message);
    //sem_post(&out_lock);
}

void Client::subscribe(const char *topic, Callback *callback) {
    Message subMessage;
    subMessage.type = "SUBSCRIBE";
    subMessage.topic = topic;
    subMessage.sender = cid;
    std::pair <std::string, Callback*> newTopicCallback;
    newTopicCallback = std::make_pair(topic, callback);
    topicCallbacks.insert (newTopicCallback);

    //sem_wait(&out_lock);
    outMessages.push_back(subMessage);
    //sem_post(&out_lock);
}

void Client::unsubscribe(const char *topic) {
    Message unsubMessage;
    unsubMessage.type  = "UNSUBSCRIBE";
    unsubMessage.topic = topic;
    unsubMessage.sender = cid;
    topicCallbacks.erase (topicCallbacks.find(topic));

    //sem_wait(&out_lock);
    outMessages.push_back(unsubMessage);
    //sem_post(&out_lock);
}

void Client::disconnect() {
    Message dMessage;
    dMessage.type  = "DISCONNECT";
    dMessage.sender = this->cid;
    dMessage.nonce = this->nonce;

    //sem_wait(&out_lock);
    outMessages.push_back(dMessage);
    //sem_post(&out_lock);
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
        this
    };
    Thread  publisher;
    Thread  receiver;
    Thread  callbacks;
    std::cout << "outMessages.front: " << outMessages.front().type << "\n";
    publisher.start(publishing_thread,(void*)&arg);
    publisher.detach();
    receiver.start(receiving_thread, (void*)&arg);
    receiver.detach();
    while (!shutdown()){
        signal(SIGPIPE, SIG_IGN);
        callbacks_thread((void*)&arg);
    }
    std::cout << "AFTER SHUTDOWN\n";
    pthread_cancel(publisher.get_thread_var());
    pthread_cancel(receiver.get_thread_var());
}

bool Client::shutdown() {
    sem_wait(&out_lock);
    bool result = disconnect_msg;
    sem_post(&out_lock);
    return result;
}


