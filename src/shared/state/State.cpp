#include "State.h"

namespace state {

    State::State(int numPlayers) : numPlayers(numPlayers) {
        Board b;
        this->board = &b;
    }

}