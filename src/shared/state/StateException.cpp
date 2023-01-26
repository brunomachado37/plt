#include "StateException.h"

namespace state {

    StateException::StateException(std::string msg) {

        this->msg = msg;

    }

    const char* StateException::what() {

        return msg.c_str();
    
    }

}