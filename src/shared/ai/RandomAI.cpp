#include "RandomAI.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"
#include "../engine.h"

#include <algorithm>

namespace ai {

    RandomAI::RandomAI(int playerID) : AI(playerID) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1.0);

        this->gen = gen;
        this->dis = dis;

    }

    void RandomAI::run(engine::Engine& engine) {

        // Check if there's a defense pendent
        if(engine.getDefensePendent()) {

            // Get attack data
            std::string warType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getWarLeaderType();
            state::Position position = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPosition();
            int attackerSupporters = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getSupporters();
            int conflictType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getConflictType();

            // Count how many supporters player has in hand
            std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
            int count = 0;

            if(conflictType == WAR) {
                for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                    if(warType == tileToLeaderMap[tile.getType()]) {
                        count++;
                    }
                }
            }
            else {
                for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                    if(tile.getType() == TEMPLE) {
                        count++;
                    }
                }
            }

            // Create action
            engine::PlayDefense* action = new engine::PlayDefense(conflictType, position, attackerSupporters, count, this->playerID, warType);

            // Execute action
            engine.play(action);

            return;
        
        }

        // If attack required
        if(engine.getAttackPendent()) {

            this->attack(engine);

            return;

        }

        // Sanity checks
        // Check if I am the active player
        if(engine.getState().getActivePlayerID() != this->playerID) {
            throw std::logic_error(AI_NOT_ACTIVE_ERROR_MSG);
        }
        // Check if there's a monument build pendent
        if(engine.getMonumentPendent()) {
            throw std::logic_error(AI_MONUMENT_ERROR_MSG);
        }


        // Get player
        state::Player player = engine.getState().getPlayers()[this->playerID];

        // Choose 1 of the 4 possible actions randomly
        float choice = this->dis(this->gen);

        // If play tile
        if(choice < PROB_TILE) {
            // Choose one of the tiles in hand
            std::uniform_int_distribution<> int_dis(0, (int)player.getTilesInHand().size() - 1);
            state::Tile tile = player.getTilesInHand()[int_dis(this->gen)];

            // Check all possible positions
            std::vector<state::Position> possible_positions;
            std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
            
            for(int i = 0; i < NUM_LINES; i++) {
                for(int j = 0; j < NUM_ROWS; j++) {
                    if(tile.getType() == FARM) {
                        if(boardMap[i][j] == RIVER) {
                            possible_positions.push_back({i, j});
                        }
                    }
                    else {
                        if(boardMap[i][j] == LAND) {
                            possible_positions.push_back({i, j});
                        }                        
                    }
                }
            }

            // Choose a position randomly
            std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
            state::Position pos = possible_positions[int_dis_pos(this->gen)];

            // Create action
            engine::PlayTile* action = new engine::PlayTile(tile, pos, this->playerID);

            // Execute action
            engine.play(action);

        }
        // If play a leader
        else if(choice < PROB_TILE + PROB_LEADER) {

            // If player has leaders in hands, choose between play or move a leader
            bool move;

            if((int)player.getLeadersInHand().size() == 4) {
                move = false;
            }
            else if((int)player.getLeadersInHand().size() > 0) {
                if(this->dis(this->gen) < PROB_MOVE) {
                    move = true;
                }
                else {
                    move = false;
                }
            }
            else {
                move = true;
            }

            // Check all possible positions
            std::vector<state::Position> possible_positions;
            std::vector<state::Position> posMap = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
            state::Board board = engine.getState().getBoard();

            for(auto reg: board.getRegions()) {
                for(auto tile: reg.second.getTiles()) {
                    if(tile.getType() == TEMPLE) {
                        std::vector<std::string> adjPos = board.checkAdjacentPositions(tile.getPosition());
                        for(int i = 0; i < (int)adjPos.size(); i++) {
                            if(adjPos[i] == LAND) {
                                // Check if it would unite 2 or more regions
                                std::vector<int> adjacentRegions;
                                int numberOfAdjacentRegions;
                                tie(adjacentRegions, numberOfAdjacentRegions) = board.checkAdjacentRegions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});

                                if(numberOfAdjacentRegions < 2) {
                                    possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});
                                }
                            }
                        }
                    }
                }
            }

            // Choose a position randomly
            std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
            state::Position pos = possible_positions[int_dis_pos(this->gen)];

            // Move a leader
            if(move) {

                // Check which leaders aren't in player's hand
                std::vector<std::string> leadersOnTheBoard = {KING, TRADER, PRIEST, FARMER};
                for(auto leader: player.getLeadersInHand()) {
                    for(int k = 0; k < (int)leadersOnTheBoard.size(); k++) {
                        if(leader.first == leadersOnTheBoard[k]) {
                            leadersOnTheBoard.erase(leadersOnTheBoard.begin() + k);
                        }
                    }
                }

                // Choose one of the leaders on the board
                std::uniform_int_distribution<> int_dis(0, (int)leadersOnTheBoard.size() - 1);
                std::string leaderType = leadersOnTheBoard[int_dis(this->gen)];

                // Create action
                engine::PlayMoveLeader* action = new engine::PlayMoveLeader(leaderType, pos, this->playerID);

                // Execute action
                engine.play(action);

            }
            // Play a leader from the hand
            else {

                // Choose one of the leaders in hand
                std::vector<std::string> leadersInHandKeys;
                for(auto leader: player.getLeadersInHand()) {
                    leadersInHandKeys.push_back(leader.first);
                }

                std::uniform_int_distribution<> int_dis(0, leadersInHandKeys.size() - 1);
                state::Leader leader = player.getLeadersInHand()[leadersInHandKeys[int_dis(this->gen)]];

                // Create action
                engine::PlayLeader* action = new engine::PlayLeader(leader, pos, this->playerID);

                // Execute action
                engine.play(action);

            }

        }
        // If play catastrophe
        else if(choice < PROB_TILE + PROB_LEADER + PROB_CATASTRO) {

            // Check if player has any catastrophe left
            if(player.getCatastropheTiles() > 0) {

                // Check all possible positions
                std::vector<state::Position> possible_positions;
                std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
                
                for(int i = 0; i < NUM_LINES; i++) {
                    for(int j = 0; j < NUM_ROWS; j++) {
                        if(boardMap[i][j] != LEADER && boardMap[i][j] != MONUMENT) {
                            possible_positions.push_back({i, j});
                        }                        
                    }
                }

                // Choose a position randomly
                std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
                state::Position pos = possible_positions[int_dis_pos(this->gen)];

                // Create action
                engine::PlayCatastrophe* action = new engine::PlayCatastrophe(pos, this->playerID);

                // Execute action
                engine.play(action);

            }

        }
        // Play draw tile
        else {

            // Take tiles in player hand
            std::vector<state::Tile> tiles = player.getTilesInHand();

            // Choose number of tiles to be traded
            std::uniform_int_distribution<> int_dis_pos(0, (int)tiles.size() - 1);
            int numberOfTiles = int_dis_pos(this->gen) + 1;

            // Choose which tiles will be traded
            std::vector<state::Tile> trade;
            std::vector<int> sortedNumbers;

            while((int)sortedNumbers.size() < numberOfTiles) {
                int index = int_dis_pos(this->gen);

                if(std::count(sortedNumbers.begin(), sortedNumbers.end(), index) == 0) {
                    trade.push_back(tiles[index]);
                    sortedNumbers.push_back(index);
                }
            }

            // Create action
            engine::PlayDrawTiles* action = new engine::PlayDrawTiles(trade, this->playerID);

            // Execute action
            engine.play(action);

        }
        

        // If monument required
        if(engine.getMonumentPendent()) {
            state::Monument m;
            engine::PlayBuildMonument* action_monu = new engine::PlayBuildMonument(false, m, {-1, -1}, this->playerID);
            engine.play(action_monu);
        }

        // If attack required
        if(engine.getAttackPendent()) {

            this->attack(engine);

        }

    }

    void RandomAI::attack(engine::Engine& engine) {

        // Identify the type of conflict
        int actionID = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getActionID();
        int typeOfConflict;

        if(actionID == ACTION_ID_DEFENSE) {
            typeOfConflict = engine.getActionsLog()[engine.getActionsLog().size() - 2]->getConflictType();
        }
        else {
            if(actionID == ACTION_ID_LEADER || actionID == ACTION_ID_MOVE_LEADER) {
                typeOfConflict = REVOLT;
            }
            else if(actionID == ACTION_ID_TILE) {
                typeOfConflict = WAR;
            }
            else {
                throw std::logic_error(AI_CONFLICT_RESOLUTION_ERROR_MSG);
            }
        }

        // Check if it is a revolt
        if(typeOfConflict == REVOLT) {

            // Get position
            state::Position position = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPosition();

            // Get regionID
            int regionID = engine.getState().getBoard().getRegionMap()[position.i][position.j];

            // Sanity checks
            if(!engine.getState().getBoard().getRegions()[regionID].getIsInRevolt() || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] != LEADER) {
                throw std::logic_error(AI_CONFLICT_RESOLUTION_ERROR_MSG);
            }

            // Check leader type
            std::string leaderType;

            for(auto leader: engine.getState().getBoard().getRegions()[regionID].getLeaders()) {
                if(leader.getPosition() == position) {
                    leaderType = leader.getType();
                    break;
                }
            }

            // Count how many supporters player has in hand
            int count = 0;

            for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                if(tile.getType() == TEMPLE) {
                    count++;
                }
            }

            // Create action
            engine::PlayAttack* action = new engine::PlayAttack(REVOLT, position, count, this->playerID, leaderType);

            // Execute action
            engine.play(action);

        }
        // Check if it is a war
        else if(typeOfConflict == WAR) {

            // Get position
            state::Position position = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPosition();

            // Get regionID
            int regionID = engine.getState().getBoard().getRegionMap()[position.i][position.j];

            // Sanity checks
            if(!engine.getState().getBoard().getRegions()[regionID].getIsAtWar() || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] == LEADER || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] == CATASTRO || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] == MONUMENT || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] == LAND || engine.getState().getBoard().getBoardStateMap()[position.i][position.j] == RIVER) {
                throw std::logic_error(AI_CONFLICT_RESOLUTION_ERROR_MSG);
            }

            // Check leader type
            std::vector<std::string> temp;
            std::vector<std::string> leaderTypes;

            for(auto leader: engine.getState().getBoard().getRegions()[regionID].getLeaders()) {
                temp.push_back(leader.getType());
            }

            for(auto type: temp) {
                if(std::count(temp.begin(), temp.end(), type) > 1) {
                    if(std::count(leaderTypes.begin(), leaderTypes.end(), type) == 0) {
                        leaderTypes.push_back(type);
                    }
                }
            }

            // Choose a conflict randomly
            std::string warType;

            if((int)leaderTypes.size() > 1) {
                std::uniform_int_distribution<> int_dis(0, leaderTypes.size() - 1);
                warType = leaderTypes[int_dis(this->gen)];
            }
            else {
                warType = leaderTypes[0];
            }

            // Count how many supporters player has in hand
            std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
            int count = 0;

            for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                if(warType == tileToLeaderMap[tile.getType()]) {
                    count++;
                }
            }

            // Create action
            engine::PlayAttack* action = new engine::PlayAttack(WAR, position, count, this->playerID, warType);

            // Execute action
            engine.play(action);

        }
        else {
            throw std::logic_error(AI_CONFLICT_RESOLUTION_ERROR_MSG);
        }
    
    }

}