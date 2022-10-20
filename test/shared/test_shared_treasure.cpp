#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Treasure.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTreasure) {
    
  {
    Treasure treasure({2, 1}, false);
    BOOST_CHECK_EQUAL(treasure.getIsSpecial(), false);
    BOOST_CHECK_EQUAL(treasure.getPosition().i, 2);
    BOOST_CHECK_EQUAL(treasure.getPosition().j, 1);

    Treasure anotherTreasure({2, 1}, true);
    BOOST_CHECK_EQUAL(anotherTreasure.getIsSpecial(), true);

  }

}