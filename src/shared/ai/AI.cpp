#include "AI.h"


namespace ai {

    AI::AI() {}

    AI::AI(int playerID, std::string type) {

        this->playerID = playerID;
        this->type = type;

    }

    void AI::run(engine::Engine& engine) {}

    std::string AI::getType() {
        return this->type;
    }

}