//thread_functions.cpp
//Authors: Jeff Klouda and Matthew D'Alonzo

#include "ps_client/client.h"

void* publishing_thread(void* arg){
    std::queue<Message> *out_queue = (std::queue<Message>* ) arg;
}

void* receiving_thread(void* arg){

}

void* callbacks_thread(void* arg){

}
