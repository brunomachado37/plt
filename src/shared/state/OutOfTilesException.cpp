#include "OutOfTilesException.h"

namespace state {

    OutOfTilesException::OutOfTilesException(std::string msg) {

        this->msg = msg;

    }

    const char* OutOfTilesException::what() {

        return msg.c_str();
    
    }

}