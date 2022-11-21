#include "PlayDrawTiles.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

namespace engine {

    PlayDrawTiles::PlayDrawTiles(std::vector<state::Tile> discard, int playerID) : Action(ACTION_ID_DRAW, playerID) {

        this->discard = discard;

    }


    void PlayDrawTiles::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw std::invalid_argument(NOT_ACTIVE_PLAYER_MSG);
        }

        // Remove tiles from player's hand
        state::Player player = state.getPlayers()[this->playerID];
        
        for(auto tile: this->discard) {
            try {
                player.removeTileFromHand(tile.getType());
            }
            catch(const std::invalid_argument& e) {
                throw;
            }
        }

        // Buy same number of discarded tiles
        for(int i = 0; i < (int)this->discard.size(); i++) {
            try {
                player.addTileToHand(state.getRandomTileType());
            }
            catch(const std::invalid_argument& e) {
                throw;
            }
        }      

        // Transfer actions to the state, if everything goes well
        state.setPlayer(player);

    }

}