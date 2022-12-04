#include <boost/test/unit_test.hpp>
#include "../../src/shared/state/Board.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestBoard) {
    
  {
    // Constructor
    Board board;

    BOOST_CHECK_EQUAL(board.getRegions().size(), 0);
    BOOST_CHECK_EQUAL(board.getMonuments().size(), 6);

    for(auto line: board.getBoardStateMap()) {
      for(auto type: line) {
        BOOST_CHECK(type == "land" || type == "river");
      }
    }

    for(auto line: board.getRegionMap()) {
      for(auto regionID: line) {
        BOOST_CHECK_EQUAL(regionID, -1);
      }
    }


    // Board initialization
    board.init();

    BOOST_CHECK_EQUAL(board.getRegions().size(), 10);

    std::vector<Position> initialTemples{{0, 10}, {2, 5}, {4, 13}, {6, 8}, {9, 5}, {10, 10}, {1, 1}, {1, 15}, {7, 1}, {8, 14}};
    std::vector<std::vector<std::string>> boardStateMap = board.getBoardStateMap();
    std::vector<std::vector<int>> regionMap = board.getRegionMap();
    int count = 0;

    for(auto pos: initialTemples) {
      BOOST_CHECK(boardStateMap[pos.i][pos.j] == "temple");
      BOOST_CHECK_EQUAL(regionMap[pos.i][pos.j], count);
      count++;
    }


    // Tiles
    // Add tile with no adjacent tiles
    Tile tile_settlement("settlement", {-1, -1});
    Tile tile_market("market", {-1, -1});
    Tile tile_farm("farm", {-1, -1});
    Tile tile_temple("temple", {-1, -1});

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[0][0], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[0][0], -1);

    board.addTileToTheBoard(tile_settlement, {0, 0});

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[0][0], "settlement");
    BOOST_CHECK_EQUAL(board.getRegionMap()[0][0], 10);

    BOOST_CHECK_EQUAL(board.getRegions()[10].getTiles()[0].getType(), "settlement");
    BOOST_CHECK_EQUAL(board.getRegions()[10].getTiles()[0].getPosition().i, 0);
    BOOST_CHECK_EQUAL(board.getRegions()[10].getTiles()[0].getPosition().j, 0);


    // Add not a farm tile to a river space
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_market, {3, 0}), std::invalid_argument);
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_settlement, {3, 3}), std::invalid_argument);
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_temple, {6, 0}), std::invalid_argument);

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][0], "river");
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][0], -1);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][3], "river");
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][3], -1);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[6][0], "river");
    BOOST_CHECK_EQUAL(board.getRegionMap()[6][0], -1);


    // Add farm tile to a land space
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_farm, {0, 3}), std::invalid_argument);

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[0][3], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[0][3], -1);


    // Add tile with 1 tile adjacent (add to existent region)
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][2], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][2], -1);

    board.addTileToTheBoard(tile_market, {1, 2});

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][2], "market");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][2], 6);

    BOOST_CHECK_EQUAL(board.getRegions()[6].getTiles().size(), 2);

    // Add another tile to existent region
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][3], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][3], -1);

    board.addTileToTheBoard(tile_temple, {1, 3});

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][3], "temple");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][3], 6);

    BOOST_CHECK_EQUAL(board.getRegions()[6].getTiles().size(), 3);

    // Add farm to existent region
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[2][3], "river");
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][3], -1);

    board.addTileToTheBoard(tile_farm, {2, 3});

    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[2][3], "farm");
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][3], 6);

    BOOST_CHECK_EQUAL(board.getRegions()[6].getTiles().size(), 4);

    // Add farm with no adjacent tiles
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][0], "river");
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][0], -1);

    board.addTileToTheBoard(tile_farm, {3, 0});

    BOOST_CHECK_EQUAL(board.getRegions().size(), 12);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][0], "farm");
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][0], 11);

    // Add tile to already occupied position
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_farm, {3, 0}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 12);
    
    // Add tile with 2 adjacent regions
    board.addTileToTheBoard(tile_market, {10, 15});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 13);

    board.addTileToTheBoard(tile_settlement, {8, 15});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 13);

    board.addTileToTheBoard(tile_temple, {9, 15});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 12);

    BOOST_CHECK_EQUAL(board.getRegions()[13].getRegionID(), 13);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 13);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 13);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 13);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 13);
    BOOST_CHECK(board.getBoardStateMap()[10][15] == "market" && board.getBoardStateMap()[8][15] == "settlement" && board.getBoardStateMap()[9][15] == "temple" && board.getBoardStateMap()[8][14] == "temple");

    BOOST_CHECK_EQUAL(board.getRegions()[13].getTiles().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getTreasures().size(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 0);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getMonuments().size(), 0);


    // Add tile with 3 adjacent regions
    board.addTileToTheBoard(tile_farm, {4, 15});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 13);

    board.addTileToTheBoard(tile_farm, {5, 14});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 14);

    board.addTileToTheBoard(tile_farm, {4, 14});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 12);

    BOOST_CHECK_EQUAL(board.getRegions()[16].getRegionID(), 16);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 16);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 16);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 16);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 16);
    BOOST_CHECK(board.getBoardStateMap()[4][15] == "farm" && board.getBoardStateMap()[5][14] == "farm" && board.getBoardStateMap()[4][14] == "farm" && board.getBoardStateMap()[4][13] == "temple");

    BOOST_CHECK_EQUAL(board.getRegions()[16].getTiles().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getIsKingdom(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getTreasures().size(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getLeaders().size(), 0);
    BOOST_CHECK_EQUAL(board.getRegions()[16].getMonuments().size(), 0);

    // Add tile with 4 adjacent regions
    board.addTileToTheBoard(tile_farm, {8, 8});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 13);

    board.addTileToTheBoard(tile_market, {7, 7});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 14);

    board.addTileToTheBoard(tile_temple, {7, 9});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 15);

    board.addTileToTheBoard(tile_settlement, {7, 8});
    BOOST_CHECK_EQUAL(board.getRegions().size(), 12);

    BOOST_CHECK_EQUAL(board.getRegions()[20].getRegionID(), 20);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][8], 20);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][7], 20);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][9], 20);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][8], 20);
    BOOST_CHECK_EQUAL(board.getRegionMap()[6][8], 20);
    BOOST_CHECK(board.getBoardStateMap()[8][8] == "farm" && board.getBoardStateMap()[7][7] == "market" && board.getBoardStateMap()[7][9] == "temple" && board.getBoardStateMap()[7][8] == "settlement" && board.getBoardStateMap()[6][8] == "temple");

    BOOST_CHECK_EQUAL(board.getRegions()[20].getTiles().size(), 5);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getIsKingdom(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getTreasures().size(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getLeaders().size(), 0);
    BOOST_CHECK_EQUAL(board.getRegions()[20].getMonuments().size(), 0);


    // Removing tiles
    // Try to remove an tile that doesn't exist
    BOOST_CHECK_THROW(board.removeTileFromTheBoard({2, 9}), std::invalid_argument);

    // Remove tile from the board
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][1], "temple");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][1], 6);
    BOOST_CHECK_EQUAL(board.getRegions()[6].getTiles().size(), 4);

    board.removeTileFromTheBoard({1, 1});

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][1], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][1], -1);
    BOOST_CHECK_EQUAL(board.getRegions()[6].getTiles().size(), 3);


    // Leaders
    Leader king_1("king", {-1, -1}, 1);
    Leader priest_1("priest", {-1, -1}, 1);
    Leader trader_1("trader", {-1, -1}, 1);
    Leader farmer_1("farmer", {-1, -1}, 1);

    Leader king_2("king", {-1, -1}, 2);
    Leader priest_2("priest", {-1, -1}, 2);
    Leader trader_2("trader", {-1, -1}, 2);
    Leader farmer_2("farmer", {-1, -1}, 2);

    // Add a leader with no adjacent region
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(king_1, {3, 10}), std::invalid_argument);

    // Add a leader with no adjacent temple
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(king_1, {7, 15}), std::invalid_argument);

    // Add a leader to a river
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(farmer_1, {3, 13}), std::invalid_argument);

    // Add a leader with 1 adjacent temple
    board.addLeaderToTheBoard(king_1, {7, 14});
    
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 1);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[13].getLeaders()[0], {0, 0}), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[7][14], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][14], 13);


    // Add a leader with 2 adjacent temples
    board.addLeaderToTheBoard(priest_2, {9, 14});
    
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[13].getLeaders()[1], {0, 0}), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[9][14], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][14], 13);


    // Add a leader of the same type from the same player
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(king_1, {8, 13}), std::logic_error);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);


    // Add a leader of the same type to a kingdom
    board.addLeaderToTheBoard(king_2, {8, 13});

    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[13].getLeaders()[2], {0, 0}), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][13], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][13], 13); 


    // Try to remove leader that is not in the board
    BOOST_CHECK_THROW(board.removeLeaderFromTheBoard(1, "farmer"), std::invalid_argument);
    BOOST_CHECK_THROW(board.removeLeaderFromTheBoard(1, "priest"), std::invalid_argument);


    // Remove leader from the board
    int regionID = board.getRegionMap()[8][13];
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);

    board.removeLeaderFromTheBoard(2, "king");

    regionID = board.getRegionMap()[8][14];
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][13], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][13], -1); 


    // Add leader to an position occupied by a tile
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(farmer_1, {8, 14}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);

    // Add leader to an position occupied by a leader
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(priest_1, {7, 14}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);


    // Add tile uniting 1 kingdom to a region
    board.addTileToTheBoard(tile_farm, {6, 14});
    regionID = board.getRegionMap()[6][14];

    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {0, 0}), 1);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {0, 0}), 3);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getRegionID(), regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 9);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 1 kingdom to 2 regions
    board.addTileToTheBoard(tile_settlement, {10, 13});
    board.addTileToTheBoard(tile_temple, {9, 12});
    board.addTileToTheBoard(tile_settlement, {9, 13});
    regionID = board.getRegionMap()[9][13];

    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {0, 0}), 3);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {0, 0}), 4);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getRegionID(), regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 12);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 1 kingdom to 3 regions
    board.addTileToTheBoard(tile_temple, {3, 12});
    board.addTileToTheBoard(tile_temple, {4, 11});
    board.addTileToTheBoard(tile_market, {5, 12});
    board.addTileToTheBoard(tile_settlement, {4, 12});
    regionID = board.getRegionMap()[4][12];

    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {0, 0}), 4);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {0, 0}), 6);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 16);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 2 kingdoms without war
    board.addLeaderToTheBoard(trader_1, {9, 10});
    board.addTileToTheBoard(tile_market, {9, 11});
    regionID = board.getRegionMap()[9][11];

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {0, 0}), 3);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {0, 0}), 4);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[2], {0, 0}), 7);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][11], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 18);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, 9);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, 11);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 2 kingdoms with war
    board.addLeaderToTheBoard(trader_2, {7, 10});
    board.addTileToTheBoard(tile_farm, {8, 10});
    regionID = board.getRegionMap()[8][10];

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 4);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {8, 10}), 1);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {8, 10}), 3);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[2], {8, 10}), 4);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[3], {8, 10}), 7);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][11], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][10], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][9], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][8], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][7], regionID); 
    BOOST_CHECK_EQUAL(board.getRegionMap()[6][8], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][8], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 24);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, 8);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, 10);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 3 kingdoms
    board.addLeaderToTheBoard(farmer_1, {8, 5});
    board.addTileToTheBoard(tile_farm, {8, 6});

    board.addTileToTheBoard(tile_temple, {9, 7});
    board.addLeaderToTheBoard(priest_1, {10, 7});
    
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_temple, {8, 7}), std::invalid_argument);

    BOOST_CHECK_EQUAL(board.getRegionMap()[8][7], -1);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][7], "river");


    // Add tile uniting 3 regions with 2 kingdoms and war
    board.addTileToTheBoard(tile_farm, {0, 4});
    board.addTileToTheBoard(tile_temple, {1, 1});
    board.addLeaderToTheBoard(king_2, {2, 1});

    Leader king_3("king", {-1, -1}, 3);
    board.addLeaderToTheBoard(king_3, {1, 5});

    board.addTileToTheBoard(tile_farm, {1, 4});
    regionID = board.getRegionMap()[1][4];

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {1, 4}), 0);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {1, 4}), 0);

    BOOST_CHECK_EQUAL(board.getRegionMap()[1][1], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][2], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][3], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][4], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[0][4], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][1], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][3], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 7);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, 1);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, 4);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Add tile uniting 4 regions with 2 kingdoms and war
    board.addTileToTheBoard(tile_temple, {4, 6});
    Leader trader_3("trader", {-1, -1}, 3);
    board.addLeaderToTheBoard(trader_3, {4, 5});

    board.addTileToTheBoard(tile_temple, {4, 8});
    Leader trader_4("trader", {-1, -1}, 4);
    board.addLeaderToTheBoard(trader_4, {4, 9});

    board.addTileToTheBoard(tile_market, {3, 7});
    board.addTileToTheBoard(tile_market, {5, 7});
    board.addTileToTheBoard(tile_market, {4, 7});
    regionID = board.getRegionMap()[4][7];

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[0], {4, 7}), 0);
    BOOST_CHECK_EQUAL(board.checkLeaderStrength(board.getRegions()[regionID].getLeaders()[1], {4, 7}), 0);

    BOOST_CHECK_EQUAL(board.getRegionMap()[4][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][6], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][7], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][8], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][9], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][7], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][7], regionID);

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 5);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsAtWar(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().i, 4);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getUnificationTilePosition().j, 7);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTreasures().size(), 0);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 0);


    // Check possible monuments
    BOOST_CHECK_EQUAL(board.getPossibleMonuments().size(), 0);

    board.addTileToTheBoard(tile_farm, {3, 15});
    board.addTileToTheBoard(tile_farm, {3, 14});

    BOOST_CHECK_EQUAL(board.getPossibleMonuments().size(), 1);


    // Add monument to invalid position
    BOOST_CHECK_THROW(board.addMonumentToTheBoard(board.getMonuments()[1], {8, 14}), std::logic_error);

    // Add monument to valid position
    BOOST_CHECK_EQUAL(board.getMonuments().size(), 6);

    board.addMonumentToTheBoard(board.getMonuments()[1], {3, 14});
    regionID = board.getRegionMap()[3][14];

    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getMonuments().size(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[regionID].getTiles().size(), 22);

    BOOST_CHECK_EQUAL(board.getMonuments().size(), 5);

    BOOST_CHECK_EQUAL(board.getRegionMap()[3][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][15], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][14], "monument");
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[4][14], "monument");
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][15], "monument");
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[4][15], "monument");

    // Catastrophes
    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 0);

    // Add catastrophe in a leader
    BOOST_CHECK_THROW(board.addCatastropheToTheBoard({9, 14}), std::invalid_argument);

    // Add catastrophe in a monument
    BOOST_CHECK_THROW(board.addCatastropheToTheBoard({3, 14}), std::invalid_argument);


    // Add catastrophe in a treasure
    BOOST_CHECK_THROW(board.addCatastropheToTheBoard({8, 14}), std::invalid_argument);


    // Add catastrophe in empty space
    board.addCatastropheToTheBoard({5, 2});
    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 1);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[5][2], "catastrophe");
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][2], -1);


    // Add catastrophe in a catastrophe
    BOOST_CHECK_THROW(board.addCatastropheToTheBoard({5, 2}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 1);


    // Add catastrophe that destroys region and remove leader
    board.addTileToTheBoard(tile_temple, {10, 1});
    board.addLeaderToTheBoard(king_3, {10, 0});
    regionID = board.getRegionMap()[10][0];

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[10][1], "temple");
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[10][0], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][1], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][0], regionID);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);


    board.addCatastropheToTheBoard({10, 1});

    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 2);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[10][1], "catastrophe");
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[10][0], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][1], -1);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][0], -1);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 10);


    // Add catastrophe that doesn't break a region
    board.addCatastropheToTheBoard({3, 7});

    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 3);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[3][7], "catastrophe");
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][7], -1);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 10);

    regionID = board.getRegionMap()[4][5];

    BOOST_CHECK_EQUAL(board.getRegionMap()[4][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][6], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][7], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][8], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][9], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][7], regionID);


    // Add catastrophe that breaks region in 2 
    board.addCatastropheToTheBoard({1, 2});

    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 4);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[1][2], "catastrophe");
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][2], -1);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 11);

    regionID = board.getRegionMap()[1][1];

    BOOST_CHECK_EQUAL(board.getRegionMap()[1][1], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][1], regionID);

    regionID = board.getRegionMap()[1][3];

    BOOST_CHECK_EQUAL(board.getRegionMap()[1][3], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][4], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[1][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][5], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[0][4], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[2][3], regionID);


    // Add catastrophe that breaks region in 4
    board.addCatastropheToTheBoard({4, 12});

    BOOST_CHECK_EQUAL(board.getCatastrophes().size(), 5);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[4][12], "catastrophe");
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], -1);
    BOOST_CHECK_EQUAL(board.getRegions().size(), 14);

    regionID = board.getRegionMap()[3][12];
    int regionID2 = board.getRegionMap()[4][11];
    int regionID3 = board.getRegionMap()[5][12];
    int regionID4 = board.getRegionMap()[10][15];

    BOOST_CHECK(regionID != regionID2 && regionID != regionID3 && regionID != regionID4);
    BOOST_CHECK(regionID2 != regionID3 && regionID2 != regionID4);
    BOOST_CHECK(regionID3 != regionID4);

    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], regionID);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], regionID2);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], regionID3);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][10], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][10], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][11], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][10], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][10], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][9], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][8], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][7], regionID4); 
    BOOST_CHECK_EQUAL(board.getRegionMap()[6][8], regionID4);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][8], regionID4);

  }

}