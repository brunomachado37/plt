#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Player.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestPlayer) {
    
  {
    // Constructor
    Player player(3);
    BOOST_CHECK_EQUAL(player.getID(), 3);

    std::unordered_map<std::string, Leader> leaders = player.getLeadersInHand();
    BOOST_CHECK_EQUAL(leaders.size(), 4);

    BOOST_CHECK_EQUAL(leaders["king"].getType(), "king");
    BOOST_CHECK_EQUAL(leaders["priest"].getType(), "priest");
    BOOST_CHECK_EQUAL(leaders["farmer"].getType(), "farmer");
    BOOST_CHECK_EQUAL(leaders["trader"].getType(), "trader");

    BOOST_CHECK_EQUAL(leaders["king"].getPlayerID(), 3);
    BOOST_CHECK_EQUAL(leaders["priest"].getPlayerID(), 3);
    BOOST_CHECK_EQUAL(leaders["farmer"].getPlayerID(), 3);
    BOOST_CHECK_EQUAL(leaders["trader"].getPlayerID(), 3);

    BOOST_CHECK_EQUAL(leaders["king"].getPosition().i, -1);
    BOOST_CHECK_EQUAL(leaders["king"].getPosition().j, -1);
    BOOST_CHECK_EQUAL(leaders["priest"].getPosition().i, -1);
    BOOST_CHECK_EQUAL(leaders["priest"].getPosition().j, -1);
    BOOST_CHECK_EQUAL(leaders["farmer"].getPosition().i, -1);
    BOOST_CHECK_EQUAL(leaders["farmer"].getPosition().j, -1);
    BOOST_CHECK_EQUAL(leaders["trader"].getPosition().i, -1);
    BOOST_CHECK_EQUAL(leaders["trader"].getPosition().j, -1);

    // Tile related methodes
    player.addTileToHand("temple");
    std::vector<Tile> tiles = player.getTilesInHand();
    BOOST_CHECK_EQUAL(tiles.size(), 1);
    BOOST_CHECK_EQUAL(tiles[0].getType(), "temple");
    BOOST_CHECK_EQUAL(tiles[0].getPosition().i, -1);
    BOOST_CHECK_EQUAL(tiles[0].getPosition().j, -1);

    player.addTileToHand("farm");
    tiles = player.getTilesInHand();
    BOOST_CHECK_EQUAL(tiles.size(), 2);
    BOOST_CHECK_EQUAL(tiles[0].getType(), "temple");
    BOOST_CHECK_EQUAL(tiles[1].getType(), "farm");

    for(int i = 0; i < 4; i++) {
      player.addTileToHand("market");
    }

    BOOST_CHECK_EQUAL(player.getTilesInHand().size(), 6);
    BOOST_CHECK_THROW(player.addTileToHand("settlement"), std::invalid_argument);
    BOOST_CHECK_EQUAL(player.getTilesInHand().size(), 6);
    
    player.removeTileFromHand("temple");
    BOOST_CHECK_EQUAL(player.getTilesInHand().size(), 5);

    BOOST_CHECK_THROW(player.removeTileFromHand("settlement"), std::invalid_argument);
    BOOST_CHECK_THROW(player.removeTileFromHand("temple"), std::invalid_argument);
    BOOST_CHECK_EQUAL(player.getTilesInHand().size(), 5);

    player.removeTileFromHand("market");
    BOOST_CHECK_EQUAL(player.getTilesInHand().size(), 4);

    // Leader related methodes
    Leader leader("king", {5, 7}, 3);
    BOOST_CHECK_THROW(player.addLeaderToHand(leader), std::invalid_argument);

    player.removeLeaderFromHand("king");
    BOOST_CHECK_EQUAL(player.getLeadersInHand().size(), 3);
    BOOST_CHECK_THROW(player.removeLeaderFromHand("king"), std::invalid_argument);

    player.addLeaderToHand(leader);
    BOOST_CHECK_EQUAL(player.getLeadersInHand().size(), 4);
    BOOST_CHECK_EQUAL(player.getLeadersInHand()["king"].getPosition().i, -1);
    BOOST_CHECK_EQUAL(player.getLeadersInHand()["king"].getPosition().j, -1);

    Leader anotherLeader("king", {7, 5}, 2);
    player.removeLeaderFromHand("king");
    BOOST_CHECK_EQUAL(player.getLeadersInHand().size(), 3);
    BOOST_CHECK_THROW(player.addLeaderToHand(anotherLeader), std::invalid_argument);
    BOOST_CHECK_EQUAL(player.getLeadersInHand().size(), 3);

    // Victory points
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["blue"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["green"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["black"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["red"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["treasures"], 0);

    player.addVictoryPoints("blue", 5);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["blue"], 5);

    player.addVictoryPoints("blue", 2);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["blue"], 7);

    player.addVictoryPoints("treasures", 1);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["blue"], 7);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["green"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["black"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["red"], 0);
    BOOST_CHECK_EQUAL(player.getVictoryPoints()["treasures"], 1);

    // Catastrophe
    BOOST_CHECK_EQUAL(player.getCatastropheTiles(), 2);

    player.useCatastropheTile();
    BOOST_CHECK_EQUAL(player.getCatastropheTiles(), 1);

    player.useCatastropheTile();
    BOOST_CHECK_EQUAL(player.getCatastropheTiles(), 0);

    BOOST_CHECK_THROW(player.useCatastropheTile(), std::invalid_argument);

  }

}