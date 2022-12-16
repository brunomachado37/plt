#include "DeepAI.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"
#include "../engine.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <chrono>


namespace ai {

    DeepAI::DeepAI(int playerID) : AI(playerID) {

        std::random_device rd;
        std::mt19937 gen(rd());

        this->gen = gen;

    }

    void DeepAI::run(engine::Engine& engine) {

        // Check if there's a defense pendent
        if(engine.getDefensePendent()) {

            this->defend(engine);

            return;
        
        }

        // If attack required
        if(engine.getAttackPendent()) {

            this->attack(engine);

            return;

        }

        // If monument required
        if(engine.getMonumentPendent()) {
            // If attack or defense is not required
            if(!(engine.getAttackPendent() || engine.getDefensePendent())) {
                this->buildMonument(engine);
            }
        }

        // Sanity checks
        // Check if I am the active player
        if(engine.getState().getActivePlayerID() != this->playerID) {
            throw std::logic_error(AI_NOT_ACTIVE_ERROR_MSG);
        }

        // Chose an action
        // Check type with less points
        std::unordered_map<std::string, int> playerPoints = engine.getState().getPlayers()[this->playerID].getVictoryPoints();
        playerPoints.erase(TREASURE);

        std::string lowerType;
        int lowerAmount = std::numeric_limits<int>::max();

        for(auto type: playerPoints) {
            if(type.second < lowerAmount) {
                lowerAmount = type.second;
                lowerType = type.first;
            }
        }

        bool not_played = true;

        // If leader not in the board, play it
        if((int)engine.getState().getPlayers()[this->playerID].getLeadersInHand().size() > 0) {
            std::unordered_map<std::string, std::string> colorToLeaderMap = {{BLUE, FARMER}, {RED, PRIEST}, {GREEN, TRADER}, {BLACK, KING}};

            for(auto leader: engine.getState().getPlayers()[this->playerID].getLeadersInHand()) {
                if(leader.second.getType() == colorToLeaderMap[lowerType]) {
                    this->playLeader(colorToLeaderMap[lowerType], engine);
                    not_played = false;
                }
            }
        }
        // If leader on the board, play a tile in his region
        if(not_played) {
            std::unordered_map<std::string, std::string> colorToTileMap = {{BLUE, FARM}, {RED, TEMPLE}, {GREEN, MARKET}, {BLACK, SETTLEMENT}};

            // Check if player has tile in hand
            for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                if(tile.getType() == colorToTileMap[lowerType]) {
                    engine::Action* action = this->bestMove(engine, colorToTileMap[lowerType], 3, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                    engine.play(action);
                    not_played = false;
                    break;
                }
            }                

        }
        // TRY SECOND SMALLEST POINT HERE

        // If notPlayed draw tiles
        // Explore each possibility
        

        // If attack required
        if(engine.getAttackPendent()) {

            this->attack(engine);

        }

        // If monument required
        if(engine.getMonumentPendent()) {
            // If attack or defense is not required
            if(!(engine.getAttackPendent() || engine.getDefensePendent())) {
                this->buildMonument(engine);
            }
        }

    }


    std::vector<engine::Action*> DeepAI::checkPossibleActions(engine::Engine& engine) {

        std::vector<engine::Action*> possibleActions;
        int activePlayerID = engine.getState().getActivePlayerID();

        // Add actions for PlayDraw
        
        // Check all different tile types in hand
        std::vector<state::Tile> tilesInHand;
        std::vector<std::string> tileTypesInHand;

        for(auto tile: engine.getState().getPlayers()[activePlayerID].getTilesInHand()) {
            if(std::count(tileTypesInHand.begin(), tileTypesInHand.end(), tile.getType()) == 0) {
                tilesInHand.push_back(tile);
                tileTypesInHand.push_back(tile.getType());
            }
        }

        // Just do this if this AI is the active player
        if(activePlayerID == this->playerID) {
            // Add one action for trading all tiles but the ones of a given type in hand
            // (As the tiles draw are random is pointless to explore all possible combinations of draw)
            for(auto type: tileTypesInHand) {
                std::vector<state::Tile> trade;

                for(auto tile: engine.getState().getPlayers()[activePlayerID].getTilesInHand()) {
                    if(tile.getType() != type) {
                        trade.push_back(tile);
                    }
                }

                engine::PlayDrawTiles* action = new engine::PlayDrawTiles(trade, activePlayerID);
                possibleActions.push_back(action);

            }

            // Add one action changing the whole hand
            engine::PlayDrawTiles* action = new engine::PlayDrawTiles(engine.getState().getPlayers()[activePlayerID].getTilesInHand(), activePlayerID);
            possibleActions.push_back(action);
        }

        // Check all possible tile actions

        // Check all possible positions
        std::vector<state::Position> free_lands;
        std::vector<state::Position> free_rivers;
        std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
        
        for(int i = 0; i < NUM_LINES; i++) {
            for(int j = 0; j < NUM_ROWS; j++) {
                if(boardMap[i][j] == RIVER) {
                    free_rivers.push_back({i, j});
                }
                if(boardMap[i][j] == LAND) {
                    free_lands.push_back({i, j});
                }                        
            }
        }


        // Combine all possible positions with all different tile types in hand to form all possible PlayTile actions
        state::Board board = engine.getState().getBoard();

        for(auto tile: tilesInHand) {
            if(tile.getType() == FARM) {
                for(auto pos: free_rivers) {
                    // Check if there's at least one region adjacent
                    int numberOfAdjacentRegions;
                    tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions(pos);

                    if(numberOfAdjacentRegions > 0) {       
                        engine::PlayTile* action = new engine::PlayTile(tile, pos, activePlayerID);
                        possibleActions.push_back(action);             
                    }
                    
                }
            }
            else {
                for(auto pos: free_lands) {
                    // Check if there's at least one region adjacent
                    int numberOfAdjacentRegions;
                    tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions(pos);

                    if(numberOfAdjacentRegions > 0) {       
                        engine::PlayTile* action = new engine::PlayTile(tile, pos, activePlayerID);
                        possibleActions.push_back(action);             
                    }
                }
            }
        }


        // Check all play leader actions

        // Check all possible positions
        std::vector<state::Position> possible_positions;
        std::vector<state::Position> posMap = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
        board = engine.getState().getBoard();

        for(auto reg: board.getRegions()) {
            for(auto tile: reg.second.getTiles()) {
                if(tile.getType() == TEMPLE) {

                    std::vector<std::string> adjPos = board.checkAdjacentPositions(tile.getPosition());
                    for(int i = 0; i < (int)adjPos.size(); i++) {
                        if(adjPos[i] == LAND) {
                            // Check if it would unite 2 or more regions
                            int numberOfAdjacentRegions;
                            tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});

                            if(numberOfAdjacentRegions == 1) {       
                                possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});             
                            }
                        }
                    }
                }
            }
        }

        // Combine all possible positions with all leaders in hand to form all possible PlayLeader actions
        for(auto leader: engine.getState().getPlayers()[activePlayerID].getLeadersInHand()) {
            for(auto pos: possible_positions) {
                engine::PlayLeader* action = new engine::PlayLeader(leader.second, pos, activePlayerID);
                possibleActions.push_back(action);
            }
        } 

        // Check which leaders aren't in player's hand
        std::vector<std::string> leadersOnTheBoard = {KING, TRADER, PRIEST, FARMER};
        
        for(auto leader: engine.getState().getPlayers()[activePlayerID].getLeadersInHand()) {
            for(int k = 0; k < (int)leadersOnTheBoard.size(); k++) {
                if(leader.first == leadersOnTheBoard[k]) {
                    leadersOnTheBoard.erase(leadersOnTheBoard.begin() + k);
                }
            }
        }

        // Combine all possible positions with all leaders in the board to form all possible PlayMovesLeader actions
        for(auto leaderType: leadersOnTheBoard) {
            int leaderRegionID = NO_REGION_ID;

            // Find leader on the board
            for(auto region: board.getRegions())
                for(auto l: region.second.getLeaders())
                    if(l.getType() == leaderType && l.getPlayerID() == activePlayerID)
                        leaderRegionID = region.first;

            if(leaderRegionID == NO_REGION_ID)
                throw std::logic_error(LEADER_NOT_IN_BOARD_MSG);

            for(auto pos: possible_positions) {
                // Get region id
                std::vector<int> adjReg;
                tie(adjReg, std::ignore) = board.checkAdjacentRegions(pos);

                // Avoid moving a leader to the same region he was on
                if(adjReg[0] != leaderRegionID) {
                    engine::PlayMoveLeader* action = new engine::PlayMoveLeader(leaderType, pos, activePlayerID);
                    possibleActions.push_back(action);
                }
            }
        } 


        // Check catastrophes that would hit something
        // Check if player has any catastrophe left
        if(engine.getState().getPlayers()[activePlayerID].getCatastropheTiles() > 0) {

            // Check all possible positions
            std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
            
            for(int i = 0; i < NUM_LINES; i++) {
                for(int j = 0; j < NUM_ROWS; j++) {
                    if(boardMap[i][j] != LEADER && boardMap[i][j] != MONUMENT && boardMap[i][j] != CATASTRO && boardMap[i][j] != LAND && boardMap[i][j] != RIVER) {
                        if(boardMap[i][j] == TEMPLE) {
                            // Check if there's a treasure there
                            int regionID = engine.getState().getBoard().getRegionMap()[i][j];
                            bool treasureNotFound = true;

                            for(auto treasure: engine.getState().getBoard().getRegions()[regionID].getTreasures()) {
                                if(treasure.getPosition().i == i || treasure.getPosition().j == j) {
                                    treasureNotFound = false;
                                    break;
                                }
                            }

                            if(treasureNotFound) {
                                engine::PlayCatastrophe* action = new engine::PlayCatastrophe({i, j}, activePlayerID);
                                possibleActions.push_back(action);
                            }

                        }
                        else{
                            engine::PlayCatastrophe* action = new engine::PlayCatastrophe({i, j}, activePlayerID);
                            possibleActions.push_back(action);
                        }
                    }                 
                }
            }
        }

        return possibleActions;

    }


    std::vector<engine::Action*> DeepAI::checkReducedPossibleActions(engine::Engine& engine) {

        std::vector<engine::Action*> possibleActions;
        int activePlayerID = engine.getState().getActivePlayerID();

        // Add actions for PlayDraw
        
        // Check all different tile types in hand
        std::vector<state::Tile> allTilesInHand = engine.getState().getPlayers()[activePlayerID].getTilesInHand();
        std::vector<state::Tile> tilesInHand;
        std::vector<std::string> tileTypesInHand;

        for(auto tile: allTilesInHand) {
            if(std::count(tileTypesInHand.begin(), tileTypesInHand.end(), tile.getType()) == 0) {
                tilesInHand.push_back(tile);
                tileTypesInHand.push_back(tile.getType());
            }
        }

        // Just do this if this AI is the active player
        if(activePlayerID == this->playerID) {
            // Add one action changing the whole hand
            engine::PlayDrawTiles* action = new engine::PlayDrawTiles(allTilesInHand, activePlayerID);
            possibleActions.push_back(action);
        }

        // Check all possible tile actions

        // Check all possible positions
        std::vector<state::Position> free_lands;
        std::vector<state::Position> free_rivers;
        std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
        
        for(int i = 0; i < NUM_LINES; i++) {
            for(int j = 0; j < NUM_ROWS; j++) {
                if(boardMap[i][j] == RIVER) {
                    free_rivers.push_back({i, j});
                }
                if(boardMap[i][j] == LAND) {
                    free_lands.push_back({i, j});
                }                        
            }
        }


        // Combine all possible positions with all different tile types in hand to form all possible PlayTile actions
        state::Board board = engine.getState().getBoard();

        for(auto tile: tilesInHand) {
            if(tile.getType() == FARM) {
                for(auto pos: free_rivers) {
                    // Check if there's at least one region adjacent
                    int numberOfAdjacentRegions;
                    tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions(pos);

                    if(numberOfAdjacentRegions > 0) {       
                        engine::PlayTile* action = new engine::PlayTile(tile, pos, activePlayerID);
                        possibleActions.push_back(action);             
                    }
                    
                }
            }
            else {
                for(auto pos: free_lands) {
                    // Check if there's at least one region adjacent
                    int numberOfAdjacentRegions;
                    tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions(pos);

                    if(numberOfAdjacentRegions > 0) {       
                        engine::PlayTile* action = new engine::PlayTile(tile, pos, activePlayerID);
                        possibleActions.push_back(action);             
                    }
                }
            }
        }


        // Check all play leader actions

        // Check all possible positions
        std::vector<state::Position> possible_positions;
        std::vector<state::Position> posMap = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
        board = engine.getState().getBoard();

        for(auto reg: board.getRegions()) {
            for(auto tile: reg.second.getTiles()) {
                if(tile.getType() == TEMPLE) {

                    std::vector<std::string> adjPos = board.checkAdjacentPositions(tile.getPosition());
                    for(int i = 0; i < (int)adjPos.size(); i++) {
                        if(adjPos[i] == LAND) {
                            // Check if it would unite 2 or more regions
                            int numberOfAdjacentRegions;
                            tie(std::ignore, numberOfAdjacentRegions) = board.checkAdjacentRegions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});

                            if(numberOfAdjacentRegions == 1) {       
                                possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});   
                                // Take one position per region
                                break;          
                            }
                        }
                    }
                }
            }
        }

        // Combine all possible positions with all leaders in hand to form all possible PlayLeader actions
        for(auto leader: engine.getState().getPlayers()[activePlayerID].getLeadersInHand()) {
            for(auto pos: possible_positions) {
                engine::PlayLeader* action = new engine::PlayLeader(leader.second, pos, activePlayerID);
                possibleActions.push_back(action);
            }
        } 

        // Shuffle array
        auto rng = std::default_random_engine {};
        std::shuffle(std::begin(possibleActions), std::end(possibleActions), rng);

        // Take a smaller vector
        std::vector<engine::Action*> reducedActions(10);
        std::copy(possibleActions.begin(), possibleActions.begin() + 10, reducedActions.begin());

        return reducedActions;

    }


    std::vector<engine::Action*> DeepAI::checkTiles(std::string type, engine::Engine& engine) {

        std::vector<engine::Action*> possibleActions;

        // Choose one of the tiles in hand
        state::Tile tile;
        for(auto t: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
            if(t.getType() == type) {
                tile = t;
                break;
            }
        }

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

        // Find correspondent leader on the board
        std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
        int regionID = NO_REGION_ID;

        for(auto reg: engine.getState().getBoard().getRegions()) {
            for(auto leader: reg.second.getLeaders()) {
                if(leader.getType() == tileToLeaderMap[type] && leader.getPlayerID() == this->playerID) {
                    regionID = reg.second.getRegionID();
                }
            }
        }

        std::vector<state::Position> target_positions;

        // If leader was found
        if(regionID != NO_REGION_ID) {
            // For all possible positions, check the ones adjacent to the leader region
            for(auto pos: possible_positions) {
                std::vector<int> adjacentRegions;
                int numberOfAdjacentRegions;

                tie(adjacentRegions, numberOfAdjacentRegions) = engine.getState().getBoard().checkAdjacentRegions(pos);

                if(numberOfAdjacentRegions < 3) {
                    for(auto id: adjacentRegions) {
                        if(id == regionID) {
                            target_positions.push_back(pos);
                            break;
                        }
                    }
                }
            }

            // Select a random target position
            for(auto pos: target_positions) {
                engine::PlayTile* action = new engine::PlayTile(tile, pos, this->playerID);
                possibleActions.push_back(action);
            }

        }

        return possibleActions;

    }


    void DeepAI::playLeader(std::string type, engine::Engine& engine) {

        // Check all possible positions
        std::vector<state::Position> possible_positions;
        std::vector<state::Position> posMap = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
        state::Board board = engine.getState().getBoard();

        for(auto reg: board.getRegions()) {
            for(auto tile: reg.second.getTiles()) {
                if(tile.getType() == TEMPLE) {
                    // Avoid revolts
                    bool revolt = false;
                    for(auto leader:  board.getRegions()[board.getRegionMap()[tile.getPosition().i][tile.getPosition().j]].getLeaders()) {
                        if(leader.getType() == type) {
                            revolt = true;
                            break;
                        }
                    }

                    if(!revolt) {
                        std::vector<std::string> adjPos = board.checkAdjacentPositions(tile.getPosition());
                        for(int i = 0; i < (int)adjPos.size(); i++) {
                            if(adjPos[i] == LAND) {
                                // Check if it would unite 2 or more regions
                                std::vector<int> adjacentRegions;
                                int numberOfAdjacentRegions;
                                tie(adjacentRegions, numberOfAdjacentRegions) = board.checkAdjacentRegions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});

                                if(numberOfAdjacentRegions < 2) {       
                                    if(type == FARMER) {
                                        // Check if it is adjacent to a river/farm
                                        std::vector<std::string> adjTypes = board.checkAdjacentPositions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});
                                        for(auto adjType: adjTypes) {
                                            if(adjType == RIVER || adjType == FARM) {
                                                possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});
                                                break;
                                            }
                                        }

                                    }
                                    else {
                                        possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});
                                    }                 
                                }
                            }
                        }
                    }
                }
            }
        }

        // Choose the best position by counting leader strength
        std::vector<int> possiblePositionsStrength;
        state::Leader leader = engine.getState().getPlayers()[this->playerID].getLeadersInHand()[type];
        state::Position dummyPos = {0, 0};

        for(auto pos: possible_positions) {
            leader.setPosition(pos);
            possiblePositionsStrength.push_back(board.checkLeaderStrength(leader, dummyPos));
        }

        int max = *std::max_element(possiblePositionsStrength.begin(), possiblePositionsStrength.end());

        // Take all positions with the same strength
        std::vector<state::Position> target_positions;

        for(int i = 0; i < (int)possible_positions.size(); i++) {
            if(possiblePositionsStrength[i] == max) {
                target_positions.push_back(possible_positions[i]);
            }
        }

        // Take one target randomly
        state::Position position_leader;

        if((int)target_positions.size() > 1) {
            std::uniform_int_distribution<> int_dis_pos(0, (int)target_positions.size() - 1);
            position_leader = target_positions[int_dis_pos(this->gen)];
        }
        else if((int)target_positions.size() == 1) {
            position_leader = target_positions[0];
        }
        else {
            std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
            position_leader = possible_positions[int_dis_pos(this->gen)];
        }


        // Play a leader from the hand
        leader = engine.getState().getPlayers()[this->playerID].getLeadersInHand()[type];

        // Create action
        engine::PlayLeader* action = new engine::PlayLeader(leader, position_leader, this->playerID);

        // Execute action
        engine.play(action);

    }

    void DeepAI::defend(engine::Engine& engine, bool explore) {

        int playerID = 1 - engine.getState().getActivePlayerID();

        // Get attack data
        std::string warType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getWarLeaderType();
        state::Position position = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPosition();
        int attackerSupporters = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getSupporters();
        int conflictType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getConflictType();

        int send = 0;

        if(!explore) {
            // Count how many supporters player has in hand
            std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
            int possibleSuporters = 0;

            if(conflictType == WAR) {
                for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                    if(warType == tileToLeaderMap[tile.getType()]) {
                        possibleSuporters++;
                    }
                }
            }
            else {
                for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
                    if(tile.getType() == TEMPLE) {
                        possibleSuporters++;
                    }
                }
            }

            // Count supporters in the region
            int supporters = 0;
            int regionID = engine.getState().getBoard().getRegionMap()[position.i][position.j];
            state::Leader defender;

            // Find position of the leader
            for(auto leader: engine.getState().getBoard().getRegions()[regionID].getLeaders()) {
                if(leader.getType() == warType && leader.getPlayerID() == this->playerID) {
                    defender = leader;
                }
            }

            if(conflictType == REVOLT) {
                std::vector<std::string> adjPos = engine.getState().getBoard().checkAdjacentPositions(defender.getPosition());
                supporters = std::count(adjPos.begin(), adjPos.end(), TEMPLE);
            }
            else {
                supporters = engine.getState().getBoard().checkLeaderStrength(defender, position);
            }

            // Decide how many supporters to send
            // If attacker win anyway
            if(attackerSupporters > supporters + possibleSuporters) {
                send = 0;
            }
            // If is possible to win
            else {
                send = attackerSupporters - supporters;
            }
        }   

        // Create action
        engine::PlayDefense* action = new engine::PlayDefense(conflictType, position, attackerSupporters, send, playerID, warType);

        // Execute it
        engine.play(action, true);

    }


    void DeepAI::attack(engine::Engine& engine, bool explore) {

        int playerID = engine.getState().getActivePlayerID();

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

            if(!explore) {
                for(auto tile: engine.getState().getPlayers()[playerID].getTilesInHand()) {
                    if(tile.getType() == TEMPLE) {
                        count++;
                    }
                }
            }

            // Create action
            engine::PlayAttack* action = new engine::PlayAttack(REVOLT, position, count, playerID, leaderType);

            // Execute it
            engine.play(action, true);

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

            // Choose a conflict type
            std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
            std::string warType;
            int possibleSupporters = -1;
            int counter = 0;

            if((int)leaderTypes.size() > 1) {
                // Take the type with more supporters
                for(int i = 0; i < (int)leaderTypes.size(); i++) {
                    for(auto tile: engine.getState().getPlayers()[playerID].getTilesInHand()) {
                        if(tileToLeaderMap[tile.getType()] == leaderTypes[i]) {
                            counter++;
                        }
                    }

                    if(counter > possibleSupporters) {
                        possibleSupporters = counter;
                        warType = leaderTypes[i];
                    }

                    counter = 0;

                }
            }
            else {
                warType = leaderTypes[0];
            }

            // Count how many supporters player has in hand
            int count = 0;

            if(!explore) {
                for(auto tile: engine.getState().getPlayers()[playerID].getTilesInHand()) {
                    if(warType == tileToLeaderMap[tile.getType()]) {
                        count++;
                    }
                }
            }

            // Create action
            engine::PlayAttack* action = new engine::PlayAttack(WAR, position, count, playerID, warType);

            // Execute it
            engine.play(action, true);

        }
        else {
            throw std::logic_error(AI_CONFLICT_RESOLUTION_ERROR_MSG);
        }
    
    }

    void DeepAI::buildMonument(engine::Engine& engine) {

        int playerID = engine.getState().getActivePlayerID();

        // Take one of the possible monument positions randomly
        std::uniform_int_distribution<> int_dis_pos(0, (int)engine.getState().getBoard().getPossibleMonuments().size() - 1);
        state::Position position = engine.getState().getBoard().getPossibleMonuments()[int_dis_pos(this->gen)];
        
        // Get type
        std::string type = engine.getState().getBoard().getBoardStateMap()[position.i][position.j];
        

        // Check if there's a monument available of the corresponding color
        std::unordered_map<std::string, std::string> typeToColorMap = {{FARM, BLUE}, {TEMPLE, RED}, {MARKET, GREEN}, {SETTLEMENT, BLACK}};
        std::vector<state::Monument> monumentList;

        for(auto monu: engine.getState().getBoard().getMonuments()) {
            if(monu.getColor1() == typeToColorMap[type] || monu.getColor2() == typeToColorMap[type]) {
                monumentList.push_back(monu);
            }
        }

        // Create empty monument
        state::Monument monument;
        bool build = true;

        // Chose monument randomly, if any
        if((int)monumentList.size() > 1) {

            std::uniform_int_distribution<> int_dis_monu(0, (int)monumentList.size() - 1);
            monument = monumentList[int_dis_monu(this->gen)];

        }
        else if((int)monumentList.size() == 1) {

            monument = monumentList[0];

        }
        else {

            build = false;

        }

        // Create action
        engine::PlayBuildMonument* action = new engine::PlayBuildMonument(build, monument, position, playerID);

        // Execute it
        engine.play(action, true);

    }


    void DeepAI::solvePendencies(engine::Engine& engine) {

        while(engine.getDefensePendent() || engine.getAttackPendent() || engine.getMonumentPendent()) {

            // If defense required
            if(engine.getDefensePendent()) {

                this->defend(engine, true);

            }

            // If attack required
            else if(engine.getAttackPendent()) {

                this->attack(engine, true);

            }

            // If monument required
            else if(engine.getMonumentPendent()) {

                this->buildMonument(engine);

            }
        
        }

    }


    int DeepAI::maxValue(engine::Engine& engine, std::string type, engine::Action* action, int depth, int alpha, int beta) {
        // Solve pendencies
        this->solvePendencies(engine);

        // Execute action
        try {
            engine.play(action, true);
        }
        catch(std::invalid_argument const&) {
            return EXPETION_FLAG;
        }

        // If the maximum search depth has been reached or if the game is over, return the value of the current state.
        if (depth == 0 || engine.getFinalScore()[0] != STD_FINAL_SCORE) return engine.evaluateState(this->playerID);

        // Initialize the maximum value to a very small number.
        int maxVal = std::numeric_limits<int>::min();

        // Generate all possible next actions
        std::vector<engine::Action*> possibleActions = this->checkTiles(type, engine);

        // Consider each possible action
        for(auto act: possibleActions) {
            // Recursively compute the value of the successor state by calling the minValue function.
            int val;
            if(engine.getState().getActionsDone() == 1)
                val = maxValue(engine, type, act, depth - 1, alpha, beta);
            else
                val = minValue(engine, type, act, depth - 1, alpha, beta);

            // Rollback for next iteration
             if(val == EXPETION_FLAG)
                val = -1;
            else {
                engine.rollback();
                while(engine.getActionsLog().back()->getActionID() == ACTION_ID_ATTACK || engine.getActionsLog().back()->getActionID() == ACTION_ID_DEFENSE || engine.getActionsLog().back()->getActionID() == ACTION_ID_BUILD_MONUM)
                    engine.rollback();
            }

            // Update the maximum value if the value of the successor state is greater than the current maximum value.
            maxVal = std::max(maxVal, val);

            // Update alpha to the maximum of alpha and the value of the successor state.
            alpha = std::max(alpha, val);

            // If beta is less than or equal to alpha, prune the search by returning the current maximum value.
            if (beta <= alpha) 
                return maxVal;
        }

        // Return the maximum value.
        return maxVal;
    }

    int DeepAI::minValue(engine::Engine& engine, std::string type, engine::Action* action, int depth, int alpha, int beta) {
        // Solve pendencies
        this->solvePendencies(engine);

        // Execute action
        try {
            engine.play(action, true);
        }
        catch(std::invalid_argument const&) {
            return EXPETION_FLAG;
        }

        // If the maximum search depth has been reached or if the game is over, return the value of the current state.
        if (depth == 0 || engine.getFinalScore()[0] != STD_FINAL_SCORE) return engine.evaluateState(this->playerID);

        // Initialize the minimum value to a very large number.
        int minVal = std::numeric_limits<int>::max();       

        // Generate all possible next actions
        std::vector<engine::Action*> possibleActions = this->checkTiles(type, engine);

        // Consider each possible action
        for(auto act: possibleActions) {
            // Recursively compute the value of the successor state by calling the maxValue function.
            int val;
            if(engine.getState().getActionsDone() == 1)
                val = minValue(engine, type, act, depth - 1, alpha, beta);
            else
                val = maxValue(engine, type, act, depth - 1, alpha, beta);

            // Rollback for next iteration
            if(val == EXPETION_FLAG)
                val = -1;
            else {
                engine.rollback();
                while(engine.getActionsLog().back()->getActionID() == ACTION_ID_ATTACK || engine.getActionsLog().back()->getActionID() == ACTION_ID_DEFENSE || engine.getActionsLog().back()->getActionID() == ACTION_ID_BUILD_MONUM)
                    engine.rollback();
            }

            // Update the minimum value if the value of the successor state is less than the current minimum value.
            minVal = std::min(minVal, val);

            // Update beta to the minimum of beta and the value of the successor state.
            beta = std::min(beta, val);

            // If beta is less than or equal to alpha, prune the search by returning the current minimum value.
            if (beta <= alpha) 
                return minVal;
        }

        // Return the minimum value.
        return minVal;
    }

    engine::Action* DeepAI::bestMove(engine::Engine& engine, std::string type, int depth, int alpha, int beta) {

        // Initialize the best move to an empty action
        int bestMove = 0;

        // Generate all possible actions
        std::vector<engine::Action*> possibleActions = this->checkTiles(type, engine);

        // Consider each possible action
        for(int i = 0; i < (int)possibleActions.size(); i++) {
            // Recursively compute the value of the successor state by calling the minValue function
            int val = maxValue(engine, type, possibleActions[i], depth - 1, alpha, beta);

            // If the value of the successor state is greater than the current maximum value, update the best move and the maximum value
            if (val > alpha) {
                alpha = val;
                bestMove = i;
            }

            // If beta is less than or equal to alpha, prune the search by returning the current best move
            if (beta <= alpha) 
                return possibleActions[bestMove];

            // Rollback for next iteration
            engine.rollback(false);
            // if(engine.getActionsLog().size() > 0)
            //     while(engine.getActionsLog().back()->getActionID() == ACTION_ID_ATTACK || engine.getActionsLog().back()->getActionID() == ACTION_ID_DEFENSE || engine.getActionsLog().back()->getActionID() == ACTION_ID_BUILD_MONUM)
            //         engine.rollback();

        }

        // Return the best move.
        return possibleActions[bestMove];

    }

}