#include "Leader.h"

namespace state {

    Leader::Leader(std::string type, bool inHand, Position position, int playerID) {

        this->type      = type;
        this->inHand    = inHand;
        this->position  = position;
        this->playerID  = playerID;

    }

}