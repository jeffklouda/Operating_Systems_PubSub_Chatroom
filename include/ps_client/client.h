// client.h: PS Client Library -------------------------------------------------

//NECESSARY INCLUDES
#include <string.h>
#include <string>

#pragma once

enum thread_func { publish, retrieve, callback };

struct Message{
    std::string type;
    std::string topic;
    std::string sender;
    size_t      nonce;
    std::string body;
};

class Client{
    public:
        Client(const char*, const char*, const char*);
        ~Client();
        void publish(const char*, const char*, size_t);
        void subscribe(const char*, Callback*);
        void unsubscribe(const char*);
        void disconnect();
        void run();
        bool shutdown();
    private:
        const char* name;
        const char* host;
        const char* port;
        const char* cid;
};

class Callback{
    public:
        Callback();
        ~Callback();
        virtual void run(Message&);
};

class Thread{
    public:
        Thread();
        ~Thread();
        void start(thread_func, void*);
        void join(void**);
        void detach();
    private:
        thread_func func;
        void*       arg;
        pthread_t   thread_var;
};

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
