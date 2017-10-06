//callback.cpp
//Implementation of derived classes of Callback class.
//Authors: Jeff Klouda and Matthew D'Alonzo

//C++ INCLUDES
#include <iostream>

//HEADER FILES
#include "client.h"

class EchoCallback : public Callback {
public:
    void run(Message &m) {
        std::cout << "Echo Callback: " << message.body << std::endl;
    }
};
