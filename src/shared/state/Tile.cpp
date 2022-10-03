#include "Tile.h"

namespace state {

    Tile::Tile(std::string type, bool inHand, Position position) {

        this->type      = type;
        this->inHand    = inHand;
        this->position  = position;

    }

}