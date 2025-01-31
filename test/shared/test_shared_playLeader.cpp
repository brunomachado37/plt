#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayLeader.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayLeader) {
    
  {

    // Create a game state
    state::State state;
    state.init();

    // Take a leader
    state::Leader leader = state.getPlayers()[0].getLeadersInHand()[KING];

    // Create action
    PlayLeader action(leader, {5, 8}, 0); 

    // Check action features
    BOOST_CHECK_EQUAL(action.getActionID(), ACTION_ID_LEADER);

    // Check execute methode
    action.execute(state);

    // Leader in board state map
    BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][8], LEADER);

    // Check if leader is in region
    int regionID = state.getBoard().getRegionMap()[5][8];
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getType(), KING);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPlayerID(), 0);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().i, 5);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders()[0].getPosition().j, 8);

    // Check if leader was removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 3);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[TRADER].getType(), TRADER);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[PRIEST].getType(), PRIEST);
    BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[FARMER].getType(), FARMER);
    
    // Check throws
    // Not active player action
    PlayLeader action2(state.getPlayers()[1].getLeadersInHand()[FARMER], {1, 0}, 1); 
    BOOST_CHECK_THROW(action2.execute(state), state::StateException);
 
    // Position occupied
    PlayLeader action3(state.getPlayers()[0].getLeadersInHand()[FARMER], {5, 8}, 0); 
    BOOST_CHECK_THROW(action3.execute(state), state::StateException);

    // Position occupied
    PlayLeader action4(state.getPlayers()[0].getLeadersInHand()[FARMER], {1, 1}, 0); 
    BOOST_CHECK_THROW(action4.execute(state), state::StateException);

    // Leader already on the board
    PlayLeader action7(state.getPlayers()[0].getLeadersInHand()[KING], {1, 0}, 0); 
    BOOST_CHECK_THROW(action7.execute(state), state::StateException);

  }

  {
    // Serialize / deserialize

    state::Leader leader(FARMER, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, 1);

    PlayLeader action(leader, {4, 8}, 1);
    Json::Value serialAction = action.serialize();

    BOOST_CHECK_EQUAL(serialAction["actionID"], ACTION_ID_LEADER);
    BOOST_CHECK_EQUAL(serialAction["playerID"], 1);
    BOOST_CHECK_EQUAL(serialAction["leader_type"], FARMER);
    BOOST_CHECK_EQUAL(serialAction["position_i"], 4);
    BOOST_CHECK_EQUAL(serialAction["position_j"], 8);
    
    PlayLeader deserializedAction;
    deserializedAction.deserialize(serialAction);

    BOOST_CHECK_EQUAL(deserializedAction.getActionID(), ACTION_ID_LEADER);
    BOOST_CHECK_EQUAL(deserializedAction.getPlayerID(), 1);
    BOOST_CHECK_EQUAL(deserializedAction.getPosition().i, 4);
    BOOST_CHECK_EQUAL(deserializedAction.getPosition().j, 8);

  }

}