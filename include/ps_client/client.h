// client.h: PS Client Library -------------------------------------------------

//NECESSARY INCLUDES
#include <string.h>
#include <string>
#include <deque>
#include <queue>
#include <map>
#include <semaphore.h>

#pragma once

struct Message{
    std::string type;
    std::string topic;
    std::string sender;
    size_t      nonce;
    std::string body;
    int         length;
};

class Callback{
    public:
        //Callback();
        //~Callback();
        virtual void run(Message&){};
};

class EchoCallback : public Callback{
    public:
        void run(Message&);
};

class ChatCallback : public Callback {
    public:
        void run(Message&);
};

typedef void* (*thread_func)(void*);

class Socket {
    public:
        Socket();
        ~Socket();
        int sock_connect(const char*, const char*);
        void sock_disconnect();
    private:
        int sfd;    //  socket file descriptor
};

class Client{
    public:
        Client(const char*, const char*, const char*);
        ~Client();
        void publish(const char*, const char*, size_t);
        void subscribe(const char*, Callback*);
        void unsubscribe(const char*);
        int  serverConnect();
        void disconnect();
        void run();
        bool shutdown();
        sem_t* get_io_lock();
    private:
        const char*         name;
        const char*         host;
        const char*         port;
        const char*         cid;
        size_t              nonce;
        bool                disconnect_msg;
        int                 socket_fd;
        std::deque<Message> outMessages;
        std::deque<std::string> inbox;
        std::map<std::string, Callback*> topicCallbacks;
        sem_t               out_lock;
        sem_t               callback_lock;
        sem_t               sock_lock;
        sem_t               io_lock;
};


struct thread_args {
    const char* host;
    const char* port;
    const char* cid;
    size_t      nonce;
    std::deque<Message> *out_messages;
    std::deque<std::string> *inbox;
    std::map<std::string, Callback*> *message_callbacks;
    sem_t* out_lock;
    sem_t* callback_lock;
    sem_t* sock_lock;
    sem_t* io_lock;
    Client* client;
};

class Thread{
    public:
        Thread();
        ~Thread();
        void start(thread_func, void*);
        void join(void**);
        void detach();
        pthread_t get_thread_var();
    private:
        thread_func func;
        void*       arg;
        pthread_t   thread_var;
        Socket      sock;
};


void* publishing_thread(void*);
void* receiving_thread(void*);
void* callbacks_thread(void*);



// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
