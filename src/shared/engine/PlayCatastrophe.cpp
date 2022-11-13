#include "PlayCatastrophe.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayCatastrophe::PlayCatastrophe(state::Position position, int playerID) : Action(ACTION_ID_CATASTRO, playerID) {

        this->position = position;

    }

    void PlayCatastrophe::execute(state::State& state) {

        // Add catastrophe to the Board
        state::Board board = state.getBoard();

        try {
            board.addCatastropheToTheBoard(this->position);
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Remove catastrophe from player's hand
        state::Player player = state.getPlayers()[state.getActivePlayerID()];
        
        try {
            player.useCatastropheTile();
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }

}