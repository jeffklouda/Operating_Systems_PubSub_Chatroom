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

}
