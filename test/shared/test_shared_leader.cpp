#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Leader.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestLeader) {
    
  {
    Leader leader("king", {5, 8}, 2);
    BOOST_CHECK_EQUAL(leader.getType(), "king");
    BOOST_CHECK_EQUAL(leader.getPosition().i, 5);
    BOOST_CHECK_EQUAL(leader.getPosition().j, 8);
    BOOST_CHECK_EQUAL(leader.getPlayerID(), 2);
    BOOST_CHECK_EQUAL(leader.getStrength(), 0);

    leader.setPosition({18, 56});
    BOOST_CHECK_EQUAL(leader.getPosition().i, 18);
    BOOST_CHECK_EQUAL(leader.getPosition().j, 56);

    leader.setStrength(6);
    BOOST_CHECK_EQUAL(leader.getStrength(), 6);

  }

}