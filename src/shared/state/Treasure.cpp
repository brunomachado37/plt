#include "Treasure.h"

namespace state {

    Treasure::Treasure(Position position, bool isSpecial) {

        this->position  = position;
        this->isSpecial = isSpecial;

    }

    Position Treasure::getPosition() {
        return this->position;
    }

    bool Treasure::getIsSpecial() {
        return this->isSpecial;
    }

}