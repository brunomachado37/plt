#include "Leader.h"

namespace state {

    Leader::Leader(std::string type, bool inHand, Position position, int playerID) : Piece::Piece(type, inHand, position){

        this->playerID = playerID;

    }

}