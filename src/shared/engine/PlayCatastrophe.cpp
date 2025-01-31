#include <stdexcept>
#include "PlayCatastrophe.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

namespace engine {

    PlayCatastrophe::PlayCatastrophe() {}

    PlayCatastrophe::PlayCatastrophe(state::Position position, int playerID) : Action(ACTION_ID_CATASTRO, playerID) {

        this->position = position;

    }

    void PlayCatastrophe::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw state::StateException(NOT_ACTIVE_PLAYER_MSG);
        }

        // Get board
        state::Board board = state.getBoard();

        // Create removed leaders list
        std::vector<state::Leader> removedLeaders;

        // Add catastrophe to the Board
        try {
            removedLeaders = board.addCatastropheToTheBoard(this->position);
        }
        catch(state::StateException& e) {
            throw;
        }


        // Get Player
        state::Player player = state.getPlayers()[this->playerID];

        // Remove catastrophe from player's hand
        try {
            player.useCatastropheTile();
        }
        catch(state::StateException& e) {
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
            catch(state::StateException& e) {
                throw;
            }
            
            // Set player
            state.setPlayer(player);

        }

        // Transfer actions to the state, if everything goes well
        state.setBoard(board);

    }

    Json::Value PlayCatastrophe::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;
        jsonAction["position_i"] = this->position.i;
        jsonAction["position_j"] = this->position.j;

        return jsonAction;

    }

    void PlayCatastrophe::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();

        state::Position pos;
        pos.i = jsonAction["position_i"].asInt();
        pos.j = jsonAction["position_j"].asInt();

        this->position = pos;

    }

}