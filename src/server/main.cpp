#include <iostream>
#include <string.h>

#include <state.h>
#include <engine.h>
#include <ai.h>
#include <server.h>

#include "../shared/messages.h"
#include "../shared/arguments.h"

using namespace std;
using namespace state;
using namespace engine;
using namespace ai;
using namespace server;

#define MAX_BUFFER_SIZE 2048


#if MHD_VERSION >= 0x00097002
    #define MHD_RESULT enum MHD_Result
#else
    #define MHD_RESULT int
#endif


MHD_RESULT handler(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* upload_data, size_t* upload_data_size, void** con_cls) {

    Request *request = (Request*)*con_cls;

    if (!request) {

        request = new Request();

        if (!request)
            return MHD_NO;

        *con_cls = request;

        return MHD_YES;

    }

    if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 || strcmp(method, MHD_HTTP_METHOD_PUT) == 0) {

        if (*upload_data_size != 0) {
            request->data = upload_data;
            *upload_data_size = 0;

            return MHD_YES;
        }
    }

    HttpStatus status;
    string response;

    try {

        ServiceManager *manager = (ServiceManager*)cls;
        status = manager->queryService(response, request->data, url, method);

    }
    catch(ServiceException &e) {

        status = e.status();
        response = e.what();
        response += "\n";

    }
    catch(exception &e) {

        status = HttpStatus::SERVER_ERROR;
        response = e.what();
        response += "\n";

    }

    struct MHD_Response *mhd_response;
    mhd_response = MHD_create_response_from_buffer(response.size(), (void *)response.c_str(), MHD_RESPMEM_MUST_COPY);

    if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
        MHD_add_response_header(mhd_response, "Content-Type", "application/json; charset=utf-8");

    MHD_RESULT ret = MHD_queue_response(connection, status, mhd_response);
    MHD_destroy_response(mhd_response);

    return ret;

}

static void request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe) {

    Request *request = (Request *)*con_cls;

    if (request) {

        delete request;
        *con_cls = nullptr;

    }

}


int main(int argc, char* argv[]) {

    if(argc != 2) {

        cout << MULTIPLE_ARGS_MSG << endl;

        exit(EXIT_FAILURE);

    }

    if(string(argv[1]) == ARG_HELLO) {

        cout << INTRODUCTORY_MSG << endl;

        exit(EXIT_SUCCESS);

    }
    else if(string(argv[1]) == ARG_RECORD) {

        std::shared_ptr<HeuristicAI> ai_1 = std::make_shared<HeuristicAI>(0);
        std::shared_ptr<RandomAI> ai_2 = std::make_shared<RandomAI>(1);

        Game game(NUM_TURNS_RECORD);

        game.enableRecord();

        game.run_ai(ai_1, ai_2);

        if(game.saveRecord(RECORD_FILE)) {
            cout << RECORD_FAILURE_MSG << endl;
            exit(EXIT_FAILURE);
        }

        cout << RECORD_SUCCESS_MSG << endl;

        exit(EXIT_SUCCESS);

    }
    else if(string(argv[1]) == ARG_LOBBY) {

        Game game;
        ServiceManager serviceManager(game);

        struct MHD_Daemon* daemon;

        daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, PORT, NULL, NULL, &handler, (void*)&serviceManager, MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL, MHD_OPTION_END);

        if(daemon == NULL)
            exit(EXIT_FAILURE);

        cout << STOP_SERVER_MSG << endl;

        getchar();

        MHD_stop_daemon(daemon);

        exit(EXIT_SUCCESS);

    }
    else if(string(argv[1]) == ARG_LISTEN) {

        Game game;
        ServiceManager serviceManager(game);

        struct MHD_Daemon* daemon;

        daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, PORT, NULL, NULL, &handler, (void*)&serviceManager, MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL, MHD_OPTION_END);

        if(daemon == NULL)
            exit(EXIT_FAILURE);


        cout << WAITING_PLAYERS_MSG << endl;

        game.enableRecord();

        game.run();

        cout << endl << STOP_SERVER_MSG << endl;

        getchar();

        MHD_stop_daemon(daemon);

        exit(EXIT_SUCCESS);

    }
    else if(string(argv[1]) == ARG_HELP) {

        cout << INTRODUCTORY_MSG << endl << endl << HELP_SERVER_MSG << endl;

        exit(EXIT_SUCCESS);

    }
    else {

        cout << UNKNOW_ARG_MSG << endl;

        exit(EXIT_FAILURE);

    }

}