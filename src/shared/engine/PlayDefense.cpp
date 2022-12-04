#include "PlayDefense.h"
#include "../constants.h"
#include "../messages.h"
#include "../state.h"

#include <algorithm>

namespace engine {

    PlayDefense::PlayDefense(int conflictType, state::Position triggerPosition, int attackSupporters, int additionalSupporters, int playerID, std::string warLeaderType) : Action(ACTION_ID_DEFENSE, playerID) {

        this->conflictType         = conflictType;
        this->triggerPosition      = triggerPosition;
        this->attackerSupporters   = attackSupporters;
        this->supporters           = 0;
        this->additionalSupporters = additionalSupporters;
        this->warLeaderType        = warLeaderType;

    }

    void PlayDefense::execute(state::State& state) {

        // Sanity check
        if(state.getActivePlayerID() == this->playerID) {
            throw std::invalid_argument(ACTIVE_PLAYER_MSG);
        }

        if(this->conflictType == REVOLT) {

            // Sanity checks
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != LEADER) {
                throw std::logic_error(TRIGGER_POSITION_REVOLT_ERROR_MSG);
            }

            // Get board
            state::Board board = state.getBoard();
            int regionID = board.getRegionMap()[this->triggerPosition.i][this->triggerPosition.j];

            if(!(state.getBoard().getRegions()[regionID].getIsInRevolt())) {
                throw std::logic_error(REGION_REVOLT_ERROR_MSG);
            }          
            
            // Find attacker leader
            state::Leader attacker;

            for(auto leader: board.getRegions()[regionID].getLeaders()) {
                if(leader.getPosition() == this->triggerPosition) {
                    attacker = leader;
                }
            }

            // Find defender leader
            state::Leader defender("", {-1, -1}, -1); 

            for(auto leader: board.getRegions()[regionID].getLeaders()) {
                if(leader.getType() == attacker.getType() && leader.getPlayerID() == this->playerID) {
                    defender = leader;
                }
            }

            // Check if defender was found
            if(defender.getPlayerID() == -1) {
                throw std::logic_error(DEFENDER_LEADER_REVOLT_ERROR_MSG);
            }

            // Count Supporters
            std::vector<std::string> adjPos = state.getBoard().checkAdjacentPositions(defender.getPosition());
            this->supporters = std::count(adjPos.begin(), adjPos.end(), TEMPLE);
            

            // Remove additional supporters from player's hand
            state::Player defenderPlayer = state.getPlayers()[this->playerID];
            state::Player attackerPlayer = state.getPlayers()[attacker.getPlayerID()];
            
            for(int i = 0; i < this->additionalSupporters; i++) {
                try {
                    defenderPlayer.removeTileFromHand(TEMPLE);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }
            }

            // If defender wins
            if(this->supporters + this->additionalSupporters >= this->attackerSupporters) {
                // Add attacking leader to player's hand
                try {
                    attackerPlayer.addLeaderToHand(attacker);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Remove attacking leader from the board
                try {
                    board.removeLeaderFromTheBoard(attacker.getPlayerID(), attacker.getType());
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Defender wins a red victory point
                defenderPlayer.addVictoryPoints(RED, 1);

            }
            // If attacker wins
            else {
                // Add defender leader to player's hand
                try {
                    defenderPlayer.addLeaderToHand(defender);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Remove defender leader from the board
                try {
                    board.removeLeaderFromTheBoard(defender.getPlayerID(), defender.getType());
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Attacker wins a red victory point
                attackerPlayer.addVictoryPoints(RED, 1);
            }


            // Transfer actions to the state, if everything goes well
            state.setBoard(board);
            state.setPlayer(attackerPlayer);
            state.setPlayer(defenderPlayer);

        }

        if(this->conflictType == WAR) {
            // Sanity checks
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != FARM && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != TEMPLE && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != SETTLEMENT && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != MARKET) {
                throw std::logic_error(TRIGGER_POSITION_WAR_ERROR_MSG);
            }

            // Get board
            state::Board board = state.getBoard();
            int regionID = board.getRegionMap()[this->triggerPosition.i][this->triggerPosition.j];

            if(!(state.getBoard().getRegions()[regionID].getIsAtWar())) {
                throw std::logic_error(REGION_WAR_ERROR_MSG);
            }
            if(state.getBoard().getRegions()[regionID].getUnificationTilePosition() != this->triggerPosition) {
                throw std::logic_error(REGION_WAR_POSITION_ERROR_MSG);
            }

            // Find attacker and defender leaders
            state::Leader attacker("", {-1, -1}, -1);
            state::Leader defender("", {-1, -1}, -1);

            for(auto leader: board.getRegions()[regionID].getLeaders()) {
                if(leader.getType() == this->warLeaderType) {
                    if(leader.getPlayerID() == this->playerID) {
                        defender = leader;
                    }
                    else {
                        attacker = leader;
                    }
                }
            }

            // Check if attacker and defender were found
            if(attacker.getPlayerID() == -1 || defender.getPlayerID() == -1) {
                throw std::logic_error(LEADER_WAR_ERROR_MSG);
            }

            // Count Supporters
            this->supporters = board.checkLeaderStrength(defender, this->triggerPosition);

            // Remove additional supporters from player's hand
            state::Player defenderPlayer = state.getPlayers()[this->playerID];
            state::Player attackerPlayer = state.getPlayers()[attacker.getPlayerID()];

            // Leader to tile map
            std::unordered_map<std::string, std::string> leaderToTileMap = {{FARMER, FARM}, {PRIEST, TEMPLE}, {TRADER, MARKET}, {KING, SETTLEMENT}};
            std::unordered_map<std::string, std::string> typeToColorMap = {{FARM, BLUE}, {TEMPLE, RED}, {MARKET, GREEN}, {SETTLEMENT, BLACK}};
            
            for(int i = 0; i < this->additionalSupporters; i++) {
                try {
                    defenderPlayer.removeTileFromHand(leaderToTileMap[this->warLeaderType]);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }
            }

            // If defender wins
            if(this->supporters + this->additionalSupporters >= this->attackerSupporters) {
                // Add attacking leader to player's hand
                try {
                    attackerPlayer.addLeaderToHand(attacker);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Remove all tiles from attacker side of the region and remove attacker leader
                int removedTiles = 0;
                try {
                    removedTiles = board.warLost(attacker,  this->triggerPosition, leaderToTileMap[this->warLeaderType]);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }              

                // Defender wins victory points
                defenderPlayer.addVictoryPoints(typeToColorMap[leaderToTileMap[this->warLeaderType]], removedTiles + 1);

            }
            // If attacker wins
            else {
                // Add defending leader to player's hand
                try {
                    defenderPlayer.addLeaderToHand(defender);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }

                // Remove all tiles from defender side of the region and remove defending leader
                int removedTiles = 0;
                try {
                    removedTiles = board.warLost(defender,  this->triggerPosition, leaderToTileMap[this->warLeaderType]);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }  

                // Attacker wins victory points
                attackerPlayer.addVictoryPoints(typeToColorMap[leaderToTileMap[this->warLeaderType]], removedTiles + 1);

            }

            // Transfer actions to the state, if everything goes well
            state.setBoard(board);
            state.setPlayer(attackerPlayer);
            state.setPlayer(defenderPlayer);

        }

    }

    state::Position PlayDefense::getPosition() {

        return this->triggerPosition;

    }

}