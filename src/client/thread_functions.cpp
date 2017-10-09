//thread_functions.cpp
//Authors: Jeff Klouda and Matthew D'Alonzo

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ps_client/client.h"


void* publishing_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket publishing_socket;
	int sock_fd = publishing_socket.sock_connect(ta->host, ta->port);
    
    while(true){
        if(!ta->out_messages->empty()){
            //std::cout << "OUTBOX SIZE: " << ta->out_messages->size() << std::endl;
            sem_wait(ta->out_lock);
            Message msg_to_post = ta->out_messages->front();
            //std::cout << "msg_to_post type before: [" << msg_to_post.type << "]" << std::endl;
            ta->out_messages->pop_front();
            sem_post(ta->out_lock);
            //std::cout << "msg_to_post type after: " << msg_to_post.type << std::endl;
            std::string msg_type = msg_to_post.type;
            //std::cout << "msg_type [" << msg_type << "]\n";
            if (msg_type == "MESSAGE"){
                std::cout << "IN Publish\n" << std::endl;
                std::string msg_line = "PUBLISH " + msg_to_post.topic + " " + std::to_string(msg_to_post.body.size()) + "\n" + msg_to_post.body;
                sem_wait(ta->sock_lock);
                if (send(sock_fd, msg_line.c_str(), msg_line.size(), 0) < 0){
                    std::cout << "send MESSAGE failed\n";
                }
                sem_post(ta->sock_lock);
            }else if (msg_type == "IDENTIFY"){
                std::string id_line = "IDENTIFY " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
                sem_wait(ta->sock_lock);
                if (send(sock_fd, id_line.c_str(), id_line.size(), 0) < 0){
                    std::cout << "send IDENTIFY failed\n";
                }
                sem_post(ta->sock_lock);
            }else if (msg_type == "SUBSCRIBE"){
                std::string sub_line = "SUBSCRIBE " + msg_to_post.topic + "\n";
                sem_wait(ta->sock_lock);
                if (send(sock_fd, sub_line.c_str(), sub_line.size(), 0) < 0){
                    std::cout << "send SUBSCRIBE failed\n";
                }
                sem_post(ta->sock_lock);
            }else if (msg_type == "UNSUBSCRIBE"){
                std::string unsub_line = "UNSUBSCRIBE " + msg_to_post.topic + "\n";
                sem_wait(ta->sock_lock);
                if (send(sock_fd, unsub_line.c_str(), unsub_line.size(), 0) < 0){
                    std::cout << "send UNSUBSCRIBE failed\n";
                }
                sem_post(ta->sock_lock);
            }else if (msg_type == "RETRIEVE"){
                std::string retrieve_line = "RETRIEVE " + std::string(ta->cid) + "\n";
                sem_wait(ta->sock_lock);
                if (send(sock_fd, retrieve_line.c_str(), retrieve_line.size(), 0) < 0){
                    std::cout << "send RETRIEVE failed\n";
                }
                sem_post(ta->sock_lock);
            }else if (msg_type == "DISCONNECT"){
                std::string disconnect_line = "DISCONNECT " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
                sem_wait(ta->sock_lock);
                if (send(sock_fd, disconnect_line.c_str(), disconnect_line.size(), 0) < 0){
                    std::cout << "send DISCONNECT failed\n";
                }
                sem_post(ta->sock_lock);
            }else{
                std::cout << "Error: Could not understand message type. Message title is: " << msg_type << "]" << std::endl;
            }
        }

    }
    return NULL;
}

void* receiving_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket receiving_socket;
	int sock_fd = receiving_socket.sock_connect(ta->host, ta->port);

    char buffer[BUFSIZ];
    while (true){
        std::cout << "BEFORESOCKLOCK#$#$#$#$\n";
        //sem_wait(ta->sock_lock);
        std::cout << "\nINBOX SIZE: " << ta->inbox->size() << "\n";
        if (recv(sock_fd, buffer, BUFSIZ, 0) < 0) {
            std::cout << "HEY WE DIDNT GET SOMETHING:(((((((((((((((((((((\n";
            //sem_post(ta->sock_lock);
            continue;
        }
        std::cout << "HEY WE GOT SOMETHING**((**((**\n";
        //sem_post(ta->sock_lock);
        sem_wait(ta->callback_lock);
        ta->inbox->push_back(buffer);
        sem_post(ta->callback_lock);
    }
    return NULL;
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
    return NULL;
}
