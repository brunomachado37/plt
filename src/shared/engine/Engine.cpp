#include "Engine.h"
#include "PlayTile.h"
#include "PlayLeader.h"

#include <iostream>

namespace engine {

    Engine::Engine(int numPlayers) : numPlayers(numPlayers) {
        
        // Create empty state with given number of players
        state::State state(numPlayers);
        this->state = state;

    }

    void Engine::init() {
        // Initialize game state
        this->state.init();

    }

    void Engine::play(state::Tile tile, state::Position position) {

        // Create action object
        PlayTile action(tile, position);

        try {
            action.execute(this->state);
        }
        catch(const std::invalid_argument& e) {
            std::cout << e.what() << std::endl;
            return;            
        }

        // Check for war

        // Update game state if action was successful
        this->state.nextAction();

    }

    void Engine::play(state::Leader leader, state::Position position) {

        // Create action object
        PlayLeader action(leader, position);

        try {
            action.execute(this->state);
        }
        catch(const std::invalid_argument& e) {
            std::cout << e.what() << std::endl;
            return;            
        }

        // Check for revolts

        // Update game state if action was successful
        this->state.nextAction();

    }

    state::State Engine::getState() {

        return this->state;

    }

}