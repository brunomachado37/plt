#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/State.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestState) {
    
  {
    // Constructor
    State state;
    BOOST_CHECK_EQUAL(state.getPlayers().size(), 2);

    State anotherState(4);
    BOOST_CHECK_EQUAL(anotherState.getPlayers().size(), 4);

    BOOST_CHECK_EQUAL(state.getBoard().getRegions().size(), 0);

    // Initialization
    state.init();
    BOOST_CHECK_EQUAL(state.getBoard().getRegions().size(), 10);

    anotherState.setBoard(state.getBoard());
    BOOST_CHECK_EQUAL(anotherState.getBoard().getRegions().size(), 10);
    
    // Test random tile generation
    std::string tileType = state.getRandomTileType();
    BOOST_CHECK(tileType == "temple" || tileType == "market" || tileType == "settlement" || tileType == "farm");

    // Test remaining tiles consistency
    std::unordered_map<std::string, int> usedTilesCount = {{"farm", 0}, {"market", 0}, {"temple", 0}, {"settlement", 0}};

    for(int i = 0; i < (int)state.getPlayers().size(); i++) {
      for(int j = 0; j < (int)state.getPlayers()[i].getTilesInHand().size(); j++) {
        usedTilesCount[state.getPlayers()[i].getTilesInHand()[j].getType()]++;
      }
    }

    if(tileType == "temple") {
      BOOST_CHECK_EQUAL(state.getRemainingTiles()[tileType], 46 - usedTilesCount[tileType]);
    }
    else if(tileType == "market") {
      BOOST_CHECK_EQUAL(state.getRemainingTiles()[tileType], 29 - usedTilesCount[tileType]);
    }
    else if(tileType == "farm") {
      BOOST_CHECK_EQUAL(state.getRemainingTiles()[tileType], 35 - usedTilesCount[tileType]);
    }
    else {
      BOOST_CHECK_EQUAL(state.getRemainingTiles()[tileType], 29 - usedTilesCount[tileType]);
    }

  }

}