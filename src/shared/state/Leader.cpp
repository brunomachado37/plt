#include "Leader.h"

namespace state {

    Leader::Leader() {}

    Leader::Leader(std::string type, Position position, int playerID) {

        this->type      = type;
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

    int Leader::getStrength() {
        return this->strength;
    }

    void Leader::setStrength(int strength) {
        this->strength = strength;
    }

}