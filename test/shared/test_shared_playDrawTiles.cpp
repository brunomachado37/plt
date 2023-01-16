#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayDrawTiles.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayDrawTiles) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    // Take players hand
    std::vector<state::Tile> discard = state.getPlayers()[0].getTilesInHand();
    BOOST_CHECK_EQUAL(discard.size(), state::HAND_LIMIT);

    // Save number of tiles in the bag
    int numberOfTiles = 0;

    for(auto type: state.getRemainingTiles()) {
      numberOfTiles += type.second;
    }

    // Create action
    PlayDrawTiles action(discard, 0);

    // Execute
    action.execute(state);

    // Check if player's hand changed
    int numberOfEqualTiles = 0;

    for(int i = 0; i < (int)state.getPlayers()[0].getTilesInHand().size(); i++) {
      if(state.getPlayers()[0].getTilesInHand()[i].getType() == discard[i].getType()) {
        numberOfEqualTiles++;
      }
    }

    BOOST_CHECK_NE(numberOfEqualTiles, state::HAND_LIMIT);

    // Check if player has 6 tiles in hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getTilesInHand().size(), state::HAND_LIMIT);

    // Check if number of total tiles in the bag was reduced
    int newNumberOfTiles = 0;

    for(auto type: state.getRemainingTiles()) {
      newNumberOfTiles += type.second;
    }

    BOOST_CHECK_EQUAL(newNumberOfTiles, numberOfTiles - state::HAND_LIMIT);

  }

}