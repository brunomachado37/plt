#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <state.h>
#include <render.h>
#include <engine.h>
#include "../shared/messages.h"
#include "arguments.h"

using namespace std;
using namespace state;
using namespace render;
using namespace engine;

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
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            scene.displayDemo(state, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_ENGINE) {
            Engine engine;
            engine.init();
            Tile tile("market", {-1, -1});
            Leader leader("trader", {-1, -1}, 0);
            engine.play(leader, {8, 5});
            engine.play(tile, {9, 4});
            

            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            scene.display(engine.getState(), window);

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