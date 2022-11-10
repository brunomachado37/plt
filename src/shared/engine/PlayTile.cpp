#include "PlayTile.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayTile::PlayTile(state::Tile tile, state::Position position) : Action(ACTION_ID_TILE) {

        this->tile = tile;
        this->position = position;

    }

    void PlayTile::execute(state::State& state) {

        // Add tile to the Board
        state::Board board = state.getBoard();

        try {
            board.addTileToTheBoard(tile, position);
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Remove tile from player's hand
        state::Player player = state.getPlayers()[state.getActivePlayerID()];
        
        try {
            player.removeTileFromHand(tile.getType());
        }
        catch(const std::invalid_argument& e) {
            throw;
        }

        // Check if any player will receive victory points
        int regionID = board.getRegionMap()[position.i][position.j];

        // If region not at war
        if(!board.getRegions()[regionID].getIsAtWar()) {
            // Leader to tile map
            std::unordered_map<std::string, std::string> leaderToTileMap = {{FARMER, FARM}, {PRIEST, TEMPLE}, {TRADER, MARKET}, {KING, SETTLEMENT}};
            std::unordered_map<std::string, std::string> typeToColorMap = {{FARM, BLUE}, {TEMPLE, RED}, {MARKET, GREEN}, {SETTLEMENT, BLACK}};

            for(int i = 0; i < (int)board.getRegions()[regionID].getLeaders().size(); i++) {
                if(leaderToTileMap[board.getRegions()[regionID].getLeaders()[i].getType()] == tile.getType()) {
                    if(board.getRegions()[regionID].getLeaders()[i].getPlayerID() == state.getActivePlayerID()) {
                        player.addVictoryPoints(typeToColorMap[tile.getType()], 1);

                        break;
                    }
                    else {
                        state::Player player_leader = state.getPlayers()[board.getRegions()[regionID].getLeaders()[i].getPlayerID()];
                        player_leader.addVictoryPoints(typeToColorMap[tile.getType()], 1);
                        state.setPlayer(player_leader);

                        break;
                    }
                }
            }
        }

        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }

}