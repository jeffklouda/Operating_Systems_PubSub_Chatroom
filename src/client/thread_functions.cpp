//thread_functions.cpp
//Authors: Jeff Klouda and Matthew D'Alonzo

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "ps_client/client.h"


void* publishing_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket publishing_socket;
    int sock_fd = publishing_socket.sock_connect(ta->host, ta->port);
    char buffer[BUFSIZ];

    FILE* fp = fdopen(sock_fd, "w+");    

    std::string identify_string = "IDENTIFY " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";

    sem_wait(ta->sock_lock);
    if (fputs(identify_string.c_str(), fp) == EOF){
        fprintf(stderr, "PUBLISH Failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_post(ta->sock_lock);

   
    if(fgets(buffer, BUFSIZ, fp)==NULL){
        fprintf(stderr, "First publishing fgets failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //while(true){
    while(!ta->client->shutdown()){
        if(!ta->out_messages->empty()){
            //std::cout << "OUTBOX SIZE: " << ta->out_messages->size() << std::endl;
            sem_wait(ta->out_lock);
            Message msg_to_post = ta->out_messages->front();
            //std::cout << "msg_to_post type before: [" << msg_to_post.type << "]" << std::endl;
            ta->out_messages->pop_front();
            sem_post(ta->out_lock);
            //std::cout << "msg_to_post type after: " << msg_to_post.type << std::endl;
            std::string msg_type = msg_to_post.type;
            //std::cout << "sending msg_type [" << msg_type << "]\n";
            std::string good_line = "";
            if (msg_type == "MESSAGE"){
                //std::cout << "IN Publish\n" << std::endl;
                good_line = "PUBLISH " + msg_to_post.topic + " " + std::to_string(msg_to_post.body.size()) + "\n" + msg_to_post.body;
            }else if (msg_type == "IDENTIFY"){
                good_line = "IDENTIFY " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
            }else if (msg_type == "SUBSCRIBE"){
                good_line = "SUBSCRIBE " + msg_to_post.topic + "\n";
            }else if (msg_type == "UNSUBSCRIBE"){
                good_line = "UNSUBSCRIBE " + msg_to_post.topic + "\n";
            }else if (msg_type == "RETRIEVE"){
                good_line = "RETRIEVE " + std::string(ta->cid) + "\n";
            }else if (msg_type == "DISCONNECT"){
                good_line = "DISCONNECT " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";
            }else{
                std::cout << "Error: Could not understand message type. Message title is: " << msg_type << "]" << std::endl;
                //exit(EXIT_FAILURE);
            }
            //std::cout << "MESSASGE STATEMENT:{\n" << good_line << "}\n";
 
            sem_wait(ta->sock_lock);         
            if (fputs(good_line.c_str(), fp) == EOF){
                fprintf(stderr, "PUBLISH Failed: %s\n", strerror(errno));
            }
            sem_post(ta->sock_lock); 

            if (fgets(buffer, BUFSIZ, fp)==NULL){
                fprintf(stderr, "Second Publishing fgets failed: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    publishing_socket.sock_disconnect();
    return NULL;
}

void* receiving_thread(void* arg){
    struct thread_args *ta = (struct thread_args *) arg;
    Socket receiving_socket;
	int sock_fd = receiving_socket.sock_connect(ta->host, ta->port);
    char buffer[BUFSIZ];
    FILE* fp = fdopen(sock_fd, "w+");

    std::string identify_string = "IDENTIFY " + std::string(ta->cid) + " " + std::to_string(ta->nonce) + "\n";

    sem_wait(ta->sock_lock);
    fputs(identify_string.c_str(), fp);
    sem_post(ta->sock_lock);

    if (fgets(buffer, BUFSIZ, fp)==NULL){
        fprintf(stderr, "First receiving fgets failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //  send retrieval message
    std::string rMessage = "RETRIEVE " + std::string(ta->cid) + "\n";
    //std::cout << "rMessage: " << rMessage << std::endl; 
    //while(true){
    while (!ta->client->shutdown()){
        char buffer_2[BUFSIZ];
        //sem_wait(ta->sock_lock);
        sem_wait(ta->sock_lock);
        if (fputs(rMessage.c_str(), fp) == EOF) {
            fprintf(stderr,"send RETRIEVE failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        sem_post(ta->sock_lock);
        //sem_post(ta->sock_lock);
        //sem_wait(ta->sock_lock);
        //std::cout << "\nINBOX SIZE: " << ta->inbox->size() << "\n";
        //std::cout << "BEFORE\n";
        if (fgets(buffer_2, BUFSIZ, fp)==NULL) {
            fprintf(stderr, "Second receiving failed: %s\n", strerror(errno));
            //sem_post(ta->sock_lock);
            exit(EXIT_FAILURE);
        }
        //std::cout <<"BUFFER: " << buffer << std::endl;

        std::string to_parse = buffer_2;

        size_t start_length = to_parse.find("LENGTH")+7;

        int length_num = stoi(to_parse.substr(start_length, to_parse.size() - start_length));

        //std::cout << "length_num " << length_num << std::endl;     

        char buffer_3[BUFSIZ];
        fread(buffer_3, 1, length_num, fp);

        std::string to_push = std::string(buffer_2) + std::string(buffer_3);

        memset(&buffer_2[0], 0, BUFSIZ);
        memset(&buffer_3[0], 0, BUFSIZ);

        ta->inbox->push_back(to_push);
        //sem_post(ta->callback_lock);
        
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
        if (message.substr (start, end - start) == "MESSAGE") {
            //  Process message with callback map
            Message pMessage;
            pMessage.type = "MESSAGE";
            //  get topic
            start = message.find("MESSAGE");
            start = start + 8;
            end = message.find(" ", start);
            pMessage.topic = message.substr(start, end-start);
            start = end + 1;
            //  get sender
            start = message.find("FROM", start) + 5;
            end = message.find(" ", start);
            pMessage.sender = message.substr(start, end-start);
            start = end + 1;
            //  get length
            start = message.find("LENGTH", start) + 7;
            end = message.find("\n", start);
            pMessage.length = std::stoi(message.substr(start, end-start));
            //  get body
            start = end+1;
            pMessage.body = message.substr(start, message.length()-start);
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
