//callback.cpp
//Implementation of derived classes of Callback class.
//Authors: Jeff Klouda and Matthew D'Alonzo

//C++ INCLUDES
#include <iostream>

//HEADER FILES
#include "ps_client/client.h"

//Callback::Callback(){}

//Callback::~Callback(){}

void EchoCallback::run(Message &m) {
    std::cout << "Echo Callback: " << m.body << std::endl;
}

void ChatCallback::run(Message &m) {
    std::cout << "[" << m.sender << "] " << m.body << std::endl;
}
