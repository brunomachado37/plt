#include "PlayAttack.h"
#include "../messages.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayAttack::PlayAttack(int conflictType, state::Position triggerPosition, int additionalSupporters, int playerID, std::string warLeaderType) : Action(ACTION_ID_ATTACK, playerID) {

        this->conflictType         = conflictType;
        this->triggerPosition      = triggerPosition;
        this->supporters           = 0;
        this->additionalSupporters = additionalSupporters;
        this->warLeaderType        = warLeaderType;

    }

    void PlayAttack::execute(state::State& state) {

        if(this->conflictType == REVOLT) {

            // Sanity checks
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != LEADER) {
                throw std::logic_error(TRIGGER_POSITION_REVOLT_ERROR_MSG);
            }
            if(!(state.getBoard().getRegions()[state.getBoard().getRegionMap()[this->triggerPosition.i][this->triggerPosition.j]].getIsInRevolt())) {
                throw std::logic_error(REGION_REVOLT_ERROR_MSG);
            }

            // Count Supporters
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i + 1][this->triggerPosition.j] == TEMPLE) {
                this->supporters++;
            }
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i - 1][this->triggerPosition.j] == TEMPLE) {
                this->supporters++;
            }
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j + 1] == TEMPLE) {
                this->supporters++;
            }
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j - 1] == TEMPLE) {
                this->supporters++;
            }

            // Remove additional supporters from player's hand
            state::Player player = state.getPlayers()[this->playerID];
            
            for(int i = 0; i < this->additionalSupporters; i++) {
                try {
                    player.removeTileFromHand(TEMPLE);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }
            }

            // Transfer modified player to the state, if everything goes well
            state.setPlayer(player);

        }

        if(this->conflictType == WAR) { 

            // Sanity checks
            if(state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != FARM && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != TEMPLE && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != SETTLEMENT && state.getBoard().getBoardStateMap()[this->triggerPosition.i][this->triggerPosition.j] != MARKET) {
                throw std::logic_error(TRIGGER_POSITION_WAR_ERROR_MSG);
            }

            int regionID = state.getBoard().getRegionMap()[this->triggerPosition.i][this->triggerPosition.j];

            if(!(state.getBoard().getRegions()[regionID].getIsAtWar())) {
                throw std::logic_error(REGION_WAR_ERROR_MSG);
            }
            if(state.getBoard().getRegions()[regionID].getUnificationTilePosition() != this->triggerPosition) {
                throw std::logic_error(REGION_WAR_POSITION_ERROR_MSG);
            }

            // Count Supporters
            for(auto leader: state.getBoard().getRegions()[regionID].getLeaders()) {
                if(leader.getPlayerID() == this->playerID && leader.getType() == this->warLeaderType) {
                    this->supporters = leader.getStrength();
                }
            }

            // Remove additional supporters from player's hand
            state::Player player = state.getPlayers()[this->playerID];

            // Leader to tile map
            std::unordered_map<std::string, std::string> leaderToTileMap = {{FARMER, FARM}, {PRIEST, TEMPLE}, {TRADER, MARKET}, {KING, SETTLEMENT}};
            
            for(int i = 0; i < this->additionalSupporters; i++) {
                try {
                    player.removeTileFromHand(leaderToTileMap[this->warLeaderType]);
                }
                catch(const std::invalid_argument& e) {
                    throw;
                }
            }

            // Transfer modified player to the state, if everything goes well
            state.setPlayer(player);

        }

    }

    int PlayAttack::getSupporters() {
        return this->supporters + this->additionalSupporters;
    }

}