// thread.cpp
// Implementation of thread class
// Authors: Matthew D'Alonzo and Jeff Klouda

#include "client.h"
#include <pthread.h>

Thread::Thread(){

}

Thread::~Thread(){

}

void Thread::start(thread_func func, void* arg){
    pthread_create(thread_var, NULL, func, arg);
}

void Thread::join(void** result){
    pthread_join(thread_var, result);
}

void Thread::detach(){
    pthread_detach(thread_var);
}
