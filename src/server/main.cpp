#include <iostream>

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

        game.run(ai_1, ai_2);

        if(game.saveRecord(RECORD_FILE)) {
            cout << RECORD_FAILURE_MSG << endl;
            exit(EXIT_FAILURE);
        }

        cout << RECORD_SUCCESS_MSG << endl;

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