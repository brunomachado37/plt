#include "Request.h"

namespace server {
    
    Request::~Request() {

        if (pp)
            MHD_destroy_post_processor(pp);

    }

}