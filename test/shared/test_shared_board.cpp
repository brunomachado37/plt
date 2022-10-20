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
      BOOST_CHECK(boardStateMap[pos.i][pos.j] == "temple" || boardStateMap[pos.i][pos.j] == "specialTemple");
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
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders()[0].getStrength(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[7][14], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][14], 13);


    // Add a leader with 2 adjacent temples
    board.addLeaderToTheBoard(priest_2, {9, 14});
    
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders()[1].getStrength(), 2);
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
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders()[2].getStrength(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsInRevolt(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][13], "leader");
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][13], 13); 


    // Try to remove leader that is not in the board
    BOOST_CHECK_THROW(board.removeLeaderFromTheBoard(1, "farmer"), std::invalid_argument);
    BOOST_CHECK_THROW(board.removeLeaderFromTheBoard(1, "priest"), std::invalid_argument);


    // Remove leader from the board
    board.removeLeaderFromTheBoard(2, "king");

    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getIsKingdom(), true);

    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][13], "land");
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][13], -1); 


    // Add leader to an position occupied by a tile
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(farmer_1, {8, 14}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);

    // Add leader to an position occupied by a leader
    BOOST_CHECK_THROW(board.addLeaderToTheBoard(priest_1, {7, 14}), std::invalid_argument);
    BOOST_CHECK_EQUAL(board.getRegions()[13].getLeaders().size(), 2);


    // Add tile uniting 1 kingdom to a region
    board.addTileToTheBoard(tile_farm, {6, 14});

    BOOST_CHECK_EQUAL(board.getRegions()[21].getLeaders()[0].getStrength(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getLeaders()[1].getStrength(), 3);

    BOOST_CHECK_EQUAL(board.getRegions()[21].getRegionID(), 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 21);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 21);

    BOOST_CHECK_EQUAL(board.getRegions()[21].getTiles().size(), 9);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[21].getMonuments().size(), 0);


    // Add tile uniting 1 kingdom to 2 regions
    board.addTileToTheBoard(tile_settlement, {10, 13});
    board.addTileToTheBoard(tile_temple, {9, 12});
    board.addTileToTheBoard(tile_settlement, {9, 13});

    BOOST_CHECK_EQUAL(board.getRegions()[24].getLeaders()[0].getStrength(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getLeaders()[1].getStrength(), 4);

    BOOST_CHECK_EQUAL(board.getRegions()[24].getRegionID(), 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], 24);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], 24);

    BOOST_CHECK_EQUAL(board.getRegions()[24].getTiles().size(), 12);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[24].getMonuments().size(), 0);


    // Add tile uniting 1 kingdom to 3 regions
    board.addTileToTheBoard(tile_temple, {3, 12});
    board.addTileToTheBoard(tile_temple, {4, 11});
    board.addTileToTheBoard(tile_market, {5, 12});
    board.addTileToTheBoard(tile_settlement, {4, 12});

    BOOST_CHECK_EQUAL(board.getRegions()[28].getLeaders()[0].getStrength(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getLeaders()[1].getStrength(), 6);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], 28);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], 28);

    BOOST_CHECK_EQUAL(board.getRegions()[28].getTiles().size(), 16);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getTreasures().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getLeaders().size(), 2);
    BOOST_CHECK_EQUAL(board.getRegions()[28].getMonuments().size(), 0);


    // Add tile uniting 2 kingdoms without war
    board.addLeaderToTheBoard(trader_1, {9, 10});
    board.addTileToTheBoard(tile_market, {9, 11});

    BOOST_CHECK_EQUAL(board.getRegions()[29].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getLeaders()[0].getStrength(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getLeaders()[1].getStrength(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getLeaders()[2].getStrength(), 7);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][10], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][10], 29);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][11], 29);

    BOOST_CHECK_EQUAL(board.getRegions()[29].getTiles().size(), 18);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getIsAtWar(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getUnificationTilePosition().i, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getUnificationTilePosition().j, -2);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getTreasures().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getLeaders().size(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[29].getMonuments().size(), 0);


    // Add tile uniting 2 kingdoms with war
    board.addLeaderToTheBoard(trader_2, {7, 10});
    board.addTileToTheBoard(tile_farm, {8, 10});

    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders()[0].getStrength(), 1);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders()[1].getStrength(), 3);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders()[2].getStrength(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders()[3].getStrength(), 7);

    BOOST_CHECK_EQUAL(board.getRegionMap()[10][15], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][13], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[10][10], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][15], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][15], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][14], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][15], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][14], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][14], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][13], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][12], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][13], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[3][12], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][11], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[5][12], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[4][12], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][10], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[9][11], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][10], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][10], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][9], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][8], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[7][7], 30); 
    BOOST_CHECK_EQUAL(board.getRegionMap()[6][8], 30);
    BOOST_CHECK_EQUAL(board.getRegionMap()[8][8], 30);

    BOOST_CHECK_EQUAL(board.getRegions()[30].getTiles().size(), 24);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getIsAtWar(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getIsInRevolt(), false);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getIsKingdom(), true);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getUnificationTilePosition().i, 8);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getUnificationTilePosition().j, 10);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getTreasures().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getLeaders().size(), 4);
    BOOST_CHECK_EQUAL(board.getRegions()[30].getMonuments().size(), 0);


    // Add tile uniting 3 kingdoms
    board.addLeaderToTheBoard(farmer_1, {8, 5});
    board.addTileToTheBoard(tile_farm, {8, 6});

    board.addTileToTheBoard(tile_temple, {9, 7});
    board.addLeaderToTheBoard(priest_1, {10, 7});
    
    BOOST_CHECK_THROW(board.addTileToTheBoard(tile_temple, {8, 7}), std::invalid_argument);

    BOOST_CHECK_EQUAL(board.getRegionMap()[8][7], -1);
    BOOST_CHECK_EQUAL(board.getBoardStateMap()[8][7], "river");

    // TODO
    // Add tile uniting 3 regions with 2 kingdoms 
    // Add tile uniting 4 kingdoms
    // Add tile uniting 4 regions with 2 kingdoms

  }

}