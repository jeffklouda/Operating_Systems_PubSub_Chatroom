//callback.cpp
//Implementation of derived classes of Callback class.
//Authors: Jeff Klouda and Matthew D'Alonzo

//C++ INCLUDES
#include <iostream>

//HEADER FILES
#include "../../include/ps_client/client.h"

class EchoCallback : public Callback {
public:
    void run(Message &m) {
        std::cout << "Echo Callback: " << m.body << std::endl;
    }
};
