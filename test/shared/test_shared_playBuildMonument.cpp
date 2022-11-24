#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayBuildMonument.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayBuildMonument) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    // Get Board
    state::Board board = state.getBoard();

    // Add 4 tiles to the board
    state::Tile tile(MARKET, {-1, -1});
    board.addTileToTheBoard(tile, {2, 9});
    board.addTileToTheBoard(tile, {2, 10});
    board.addTileToTheBoard(tile, {3, 9});
    board.addTileToTheBoard(tile, {3, 10});

    // Set Board
    state.setBoard(board);

    // Create action
    PlayBuildMonument action(true, board.getMonuments()[2], {2, 9}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_BUILD_MONUM);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][9], MARKET);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][10], MARKET);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][9], MARKET);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][10], MARKET); 

    int regionID = state.getBoard().getRegionMap()[2][9];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][9], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][10], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][9], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][10], regionID); 

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 4);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0);

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 6);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 1);


    // Check execute methode
    action.execute(state);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][9], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][10], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][9], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][10], MONUMENT);  

    regionID = state.getBoard().getRegionMap()[2][9];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][9], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][10], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][9], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][10], regionID); 

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 1);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0); 

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 0);


    // Not build monument action
    // Add 4 tiles to the board
    board = state.getBoard();

    state::Tile til(FARM, {-1, -1});
    board.addTileToTheBoard(til, {3, 14});
    board.addTileToTheBoard(til, {3, 15});
    board.addTileToTheBoard(til, {4, 14});
    board.addTileToTheBoard(til, {4, 15});

    // Set Board
    state.setBoard(board);

    // Create action
    PlayBuildMonument action2(false, board.getMonuments()[4], {3, 14}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action2.getActionID(), ACTION_ID_BUILD_MONUM);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][14], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][15], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][14], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][15], FARM); 

    regionID = state.getBoard().getRegionMap()[3][14];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][14], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][15], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][15], regionID); 

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0);

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 1);


    // Check execute methode
    action2.execute(state);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][14], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][15], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][14], FARM);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][15], FARM);   

    regionID = state.getBoard().getRegionMap()[3][14];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][14], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][15], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][15], regionID); 

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0);

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 0);


    // Check if leaders are removed in temple monument
    // Add 3 TEMPLES to the board
    board = state.getBoard();

    state::Tile tile2(TEMPLE, {-1, -1});
    board.addTileToTheBoard(tile2, {0, 0});
    board.addTileToTheBoard(tile2, {0, 1});
    board.addTileToTheBoard(tile2, {1, 0});


    // Add leaders to the board
    state::Player player1 = state.getPlayers()[0];
    state::Player player2 = state.getPlayers()[1];

    state::Leader leader1 = player1.getLeadersInHand()[KING];
    state::Leader leader2 = player2.getLeadersInHand()[TRADER];

    board.addLeaderToTheBoard(leader1, {0, 2});
    board.addLeaderToTheBoard(leader2, {2, 1});

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
  
    state.setPlayer(player1);
    state.setPlayer(player2);
    state.setBoard(board);

    regionID = state.getBoard().getRegionMap()[0][0];

    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][2], LEADER);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][2], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][1], LEADER);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][1], regionID);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 3);

    // Create action
    PlayBuildMonument action3(true, board.getMonuments()[1], {0, 0}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action3.getActionID(), ACTION_ID_BUILD_MONUM);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][0], TEMPLE);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][1], TEMPLE);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][0], TEMPLE);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][1], TEMPLE); 

    regionID = state.getBoard().getRegionMap()[0][0];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][0], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][1], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][0], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][1], regionID); 

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 4);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 2);

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 5);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 1);


    // Check execute methode
    action3.execute(state);

    // Board maps
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][0], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][1], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][0], MONUMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][1], MONUMENT);   

    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][2], LAND);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][2], LAND);


    regionID = state.getBoard().getRegionMap()[0][0];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][0], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][1], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][0], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][1], regionID);  

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[0][2], NO_REGION_ID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][1], NO_REGION_ID);

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getMonuments().size(), 1);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0); 

    BOOST_CHECK_EQUAL(state.getBoard().getMonuments().size(), 4);
    BOOST_CHECK_EQUAL(state.getBoard().getPossibleMonuments().size(), 0);

    // Check if leaders were returned to player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 4);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 4);

    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[KING].getType(), KING);
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand()[TRADER].getType(), TRADER);

   
    // Check throws
    // Not active player action
    PlayBuildMonument action4(true, board.getMonuments()[0], {0, 0}, 1); 
    BOOST_CHECK_THROW(action4.execute(state), std::invalid_argument);

    // Monument in invalid position
    PlayBuildMonument action5(true, board.getMonuments()[0], {5, 5}, 0); 
    BOOST_CHECK_THROW(action5.execute(state), std::logic_error);
 
  }

}