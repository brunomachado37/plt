#include <stdexcept>
#include "PlayDrawTiles.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

namespace engine {

    PlayDrawTiles::PlayDrawTiles() {}

    PlayDrawTiles::PlayDrawTiles(std::vector<state::Tile> discard, int playerID) : Action(ACTION_ID_DRAW, playerID) {

        this->discard = discard;

    }


    void PlayDrawTiles::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw state::StateException(NOT_ACTIVE_PLAYER_MSG);
        }

        // Remove tiles from player's hand
        state::Player player = state.getPlayers()[this->playerID];
        
        for(auto tile: this->discard) {
            try {
                player.removeTileFromHand(tile.getType());
            }
            catch(state::StateException& e) {
                throw;
            }
        }

        // Buy same number of discarded tiles
        for(int i = 0; i < (int)this->discard.size(); i++) {
            try {
                player.addTileToHand(state.getRandomTileType());
            }
            catch(state::StateException& e) {
                throw;
            }
        }      

        // Transfer actions to the state, if everything goes well
        state.setPlayer(player);

    }

    Json::Value PlayDrawTiles::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;

        for(int i = 0; i < (int)this->discard.size(); i++)
            jsonAction["discard"][i] = this->discard[i].getType();

        return jsonAction;

    }

    void PlayDrawTiles::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();

        for(auto jsonType: jsonAction["discard"]) {
        
            state::Tile tile(jsonType.asString(), {-1, -1});
            this->discard.push_back(tile);
        
        }

    }

}