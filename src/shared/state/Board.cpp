#include "Board.h"
#include "../constants.h"
#include "../messages.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace state {

    Board::Board() {

        for(int i = 0; i < NUM_LINES; i++) {
            this->terrainMap.push_back(std::vector<std::string>(NUM_ROWS, LAND));
            this->regionMap.push_back(std::vector<int>(NUM_ROWS, NO_REGION_ID));
        }

        std::vector<Position> riverTiles{{0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 12}, {1, 4}, {1, 12}, {2, 3}, {2, 4}, {2, 12}, {2, 13}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 13}, {3, 14}, {3, 15}, {4, 14}, {4, 15}, {5, 14}, {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 12}, {6, 13}, {6, 14}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 12}, {8, 6}, {8, 7}, {8, 8}, {8, 9}, {8, 10}, {8, 11}, {8, 12}};
        std::vector<Position> initialNormalTemples{{0, 10}, {2, 5}, {4, 13}, {6, 8}, {9, 5}, {10, 10}};
        std::vector<Position> initialSpecialTemples{{1, 1}, {1, 15}, {7, 1}, {8, 14}};
        std::vector<std::vector<std::string>> monumentColors{{"green", "red"}, {"red", "blue"}, {"blue", "green"}, {"black", "green"}, {"black", "blue"}, {"black", "red"}};

        for(auto p : riverTiles) {
            this->terrainMap[p.i][p.j] = RIVER;
        }
        for(auto p : initialNormalTemples) {
            this->terrainMap[p.i][p.j] = TEMPLE;

            int regionID = this->totalRegionCounter;
            this->totalRegionCounter++;

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, false);

            this->regionMap[p.i][p.j] = regionID;

            this->regions[regionID] = region;

        }
        for(auto p : initialSpecialTemples) {
            this->terrainMap[p.i][p.j] = SPECIAL;

            int regionID = this->totalRegionCounter;
            this->totalRegionCounter++;

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, true);

            this->regionMap[p.i][p.j] = regionID;

            this->regions[regionID] = region;

        }

        this->boardStateMap = this->terrainMap;

        for(auto colors: monumentColors) {
            Monument monument({-1, -1}, colors[0], colors[1]);
            this->monuments.push_back(monument);
        }
       
    }

    void Board::addTileToTheBoard(Tile tile, Position position) {

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == SPECIAL || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUM_BOARD || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
            throw std::invalid_argument(OCCUPIED_POS_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == LAND && tile.getType() == FARM) {
            throw std::invalid_argument(INVALID_POS_LAND_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == RIVER && (tile.getType() == MARKET || tile.getType() == SETTLEMENT || tile.getType() == TEMPLE)) {
            throw std::invalid_argument(INVALID_POS_RIVER_MSG);
        }
        else {
            // Check adjacent positions
            std::vector<int> adjacentRegions(4);

            if(position.i == 0) {
                adjacentRegions[0] = NO_REGION_ID;
            }
            else {
                adjacentRegions[0] = this->regionMap[position.i - 1][position.j];
            }
            if(position.j == 0) {
                adjacentRegions[1] = NO_REGION_ID;
            }
            else {
                adjacentRegions[1] = this->regionMap[position.i][position.j - 1];
            }
            if(position.i == NUM_LINES - 1){
                adjacentRegions[2] = NO_REGION_ID;
            }
            else {
                adjacentRegions[2] = this->regionMap[position.i + 1][position.j];
            }
            if(position.j == NUM_ROWS - 1) {
                adjacentRegions[3] = NO_REGION_ID;
            }
            else {
                adjacentRegions[3] = this->regionMap[position.i][position.j + 1];
            }
           
            adjacentRegions.erase(remove(adjacentRegions.begin(), adjacentRegions.end(), NO_REGION_ID), adjacentRegions.end());
            std::sort(adjacentRegions.begin(), adjacentRegions.end());
            int numberOfAdjacentRegions = std::unique(adjacentRegions.begin(), adjacentRegions.end()) - adjacentRegions.begin();

            if(this->regionMap[position.i][position.j] != NO_REGION_ID) {
                throw std::invalid_argument(REGION_ERROR_MSG);
            }
            else if(numberOfAdjacentRegions == 0) {
                // Set tile position
                tile.setPosition(position);

                // Create new region
                int regionID = this->totalRegionCounter;
                this->totalRegionCounter++;
                
                Region region(regionID);

                // Add tile to the region
                region.addTile(tile);
                this->regions[regionID] = region;

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;
                
            }
            else if(numberOfAdjacentRegions == 1) {
                // Set tile position
                tile.setPosition(position);

                // Add tile to the respective region
                int regionID;

                for(auto reg: adjacentRegions) {
                    if(reg != NO_REGION_ID) {
                        regionID = reg;
                        break;
                    }
                }

                this->regions[regionID].addTile(tile);

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;


            }   
            else if(numberOfAdjacentRegions == 2) {
                // Set tile position
                tile.setPosition(position);
                // Conflits check and trigger
            }
            else {
                throw std::invalid_argument(INVALID_REGION_MSG);
            }
        }

    }


    void Board::addLeaderToTheBoard(Leader leader, Position position) {

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == SPECIAL || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUM_BOARD || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
            throw std::invalid_argument(OCCUPIED_POS_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == RIVER) {
            throw std::invalid_argument(INVALID_LEAD_RIVER_MSG);
        }
        else {
            // Check adjacent positions
            std::vector<int> adjacentRegions(4);

            if(position.i == 0) {
                adjacentRegions[0] = NO_REGION_ID;
            }
            else {
                adjacentRegions[0] = this->regionMap[position.i - 1][position.j];
            }
            if(position.j == 0) {
                adjacentRegions[1] = NO_REGION_ID;
            }
            else {
                adjacentRegions[1] = this->regionMap[position.i][position.j - 1];
            }
            if(position.i == NUM_LINES - 1){
                adjacentRegions[2] = NO_REGION_ID;
            }
            else {
                adjacentRegions[2] = this->regionMap[position.i + 1][position.j];
            }
            if(position.j == NUM_ROWS - 1) {
                adjacentRegions[3] = NO_REGION_ID;
            }
            else {
                adjacentRegions[3] = this->regionMap[position.i][position.j + 1];
            }
           
            adjacentRegions.erase(remove(adjacentRegions.begin(), adjacentRegions.end(), NO_REGION_ID), adjacentRegions.end());
            std::sort(adjacentRegions.begin(), adjacentRegions.end());
            int numberOfAdjacentRegions = std::unique(adjacentRegions.begin(), adjacentRegions.end()) - adjacentRegions.begin();

            if(this->regionMap[position.i][position.j] != NO_REGION_ID) {
                throw std::invalid_argument(REGION_ERROR_MSG);
            }
            else if(numberOfAdjacentRegions == 0) {
                throw std::invalid_argument(INVALID_LEAD_TEMPLE_MSG);
            }
            else if(numberOfAdjacentRegions == 1) {
                // Check if is adjacent to a temple
                std::vector<std::string> adjacentPieces{this->boardStateMap[position.i - 1][position.j], this->boardStateMap[position.i][position.j - 1], this->boardStateMap[position.i + 1][position.j], this->boardStateMap[position.i][position.j + 1]};
                bool atLeastOneAdjacentTemple = false;

                for(auto tile: adjacentPieces) {
                    if(tile == TEMPLE || tile == SPECIAL) {
                        atLeastOneAdjacentTemple = true;
                        break;
                    }
                }

                if(!atLeastOneAdjacentTemple) {
                    throw std::invalid_argument(INVALID_LEAD_TEMPLE_MSG);
                }
                else {
                    int regionID = adjacentRegions[0];

                    // Set leader position
                    leader.setPosition(position);

                    // Check for conflit here
                    // Solve conflict -> For now just print a message REVOLT!
                    
                    this->regions[regionID].addLeader(leader);

                    // Update maps
                    this->boardStateMap[position.i][position.j] = LEADER;
                    this->regionMap[position.i][position.j] = regionID;

                }
            }   
            else {
                throw std::invalid_argument(INVALID_LEAD_REG_MSG);
            }
        }

    }

    std::vector<std::vector<std::string>> Board::getBoardStateMap() {
        return this->boardStateMap;
    }

    std::unordered_map<int, Region> Board::getRegions() {
        return this->regions;
    }

}