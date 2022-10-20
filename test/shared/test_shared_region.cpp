#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Region.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestRegion) {
    
  {
    // Constructor
    Region region(5);
    BOOST_CHECK_EQUAL(region.getRegionID(), 5);
    BOOST_CHECK(!region.getIsKingdom());
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 0);
    BOOST_CHECK_EQUAL(region.getTiles().size(), 0);
    BOOST_CHECK_EQUAL(region.getMonuments().size(), 0);
    BOOST_CHECK_EQUAL(region.getTreasures().size(), 0);
    
    // Tiles
    region.addTile("temple", {2, 4});
    BOOST_CHECK_EQUAL(region.getTiles().size(), 1);
    BOOST_CHECK_EQUAL(region.getTiles()[0].getType(), "temple");

    Tile tile("farm", {1, 3});
    region.addTile(tile);
    BOOST_CHECK_EQUAL(region.getTiles().size(), 2);

    BOOST_CHECK_THROW(region.removeTile({4, 2}), std::invalid_argument);
    BOOST_CHECK_EQUAL(region.getTiles().size(), 2);

    region.removeTile({1, 3});
    BOOST_CHECK_EQUAL(region.getTiles().size(), 1);

    region.removeTile({2, 4});
    BOOST_CHECK_EQUAL(region.getTiles().size(), 0);

    // Leaders
    Leader leader("king", {4, 5}, 2);
    Leader anotherLeader("trader", {5, 4}, 2);
    Leader anotherPlayersLeader("trader", {5, 5}, 1);

    region.addLeader(leader);
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 1);

    region.addLeader(anotherLeader);
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 2);

    region.addLeader(anotherPlayersLeader);
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 3);

    BOOST_CHECK_THROW(region.removeLeader(1, "king"), std::invalid_argument);

    region.removeLeader(2, "king");
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 2);

    region.removeLeader(1, "trader");
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 1);
    BOOST_CHECK_EQUAL(region.getLeaders()[0].getPlayerID(), 2);
    BOOST_CHECK_EQUAL(region.getLeaders()[0].getType(), "trader");

    // Leader Strength
    region.addTile(tile);
    region.addTile(tile);
    region.addTile("market", {4, 7});

    Leader farmer("farmer", {5, 7}, 1);
    region.addLeader(farmer);

    BOOST_CHECK_EQUAL(region.getTiles().size(), 3);
    BOOST_CHECK_EQUAL(region.getLeaders().size(), 2);

    region.updateAllLeadersStrength();

    BOOST_CHECK_EQUAL(region.getLeaders()[0].getStrength(), 1);
    BOOST_CHECK_EQUAL(region.getLeaders()[1].getStrength(), 2);

    region.addTile("market", {4, 8});    
    region.addTile("market", {4, 9});

    region.updateAllLeadersStrength();

    BOOST_CHECK_EQUAL(region.getTiles().size(), 5);
    BOOST_CHECK_EQUAL(region.getLeaders()[0].getStrength(), 3);
    BOOST_CHECK_EQUAL(region.getLeaders()[1].getStrength(), 2);

    region.addTile("market", {4, 10});    
    region.addTile("market", {4, 11});
    region.addTile("settlement", {4, 12});
    region.addTile(tile);    
    region.addTile(tile);

    region.updateLeaderStrength("market");
    region.updateLeaderStrength("market");

    BOOST_CHECK_EQUAL(region.getTiles().size(), 10);
    BOOST_CHECK_EQUAL(region.getLeaders()[0].getStrength(), 5);
    BOOST_CHECK_EQUAL(region.getLeaders()[1].getStrength(), 2);

    region.updateLeaderStrength("farm");
    region.updateLeaderStrength("farm");

    BOOST_CHECK_EQUAL(region.getLeaders()[0].getStrength(), 5);
    BOOST_CHECK_EQUAL(region.getLeaders()[1].getStrength(), 4);

    region.updateAllLeadersStrength();

    BOOST_CHECK_EQUAL(region.getLeaders()[0].getStrength(), 5);
    BOOST_CHECK_EQUAL(region.getLeaders()[1].getStrength(), 4);


    // Monuments
    Monument monument({1, 2}, "red", "blue");
    region.addMonument(monument);
    BOOST_CHECK_EQUAL(region.getMonuments().size(), 1);

    // Treasures
    region.addTreasure({5, 2}, true);
    BOOST_CHECK_EQUAL(region.getTreasures().size(), 1);

    Treasure treasure({6, 2}, false);
    region.addTreasure(treasure);
    BOOST_CHECK_EQUAL(region.getTreasures().size(), 2);

    BOOST_CHECK_THROW(region.removeTreasure({1, 1}), std::invalid_argument);

    region.removeTreasure({6, 2});
    BOOST_CHECK_EQUAL(region.getTreasures().size(), 1);
    BOOST_CHECK(region.getTreasures()[0].getIsSpecial());

    // Is Kingdom
    region.setIsKingdom(true);
    BOOST_CHECK(region.getIsKingdom());

    // Is at war
    BOOST_CHECK(!region.getIsAtWar());
    region.setIsAtWar(true);
    BOOST_CHECK(region.getIsAtWar());

    // Is in revolt
    BOOST_CHECK(!region.getIsInRevolt());
    region.setIsInRevolt(true);
    BOOST_CHECK(region.getIsInRevolt());

    // Unification position
    BOOST_CHECK_EQUAL(region.getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(region.getUnificationTilePosition().j, -2);

    region.setUnificationTilePosition({8, 7});

    BOOST_CHECK_EQUAL(region.getUnificationTilePosition().i, 8);
    BOOST_CHECK_EQUAL(region.getUnificationTilePosition().j, 7);

  }

}