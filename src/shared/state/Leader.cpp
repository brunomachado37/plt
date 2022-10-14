#include "Leader.h"

namespace state {

    Leader::Leader() {}

    Leader::Leader(std::string type, bool inHand, Position position, int playerID) {

        this->type      = type;
        this->inHand    = inHand;
        this->position  = position;
        this->playerID  = playerID;

    }

    std::string Leader::getType() {
        return this->type;
    }

    int Leader::getPlayerID() {
        return this->playerID;
    }

    Position Leader::getPosition() {
        return this->position;
    }

    void Leader::setPosition(Position position) {
        this->position = position;
    }

}