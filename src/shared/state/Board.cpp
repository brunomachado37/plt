#include "Board.h"
#include "../constants.h"

#include <iostream>

namespace state {

    Board::Board() {

        for(int i = 0; i < NUM_LINES; i++) {
            this->terrainMap.push_back(std::vector<std::string>(NUM_ROWS, LAND));
            this->regionMap.push_back(std::vector<int>(NUM_ROWS, -1));
        }

        std::vector<Position> riverTiles{{0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 12}, {1, 4}, {1, 12}, {2, 3}, {2, 4}, {2, 12}, {2, 13}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 13}, {3, 14}, {3, 15}, {4, 14}, {4, 15}, {5, 14}, {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 12}, {6, 13}, {6, 14}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 12}, {8, 6}, {8, 7}, {8, 8}, {8, 9}, {8, 10}, {8, 11}, {8, 12}};
        std::vector<Position> initialNormalTemples{{0, 10}, {2, 5}, {4, 13}, {6, 8}, {9, 5}, {10, 10}};
        std::vector<Position> initialSpecialTemples{{1, 1}, {1, 15}, {7, 1}, {8, 14}};

        for(auto p : riverTiles) {
            this->terrainMap[p.i][p.j] = RIVER;
        }
        for(auto p : initialNormalTemples) {
            this->terrainMap[p.i][p.j] = TEMPLE;

            int regionID = this->regions.size();

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, false);

            regionMap[p.i][p.j] = regionID;

            this->regions.push_back(region);
        }
        for(auto p : initialSpecialTemples) {
            this->terrainMap[p.i][p.j] = SPECIAL;

            int regionID = this->regions.size();

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, true);

            regionMap[p.i][p.j] = regionID;

            this->regions.push_back(region);
        }

        this->boardStateMap = this->terrainMap;
       
    }

}