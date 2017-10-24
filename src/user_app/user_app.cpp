// echo_test.cpp ---------------------------------------------------------------

// SYSTEM INCLUDES
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

// C++ PROJECT INCLUDES
#include <ps_client/client.h>

// Constants -------------------------------------------------------------------

const char  *CHAT_TOPIC    = "chat";

// Generator -------------------------------------------------------------------

void *echo_generator(void *arg) {
    Client *client = (Client *)arg;

    /*for (size_t i = 0; i < ECHO_MESSAGES; i++) {
        char message[BUFSIZ];
        snprintf(message, BUFSIZ, "%lu %03lu", time(NULL), i);
        client->publish(ECHO_TOPIC, message, strlen(message));
        sleep(1);
        printf("PUBLISH TO %s: %s\n", ECHO_TOPIC, message);
    }*/
    while (true) {
        char c = 'a';
        while (c != 'm' ) {
            std::cin.get(c);
        }
        c = 'a';
        sem_wait(client->get_io_lock());
        std::string user_message = "";
        std::cout << "Enter your message: ";
        std::cin.ignore();
        std::getline(std::cin, user_message);
        sem_post(client->get_io_lock());
        client->publish(CHAT_TOPIC, user_message.c_str(), user_message.length());
    }
    //std::cout << "before disconnect!\n";
    client->disconnect();
    //std::cout << "after disconnect!\n";
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

    generator.start(echo_generator, (void *)&client);
    //std::cout << "before detach!\n";
    generator.detach();
    //std::cout << "before subscribe!\n";
    client.subscribe(CHAT_TOPIC, &e);
    //std::cout << "before run!\n";
    client.run();

    return EXIT_SUCCESS;
}

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
