#include "PlayDefense.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayDefense::PlayDefense(int conflictType, state::Position triggerPosition, int attackSupporters, int additionalSupporters, int playerID) : Action(ACTION_ID_DEFENSE, playerID) {

        this->conflictType         = conflictType;
        this->triggerPosition      = triggerPosition;
        this->attackerSupporters   = attackerSupporters;
        this->supporters           = 0;
        this->additionalSupporters = additionalSupporters;

    }

    void PlayDefense::execute(state::State& state) {

        if(this->conflictType == REVOLT) {

            // Sanity checks

            // Find defender leader position
            // Take trigger position check type and region -> find leader from defender (this->playerID) with that type in that region

            // Count Supporters

            // Remove additional supporters from player's hand

            // If defender wins
                //remove attacking leader
                //win a red victory point

            // If attacker wins
                //remove defending leader
                //win a red victory point

            // Check for conflicts? (Just in case? (Mandatory for wars?))

            // set isInRevolt to false

        }

    }

}