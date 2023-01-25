#include <boost/test/unit_test.hpp>
#include "../../src/shared/constants.h"

#include <engine.h>
#include <state.h>

using namespace engine;

BOOST_AUTO_TEST_CASE(TestEngine) {
    
  {

    Engine engine;
    
    engine.init();

    for(auto score: engine.getFinalScore())
        BOOST_CHECK_EQUAL(score, STD_FINAL_SCORE);

    engine.enableRecord();

    state::Position pos = {4, 2};
    std::shared_ptr<PlayCatastrophe> action = std::make_shared<PlayCatastrophe>(pos, 0);

    engine.play(action, false);

    Json::Value serialAction = engine.getLastPlayedAction();

    BOOST_CHECK_EQUAL(serialAction["actionID"], ACTION_ID_CATASTRO);
    BOOST_CHECK_EQUAL(serialAction["playerID"], 0);
    BOOST_CHECK_EQUAL(serialAction["position_i"], 4);
    BOOST_CHECK_EQUAL(serialAction["position_j"], 2);

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], LAND);

    engine.update();

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], CATASTRO);

    std::vector<std::shared_ptr<Action>> log = engine.getActionsLog();
    BOOST_CHECK_EQUAL(log.size(), 1);
    BOOST_CHECK_EQUAL(log[0]->getActionID(), ACTION_ID_CATASTRO);
    BOOST_CHECK_EQUAL(log[0]->getPlayerID(), 0);

    Json::Value record = engine.getRecord();

    BOOST_CHECK_EQUAL(record[JSON_ARRAY][0]["actionID"], ACTION_ID_CATASTRO);
    BOOST_CHECK_EQUAL(record[JSON_ARRAY][0]["playerID"], 0);
    BOOST_CHECK_EQUAL(record[JSON_ARRAY][0]["position_i"], 4);
    BOOST_CHECK_EQUAL(record[JSON_ARRAY][0]["position_j"], 2);

    pos = {7, 12};
    std::shared_ptr<PlayCatastrophe> action2 = std::make_shared<PlayCatastrophe>(pos, 0);

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], RIVER);

    engine.play(action2, false);
    engine.clearAction();
    engine.update();

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], RIVER);

    engine.play(action2, false);
    engine.update();

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], CATASTRO);

    log = engine.getActionsLog();
    BOOST_CHECK_EQUAL(log.size(), 2);

    engine.rollback();

    BOOST_CHECK_EQUAL(engine.getState().getBoard().getBoardStateMap()[pos.i][pos.j], RIVER);
    log = engine.getActionsLog();
    BOOST_CHECK_EQUAL(log.size(), 1);

    Engine engine2(4);
    engine2 = engine;
    Engine engine3(engine);

    BOOST_CHECK_EQUAL(engine.turnNotReached(1), true);
    BOOST_CHECK_EQUAL(engine.turnNotReached(0), false);

    BOOST_CHECK_EQUAL(engine.getDefensePendent(), false);
    BOOST_CHECK_EQUAL(engine.getAttackPendent(), false);
    BOOST_CHECK_EQUAL(engine.getMonumentPendent(), false);

    BOOST_CHECK_EQUAL(engine.gameNotOver(), true);
    engine.finishGame();
    BOOST_CHECK_EQUAL(engine.gameNotOver(), false);


    Engine engine4;

    engine4.init();

    Json::Value rec = engine.getRecord();

    engine4.setRandomConditions(rec[JSON_INITIAL]);
    engine4.enablePlayingRecord(rec[JSON_RANDOM_ARRAY]);

    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[4][2], LAND);
    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[7][12], RIVER);

    std::shared_ptr<PlayCatastrophe> action3 = std::make_shared<PlayCatastrophe>();
    action3->deserialize(rec[JSON_ARRAY][0]);
    engine4.play(action3);

    engine4.update();

    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[4][2], CATASTRO);
    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[7][12], RIVER);

    std::shared_ptr<PlayCatastrophe> action4 = std::make_shared<PlayCatastrophe>();
    action4->deserialize(rec[JSON_ARRAY][1]);
    engine4.play(action4);

    engine4.update();

    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[4][2], CATASTRO);
    BOOST_CHECK_EQUAL(engine4.getState().getBoard().getBoardStateMap()[7][12], CATASTRO);

    BOOST_CHECK_EQUAL(engine4.evaluateState(0), 0);
    BOOST_CHECK_EQUAL(engine4.evaluateState(1), 0);

    BOOST_CHECK_EQUAL(engine4.gameNotOver(), true);

    engine4.finishGame();

    BOOST_CHECK_EQUAL(engine4.getFinalScore()[0], GAME_INTERRUPTED);
    BOOST_CHECK_EQUAL(engine4.getFinalScore()[1], STD_FINAL_SCORE);

    BOOST_CHECK_EQUAL(engine4.gameNotOver(), false);

    Json::Value endGameValues;
    endGameValues["RemainingTiles"][FARM] = 0;
    endGameValues["RemainingTiles"][TEMPLE] = 0;
    endGameValues["RemainingTiles"][SETTLEMENT] = 0;
    endGameValues["RemainingTiles"][MARKET] = 0;

    engine4.setRandomConditions(endGameValues);

    pos = {6, 1};
    std::shared_ptr<PlayCatastrophe> action5 = std::make_shared<PlayCatastrophe>(pos, 1);
    engine4.play(action5);
    engine4.update();

    pos = {2, 1};
    std::shared_ptr<PlayCatastrophe> action6 = std::make_shared<PlayCatastrophe>(pos, 1);
    engine4.play(action6);
    engine4.update();

    BOOST_CHECK_EQUAL(engine4.getState().getRemainingTiles()[FARM], 0);
    BOOST_CHECK_EQUAL(engine4.getState().getRemainingTiles()[TEMPLE], 0);
    BOOST_CHECK_EQUAL(engine4.getState().getRemainingTiles()[SETTLEMENT], 0);
    BOOST_CHECK_EQUAL(engine4.getState().getRemainingTiles()[MARKET], 0);

    BOOST_CHECK_EQUAL(engine4.getFinalScore()[0], 0);
    BOOST_CHECK_EQUAL(engine4.getFinalScore()[1], 0);

    BOOST_CHECK_EQUAL(engine4.evaluateState(0), 0);
    BOOST_CHECK_EQUAL(engine4.evaluateState(1), 0);

  }

  {

    Engine engine;
    engine.init();

    state::State state = engine.getState();
    state::Board board = state.getBoard();
    
    state::Tile temple(TEMPLE, {NOT_IN_MAP_ID, NOT_IN_MAP_ID});
    state::Monument monument({NOT_IN_MAP_ID, NOT_IN_MAP_ID}, GREEN, RED);

    board.addTileToTheBoard(temple, {0, 11});
    board.addTileToTheBoard(temple, {1, 11});
    board.addTileToTheBoard(temple, {1, 10});

    board.addMonumentToTheBoard(monument, {0, 10});

    board.addTileToTheBoard(temple, {2, 11});
    board.addTileToTheBoard(temple, {3, 11});
    board.addTileToTheBoard(temple, {3, 12});
    board.addTileToTheBoard(temple, {4, 12});

    state.setBoard(board);
    engine.setState(state);

    state::Position pos = {5, 13};
    state::Leader leader(TRADER, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, 0);
    std::shared_ptr<PlayLeader> action = std::make_shared<PlayLeader>(leader, pos, 0);

    engine.play(action);
    engine.update();

    pos = {0, 0};
    std::shared_ptr<PlayCatastrophe> action2 = std::make_shared<PlayCatastrophe>(pos, 0);

    engine.play(action2);
    engine.update();
    
    BOOST_CHECK_EQUAL(engine.evaluateState(0), 4);

        
    BOOST_CHECK_EQUAL(engine.gameNotOver(), true);

    Json::Value endGameValues;
    endGameValues["RemainingTiles"][FARM] = 0;
    endGameValues["RemainingTiles"][TEMPLE] = 0;
    endGameValues["RemainingTiles"][SETTLEMENT] = 0;
    endGameValues["RemainingTiles"][MARKET] = 0;

    engine.setRandomConditions(endGameValues);

    pos = {6, 1};
    std::shared_ptr<PlayCatastrophe> action3 = std::make_shared<PlayCatastrophe>(pos, 1);
    engine.play(action3);
    engine.update();

    pos = {2, 1};
    std::shared_ptr<PlayCatastrophe> action4 = std::make_shared<PlayCatastrophe>(pos, 1);
    engine.play(action4);
    engine.update();

    BOOST_CHECK_EQUAL(engine.gameNotOver(), false);

    BOOST_CHECK_EQUAL(engine.getFinalScore()[0], 1);
    BOOST_CHECK_EQUAL(engine.getFinalScore()[1], 0);

    BOOST_CHECK_EQUAL(engine.evaluateState(0), 0);
    BOOST_CHECK_EQUAL(engine.evaluateState(1), 0);

  }

}