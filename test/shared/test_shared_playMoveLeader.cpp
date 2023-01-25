#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayMoveLeader.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayMoveLeader) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    // Take a leader
    state::Leader leader = state.getPlayers()[0].getLeadersInHand()[KING];

    // Add it to the board
    state::Board board = state.getBoard();
    board.addLeaderToTheBoard(leader, {5, 8});
    state.setBoard(board);

    // Remove from player's hand
    state::Player player = state.getPlayers()[0];
    player.removeLeaderFromHand(leader.getType());
    state.setPlayer(player);

    // Leader in board state map
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][8], LEADER);

    // Check if leader is in region
    int regionID = state.getBoard().getRegionMap()[5][8];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getType(), KING);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().i, 5);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().j, 8);

    // Create action
    PlayMoveLeader action(leader.getType(), {0, 1}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_MOVE_LEADER);

    // Check execute methode
    action.execute(state);

    // Leader in board state map
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][1], LEADER);

    // Check if leader is in region
    regionID = state.getBoard().getRegionMap()[0][1];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getType(), KING);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().i, 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().j, 1);

    // Check if leader was removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[TRADER].getType(), TRADER);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[PRIEST].getType(), PRIEST);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[FARMER].getType(), FARMER);

    // Check if leader was removed from previous region
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][8], LAND);
    regionID = state.getBoard().getRegionMap()[6][8];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0);
    
    // Check throws
    // Not active player action
    PlayMoveLeader action2(FARMER, {0, 1}, 1); 
    BOOST_CHECK_THROW(action2.execute(state), std::invalid_argument);
 
    // Position occupied
    PlayMoveLeader action3(leader.getType(), {1, 1}, 0); 
    BOOST_CHECK_THROW(action3.execute(state), std::invalid_argument);

    // Leader not in the board
    PlayMoveLeader action4(FARMER, {1, 0}, 0); 
    BOOST_CHECK_THROW(action4.execute(state), std::invalid_argument);


    // Test add leader to the hand
    PlayMoveLeader action5(leader.getType(), {-1, -1}, 0);

    // Check execute methode
    action5.execute(state);

    // Check if leader was removed from the board
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[0][1], LAND);
    regionID = state.getBoard().getRegionMap()[1][1];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 0);

    // Check if leader was added to player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 4);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[KING].getType(), KING);

  }

  {
    // Serialize / deserialize

    PlayMoveLeader action(KING, {7, 1}, 0);
    Json::Value serialAction = action.serialize();

    BOOST_CHECK_EQUAL(serialAction["actionID"], ACTION_ID_MOVE_LEADER);
    BOOST_CHECK_EQUAL(serialAction["playerID"], 0);
    BOOST_CHECK_EQUAL(serialAction["leaderType"], KING);
    BOOST_CHECK_EQUAL(serialAction["position_i"], 7);
    BOOST_CHECK_EQUAL(serialAction["position_j"], 1);
    
    PlayMoveLeader deserializedAction;
    deserializedAction.deserialize(serialAction);

    BOOST_CHECK_EQUAL(deserializedAction.getActionID(), ACTION_ID_MOVE_LEADER);
    BOOST_CHECK_EQUAL(deserializedAction.getPlayerID(), 0);
    BOOST_CHECK_EQUAL(deserializedAction.getPosition().i, 7);
    BOOST_CHECK_EQUAL(deserializedAction.getPosition().j, 1);

  }

}