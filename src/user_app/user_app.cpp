// echo_test.cpp ---------------------------------------------------------------

// SYSTEM INCLUDES
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <limits>
#include <csignal>

// C++ PROJECT INCLUDES
#include <ps_client/client.h>

// Constants -------------------------------------------------------------------

const char  *CHAT_TOPIC    = "chat";

bool SIGINT_flag = false;

// Generator -------------------------------------------------------------------

void *chat_generator(void *arg) {
    Client *client = (Client *)arg;
    std::cout << "Welcome, press enter to send a message\n";

    std::string enterMessage = "has joined.\n";
    client->publish(CHAT_TOPIC, enterMessage.c_str(), enterMessage.length());

    while (!client->shutdown()) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        sem_wait(client->get_io_lock());
        std::string user_message = "";
        std::cout << "Enter your message: ";
        std::getline(std::cin, user_message);
        sem_post(client->get_io_lock());
        client->publish(CHAT_TOPIC, user_message.c_str(), user_message.length());
    }
    
    return NULL;
}

// Main execution --------------------------------------------------------------

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
    	fprintf(stderr, "Usage %s: host port client_id\n", argv[0]);
    	return EXIT_FAILURE;
    }

    const char *host      = argv[1];
    const char *port      = argv[2];
    const char *client_id = argv[3];

    Client       client(host, port, client_id);
    Thread       generator;
    ChatCallback e;

    generator.start(chat_generator, (void *)&client);
    generator.detach();
    client.subscribe(CHAT_TOPIC, &e);
    client.run();

    return EXIT_SUCCESS;
}

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
