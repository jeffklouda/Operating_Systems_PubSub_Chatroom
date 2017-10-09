//thread_functions.cpp
//Authors: Jeff Klouda and Matthew D'Alonzo

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "ps_client/client.h"


void* publishing_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket publishing_socket;
	int sock_fd = publishing_socket.sock_connect(ta->host, ta->port);
    
    while(true){
        if(!ta->out_messages->empty()){
            sem_wait(ta->out_lock);
            Message msg_to_post = ta->out_messages->front();
            ta->out_messages->pop_front();
            std::string msg_type = msg_to_post.type;

            if (msg_type == "MESSAGE"){
                std::string msg_line = "PUBLISH " + msg_to_post.topic + " " + std::to_string(msg_to_post.body.size()) + "\n" + msg_to_post.body;
                send(sock_fd, msg_line.c_str(), msg_line.size(), 0);
            }else if (msg_type == "IDENTIFY"){
                std::string id_line = "IDENTIFY " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
                send(sock_fd, id_line.c_str(), id_line.size(), 0);
            }else if (msg_type == "SUBSCRIBE"){
                std::string sub_line = "SUBSCRIBE " + msg_to_post.topic + "\n";
                send(sock_fd, sub_line.c_str(), sub_line.size(), 0);
            }else if (msg_type == "UNSUBSCRIBE"){
                std::string unsub_line = "UNSUBSCRIBE " + msg_to_post.topic + "\n";
                send(sock_fd, unsub_line.c_str(), unsub_line.size(), 0);
            }else if (msg_type == "RETRIEVE"){
                std::string retrieve_line = "RETRIEVE " + std::string(ta->cid) + "\n";
                send(sock_fd, retrieve_line.c_str(), retrieve_line.size(), 0);
            }else if (msg_type == "DISCONNECT"){
                std::string disconnect_line = "DISCONNECT " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
                send(sock_fd, disconnect_line.c_str(), disconnect_line.size(), 0);
            }else{
                std::cout << "Error: Could not understand message type" << std::endl;
            }
            sem_post(ta->out_lock);
        }

    }
}

void* receiving_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket receiving_socket;
	int sock_fd = receiving_socket.sock_connect(ta->host, ta->port);

    char buffer[BUFSIZ];
    while (true){
        if (recv(sock_fd, buffer, BUFSIZ, 0) < 0) {
            continue;
        }

        sem_wait(ta->callback_lock);
        ta->inbox->push_back(buffer);
        sem_post(ta->callback_lock);
    }
}

void* callbacks_thread(void* arg){ 
    struct thread_args *ta = (struct thread_args *) arg;
    if (!ta->inbox->empty()) {
        std::string message = ta->inbox->front();
        ta->inbox->pop_front();
        size_t start = 0;
        size_t end = message.find(" ");
        if (message.substr (start, end - start - 1) == "MESSAGE") {
            //  Process message with callback map
            Message pMessage;
            pMessage.type = "MESSAGE";
            //  get topic
            start = message.find("MESSAGE");
            start = start + 8;
            end = message.find(" ", start);
            pMessage.topic = message.substr(start, end - 1);
            start = end + 1;
            //  get sender
            start = message.find("FROM", start) + 5;
            end = message.find(" ", start);
            pMessage.sender = message.substr(start, end - 1);
            start = end + 1;
            //  get length
            start = message.find("LENGTH", start) + 7;
            end = message.find(" ", start);
            pMessage.length = std::stoi(message.substr(start, end-1));
            //  get body
            start = end + 1;
            end = message.find("\n", start);
            pMessage.body = message.substr(start, end);
            //  Look up topic in map
            std::map<std::string, Callback*>::iterator it;
            it = ta->message_callbacks->find(pMessage.topic);
            if (it != ta->message_callbacks->end()) {
                it->second->run(pMessage);
            }
        }
    }
}
