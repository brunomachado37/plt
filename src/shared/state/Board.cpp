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
        std::vector<std::vector<std::string>> monumentColors{{"green", "red"}, {"red", "blue"}, {"blue", "green"}, {"black", "green"}, {"black", "blue"}, {"black", "red"}};

        for(auto p : riverTiles) {
            this->terrainMap[p.i][p.j] = RIVER;
        }

        this->boardStateMap = this->terrainMap;

        for(auto colors: monumentColors) {
            Monument monument({-1, -1}, colors[0], colors[1]);
            this->monuments.push_back(monument);
        }
       
    }

    void Board::init() {

        std::vector<Position> initialNormalTemples{{0, 10}, {2, 5}, {4, 13}, {6, 8}, {9, 5}, {10, 10}};
        std::vector<Position> initialSpecialTemples{{1, 1}, {1, 15}, {7, 1}, {8, 14}};

        for(auto p : initialNormalTemples) {
            this->boardStateMap[p.i][p.j] = TEMPLE;

            int regionID = this->totalRegionCounter;
            this->totalRegionCounter++;

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, false);

            this->regionMap[p.i][p.j] = regionID;

            this->regions[regionID] = region;

        }

        for(auto p : initialSpecialTemples) {
            this->boardStateMap[p.i][p.j] = TEMPLE;

            int regionID = this->totalRegionCounter;
            this->totalRegionCounter++;

            Region region(regionID);
            region.addTile(TEMPLE, {p.i, p.j});
            region.addTreasure({p.i, p.j}, true);

            this->regionMap[p.i][p.j] = regionID;

            this->regions[regionID] = region;

        }

    }

    std::tuple<std::vector<int>, int> Board::checkAdjacentRegions(Position position) {

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

            return std::make_tuple(adjacentRegions, numberOfAdjacentRegions);

    }


    std::vector<std::string> Board::checkAdjacentPositions(Position position) {

            std::vector<std::string> adjacentPositions(4);

            if(position.i == 0) {
                adjacentPositions[0] = OUTSIDE_OF_BOARD;
            }
            else {
                adjacentPositions[0] = this->boardStateMap[position.i - 1][position.j];
            }
            if(position.j == 0) {
                adjacentPositions[1] = OUTSIDE_OF_BOARD;
            }
            else {
                adjacentPositions[1] = this->boardStateMap[position.i][position.j - 1];
            }
            if(position.i == NUM_LINES - 1){
                adjacentPositions[2] = OUTSIDE_OF_BOARD;
            }
            else {
                adjacentPositions[2] = this->boardStateMap[position.i + 1][position.j];
            }
            if(position.j == NUM_ROWS - 1) {
                adjacentPositions[3] = OUTSIDE_OF_BOARD;
            }
            else {
                adjacentPositions[3] = this->boardStateMap[position.i][position.j + 1];
            }
           
            return adjacentPositions;

    }


    void Board::addTileToTheBoard(Tile tile, Position position) {

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUM_BOARD || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
            throw std::invalid_argument(OCCUPIED_POS_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == LAND && tile.getType() == FARM) {
            throw std::invalid_argument(INVALID_POS_LAND_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == RIVER && (tile.getType() == MARKET || tile.getType() == SETTLEMENT || tile.getType() == TEMPLE)) {
            throw std::invalid_argument(INVALID_POS_RIVER_MSG);
        }
        else {
            // Check adjacent regions
            std::vector<int> adjacentRegions;
            int numberOfAdjacentRegions;
            tie(adjacentRegions, numberOfAdjacentRegions) = checkAdjacentRegions(position);

            if(this->regionMap[position.i][position.j] != NO_REGION_ID) {
                throw std::logic_error(REGION_ERROR_MSG);
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
                int regionID = adjacentRegions[0];
                this->regions[regionID].addTile(tile);

                // Check and possibly updates leader strength
                this->regions[regionID].updateLeaderStrength(tile.getType());

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;

            }   
            else if(numberOfAdjacentRegions == 2) {    
                // Take both regions IDs
                int regionID_1 = adjacentRegions[0];
                int regionID_2 = NO_REGION_ID;

                for(auto id: adjacentRegions) {
                    if(id != regionID_1) {
                        regionID_2 = id;
                        break;
                    }
                }

                // Sanity check
                if(regionID_2 == NO_REGION_ID) {
                    throw std::logic_error(ADJ_REG_ERROR_1_MSG);
                }


                // Create new region
                int regionID = this->totalRegionCounter;
                this->totalRegionCounter++;
                
                Region region(regionID);


                // Check if both positions are kingdoms
                if(this->regions[regionID_1].getIsKingdom() && this->regions[regionID_2].getIsKingdom()) {
                    // Check if there's a war
                    std::vector<std::string> region1_leaders;
                    std::vector<std::string> region2_leaders;

                    for(auto leader: this->regions[regionID_1].getLeaders()) {
                        region1_leaders.push_back(leader.getType());
                    }

                    for(auto leader: this->regions[regionID_2].getLeaders()) {
                        region2_leaders.push_back(leader.getType());
                    }

                    // Check if there's leaders of the same type in the 2 regions
                    if(std::find_first_of(region1_leaders.begin(), region1_leaders.end(), region2_leaders.begin(), region2_leaders.end()) != region1_leaders.end()) {
                        // Raise war flags
                        region.setIsAtWar(true);
                        region.setUnificationTilePosition(position);
                    }
                }

                // Check if one of the regions was a kingdom
                if(this->regions[regionID_1].getIsKingdom() || this->regions[regionID_2].getIsKingdom()) {
                    region.setIsKingdom(true);
                }

                // Transfer all itens in region 1 to the new region
                for(auto tile: this->regions[regionID_1].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_1].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_1].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_1].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Set tile position
                tile.setPosition(position);   

                // Add tile that united both regions between their tiles in the list
                region.addTile(tile);

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;

                // Transfer all itens in region 2 to the new region
                for(auto tile: this->regions[regionID_2].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_2].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_2].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_2].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Update leaders strenghts (in case of a war, this will be done after the war treatment)
                if(!region.getIsAtWar()) {
                    region.updateAllLeadersStrength();
                }
                
                // Remove old regions from the board
                this->regions.erase(regionID_1);
                this->regions.erase(regionID_2);

                // Add new region to the board
                this->regions[regionID] = region;

            }
            else if(numberOfAdjacentRegions == 3) {
                // Take the 3 regions IDs
                int regionID_1 = adjacentRegions[0];
                int regionID_2 = NO_REGION_ID;
                int regionID_3 = NO_REGION_ID;

                for(auto id: adjacentRegions) {
                    if(id != regionID_1) {
                        regionID_2 = id;
                        break;
                    }
                }

                for(auto id: adjacentRegions) {
                    if(id != regionID_1 && id != regionID_2) {
                        regionID_3 = id;
                        break;
                    }
                }

                // Sanity check
                if(regionID_2 == NO_REGION_ID || regionID_3 == NO_REGION_ID) {
                    throw std::logic_error(ADJ_REG_ERROR_1_MSG);
                }

                // Create auxiliary vector to store kingoms IDs
                std::vector<int> kingdomsInTheRegion;
                if(this->regions[regionID_1].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_1);
                }
                if(this->regions[regionID_2].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_2);
                }
                if(this->regions[regionID_3].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_3);
                }

                // Check if 3 kingdoms were united (not legal play)
                if(kingdomsInTheRegion.size() == 3) {
                    throw std::invalid_argument(INVALID_REGION_MSG);
                }

                // Create new region
                int regionID = this->totalRegionCounter;
                this->totalRegionCounter++;
                
                Region region(regionID);

                // Check if there's 2 kingdoms between the 3 regions
                if(kingdomsInTheRegion.size() == 2) {
                    // Check if there's a war
                    std::vector<std::string> region1_leaders;
                    std::vector<std::string> region2_leaders;

                    for(auto leader: this->regions[kingdomsInTheRegion[0]].getLeaders()) {
                        region1_leaders.push_back(leader.getType());
                    }

                    for(auto leader: this->regions[kingdomsInTheRegion[1]].getLeaders()) {
                        region2_leaders.push_back(leader.getType());
                    }

                    // Check if there's leaders of the same type in the 2 regions
                    if(std::find_first_of(region1_leaders.begin(), region1_leaders.end(), region2_leaders.begin(), region2_leaders.end()) != region1_leaders.end()) {
                        // War treatment
                        region.setIsAtWar(true);
                        region.setUnificationTilePosition(position);
                    }
                }
                
                // Check if at least one of the regions was a kingdom
                if(kingdomsInTheRegion.size() > 0) {
                    region.setIsKingdom(true);
                }

                // Transfer all itens in region 1 to the new region
                for(auto tile: this->regions[regionID_1].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_1].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_1].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_1].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Set tile position
                tile.setPosition(position);   

                // Add tile that united both regions between their tiles in the list
                region.addTile(tile);

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;

                // Transfer all itens in region 2 to the new region
                for(auto tile: this->regions[regionID_2].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_2].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_2].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_2].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Add tile that united both regions again to split the regions tiles (this tile will be deleted during war treatment)
                if(region.getIsAtWar()) {
                    region.addTile(tile);
                }

                // Transfer all itens in region 3 to the new region
                for(auto tile: this->regions[regionID_3].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_3].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_3].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_3].getTreasures()) {
                    region.addTreasure(treasure);
                }


                // Update leaders strenghts (in case of a war, this will be done after the war treatment)
                if(!region.getIsAtWar()) {
                    region.updateAllLeadersStrength();
                }

                // Remove old regions from the board
                this->regions.erase(regionID_1);
                this->regions.erase(regionID_2);
                this->regions.erase(regionID_3);

                // Add new region to the board
                this->regions[regionID] = region;


            }
            else if(numberOfAdjacentRegions == 4) {
                // Take the 4 regions IDs
                int regionID_1 = adjacentRegions[0];
                int regionID_2 = NO_REGION_ID;
                int regionID_3 = NO_REGION_ID;
                int regionID_4 = NO_REGION_ID;

                for(auto id: adjacentRegions) {
                    if(id != regionID_1) {
                        regionID_2 = id;
                        break;
                    }
                }

                for(auto id: adjacentRegions) {
                    if(id != regionID_1 && id != regionID_2) {
                        regionID_3 = id;
                        break;
                    }
                }

                for(auto id: adjacentRegions) {
                    if(id != regionID_1 && id != regionID_2 && id != regionID_3) {
                        regionID_4 = id;
                        break;
                    }
                }

                // Sanity check
                if(regionID_2 == NO_REGION_ID || regionID_3 == NO_REGION_ID || regionID_4 == NO_REGION_ID) {
                    throw std::logic_error(ADJ_REG_ERROR_1_MSG);
                }

                // Create auxiliary vector to store kingoms IDs
                std::vector<int> kingdomsInTheRegion;
                if(this->regions[regionID_1].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_1);
                }
                if(this->regions[regionID_2].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_2);
                }
                if(this->regions[regionID_3].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_3);
                }
                if(this->regions[regionID_4].getIsKingdom()) {
                    kingdomsInTheRegion.push_back(regionID_4);
                }

                // Check if 3 or 4 kingdoms were united (not legal play)
                if(kingdomsInTheRegion.size() >= 3) {
                    throw std::invalid_argument(INVALID_REGION_MSG);
                }

                // Create new region
                int regionID = this->totalRegionCounter;
                this->totalRegionCounter++;
                
                Region region(regionID);

                // Check if there's 2 kingdoms between the 4 regions
                if(kingdomsInTheRegion.size() == 2) {
                    // Check if there's a war
                    std::vector<std::string> region1_leaders;
                    std::vector<std::string> region2_leaders;

                    for(auto leader: this->regions[kingdomsInTheRegion[0]].getLeaders()) {
                        region1_leaders.push_back(leader.getType());
                    }

                    for(auto leader: this->regions[kingdomsInTheRegion[1]].getLeaders()) {
                        region2_leaders.push_back(leader.getType());
                    }

                    // Check if there's leaders of the same type in the 2 regions
                    if(std::find_first_of(region1_leaders.begin(), region1_leaders.end(), region2_leaders.begin(), region2_leaders.end()) != region1_leaders.end()) {
                        // War treatment
                        region.setIsAtWar(true);
                        region.setUnificationTilePosition(position);
                    }
                }
                
                // Check if at least one of the regions was a kingdom
                if(kingdomsInTheRegion.size() > 0) {
                    region.setIsKingdom(true);
                }

                // Transfer all itens in region 1 to the new region
                for(auto tile: this->regions[regionID_1].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_1].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_1].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_1].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Set tile position
                tile.setPosition(position);   

                // Add tile that united both regions between their tiles in the list
                region.addTile(tile);

                // Update maps
                this->boardStateMap[position.i][position.j] = tile.getType();
                this->regionMap[position.i][position.j] = regionID;

                // Transfer all itens in region 2 to the new region
                for(auto tile: this->regions[regionID_2].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_2].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_2].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_2].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Add tile that united both regions again to split the regions tiles (this tile will be deleted during war treatment)
                if(region.getIsAtWar()) {
                    region.addTile(tile);
                }

                // Transfer all itens in region 3 to the new region
                for(auto tile: this->regions[regionID_3].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_3].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_3].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_3].getTreasures()) {
                    region.addTreasure(treasure);
                }

                // Add tile that united both regions again to split the regions tiles (this tile will be deleted during war treatment)
                if(region.getIsAtWar()) {
                    region.addTile(tile);
                }

                // Transfer all itens in region 4 to the new region
                for(auto tile: this->regions[regionID_4].getTiles()) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = regionID;
                }
                for(auto leader: this->regions[regionID_4].getLeaders()) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = regionID;
                }
                for(auto monument: this->regions[regionID_4].getMonuments()) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = regionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = regionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = regionID;
                }
                for(auto treasure: this->regions[regionID_4].getTreasures()) {
                    region.addTreasure(treasure);
                }


                // Update leaders strenghts (in case of a war, this will be done after the war treatment)
                if(!region.getIsAtWar()) {
                    region.updateAllLeadersStrength();
                }

                // Remove old regions from the board
                this->regions.erase(regionID_1);
                this->regions.erase(regionID_2);
                this->regions.erase(regionID_3);
                this->regions.erase(regionID_4);

                // Add new region to the board
                this->regions[regionID] = region;

            }
            else {
                throw std::logic_error(ADJ_REG_ERROR_2_MSG);
            }
        }

    }


    void Board::removeTileFromTheBoard(Position position) {

        // Read maps in given position
        int regionID = this->regionMap[position.i][position.j];
        std::string posState = this->boardStateMap[position.i][position.j];

        // Check if there's something in that position
        if(regionID == NO_REGION_ID || (posState != TEMPLE && posState != MARKET && posState != SETTLEMENT && posState != FARM)) {
            throw std::invalid_argument(TILE_NOT_IN_POS_MSG);
        }

        // Remove tile from the region
        this->regions[regionID].removeTile(position);

        // Update maps
        this->regionMap[position.i][position.j] = NO_REGION_ID;
        this->boardStateMap[position.i][position.j] = this->terrainMap[position.i][position.j];

    }


    void Board::addLeaderToTheBoard(Leader leader, Position position) {

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUM_BOARD || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
            throw std::invalid_argument(OCCUPIED_POS_MSG);
        }
        else if(this->boardStateMap[position.i][position.j] == RIVER) {
            throw std::invalid_argument(INVALID_LEAD_RIVER_MSG);
        }
        else {
            // Check adjacent regions
            std::vector<int> adjacentRegions;
            int numberOfAdjacentRegions;
            tie(adjacentRegions, numberOfAdjacentRegions) = checkAdjacentRegions(position);

            // Sanity check
            if(this->regionMap[position.i][position.j] != NO_REGION_ID) {
                throw std::logic_error(REGION_ERROR_MSG);
            }
            else if(numberOfAdjacentRegions == 0) {
                throw std::invalid_argument(INVALID_LEAD_TEMPLE_MSG);
            }
            else if(numberOfAdjacentRegions == 1) {
                // Check if is adjacent to a temple
                std::vector<std::string> adjacentPieces = checkAdjacentPositions(position);
                bool atLeastOneAdjacentTemple = false;

                for(auto tile: adjacentPieces) {
                    if(tile == TEMPLE) {
                        atLeastOneAdjacentTemple = true;
                        break;
                    }
                }

                if(!atLeastOneAdjacentTemple) {
                    throw std::invalid_argument(INVALID_LEAD_TEMPLE_MSG);
                }
                else {
                    int regionID = adjacentRegions[0];

                    // Check for a revolt
                    for(auto leaderInRegion: this->regions[regionID].getLeaders()) {
                        if(leaderInRegion.getType() == leader.getType()) {
                            // Sanity check
                            if(leaderInRegion.getPlayerID() == leader.getPlayerID()) {
                                throw std::logic_error(LEADER_ID_ERROR_MSG);
                            }

                            this->regions[regionID].setIsInRevolt(true);

                        }
                    }

                    // Sanity check
                    if(leader.getStrength() != 0) {
                        throw std::logic_error(LEADER_STRENGTH_ERROR_MSG);
                    }

                    // Set leader position
                    leader.setPosition(position);

                    // Set leader strength
                    std::unordered_map<std::string, std::string> tile_leader_map = {{SETTLEMENT, KING}, {TEMPLE, PRIEST}, {FARM, FARMER}, {MARKET, TRADER}};
                    for(auto tileInRegion: this->regions[regionID].getTiles()) {
                        if(tile_leader_map[tileInRegion.getType()] == leader.getType()) {
                            leader.setStrength(leader.getStrength() + 1);
                        }
                    }
                    
                    // Add Leader to the region
                    this->regions[regionID].addLeader(leader);

                    // Transform region in kingdom if not yet
                    if(!this->regions[regionID].getIsKingdom()) {
                        this->regions[regionID].setIsKingdom(true);
                    }

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


    void Board::removeLeaderFromTheBoard(int playerID, std::string type) {

        Position leaderPosition = {NOT_FOUND_POS, NOT_FOUND_POS};

        // Scan all regions to find the specified leader
        for(int i = 0; i < (int)this->regions.size(); i++) {
            // Try to remove leader from region
            try {
                leaderPosition = this->regions[i].removeLeader(playerID, type);

                break;
            }
            // Ignore LEADER_NOT_IN_REGION exception, throw otherwise
            catch(const std::exception& e) {
                if(std::string(e.what()) != LEADER_NOT_IN_REGION_MSG) {
                    throw;
                }
            }
        }

        // Check if leader was removed
        if(leaderPosition.i == NOT_FOUND_POS || leaderPosition.j == NOT_FOUND_POS) {
            throw std::invalid_argument(LEADER_NOT_IN_BOARD_MSG);
        }

        // Sanity check
        if(this->regionMap[leaderPosition.i][leaderPosition.j] == NO_REGION_ID || this->boardStateMap[leaderPosition.i][leaderPosition.j] != LEADER) {
            throw std::logic_error(LEADER_MAP_ERROR_MSG);
        }

        // Update maps
        this->regionMap[leaderPosition.i][leaderPosition.j] = NO_REGION_ID;
        this->boardStateMap[leaderPosition.i][leaderPosition.j] = this->terrainMap[leaderPosition.i][leaderPosition.j];

    }


    std::vector<std::vector<std::string>> Board::getBoardStateMap() {
        return this->boardStateMap;
    }

    std::vector<std::vector<int>> Board::getRegionMap() {
        return this->regionMap;
    }

    std::unordered_map<int, Region> Board::getRegions() {
        return this->regions;
    }

    std::vector<Monument> Board::getMonuments() {
        return this->monuments;
    }

}