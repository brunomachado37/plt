#include "VersionService.h"

namespace server {

    VersionService::VersionService() : Service("version") {}

    HttpStatus VersionService::get(int id, Json::Value& msg) {

        msg["version"] = "v1.0";

        return HttpStatus::OK;

    }

}