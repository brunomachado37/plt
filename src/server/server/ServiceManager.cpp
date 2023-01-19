#include "ServiceManager.h"
#include "VersionService.h"
#include "PlayerService.h"

#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <microhttpd.h>

namespace server {

    ServiceManager::ServiceManager() {

        std::shared_ptr<Service> versionService(new VersionService());
        services.push_back(std::move(versionService));

        std::shared_ptr<Service> playerService(new PlayerService());
        services.push_back(std::move(playerService));

    }

    std::shared_ptr<Service> ServiceManager::findService(const std::string& url) {

        for(auto service: services) {
            if(service->checkPattern(url))
                return service;
        }

        return NULL;

    }

    HttpStatus ServiceManager::queryService(std::string& out, const std::string& in, const std::string& url, const std::string& method) {

        // url expected format /<service>/<id>
        std::vector<std::string> splittedString;
        boost::split(splittedString, url, boost::is_any_of("/"));

        if(splittedString.size() < 2 || splittedString.size() > 3)
            return HttpStatus::BAD_REQUEST;

        int id = -1;
        if(splittedString.size() == 3)
            id = std::stoi(splittedString[2]);

        std::shared_ptr<Service> service = findService(splittedString[1]);

        if(!service)
            throw ServiceException(HttpStatus::NOT_FOUND, "Service not found: " + splittedString[1]);


        Json::Value inJson, outJson;
        Json::Reader jsonReader;

        if(method == MHD_HTTP_METHOD_POST || method == MHD_HTTP_METHOD_PUT)
            if(!jsonReader.parse(in, inJson))
                throw ServiceException(HttpStatus::BAD_REQUEST, "Invalid data: " + jsonReader.getFormattedErrorMessages());


        if(method == MHD_HTTP_METHOD_GET) {

            HttpStatus ret = service->get(id, outJson);
            out = outJson.toStyledString();
            return ret;

        }
        else if(method == MHD_HTTP_METHOD_PUT) {

            HttpStatus ret = service->put(inJson, outJson);
            out = outJson.toStyledString();
            return ret;

        }
        else if(method == MHD_HTTP_METHOD_POST) {

            return service->post(id, inJson);

        }
        else if(method == MHD_HTTP_METHOD_DELETE) {

            return service->remove(id);

        }
        else {

            throw ServiceException(HttpStatus::BAD_REQUEST, "Invalid method: " + method);

        }      

    }

}