#include "Action.h"
#include "../constants.h"

namespace engine {

    Action::Action(int actionID, int playerID) {

        this->actionID = actionID;
        this->playerID = playerID;

    }

    void Action::execute(state::State& state) {}

    int Action::getActionID() const {
        return this->actionID;
    }

    int Action::getPlayerID() const {
        return this->playerID;
    }

    state::Position Action::getPosition() {
        return {NOT_IN_MAP_ID, NOT_IN_MAP_ID};
    }

    std::string Action::getWarLeaderType() {
        return "";
    }

    int Action::getSupporters() {
        return NOT_IN_MAP_ID;
    }

    int Action::getConflictType() {
        return NOT_IN_MAP_ID;
    }

}