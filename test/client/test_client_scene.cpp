
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
    Scene scene(0);

    // Define state
    State state;
    state.init();

    Board board = state.getBoard();
    Leader leader("king", {-1, -1}, 1);
    Leader leader2("king", {-1, -1}, 2);
    Tile temple("temple", {-1, -1});
    board.addLeaderToTheBoard(leader, {1, 0});
    board.addTileToTheBoard(temple, {0, 11});
    board.addTileToTheBoard(temple, {1, 11});
    board.addTileToTheBoard(temple, {1, 10});
    board.addMonumentToTheBoard(board.getMonuments()[1], {0, 10});
    board.addCatastropheToTheBoard({7, 8});
    board.addCatastropheToTheBoard({5, 4});
    board.addTileToTheBoard(temple, {1, 3});
    board.addLeaderToTheBoard(leader2, {0, 3});
    board.addTileToTheBoard(temple, {1, 2});
    state.setBoard(board);

    // Display window
    scene.display(state, window);

  }
}
