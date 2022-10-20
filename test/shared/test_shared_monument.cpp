#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Monument.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestMonument) {
    
  {
    Monument monument({12, 7}, "blue", "red");
    BOOST_CHECK_EQUAL(monument.getPosition().i, 12);
    BOOST_CHECK_EQUAL(monument.getPosition().j, 7);
    BOOST_CHECK_EQUAL(monument.getColor1(), "blue");
    BOOST_CHECK_EQUAL(monument.getColor2(), "red");

    monument.setPosition({8, 4});
    BOOST_CHECK_EQUAL(monument.getPosition().i, 8);
    BOOST_CHECK_EQUAL(monument.getPosition().j, 4);

  }

}