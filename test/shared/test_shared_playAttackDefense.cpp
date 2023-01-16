#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayAttack.h"
#include "../../src/shared/engine/PlayDefense.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayAttackDefense) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    // Get Board
    state::Board board = state.getBoard();
    state::Player player1 = state.getPlayers()[0];
    state::Player player2 = state.getPlayers()[1];

    // Trigger revolt (add 2 leaders of the same type in a region)
    
    // Add leaders 
    state::Leader leader1 = player1.getLeadersInHand()[KING];
    state::Leader leader2 = player2.getLeadersInHand()[KING];

    board.addLeaderToTheBoard(leader2, {3, 5});
    board.addLeaderToTheBoard(leader1, {1, 5});

    player1.removeLeaderFromHand(leader1.getType());
    player2.removeLeaderFromHand(leader2.getType());

    // Add tiles
    state::Tile temple(TEMPLE, {-1, -1});
    state::Tile settlement(SETTLEMENT, {-1, -1});

    board.addTileToTheBoard(temple, {1, 6});
    board.addTileToTheBoard(settlement, {3, 4});

    // Count supporters
    int support = 0;

    for(auto s: state.getPlayers()[0].getTilesInHand()) {
      if(s.getType() == TEMPLE) {
        support++;
      }
    }

    // Set board and players
    state.setBoard(board);
    state.setPlayer(player1);
    state.setPlayer(player2);

    // Checks
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][6], TEMPLE);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][5], TEMPLE);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][4], SETTLEMENT);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][5], LEADER);
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][5], LEADER);

    int regionID = state.getBoard().getRegionMap()[2][5];

    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][6], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][5], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][4], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][5], regionID);
    BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][5], regionID);

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 3);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 2); 
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsInRevolt(), true); 
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsAtWar(), false); 


    // Create attack action
    PlayAttack action(REVOLT, {1, 5}, support, 0, KING);

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_ATTACK);

    // Execute
    action.execute(state);

    // Check attack supporters
    BOOST_CHECK_EQUAL(action.getSupporters(), support + 2);

    // Check if supporter was removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getTilesInHand().size(), state::HAND_LIMIT - support);

    for(auto t: state.getPlayers()[0].getTilesInHand()) {
      BOOST_CHECK_NE(t.getType(), TEMPLE);
    }


    // Count defender supporters
    int defSupport = 0;

    for(auto s: state.getPlayers()[1].getTilesInHand()) {
      if(s.getType() == TEMPLE) {
        defSupport++;
      }
    }

    // Create defense action
    PlayDefense action2(action.getConflictType(), action.getPosition(), action.getSupporters(), defSupport, 1, action.getWarLeaderType());

    // Check action features
    BOOST_CHECK_EQUAL(action2.getActionID(), ACTION_ID_DEFENSE);
   
    // Execute
    action2.execute(state);

    // Check if defender supporters were removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getTilesInHand().size(), state::HAND_LIMIT - defSupport);

    for(auto t: state.getPlayers()[1].getTilesInHand()) {
      BOOST_CHECK_NE(t.getType(), TEMPLE);
    }

    // If defender wins
    if(defSupport + 1 >= action.getSupporters()) {

      // Check if attacker leader is back to his hand
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 4);
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[KING].getType(), KING);

      // Check if defender won a point
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getVictoryPoints()[RED], 1);

      // Check if leader was removed from the board
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][6], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][5], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][4], SETTLEMENT);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][5], LAND);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][5], LEADER);

      regionID = state.getBoard().getRegionMap()[2][5];

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][5], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][4], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][5], NO_REGION_ID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][5], regionID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsKingdom(), true);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsInRevolt(), false); 
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsAtWar(), false); 

    }
    // If attacker wins
    else {

      // Check if defender leader is back to his hand
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 4);
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand()[KING].getType(), KING);

      // Check if attacker won a point
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getVictoryPoints()[RED], 1);

      // Check if leader was removed from the board
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][6], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[2][5], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][4], SETTLEMENT);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[1][5], LEADER);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[3][5], LAND);

      regionID = state.getBoard().getRegionMap()[2][5];

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][6], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[2][5], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][5], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][5], NO_REGION_ID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsKingdom(), true);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsInRevolt(), false); 
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsAtWar(), false); 

    }



    // War

    // Tile to leader map
    std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
    
    // Add leaders 
    state::Leader leader_1 = state.getPlayers()[0].getLeadersInHand()[tileToLeaderMap[state.getPlayers()[0].getTilesInHand()[0].getType()]];
    state::Leader leader_2 = state.getPlayers()[1].getLeadersInHand()[tileToLeaderMap[state.getPlayers()[0].getTilesInHand()[0].getType()]];



    // Check throws
    // Not active player action
    
 
  }

}