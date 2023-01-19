#include "ServiceException.h"

namespace server {

    ServiceException::ServiceException(HttpStatus status, std::string msg) {

        this->msg = msg;
        this->httpStatus = status;

    }

    const char* ServiceException::what() {

        return msg.c_str();
    
    }

    HttpStatus ServiceException::status() {

        return httpStatus;

    }


}