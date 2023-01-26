#include <stdexcept>
#include "PlayTile.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

namespace engine {

    PlayTile::PlayTile() {}

    PlayTile::PlayTile(state::Tile tile, state::Position position, int playerID) : Action(ACTION_ID_TILE, playerID) {

        this->tile = tile;
        this->position = position;

    }

    void PlayTile::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() != this->playerID) {
            throw state::StateException(NOT_ACTIVE_PLAYER_MSG);
        }

        state::Board board = state.getBoard();

        // Add tile to the Board
        try {
            board.addTileToTheBoard(tile, position);
        }
        catch(state::StateException& e) {
            throw;
        }

        state::Player player = state.getPlayers()[this->playerID];
        
        // Remove tile from player's hand
        try {
            player.removeTileFromHand(tile.getType());
        }
        catch(state::StateException& e) {
            throw;
        }

        // Check if any player will receive victory points
        int regionID = board.getRegionMap()[position.i][position.j];

        // If region not at war
        if(!board.getRegions()[regionID].getIsAtWar()) {
            // If 2 kingdoms were not united
            if(board.getRegions()[regionID].getUnificationTilePosition().i == NO_UNIFICATION_POS && board.getRegions()[regionID].getUnificationTilePosition().j == NO_UNIFICATION_POS) {

                // Leader to tile map
                std::unordered_map<std::string, std::string> leaderToTileMap = {{FARMER, FARM}, {PRIEST, TEMPLE}, {TRADER, MARKET}, {KING, SETTLEMENT}};
                std::unordered_map<std::string, std::string> typeToColorMap = {{FARM, BLUE}, {TEMPLE, RED}, {MARKET, GREEN}, {SETTLEMENT, BLACK}};

                // Point not delivered flag
                bool notDelivered = true;

                // Distribute victory points
                for(int i = 0; i < (int)board.getRegions()[regionID].getLeaders().size(); i++) {
                    if(leaderToTileMap[board.getRegions()[regionID].getLeaders()[i].getType()] == tile.getType()) {
                        if(board.getRegions()[regionID].getLeaders()[i].getPlayerID() == this->playerID) {
                            player.addVictoryPoints(typeToColorMap[tile.getType()], 1);
                            notDelivered = false;

                            break;
                        }
                        else {
                            state::Player player_leader = state.getPlayers()[board.getRegions()[regionID].getLeaders()[i].getPlayerID()];
                            player_leader.addVictoryPoints(typeToColorMap[tile.getType()], 1);
                            state.setPlayer(player_leader);
                            notDelivered = false;

                            break;
                        }
                    }
                }
                // If no point was delivered, looks for a king
                if(notDelivered) {

                    for(int i = 0; i < (int)board.getRegions()[regionID].getLeaders().size(); i++) {
                        if(board.getRegions()[regionID].getLeaders()[i].getType() == KING) {
                            if(board.getRegions()[regionID].getLeaders()[i].getPlayerID() == this->playerID) {
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
            }
            else {
                state::Region region = board.getRegions()[regionID];
                region.setUnificationTilePosition({NO_UNIFICATION_POS, NO_UNIFICATION_POS});
                board.setRegion(region);
            }
        }

        // Transfer actions to the state, if everything goes well
        state.setBoard(board);
        state.setPlayer(player);

    }

    Json::Value PlayTile::serialize() {

        Json::Value jsonAction;

        jsonAction["actionID"] = this->actionID;
        jsonAction["playerID"] = this->playerID;
        jsonAction["tile_type"] = this->tile.getType();
        jsonAction["position_i"] = this->position.i;
        jsonAction["position_j"] = this->position.j;

        return jsonAction;

    }

    void PlayTile::deserialize(Json::Value jsonAction) {

        this->actionID = jsonAction["actionID"].asInt();
        this->playerID = jsonAction["playerID"].asInt();

        state::Position pos;
        pos.i = jsonAction["position_i"].asInt();
        pos.j = jsonAction["position_j"].asInt();

        this->position = pos;

        state::Tile tile(jsonAction["tile_type"].asString(), pos);
        this->tile = tile;

    }

    state::Position PlayTile::getPosition() {

        return this->position;

    }

}