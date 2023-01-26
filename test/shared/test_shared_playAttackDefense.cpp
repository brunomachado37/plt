#include <boost/test/unit_test.hpp>
#include "../../src/shared/engine/PlayAttack.h"
#include "../../src/shared/engine/PlayDefense.h"
#include "../../src/shared/constants.h"
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestPlayAttackDefense) {
    
  {
    // Revolt

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
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[1][5], state::NO_REGION_ID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][5], regionID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 

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
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[3][5], state::NO_REGION_ID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 
      
    }

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsInRevolt(), false); 
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsAtWar(), false);
     
  }

  {
    // War

    state::State state;
    state.init();

    state::Board board = state.getBoard();
    state::Player player1 = state.getPlayers()[0];
    state::Player player2 = state.getPlayers()[1];

    // Tile to leader map
    std::unordered_map<std::string, std::string> tileToLeaderMap = {{FARM, FARMER}, {TEMPLE, PRIEST}, {MARKET, TRADER}, {SETTLEMENT, KING}};
    
    // Add leaders 
    state::Leader leader1 = player1.getLeadersInHand()[PRIEST];
    state::Leader leader2 = player2.getLeadersInHand()[PRIEST];

    board.addLeaderToTheBoard(leader1, {5, 8});
    board.addLeaderToTheBoard(leader2, {4, 12});

    player1.removeLeaderFromHand(leader1.getType());
    player2.removeLeaderFromHand(leader2.getType());

    // Add tiles
    state::Tile temple(TEMPLE, {-1, -1});

    board.addTileToTheBoard(temple, {5, 9});
    board.addTileToTheBoard(temple, {5, 10});
    board.addTileToTheBoard(temple, {5, 11});

    // Trigger war
    board.addTileToTheBoard(temple, {4, 11});

    // Region checks
    int regionID = board.getRegionMap()[4][11];
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 6);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2); 
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false); 
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), true); 

    // Count attacker supporters
    int support = 0;

    for(auto tile: player1.getTilesInHand()) {
      if(tile.getType() == TEMPLE) {
        support++;
      }
    }

    state.setPlayer(player1);
    state.setPlayer(player2);
    state.setBoard(board);

    PlayAttack attack(WAR, {4, 11}, support, 0, PRIEST);

    attack.execute(state);
    
    // Check attack supporters
    BOOST_CHECK_EQUAL(attack.getSupporters(), support + 4);

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
    PlayDefense defense(attack.getConflictType(), attack.getPosition(), attack.getSupporters(), defSupport, 1, attack.getWarLeaderType());

    defense.execute(state);

    // Check if defender supporters were removed from player's hand
    BOOST_CHECK_EQUAL(state.getPlayers()[1].getTilesInHand().size(), state::HAND_LIMIT - defSupport);

    for(auto t: state.getPlayers()[1].getTilesInHand()) {
      BOOST_CHECK_NE(t.getType(), TEMPLE);
    }

    // If defender wins
    if(defSupport + 1 >= attack.getSupporters()) {

  	  // Check if attacker leader is back to his hand
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand().size(), 4);
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getLeadersInHand()[PRIEST].getType(), PRIEST);

      // Check if defender won the respective points
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getVictoryPoints()[RED], 4);

      // Check if leader and tiles were removed from the board
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][8], LAND);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[6][8], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][9], LAND);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][10], LAND);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][11], LAND);

      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][12], LEADER);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][11], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][13], TEMPLE);

      regionID = state.getBoard().getRegionMap()[4][11];

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][12], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][11], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][13], regionID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][8], state::NO_REGION_ID);
      BOOST_CHECK_NE(state.getBoard().getRegionMap()[6][8], state::NO_REGION_ID);
      BOOST_CHECK_NE(state.getBoard().getRegionMap()[6][8], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][9], state::NO_REGION_ID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][10], state::NO_REGION_ID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][11], state::NO_REGION_ID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 2);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 

    }
    // If attacker wins
    else {

      // Check if defender leader is back to his hand
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand().size(), 4);
      BOOST_CHECK_EQUAL(state.getPlayers()[1].getLeadersInHand()[PRIEST].getType(), PRIEST);

      // Check if attacker won the respective points
      BOOST_CHECK_EQUAL(state.getPlayers()[0].getVictoryPoints()[RED], 1);

      // Check if leader was removed from the board
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][8], LEADER);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[6][8], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][9], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][10], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[5][11], TEMPLE);

      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][12], LAND);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][11], TEMPLE);
      BOOST_CHECK_EQUAL(state.getBoard().getBoardStateMap()[4][13], TEMPLE);

      regionID = state.getBoard().getRegionMap()[4][11];

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][12], state::NO_REGION_ID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[4][11], regionID);
      BOOST_CHECK_NE(state.getBoard().getRegionMap()[4][13], state::NO_REGION_ID);
      BOOST_CHECK_NE(state.getBoard().getRegionMap()[4][13], regionID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][8], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[6][8], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][9], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][10], regionID);
      BOOST_CHECK_EQUAL(state.getBoard().getRegionMap()[5][11], regionID);

      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getTiles().size(), 5);
      BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getLeaders().size(), 1); 

    }

    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsInRevolt(), false); 
    BOOST_CHECK_EQUAL(state.getBoard().getRegions()[regionID].getIsAtWar(), false); 

  }

  {
    // Check throws

    state::State state;
    state.init();
    state::Board board = state.getBoard();

    PlayAttack activePlayerAttack(WAR, {4, 11}, 0, 1, PRIEST);
    PlayDefense activePlayerDefense(activePlayerAttack.getConflictType(), activePlayerAttack.getPosition(), activePlayerAttack.getSupporters(), 0, 0, activePlayerAttack.getWarLeaderType());

    PlayAttack revoltAttack(REVOLT, {4, 12}, 7, 0, PRIEST);
    PlayDefense revoltDefense(revoltAttack.getConflictType(), revoltAttack.getPosition(), revoltAttack.getSupporters(), 7, 1, revoltAttack.getWarLeaderType());

    PlayAttack warAttack(WAR, {4, 11}, 0, 0, PRIEST);
    PlayDefense warDefense(warAttack.getConflictType(), warAttack.getPosition(), warAttack.getSupporters(), 0, 1, warAttack.getWarLeaderType());  

    // Active player
    BOOST_CHECK_THROW(activePlayerAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(activePlayerDefense.execute(state), state::StateException);

    // Revolt
    BOOST_CHECK_THROW(revoltAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(revoltDefense.execute(state), state::StateException);

    // Add Leaders
    state::Leader leader1(FARMER, {-1, -1}, 0);
    board.addLeaderToTheBoard(leader1, {4, 12});
    state.setBoard(board);

    BOOST_CHECK_THROW(revoltAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(revoltDefense.execute(state), state::StateException);

    state::Leader leader2(FARMER, {-1, -1}, 1);
    board.addLeaderToTheBoard(leader2, {5, 13});
    state.setBoard(board);

    BOOST_CHECK_THROW(revoltAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(revoltDefense.execute(state), state::StateException);


    // War

    BOOST_CHECK_THROW(warAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(warAttack.execute(state), state::StateException);

    // Add Tile
    state::Tile tile(MARKET, {-1, -1});
    board.addTileToTheBoard(tile, {4, 11});
    state.setBoard(board);

    BOOST_CHECK_THROW(revoltAttack.execute(state), state::StateException);
    BOOST_CHECK_THROW(revoltDefense.execute(state), state::StateException);

  }

  {
    // Serialize / deserialize attack

    PlayAttack attack(REVOLT, {2, 5}, 4, 1, KING);
    Json::Value serialAttack = attack.serialize();

    BOOST_CHECK_EQUAL(serialAttack["actionID"], ACTION_ID_ATTACK);
    BOOST_CHECK_EQUAL(serialAttack["playerID"], 1);
    BOOST_CHECK_EQUAL(serialAttack["conflictType"], REVOLT);
    BOOST_CHECK_EQUAL(serialAttack["triggerPosition_i"], 2);
    BOOST_CHECK_EQUAL(serialAttack["triggerPosition_j"], 5);
    BOOST_CHECK_EQUAL(serialAttack["supporters"], 0);
    BOOST_CHECK_EQUAL(serialAttack["additionalSupporters"], 4);
    BOOST_CHECK_EQUAL(serialAttack["warLeaderType"], KING);

    PlayAttack deserializedAttack;
    deserializedAttack.deserialize(serialAttack);

    BOOST_CHECK_EQUAL(deserializedAttack.getActionID(), ACTION_ID_ATTACK);
    BOOST_CHECK_EQUAL(deserializedAttack.getPlayerID(), 1);
    BOOST_CHECK_EQUAL(deserializedAttack.getConflictType(), REVOLT);
    BOOST_CHECK_EQUAL(deserializedAttack.getPosition().i, 2);
    BOOST_CHECK_EQUAL(deserializedAttack.getPosition().j, 5);
    BOOST_CHECK_EQUAL(deserializedAttack.getSupporters(), 4);
    BOOST_CHECK_EQUAL(deserializedAttack.getWarLeaderType(), KING);

  }

  {
    // Serialize / deserialize defense

    PlayDefense defense(WAR, {8,12}, 5, 7, 0, PRIEST);
    Json::Value serialDefense = defense.serialize();

    BOOST_CHECK_EQUAL(serialDefense["actionID"], ACTION_ID_DEFENSE);
    BOOST_CHECK_EQUAL(serialDefense["playerID"], 0);
    BOOST_CHECK_EQUAL(serialDefense["conflictType"], WAR);
    BOOST_CHECK_EQUAL(serialDefense["triggerPosition_i"], 8);
    BOOST_CHECK_EQUAL(serialDefense["triggerPosition_j"], 12);
    BOOST_CHECK_EQUAL(serialDefense["attackerSupporters"], 5);
    BOOST_CHECK_EQUAL(serialDefense["supporters"], 0);
    BOOST_CHECK_EQUAL(serialDefense["additionalSupporters"], 7);
    BOOST_CHECK_EQUAL(serialDefense["warLeaderType"], PRIEST);

    PlayDefense deserializedDefense;
    deserializedDefense.deserialize(serialDefense);

    BOOST_CHECK_EQUAL(deserializedDefense.getActionID(), ACTION_ID_DEFENSE);
    BOOST_CHECK_EQUAL(deserializedDefense.getPlayerID(), 0);
    BOOST_CHECK_EQUAL(deserializedDefense.getPosition().i, 8);
    BOOST_CHECK_EQUAL(deserializedDefense.getPosition().j, 12);

  }

}