#include "Game.h"

#include <fstream> 

namespace server {

    Game::Game(int maxNumTurns) : maxNumTurns(maxNumTurns) {

        engine::Engine engine;

        this->engine = engine;

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

    void Game::run(std::shared_ptr<ai::AI> ai_1, std::shared_ptr<ai::AI> ai_2) {

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

}