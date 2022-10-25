#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <state.h>
#include <render.h>
#include "../shared/messages.h"
#include "arguments.h"

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

            system(RUN_TEST);
        
            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_UNIT) {

            int status;
            pid_t pid = fork();

            if(pid == 0) {                
                chdir(BUILD_DIR);
                execlp(MAKE, MAKE, UNIT_TEST, FINAL_ARG);
            }
            else {
                wait(&status);
            }
        
            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_COV) {

            int status;
            pid_t pid = fork();

            if(pid == 0) {                
                chdir(BUILD_DIR);
                execlp(MAKE, MAKE, CODE_COV, FINAL_ARG);
            }
            else {
                wait(&status);
            }
        
            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_RENDER) {
            State state;
            state.init();

            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);
            
            Scene scene(0);
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