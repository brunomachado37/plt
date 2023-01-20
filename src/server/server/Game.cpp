#include "Game.h"
#include "../shared/messages.h"

#include <fstream> 
#include <iostream>
#include <unistd.h>

namespace server {

    Game::Game(int maxNumTurns, int numPlayers) : maxNumTurns(maxNumTurns), numPlayers(numPlayers) {

        engine::Engine engine;
        this->engine = engine;

        for(int i = 0; i < numPlayers; i++)
            this->players.push_back(Player());

    }

    void Game::enableRecord() {

        engine.enableRecord();

    }

    int Game::saveRecord(std::string path) {

        std::ofstream outFile(path, std::ios::trunc);

        // Error opening file
        if(!outFile)
            return ERROR;

        outFile << this->engine.getRecord();

        outFile.close();

        return SUCCESS;

    }

    void Game::run_ai(std::shared_ptr<ai::AI> ai_1, std::shared_ptr<ai::AI> ai_2) {

        engine.init();

        // Check for end of game
        while(engine.gameNotOver() && engine.turnNotReached(this->maxNumTurns)) {

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

            // Update Engine
            engine.update();

        }

    }

    void Game::run() {

        engine.init();

        // Wait until all players join
        while(status == WAITING) {
            sleep(1);
        }

        std::cout << START_MSG << std::endl;

        // Check for end of game
        while(engine.gameNotOver()) {

            // Update Engine
            engine.update();

        }

    }

    Json::Value Game::getAllPlayersInfo() {

        Json::Value info(Json::arrayValue);

        for(int id = 0; id < (int)players.size(); id++)
            info.append(getPlayerInfo(id));

        return info;

    }

    Json::Value Game::getPlayerInfo(int id) {

        Json::Value info;

        if(id < 0 || id >= (int)players.size())
            return info;

        if(!players[id].free) {

            info["name"] = players[id].name;

        }

        return info;

    }

    int Game::addPlayer(std::string name) {

        for(int id = 0; id < (int)players.size(); id++) {

            if(players[id].free) {

                players[id].name = name;
                players[id].free = false;

                if(id == ((int)players.size() - 1))
                    status = RUNNING;

                return id;

            }

        }

    	return ERROR;

    }

    bool Game::checkPlayerID(int id) {

        if(id < 0 || id >= (int)players.size())
            return false;

        if(players[id].free)
            return false;

        return true;

    }

    void Game::changePlayerName(int id, std::string name) {

        players[id].name = name;

    }

    void Game::removePlayer(int id) {

        players[id].name = "";
        players[id].free = true;

    }

    Json::Value Game::getSerializedInitialState() {

        return engine.getRecord()["InitialState"];

    }

    engine::Engine& Game::getEngine() {

        return engine;

    }

}