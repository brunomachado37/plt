#include <stdexcept>
#include "PlayBuildMonument.h"
#include "../constants.h"
#include "../state.h"
#include "../messages.h"

namespace engine {

    PlayBuildMonument::PlayBuildMonument() {}

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

    Json::Value PlayBuildMonument::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;
        jsonAction["build"] = this->build;
        jsonAction["color_1"] = this->monument.getColor1();
        jsonAction["color_2"] = this->monument.getColor2();
        jsonAction["position_i"] = this->position.i;
        jsonAction["position_j"] = this->position.j;

        return jsonAction;

    }

    void PlayBuildMonument::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();
        this->build = jsonAction["build"].asBool();

        state::Position pos;
        pos.i = jsonAction["position_i"].asInt();
        pos.j = jsonAction["position_j"].asInt();
        this->position = pos;

        std::string color1 = jsonAction["color_1"].asString();
        std::string color2 = jsonAction["color_2"].asString();

        state::Monument monument({NOT_IN_MAP_ID, NOT_IN_MAP_ID}, color1, color2);

        this->monument = monument;

    }

}