#include "PlayAttack.h"
#include "../messages.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayAttack::PlayAttack(int conflictType, state::Position triggerPosition, int additionalSupporters, int playerID) : Action(ACTION_ID_ATTACK, playerID) {

        this->conflictType         = conflictType;
        this->triggerPosition      = triggerPosition;
        this->supporters           = 0;
        this->additionalSupporters = additionalSupporters;

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

    }

}