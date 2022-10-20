#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Tile.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTile) {
    
  {
    Tile tile("temple", {42, 24});
    BOOST_CHECK_EQUAL(tile.getType(), "temple");
    BOOST_CHECK_EQUAL(tile.getPosition().i, 42);
    BOOST_CHECK_EQUAL(tile.getPosition().j, 24);

    tile.setPosition({8, 56});
    BOOST_CHECK_EQUAL(tile.getPosition().i, 8);
    BOOST_CHECK_EQUAL(tile.getPosition().j, 56);
  }

}