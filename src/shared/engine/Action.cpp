#include "Action.h"

namespace engine {

    Action::Action(int actionID, int playerID) {

        this->actionID = actionID;
        this->playerID = playerID;

    }

    void Action::execute(state::State& state) {}

    int Action::getActionID() const {
        return this->actionID;
    }

}