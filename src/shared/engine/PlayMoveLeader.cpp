#include <stdexcept>
#include "PlayMoveLeader.h"
#include "../state.h"
#include "../messages.h"
#include "../constants.h"

namespace engine {

    PlayMoveLeader::PlayMoveLeader() {}

    PlayMoveLeader::PlayMoveLeader(std::string leaderType, state::Position position, int playerID) : Action(ACTION_ID_MOVE_LEADER, playerID) {

        this->leaderType = leaderType;
        this->position = position;

    }

    void PlayMoveLeader::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw state::StateException(NOT_ACTIVE_PLAYER_MSG);
        }

        // Get board and player
        state::Board board = state.getBoard();
        state::Player player = state.getPlayers()[this->playerID];
        state::Leader leader;

        // Find leader
        for(auto reg: board.getRegions()) {
            for(auto lead: reg.second.getLeaders()) {
                if(lead.getType() == this->leaderType && lead.getPlayerID() == this->playerID) {
                    leader = lead;
                }
            }
        }

        // Check if leader was found
        if(leader.getType() != this->leaderType || leader.getPlayerID() != this->playerID) {
            throw state::StateException(LEADER_NOT_IN_BOARD_MSG);
        }

        // Remove leader from the board
        try {
            board.removeLeaderFromTheBoard(this->playerID, this->leaderType);
        }
        catch(state::StateException& e) {
            throw;
        }

        // Move leader
        if(this->position.i == -1 && this->position.j == -1) {

            // Return leader to player's hand 
            try {
                player.addLeaderToHand(leader);
            }
            catch(state::StateException& e) {
                throw;
            }

        }
        else {

            // Add leader to the board in the new position
            try {
                board.addLeaderToTheBoard(leader, this->position);
            }
            catch(state::StateException& e) {
                throw;
            }

        }
        
        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }

    Json::Value PlayMoveLeader::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;
        jsonAction["leaderType"] = this->leaderType;
        jsonAction["position_i"] = this->position.i;
        jsonAction["position_j"] = this->position.j;

        return jsonAction;

    }

    void PlayMoveLeader::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();
        this->leaderType = jsonAction["leaderType"].asString();

        state::Position pos;
        pos.i = jsonAction["position_i"].asInt();
        pos.j = jsonAction["position_j"].asInt();

        this->position = pos;

    }

    state::Position PlayMoveLeader::getPosition() {

        return this->position;

    }

}