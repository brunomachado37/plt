#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Position.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestPosition) {
    
  {
    Position position;
    position.i = 5;
    position.j = 8;

    Position anotherPosition;
    anotherPosition.i = 8;
    anotherPosition.j = 5;

    BOOST_CHECK(position != anotherPosition);

    position.i = 8;
    BOOST_CHECK(position != anotherPosition);

    position.j = 5;
    BOOST_CHECK(position == anotherPosition);

  }

}