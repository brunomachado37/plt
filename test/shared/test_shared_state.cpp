#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/State.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestState) {
    
  {
    // Constructor
    State state;
    BOOST_CHECK_EQUAL(state.getPlayers().size(), 2);
    BOOST_CHECK_EQUAL(state.getNumPlayers(), 2);
    BOOST_CHECK_EQUAL(state.getTurn(), 0);
    BOOST_CHECK_EQUAL(state.getActivePlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getActionsDone(), 0);
    BOOST_CHECK_EQUAL(state.getRemainingTreasures(), 10);

    State anotherState(4);
    BOOST_CHECK_EQUAL(anotherState.getPlayers().size(), 4);
    BOOST_CHECK_EQUAL(anotherState.getNumPlayers(), 4);

    BOOST_CHECK_EQUAL(state.getBoard().getRegions().size(), 0);


    // Next action
    state.nextAction();
    BOOST_CHECK_EQUAL(state.getTurn(), 0);
    BOOST_CHECK_EQUAL(state.getActivePlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getActionsDone(), 1);

    state.nextAction();
    BOOST_CHECK_EQUAL(state.getTurn(), 1);
    BOOST_CHECK_EQUAL(state.getActivePlayerID(), 1);
    BOOST_CHECK_EQUAL(state.getActionsDone(), 0);

    state.nextAction();
    BOOST_CHECK_EQUAL(state.getTurn(), 1);
    BOOST_CHECK_EQUAL(state.getActivePlayerID(), 1);
    BOOST_CHECK_EQUAL(state.getActionsDone(), 1);

    state.nextAction();
    BOOST_CHECK_EQUAL(state.getTurn(), 2);
    BOOST_CHECK_EQUAL(state.getActivePlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getActionsDone(), 0);


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

    // Set Player
    Player player = state.getPlayers()[0];
    player.removeTileFromHand(player.getTilesInHand()[0].getType());
    player.removeLeaderFromHand("farmer");
    state.setPlayer(player);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getTilesInHand().size(), 5);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 3);

  }

}