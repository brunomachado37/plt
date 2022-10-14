#include <iostream>
#include <cstdlib>

#include <state.h>
#include <render.h>
#include "../shared/messages.h"

using namespace std;
using namespace state;
using namespace render;

int main(int argc, char* argv[]) {

    if(argc == 2) {
        if(string(argv[1]) == ARG_HELLO) {
            cout << INTRODUCTORY_MSG << endl;

            return EXIT_SUCCESS;
        }
        else if(string(argv[1]) == ARG_STATE) {
            State testState;
            
            return EXIT_SUCCESS;
        }
        else if(string(argv[1]) == ARG_RENDER) {
            State state;

            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);
            Scene scene(window);
            scene.drawState(state, window);
            scene.display(window);
            
            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_HELP) {
            cout << INTRODUCTORY_MSG << endl << endl << HELP_MSG << endl;

            return EXIT_SUCCESS;
        }
        else {
            cout << UNKNOW_ARG_MSG << endl;

            return EXIT_FAILURE;
        }
    }

    cout << MULTIPLE_ARGS_MSG << endl;

    return EXIT_FAILURE;

}