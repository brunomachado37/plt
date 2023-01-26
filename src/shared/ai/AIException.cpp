#include "AIException.h"

namespace ai {

    AIException::AIException(std::string msg) {

        this->msg = msg;

    }

    const char* AIException::what() {

        return msg.c_str();
    
    }

}