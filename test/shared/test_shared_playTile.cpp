#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayTile.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayTile) {
    
  {

    // Create a game state
    state::State state;
    state.init();
    state::Board board = state.getBoard();

    // Take a tile
    state::Tile tile = state.getPlayers()[0].getTilesInHand()[0];

    // Tile to leader map
    std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};

    // Take leader
    state::Leader leader = state.getPlayers()[0].getLeadersInHand()[tileToLeaderMap[tile.getType()]];
    board.addLeaderToTheBoard(leader, {2, 1});
    state.setBoard(board);

    // Check tile type
    state::Position pos;
    
    if(tile.getType() == FARM) {
      pos = {3, 1};
    }
    else {
      pos = {1, 2};
    }
    

    // Create action
    PlayTile action(tile, pos, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_TILE);

    // Check execute methode
    action.execute(state);

    // Tile in board state map
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[pos.i][pos.j], tile.getType());

    // Check if tile is in a region
    int regionID = state.getBoard().getRegionMap()[pos.i][pos.j];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles()[1].getType(), tile.getType());
    
    // Check if tile was removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getTilesInHand().size(), HAND_LIMIT - 1);

    // Check if player received victory point
    std::unordered_map<std::string, std::string> typeToColorMap = {{FARM, BLUE}, {TEMPLE, RED}, {MARKET, GREEN}, {SETTLEMENT, BLACK}};
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getVictoryPoints()[typeToColorMap[tile.getType()]], 1);

    // Save player victory points for later use
    std::unordered_map<std::string, int> points = state.getPlayers()[0].getVictoryPoints();

    // Add king to the board
    leader = state.getPlayers()[0].getLeadersInHand()[KING];
    board.addLeaderToTheBoard(leader, {1, 5});
    state.setBoard(board);
  
    // Take another tile
    tile = state.getPlayers()[0].getTilesInHand()[1];

    // Check tile type    
    if(tile.getType() == FARM) {
      pos = {2, 4};
    }
    else {
      pos = {2, 6};
    }

    // Create action
    PlayTile action2(tile, pos, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action2.getActionID(), ACTION_ID_TILE);

    // Check execute methode
    action2.execute(state);

    // Tile in board state map
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[pos.i][pos.j], tile.getType());

    // Check if tile is in a region
    regionID = state.getBoard().getRegionMap()[pos.i][pos.j];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles()[1].getType(), tile.getType());
    
    // Check if tile was removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getTilesInHand().size(), HAND_LIMIT - 2);

    // Check if player received victory point
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getVictoryPoints()[typeToColorMap[tile.getType()]], points[typeToColorMap[tile.getType()]] + 1);


    // Check throws
    // Not active player action
    PlayTile action3(state.getPlayers()[1].getTilesInHand()[0], {1, 0}, 1); 
    BOOST_CHECK_THROW(action3.execute(state), std::invalid_argument);
 
    // Position occupied
    PlayTile action4(state.getPlayers()[0].getTilesInHand()[2], {1, 1}, 0); 
    BOOST_CHECK_THROW(action4.execute(state), std::invalid_argument);

  }

}