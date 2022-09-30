#include "Piece.h"

namespace state {

    Piece::Piece(std::string type, bool inHand, Position position) {

        this->type      = type;
        this->inHand    = inHand;
        this->position  = position;

    }
    
}