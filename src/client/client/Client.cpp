#include "Client.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include <render.h>
#include <engine.h>

#include <thread>
#include <unistd.h>
#include <iostream>
#include <fstream>

namespace client {

    Client::Client(int playerID) {

        engine::Engine engine;

        this->playerID = playerID;
        this->engine = engine;

    }

    Client::Client(int playerID, std::shared_ptr<ai::AI> ai_1, std::shared_ptr<ai::AI> ai_2) {

        engine::Engine engine;

        this->playerID = playerID;
        this->engine = engine;
        this->ai_1 = ai_1;
        this->ai_2 = ai_2;
        
    }

    void Client::engineUpdateAI() {

        while(engine.gameNotOver()) {

            // If no defense is pending
            if(!engine.getDefensePendent()) {

                if(engine.getState().getActivePlayerID() == 0) {
                    ai_1->run(engine);
                }
                else {
                    ai_2->run(engine);
                }

            }
            else {

                // Play defense
                if(engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPlayerID() == 0) {
                    ai_2->run(engine);
                }
                else {
                    ai_1->run(engine);
                }

            }

            engine.update();

        }

    }

    void Client::engineUpdate() {

        while(engine.gameNotOver()) {

            engine.update();

        }

    }

    void Client::runAI(sf::RenderWindow& window) {

        engine.init();

        // Create thread
        std::thread threadEngine(&Client::engineUpdateAI, this);
        
        run(window);

        threadEngine.join();

    }

    void Client::runJson(std::string path, sf::RenderWindow& window) {

        engine.init();

        // Recover actions from file
        if(retrieveActions(path)) {
            std::cerr << READ_RECORD_FAILURE_MSG << std::endl;
            exit(EXIT_FAILURE);
        }

        // Create thread
        std::thread threadEngine(&Client::engineUpdate, this);

        run(window);

        threadEngine.join();

    }


    void Client::run(sf::RenderWindow& window) {

        // Create scene
        render::Scene scene(playerID);
        scene.drawState(engine.getState(), window);

        while (window.isOpen()) {

            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();

            // Check for end of game
            if(engine.gameNotOver()) {

                // Draw State
                scene.drawState(engine.getState(), window);

            }
            // If game ended
            else {

                // Draw game over screen
                scene.drawGameOver(engine.getFinalScore(), window);
                    
            }

        }

        // If game was not over when the window was closed, signal to the other thread that the game has ended
        if(engine.gameNotOver())
            engine.finishGame();

    }


    int Client::retrieveActions(std::string path) {

        std::ifstream actionsFile(path);

        if(!actionsFile)
            return ERROR_FILE;

        Json::Reader reader;
        Json::Value json_root;

        if(!reader.parse(actionsFile, json_root)) {
            std::cerr << reader.getFormattedErrorMessages() << std::endl;
            return ERROR_PARSE;
        }

        actionsFile.close();

        deserializeJson(json_root);

        return SUCCESS;

    }


    void Client::deserializeJson(Json::Value json_root) {

        engine.setRandomConditions(json_root[JSON_INITIAL]);
        engine.enablePlayingRecord(json_root[JSON_RANDOM_ARRAY]);

        for(auto jsonAction: json_root[JSON_ARRAY]) {

            int actionID = jsonAction["actionID"].asInt();

            if(actionID == ACTION_ID_TILE) {

                std::shared_ptr<engine::PlayTile> action = std::make_shared<engine::PlayTile>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_LEADER) {

                std::shared_ptr<engine::PlayLeader> action = std::make_shared<engine::PlayLeader>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_CATASTRO) {

                std::shared_ptr<engine::PlayCatastrophe> action = std::make_shared<engine::PlayCatastrophe>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_DRAW) {

                std::shared_ptr<engine::PlayDrawTiles> action = std::make_shared<engine::PlayDrawTiles>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_BUILD_MONUM) {

                std::shared_ptr<engine::PlayBuildMonument> action = std::make_shared<engine::PlayBuildMonument>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_ATTACK) {

                std::shared_ptr<engine::PlayAttack> action = std::make_shared<engine::PlayAttack>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_DEFENSE) {

                std::shared_ptr<engine::PlayDefense> action = std::make_shared<engine::PlayDefense>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
            else if(actionID == ACTION_ID_MOVE_LEADER) {

                std::shared_ptr<engine::PlayMoveLeader> action = std::make_shared<engine::PlayMoveLeader>();
                action->deserialize(jsonAction);
                engine.play(action);

            }
        
        }

    }

}