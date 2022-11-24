#include "PlayCatastrophe.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

namespace engine {

    PlayCatastrophe::PlayCatastrophe(state::Position position, int playerID) : Action(ACTION_ID_CATASTRO, playerID) {

        this->position = position;

    }

    void PlayCatastrophe::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw std::invalid_argument(NOT_ACTIVE_PLAYER_MSG);
        }

        // Get board
        state::Board board = state.getBoard();

        // Create removed leaders list
        std::vector<state::Leader> removedLeaders;

        // Add catastrophe to the Board
        try {
            removedLeaders = board.addCatastropheToTheBoard(this->position);
        }
        catch(const std::invalid_argument& e) {
            throw;
        }


        // Get Player
        state::Player player = state.getPlayers()[this->playerID];

        // Remove catastrophe from player's hand
        try {
            player.useCatastropheTile();
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Transfer changes to state
        state.setPlayer(player);

        // Add removed leader to player's hand (if any)
        for(auto leader: removedLeaders) {
            // Get player
            player = state.getPlayers()[leader.getPlayerID()];

            // Return leader to player's hand 
            try {
                player.addLeaderToHand(leader);
            }
            catch(const std::invalid_argument& e) {
                throw;
            }
            
            // Set player
            state.setPlayer(player);

        }

        // Transfer actions to the state, if everything goes well
        state.setBoard(board);

    }

}