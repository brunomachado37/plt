#include "PlayBuildMonument.h"
#include "../constants.h"
#include "../state.h"

namespace engine {

    PlayBuildMonument::PlayBuildMonument(bool build, state::Monument monument, state::Position position, int playerID) : Action(ACTION_ID_BUILD_MONUM, playerID) {

        this->build = build;
        this->monument = monument;
        this->position = position;

    }

    void PlayBuildMonument::execute(state::State& state) {

        if(this->build) {
            // Add monument to the Board
            state::Board board = state.getBoard();

            try {
                board.addMonumentToTheBoard(this->monument, this->position);
            }
            catch(const std::invalid_argument& e) {
                throw;
            }

            // Clear possible monuments list
            board.clearPossibleMonuments();

            // Transfer actions to the state, if everything goes well
            state.setBoard(board);
        }

    }

}