#include <stdexcept>
#include "PlayLeader.h"
#include "../state.h"
#include "../messages.h"
#include "../constants.h"

namespace engine {

    PlayLeader::PlayLeader() {}

    PlayLeader::PlayLeader(state::Leader leader, state::Position position, int playerID) : Action(ACTION_ID_LEADER, playerID) {

        this->leader = leader;
        this->position = position;

    }

    void PlayLeader::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw state::StateException(NOT_ACTIVE_PLAYER_MSG);
        }

        // Add leader to the board
        state::Board board = state.getBoard();

        try {
            board.addLeaderToTheBoard(leader, position);
        }
        catch(state::StateException& e) {
            throw;
        }

        // Remove leader from player's hand
        state::Player player = state.getPlayers()[this->playerID];
        
        try {
            player.removeLeaderFromHand(leader.getType());
        }
        catch(state::StateException& e) {
            throw;
        }
        
        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }

    Json::Value PlayLeader::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;
        jsonAction["leader_type"] = this->leader.getType();
        jsonAction["position_i"] = this->position.i;
        jsonAction["position_j"] = this->position.j;

        return jsonAction;

    }

    void PlayLeader::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();

        state::Position pos;
        pos.i = jsonAction["position_i"].asInt();
        pos.j = jsonAction["position_j"].asInt();
        this->position = pos;

        state::Leader leader(jsonAction["leader_type"].asString(), pos, playerID);
        this->leader = leader;

    }

    state::Position PlayLeader::getPosition() {

        return this->position;

    }

}