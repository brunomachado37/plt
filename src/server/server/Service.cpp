#include "Service.h"

namespace server {

    Service::Service(std::string pattern) {

        this->pattern = pattern;

    }

    HttpStatus Service::get(int id, Json::Value& msg) {

        throw ServiceException(HttpStatus::NOT_IMPLEMENTED, "Sevice Not Implemented");

    }

    HttpStatus Service::post(int id, const Json::Value& msg) {

        throw ServiceException(HttpStatus::NOT_IMPLEMENTED, "Sevice Not Implemented");

    }

    HttpStatus Service::put(const Json::Value& msgIn, Json::Value& msgOut) {

        throw ServiceException(HttpStatus::NOT_IMPLEMENTED, "Sevice Not Implemented");

    }

    HttpStatus Service::remove(int id) {

        throw ServiceException(HttpStatus::NOT_IMPLEMENTED, "Sevice Not Implemented");
    
    }

    bool Service::checkPattern(const std::string& url) {

        if(url == pattern)
            return true;

        return false;

    }

}