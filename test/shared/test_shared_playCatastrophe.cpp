#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayCatastrophe.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayCatastrophe) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    state::Board board = state.getBoard();

    state::Player player1 = state.getPlayers()[0];
    state::Player player2 = state.getPlayers()[1];

    // Add a temple to the board
    state::Tile tile(TEMPLE, {-1, -1});
    board.addTileToTheBoard(tile, {2, 9});

    // Add leaders adjacent to this temple
    state::Leader leader1 = player1.getLeadersInHand()[KING];
    state::Leader leader2 = player2.getLeadersInHand()[TRADER];

    board.addLeaderToTheBoard(leader1, {2, 10});
    board.addLeaderToTheBoard(leader2, {2, 8});

    player1.removeLeaderFromHand(leader1.getType());
    player2.removeLeaderFromHand(leader2.getType());

    BOOST_CHECK_EQUAL(player1.getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(player1.getLeadersInHand()[TRADER].getType(), TRADER);
    BOOST_CHECK_EQUAL(player1.getLeadersInHand()[PRIEST].getType(), PRIEST);
    BOOST_CHECK_EQUAL(player1.getLeadersInHand()[FARMER].getType(), FARMER);

    BOOST_CHECK_EQUAL(player2.getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(player2.getLeadersInHand()[KING].getType(), KING);
    BOOST_CHECK_EQUAL(player2.getLeadersInHand()[PRIEST].getType(), PRIEST);
    BOOST_CHECK_EQUAL(player2.getLeadersInHand()[FARMER].getType(), FARMER);
  
    state.setBoard(board);
    state.setPlayer(player1);
    state.setPlayer(player2);

    int regionID = state.getBoard().getRegionMap()[2][9];

    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][8], LEADER);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][8], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][10], LEADER);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][10], regionID);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 3);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getCatastropheTiles(), 2);

    // Create action
    PlayCatastrophe action({2, 9}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_CATASTRO);

    // Check execute methode
    action.execute(state);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][9], CATASTRO);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][9], state::NO_REGION_ID);

    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][8], LAND);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][8], state::NO_REGION_ID);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][10], LAND);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][10], state::NO_REGION_ID);
    
    // Check if leaders were returned to player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 4);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 4);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[KING].getType(), KING);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand()[TRADER].getType(), TRADER);

    // Check if leader got its catastrophes reduced
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getCatastropheTiles(), 1);
   
    // Check throws
    // Not active player action
    PlayCatastrophe action2({1, 0}, 1); 
    BOOST_CHECK_THROW(action2.execute(state), std::invalid_argument);
 
    // Position occupied by a treasure
    PlayCatastrophe action3({1, 1}, 0);  
    BOOST_CHECK_THROW(action3.execute(state), std::invalid_argument);

  }

}