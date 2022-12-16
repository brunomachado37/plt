#include <stdexcept>
#include "PlayBuildMonument.h"
#include "../constants.h"
#include "../state.h"
#include "../messages.h"

namespace engine {

    PlayBuildMonument::PlayBuildMonument(bool build, state::Monument monument, state::Position position, int playerID) : Action(ACTION_ID_BUILD_MONUM, playerID) {

        this->build = build;
        this->monument = monument;
        this->position = position;

    }

    void PlayBuildMonument::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw std::invalid_argument(NOT_ACTIVE_PLAYER_MSG);
        }

        if(this->build) {
            // Get board
            state::Board board = state.getBoard();

            // Create removed leaders list
            std::vector<state::Leader> removedLeaders;

            // Add monument to the Board
            try {
                removedLeaders = board.addMonumentToTheBoard(this->monument, this->position);
            }
            catch(const std::invalid_argument& e) {
                throw;
            }

            // Clear possible monuments list
            board.clearPossibleMonuments();

            // If any leader was removed, add it to player's hand
            for(auto leader: removedLeaders) {
                // Get player
                state::Player player = state.getPlayers()[leader.getPlayerID()];

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

        else {
            // Get board
            state::Board board = state.getBoard();

            // Clear possible monuments list
            board.clearPossibleMonuments();

            // Transfer actions to the state, if everything goes well
            state.setBoard(board);

        }

    }

}