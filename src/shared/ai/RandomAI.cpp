#include "RandomAI.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"
#include "../engine.h"

#include <algorithm>
#include <stdexcept>
#include <memory> 

namespace ai {

    RandomAI::RandomAI(int playerID) : AI(playerID, RANDOM_AI_TYPE) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1.0);

        this->gen = gen;
        this->dis = dis;

    }

    void RandomAI::run(engine::Engine& engine) {

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

                return;

            }
        }


        // Sanity checks
        // Check if I am the active player
        if(engine.getState().getActivePlayerID() != this->playerID) {
            throw AIException(AI_NOT_ACTIVE_ERROR_MSG);
        }


        // Choose 1 of the 4 possible actions randomly
        float choice = this->dis(this->gen);

        // If play tile
        if(choice < PROB_TILE) {
            
            this->playTile(engine);

        }
        // If play a leader
        else if(choice < PROB_TILE + PROB_LEADER) {

            this->playLeader(engine);

        }
        // If play catastrophe
        else if(choice < PROB_TILE + PROB_LEADER + PROB_CATASTRO) {

            this->playCatastrophe(engine);

        }
        // Play draw tile
        else {

            this->draw(engine);

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
                throw AIException(AI_CONFLICT_RESOLUTION_ERROR_MSG);
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
                throw AIException(AI_CONFLICT_RESOLUTION_ERROR_MSG);
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
            std::shared_ptr<engine::PlayAttack> action = std::make_shared<engine::PlayAttack>(REVOLT, position, count, this->playerID, leaderType);

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
                throw AIException(AI_CONFLICT_RESOLUTION_ERROR_MSG);
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
            std::shared_ptr<engine::PlayAttack> action = std::make_shared<engine::PlayAttack>(WAR, position, count, this->playerID, warType);

            // Execute action
            engine.play(action);

        }
        else {
            throw AIException(AI_CONFLICT_RESOLUTION_ERROR_MSG);
        }
    
    }

    void RandomAI::defend(engine::Engine& engine) {

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
        std::shared_ptr<engine::PlayDefense> action = std::make_shared<engine::PlayDefense>(conflictType, position, attackerSupporters, count, this->playerID, warType);
        

        // Execute action
        engine.play(action);

    }

    void RandomAI::playTile(engine::Engine& engine) {

        // Choose one of the tiles in hand
        std::uniform_int_distribution<> int_dis(0, (int)engine.getState().getPlayers()[this->playerID].getTilesInHand().size() - 1);
        state::Tile tile = engine.getState().getPlayers()[this->playerID].getTilesInHand()[int_dis(this->gen)];

        // Check all possible positions
        std::vector<state::Position> possible_positions;
        std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
        
        for(int i = 0; i < state::NUM_LINES; i++) {
            for(int j = 0; j < state::NUM_ROWS; j++) {
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
        std::shared_ptr<engine::PlayTile> action = std::make_shared<engine::PlayTile>(tile, pos, this->playerID);

        // Execute action
        engine.play(action);

    }

    void RandomAI::playLeader(engine::Engine& engine) {

        // If player has leaders in hands, choose between play or move a leader
        bool move;

        if((int)engine.getState().getPlayers()[this->playerID].getLeadersInHand().size() == 4) {
            move = false;
        }
        else if((int)engine.getState().getPlayers()[this->playerID].getLeadersInHand().size() > 0) {
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
                    std::vector<std::string> adjPos = board.retrieveAdjacentPositions(tile.getPosition());
                    for(int i = 0; i < (int)adjPos.size(); i++) {
                        if(adjPos[i] == LAND) {
                            // Check if it would unite 2 or more regions
                            std::vector<int> adjacentRegions;
                            int numberOfAdjacentRegions;
                            tie(adjacentRegions, numberOfAdjacentRegions) = board.retrieveAdjacentRegions({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});

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
            for(auto leader: engine.getState().getPlayers()[this->playerID].getLeadersInHand()) {
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
            std::shared_ptr<engine::PlayMoveLeader> action = std::make_shared<engine::PlayMoveLeader>(leaderType, pos, this->playerID);

            // Execute action
            engine.play(action);

        }
        // Play a leader from the hand
        else {

            // Choose one of the leaders in hand
            std::vector<std::string> leadersInHandKeys;
            for(auto leader: engine.getState().getPlayers()[this->playerID].getLeadersInHand()) {
                leadersInHandKeys.push_back(leader.first);
            }

            std::uniform_int_distribution<> int_dis(0, leadersInHandKeys.size() - 1);
            state::Leader leader = engine.getState().getPlayers()[this->playerID].getLeadersInHand()[leadersInHandKeys[int_dis(this->gen)]];

            // Create action
            std::shared_ptr<engine::PlayLeader> action = std::make_shared<engine::PlayLeader>(leader, pos, this->playerID);

            // Execute action
            engine.play(action);

        }

    }

    void RandomAI::playCatastrophe(engine::Engine& engine) {

        // Check if player has any catastrophe left
        if(engine.getState().getPlayers()[this->playerID].getCatastropheTiles() > 0) {

            // Check all possible positions
            std::vector<state::Position> possible_positions;
            std::vector<std::vector<std::string>> boardMap = engine.getState().getBoard().getBoardStateMap();
            
            for(int i = 0; i < state::NUM_LINES; i++) {
                for(int j = 0; j < state::NUM_ROWS; j++) {
                    if(boardMap[i][j] != LEADER && boardMap[i][j] != MONUMENT) {
                        possible_positions.push_back({i, j});
                    }                        
                }
            }

            // Choose a position randomly
            std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
            state::Position pos = possible_positions[int_dis_pos(this->gen)];

            // Create action
            std::shared_ptr<engine::PlayCatastrophe> action = std::make_shared<engine::PlayCatastrophe>(pos, this->playerID);

            // Execute action
            engine.play(action);

        }

    }

    void RandomAI::draw(engine::Engine& engine) {

        // Take tiles in player hand
        std::vector<state::Tile> tiles = engine.getState().getPlayers()[this->playerID].getTilesInHand();

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
        std::shared_ptr<engine::PlayDrawTiles> action = std::make_shared<engine::PlayDrawTiles>(trade, this->playerID);

        // Execute action
        engine.play(action);

    }

    void RandomAI::buildMonument(engine::Engine& engine) {

        // Chose one of the possible monuments randomly
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
        std::shared_ptr<engine::PlayBuildMonument> action = std::make_shared<engine::PlayBuildMonument>(build, monument, position, this->playerID);

        // Execute it
        engine.play(action);

    }

}