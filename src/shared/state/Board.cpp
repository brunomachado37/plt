#include "Board.h"
#include "../constants.h"

#include <iostream>

namespace state {

    Board::Board() {

        std::cout << NUM_LINES << std::endl;

        for(int i; i < NUM_LINES; i++) {
            this->terrainMap.push_back(std::vector<std::string>(NUM_ROWS, LAND));
        }

        std::vector<Position> riverTiles{{0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 12}, {1, 4}, {1, 12}, {2, 3}, {2, 4}, {2, 12}, {2, 13}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 13}, {3, 14}, {3, 15}, {4, 14}, {4, 15}, {5, 14}, {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 12}, {6, 13}, {6, 14}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 12}, {8, 6}, {8, 7}, {8, 8}, {8, 9}, {8, 10}, {8, 11}, {8, 12}};
        std::vector<Position> initialNormalTemples{{0, 10}, {2, 5}, {4, 13}, {6, 8}, {9, 5}, {10, 10}};
        std::vector<Position> initialSpecialTemples{{1, 1}, {1, 15}, {7, 1}, {8, 14}};

        for(auto p : riverTiles) {
            this->terrainMap[p.i][p.j] = RIVER;
        }
        for(auto p : initialNormalTemples) {
            this->terrainMap[p.i][p.j] = TEMPLE;
            //Instantiate a tile -> create a region -> add region to the list
            //Tile temple("temple", false, {p.i, p.j});
            Region region(this->regions.size());
            region.addPiece("tile", "red", {p.i, p.j});

        }
        for(auto p : initialSpecialTemples) {
            this->terrainMap[p.i][p.j] = SPECIAL;
        }

        this->boardStateMap = this->terrainMap;

        std::cout << "Board created" << std::endl;
        
    }

}