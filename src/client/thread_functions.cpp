//thread_functions.cpp
//Authors: Jeff Klouda and Matthew D'Alonzo

#include <iostream>

#include "ps_client/client.h"

ta->host
ta->port
ta->cid
ta->nonce
ta->out_messages
ta->message_callbacks
ta->out_lock
ta->callback_lock

void* publishing_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket publishing_socket;
	int sock_fd = publishing_socket.sock_connect(ta->host, ta->port);
    
    while(true){
        if(!ta->out_messages.empty()){
            sem_wait(&ta->out_lock);
            Message msg_to_post = ta->out_messages[0];
            ta->out_messages.pop_front();
            msg_type = msg_to_post->type;

            if (msg_type == "MESSAGE"){
                std::string msg_line = "PUBLISH " + msg_to_post.topic + " " + std::to_string(msg_to_post.body.size()) + "\n" + msg_to_post->body;
                send(sock_fd, msg_line.c_str(), msg_line.size(), 0);
            }else if (msg_type == "IDENTIFY"){
                std::string id_line = "IDENTIFY " + ta->cid + " " + ta->nonce + "\n";
                send(sock_fd, id_line.c_str(), id_line.size(), 0);
            }else if (msg_type == "SUBSCRIBE"){
                std::string sub_line = "SUBSCRIBE " + msg_to_post.topic + "\n";
                send(sock_fd, sub_line.c_str(), sub_line.size(), 0);
            }else if (msg_type == "UNSUBSCRIBE"){
                std::string unsub_line = "UNSUBSCRIBE " + msg_to_post.topic + "\n";
                send(sock_fd, unsub_line.c_str(), unsub_line.size(), 0);
            }else if (msg_type == "RETRIEVE"){
                std::string retrieve_line = "RETRIEVE " + ta->cid + "\n";
                send(sock_fd, retrieve_line.c_str(), retrieve_line.size(), 0);
            }else if (msg_type == "DISCONNECT"){
                std::string disconnect_line = "DISCONNECT " + ta->cid + " " + ta->nonce + "\n";
                send(sock_fd, disconnect_line.c_str(), disconnect_line.size), 0);
            }else{
                std::cout << "Error: Could not understand message type" << std::endl;
            }
            sem_post(&ta->out_lock);
        }

    }
}

void* receiving_thread(void* arg){

}

void* callbacks_thread(void* arg){

}
