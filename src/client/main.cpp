#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <state.h>
#include <render.h>
#include <engine.h>
#include <ai.h>
#include <client.h>
#include "../shared/messages.h"
#include "../shared/arguments.h"

using namespace std;
using namespace state;
using namespace render;
using namespace engine;
using namespace ai;
using namespace client;

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
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            scene.displayDemoEngine(engine, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_RANDOM_AI) {
            Engine engine;
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            RandomAI ai_1(0);
            RandomAI ai_2(1);

            scene.run(engine, &ai_1, &ai_2, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_HEURISTIC_AI) {
            Engine engine;
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            HeuristicAI ai_1(0);
            RandomAI ai_2(1);

            scene.run(engine, &ai_1, &ai_2, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_HEURISTIC_AI_2) {
            Engine engine;
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            HeuristicAI ai_1(0);
            HeuristicAI ai_2(1);

            scene.run(engine, &ai_1, &ai_2, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_ROLLBACK) {
            Engine engine;
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            HeuristicAI ai_1(0);
            HeuristicAI ai_2(1);

            scene.runRollback(engine, &ai_1, &ai_2, ROLLBACK_TURNS, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_DEEP_AI) {
            Engine engine;
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            
            Scene scene(0);

            DeepAI ai_1(0);
            HeuristicAI ai_2(1);

            scene.run(engine, &ai_1, &ai_2, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_CONTEST_HEURISTIC) {
            Scene scene(0);
                        
            HeuristicAI ai_1(0);
            RandomAI ai_2(1);

            scene.contest(&ai_1, &ai_2);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_CONTEST_DEEP) {
            Scene scene(0);
                        
            DeepAI ai_1(0);
            HeuristicAI ai_2(1);

            scene.contest(&ai_1, &ai_2);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_THREAD) {
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            

            std::shared_ptr<HeuristicAI> ai_1 = std::make_shared<HeuristicAI>(0);
            std::shared_ptr<RandomAI> ai_2 = std::make_shared<RandomAI>(1);

            Client client(0, ai_1, ai_2);

            client.runAI(window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_PLAY) {
            
            sf::RenderWindow window(sf::VideoMode(1900, 1024), GAME_NAME);            

            Client client(0);

            client.runJson(RECORD_FILE, window);

            return EXIT_SUCCESS;

        }
        else if(string(argv[1]) == ARG_HELP) {
            cout << INTRODUCTORY_MSG << endl << endl << HELP_CLIENT_MSG << endl;

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