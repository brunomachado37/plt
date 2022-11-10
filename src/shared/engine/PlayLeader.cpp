#include "PlayLeader.h"
#include "../state.h"
#include "../constants.h"

namespace engine {

    PlayLeader::PlayLeader(state::Leader leader, state::Position position) : Action(ACTION_ID_LEADER) {

        this->leader = leader;
        this->position = position;

    }

    void PlayLeader::execute(state::State& state) {

        // Add leader to the board
        state::Board board = state.getBoard();

        try {
            board.addLeaderToTheBoard(leader, position);
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Remove leader from player's hand
        state::Player player = state.getPlayers()[state.getActivePlayerID()];
        
        try {
            player.removeLeaderFromHand(leader.getType());
        }
        catch(const std::invalid_argument& e) {
            throw;
        }
        
        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }


}