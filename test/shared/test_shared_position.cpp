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

    // Operator <
    BOOST_CHECK(!(position < anotherPosition));
    BOOST_CHECK(!(anotherPosition < position));

    // Smaller i same j
    position.i = 4;
    BOOST_CHECK(position < anotherPosition);

    // Smaller i smaller j
    position.j = 2;
    BOOST_CHECK(position < anotherPosition);

    // Smaller i greater j
    position.j = 11;
    BOOST_CHECK(position < anotherPosition);

    // Same i greater j
    position.i = 8;
    BOOST_CHECK(anotherPosition < position);

    // Same i smaller j
    position.j = 4;
    BOOST_CHECK(position < anotherPosition);

  }

}