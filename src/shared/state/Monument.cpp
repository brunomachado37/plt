#include "Monument.h"

namespace state {

    Monument::Monument() {}

    Monument::Monument(Position position, std::string color1, std::string color2) {

        this->position  = position;
        this->color1    = color1;
        this->color2    = color2;

    }

    Position Monument::getPosition() {
        return this->position;
    }

    void Monument::setPosition(Position position) {
        this->position = position;
    }

    std::string Monument::getColor1() {
        return this->color1;
    }

    std::string Monument::getColor2() {
        return this->color2;
    }

}