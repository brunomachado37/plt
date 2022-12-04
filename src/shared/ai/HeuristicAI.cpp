#include "HeuristicAI.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"
#include "../engine.h"

#include <algorithm>


namespace ai {

    HeuristicAI::HeuristicAI(int playerID) : AI(playerID) {}

    void HeuristicAI::run(engine::Engine& engine) {

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

        // Sanity checks
        // Check if I am the active player
        if(engine.getState().getActivePlayerID() != this->playerID) {
            throw std::logic_error(AI_NOT_ACTIVE_ERROR_MSG);
        }
        // Check if there's a monument build pendent
        if(engine.getMonumentPendent()) {
            throw std::logic_error(AI_MONUMENT_ERROR_MSG);
        }


        // Chose an action
        // If no leader was played yet, play a king
        if((int)engine.getState().getPlayers()[this->playerID].getLeadersInHand().size() == 4) {

            this->playLeader(KING, engine);

        }
        else {
            // Check type with less points
            std::unordered_map<std::string, int> playerPoints = engine.getState().getPlayers()[this->playerID].getVictoryPoints();
            playerPoints.erase(TREASURE);

            std::string lowerType;
            int lowerAmount = 99999;

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
                        this->playTile(colorToTileMap[lowerType], engine);
                        not_played = false;
                        break;
                    }
                }                

            }
            // If not possible, check second smallest points
            if(not_played) {
                playerPoints.erase(lowerType);
                lowerAmount = 99999;

                for(auto type: playerPoints) {
                    if(type.second < lowerAmount) {
                        lowerAmount = type.second;
                        lowerType = type.first;
                    }
                }

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
                            this->playTile(colorToTileMap[lowerType], engine);
                            not_played = false;
                            break;
                        }
                    }                

                }

            }
            // If not possible, draw
            if(not_played) {

                this->draw(engine);

            }

        }

        // If monument required
        if(engine.getMonumentPendent()) {
            
            this->buildMonument(engine);

        }

        // If attack required
        if(engine.getAttackPendent()) {

            this->attack(engine);

        }

    }

    void HeuristicAI::attack(engine::Engine& engine) {

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

            // Choose a conflict type
            std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
            std::string warType;
            int possibleSupporters = -1;
            int counter = 0;

            if((int)leaderTypes.size() > 1) {
                // Take the type with more supporters
                for(int i = 0; i < (int)leaderTypes.size(); i++) {
                    for(auto tile: engine.getState().getPlayers()[this->playerID].getTilesInHand()) {
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

    void HeuristicAI::defend(engine::Engine& engine) {

        // Get attack data
        std::string warType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getWarLeaderType();
        state::Position position = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPosition();
        int attackerSupporters = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getSupporters();
        int conflictType = engine.getActionsLog()[engine.getActionsLog().size() - 1]->getConflictType();

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
        int send = 0;

        // If attacker win anyway
        if(attackerSupporters > supporters + possibleSuporters) {
            send = 0;
        }
        // If is possible to win
        else {
            send = attackerSupporters - supporters;
        }

        // Create action
        engine::PlayDefense* action = new engine::PlayDefense(conflictType, position, attackerSupporters, send, this->playerID, warType);

        // Execute action
        engine.play(action);

    }

    void HeuristicAI::playTile(std::string type, engine::Engine& engine) {

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
        state::Position play_position;

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
            if((int)target_positions.size() > 1) {
                std::uniform_int_distribution<> int_dis_pos(0, (int)target_positions.size() - 1);
                play_position = target_positions[int_dis_pos(this->gen)];
            }
            else if((int)target_positions.size() == 1) {
                play_position = target_positions[0];
            }
            else {
                std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
                play_position = possible_positions[int_dis_pos(this->gen)];
            }

        }
        else {
            std::uniform_int_distribution<> int_dis_pos(0, (int)possible_positions.size() - 1);
            play_position = possible_positions[int_dis_pos(this->gen)];
        }

        // Create action
        engine::PlayTile* action = new engine::PlayTile(tile, play_position, this->playerID);

        // Execute action
        engine.play(action);

    }

    void HeuristicAI::playLeader(std::string type, engine::Engine& engine) {

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
                                    possible_positions.push_back({tile.getPosition().i + posMap[i].i, tile.getPosition().j + posMap[i].j});
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

    void HeuristicAI::draw(engine::Engine& engine) {

        // Take tiles in player hand
        std::vector<state::Tile> tiles = engine.getState().getPlayers()[this->playerID].getTilesInHand();

        // Create action
        engine::PlayDrawTiles* action = new engine::PlayDrawTiles(tiles, this->playerID);

        // Execute action
        engine.play(action);

    }

    void HeuristicAI::buildMonument(engine::Engine& engine) {

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
        engine::PlayBuildMonument* action = new engine::PlayBuildMonument(build, monument, position, this->playerID);

        // Execute it
        engine.play(action);

    }

}