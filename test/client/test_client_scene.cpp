
#include <boost/test/unit_test.hpp>
#include "../../src/client/render/Scene.h"
#include "../../src/shared/state/State.h"

using namespace render;
using namespace state;

BOOST_AUTO_TEST_CASE(TestScene)
{
  {
    // Constructor
    sf::RenderWindow window(sf::VideoMode(1900, 1024), "Tigris & Euphrates");
    Scene scene(window);

    // Draw state
    State state;
    state.init();

    Board board = state.getBoard();
    Leader leader("king", {-1, -1}, 1);
    board.addLeaderToTheBoard(leader, {1, 0});
    state.setBoard(board);

    scene.drawState(state, window);

    // Display window
    scene.display(window);

  }
}
