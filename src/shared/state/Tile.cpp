#include "Tile.h"

namespace state {

    Tile::Tile(std::string type, Position position) {

        this->type      = type;
        this->position  = position;

    }

    std::string Tile::getType() {
        return this->type;
    }

    Position Tile::getPosition() {
        return this->position;
    }

    void Tile::setPosition(Position position) {
        this->position = position;
    }

}