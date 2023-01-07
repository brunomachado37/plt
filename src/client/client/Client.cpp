#include "Client.h"
#include "../shared/constants.h"
#include "../render.h"

#include <thread>
#include <unistd.h>

namespace client {

    Client::Client(int playerID, std::shared_ptr<ai::AI> ai_1, std::shared_ptr<ai::AI> ai_2) {

        engine::Engine engine;

        this->playerID = playerID;
        this->engine = engine;
        this->ai_1 = ai_1;
        this->ai_2 = ai_2;
        
    }

    void Client::engineUpdate() {

        while(engine.gameNotOver()) {

            engine.update();

        }

    }

    void Client::run(sf::RenderWindow& window) {

        // Create scene
        render::Scene scene(playerID);

        // Draw initial state
        engine.init();
        scene.drawState(engine.getState(), window);

        // Create thread
        std::thread threadEngine(&Client::engineUpdate, this);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();
            sleep(1);

            // Check for end of game
            if(engine.gameNotOver()) {

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

        threadEngine.join();

    }

}