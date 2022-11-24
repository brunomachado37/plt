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
        std::vector<std::vector<std::string>> monumentColors{{GREEN, RED}, {RED, BLUE}, {BLUE, GREEN}, {BLACK, GREEN}, {BLACK, BLUE}, {BLACK, RED}};

        for(auto p : riverTiles) {
            this->terrainMap[p.i][p.j] = RIVER;
        }

        this->boardStateMap = this->terrainMap;

        for(auto colors: monumentColors) {
            Monument monument({NOT_IN_MAP_ID, NOT_IN_MAP_ID}, colors[0], colors[1]);
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

    std::vector<Position> Board::checkAdjacentOccupiedPositions(Position position) {

        std::vector<Position> occupiedPositions;
        std::vector<Position> occupiedMap = {{position.i - 1, position.j}, {position.i, position.j - 1}, {position.i + 1, position.j}, {position.i, position.j + 1}};
        std::vector<std::string> adjacentPositions = this->checkAdjacentPositions(position);

        for(int n = 0; n < (int)adjacentPositions.size(); n++) {
            if(adjacentPositions[n] == FARM || adjacentPositions[n] == TEMPLE || adjacentPositions[n] == SETTLEMENT || adjacentPositions[n] == MARKET || adjacentPositions[n] == MONUMENT || adjacentPositions[n] == LEADER) {
                occupiedPositions.push_back(occupiedMap[n]);
            }
        }

        return occupiedPositions;

    }

    void Board::checkForMonuments(Position position, std::string type) {

        std::vector<std::string> surroundings(8);

        // Check all tiles arround given position 
        if(position.i == 0 || position.j == 0) {
            surroundings[0] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[0] = this->boardStateMap[position.i - 1][position.j - 1];
        }
        if(position.i == 0) {
            surroundings[1] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[1] = this->boardStateMap[position.i - 1][position.j];
        }
        if(position.i == 0 || position.j == NUM_ROWS - 1) {
            surroundings[2] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[2] = this->boardStateMap[position.i - 1][position.j + 1];
        }
        if(position.j == NUM_ROWS - 1) {
            surroundings[3] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[3] = this->boardStateMap[position.i][position.j + 1];
        }
        if(position.i == NUM_LINES - 1 || position.j == NUM_ROWS - 1) {
            surroundings[4] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[4] = this->boardStateMap[position.i + 1][position.j + 1];
        }
        if(position.i == NUM_LINES - 1) {
            surroundings[5] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[5] = this->boardStateMap[position.i + 1][position.j];
        }
        if(position.i == NUM_LINES - 1 || position.j == 0) {
            surroundings[6] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[6] = this->boardStateMap[position.i + 1][position.j - 1];
        }
        if(position.j == 0) {
            surroundings[7] = OUTSIDE_OF_BOARD;
        }
        else {
            surroundings[7] = this->boardStateMap[position.i][position.j - 1];
        }

        // Check for possible monuments
        if(surroundings[0] == type && surroundings[1] == type && surroundings[7] == type) {
            this->possibleMonuments.push_back({position.i - 1, position.j - 1});
        }
        if(surroundings[1] == type && surroundings[2] == type && surroundings[3] == type) {
            this->possibleMonuments.push_back({position.i - 1, position.j});
        }
        if(surroundings[3] == type && surroundings[4] == type && surroundings[5] == type) {
            this->possibleMonuments.push_back({position.i, position.j});
        }
        if(surroundings[5] == type && surroundings[6] == type && surroundings[7] == type) {
            this->possibleMonuments.push_back({position.i, position.j - 1});
        }

    }

    void Board::addTileToTheBoard(Tile tile, Position position) {

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUMENT || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
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

                // Check for possible monuments
                checkForMonuments(position, tile.getType());

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

                // Check for possible monuments
                checkForMonuments(position, tile.getType());

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

                // Check for possible monuments
                checkForMonuments(position, tile.getType());

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

                // Check for possible monuments
                checkForMonuments(position, tile.getType());
                
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

        if(this->boardStateMap[position.i][position.j] == TEMPLE || this->boardStateMap[position.i][position.j] == MARKET || this->boardStateMap[position.i][position.j] == FARM || this->boardStateMap[position.i][position.j] == SETTLEMENT || this->boardStateMap[position.i][position.j] == MONUMENT || this->boardStateMap[position.i][position.j] == CATASTRO || this->boardStateMap[position.i][position.j] == LEADER) {
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

                    // Set leader position
                    leader.setPosition(position);
                    
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
        for(auto region: this->regions) {
            // Try to remove leader from region
            try {
                leaderPosition = this->regions[region.first].removeLeader(playerID, type);

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

        // Save region ID
        int regionID = this->regionMap[leaderPosition.i][leaderPosition.j];

        // Update maps
        this->regionMap[leaderPosition.i][leaderPosition.j] = NO_REGION_ID;
        this->boardStateMap[leaderPosition.i][leaderPosition.j] = this->terrainMap[leaderPosition.i][leaderPosition.j];

        // Check for region break
        this->restructureRegion(regionID, {-1, -1});

    }

    std::vector<Leader> Board::addMonumentToTheBoard(Monument monument, Position position) {

        std::unordered_map<std::string, std::string> colorToTypeMap = {{BLUE, FARM}, {RED, TEMPLE}, {GREEN, MARKET}, {BLACK, SETTLEMENT}};
        std::string monumentType;
        int monumentIndex = -1;

        // Check which type of tile was used to build the monument
        if(this->boardStateMap[position.i][position.j] == colorToTypeMap[monument.getColor1()]) {
            monumentType = colorToTypeMap[monument.getColor1()];
        }
        else if(this->boardStateMap[position.i][position.j] == colorToTypeMap[monument.getColor2()]) {
            monumentType = colorToTypeMap[monument.getColor2()];
        }
        else {
            throw std::logic_error(INVALID_MONUMENT_ADD_ERROR_MSG);
        }

        // Check monument index in list of not built monuments
        for(int i = 0; i < (int)this->monuments.size(); i++) {
            if(monument.getColor1() == this->monuments[i].getColor1() && monument.getColor2() == this->monuments[i].getColor2()) {
                monumentIndex = i;
            }
        }

        // Sanity checks
        if(this->boardStateMap[position.i][position.j + 1] != monumentType || this->boardStateMap[position.i + 1][position.j] != monumentType || this->boardStateMap[position.i + 1][position.j + 1] != monumentType) {
            throw std::logic_error(INVALID_MONUMENT_ADD_ERROR_MSG);
        }
        if(this->regionMap[position.i][position.j] != this->regionMap[position.i][position.j + 1] || this->regionMap[position.i][position.j + 1] != this->regionMap[position.i + 1][position.j + 1] || this->regionMap[position.i + 1][position.j + 1] != this->regionMap[position.i + 1][position.j]) {
            throw std::logic_error(MONUMENT_REGION_ERROR_MSG);
        }
        if(monument.getPosition().i != NOT_IN_MAP_ID ||  monument.getPosition().j != NOT_IN_MAP_ID) {
            throw std::logic_error(MONUMENT_BUILT_ERROR_MSG);
        }
        if(monumentIndex == -1) {
            throw std::logic_error(MONUMENT_BUILT_2_ERROR_MSG);
        }

        // Set monument position
        monument.setPosition(position);

        // Add monument to the respective region
        int regionID = this->regionMap[position.i][position.j];
        this->regions[regionID].addMonument(monument);

        // Remove tiles from the region
        this->regions[regionID].removeTile(position);
        this->regions[regionID].removeTile({position.i, position.j + 1});
        this->regions[regionID].removeTile({position.i + 1, position.j});
        this->regions[regionID].removeTile({position.i + 1, position.j + 1});

        // Remove from not built monuments list
        this->monuments.erase(this->monuments.begin() + monumentIndex);

        // Update state map
        this->boardStateMap[position.i][position.j] = MONUMENT;
        this->boardStateMap[position.i + 1][position.j] = MONUMENT;
        this->boardStateMap[position.i][position.j + 1] = MONUMENT;
        this->boardStateMap[position.i + 1][position.j + 1] = MONUMENT;

        // Check for leaders being removed
        std::vector<Leader> removedLeaders;

        if(monumentType == TEMPLE) {
            removedLeaders = this->removeLeadersWithoutTemple(regionID);
        }

        // If any leader was removed, restructure region
        if(removedLeaders.size() > 0) {
            this->restructureRegion(regionID, {-1, -1});
        }

        return removedLeaders;

    }


    std::vector<Leader> Board::addCatastropheToTheBoard(Position position) {

        // Sanity checks
        if(this->boardStateMap[position.i][position.j] == MONUMENT || this->boardStateMap[position.i][position.j] == LEADER || this->boardStateMap[position.i][position.j] == CATASTRO) {
            throw std::invalid_argument(CATASTRO_INVALID_MSG);
        }

        // Get region ID
        int regionID = this->regionMap[position.i][position.j];

        // Create removed leaders list
        std::vector<Leader> removedLeaders;

        // If there's a tile there
        if(regionID != NO_REGION_ID) {
            // Check if there is a treasure in the position
            for(auto treasure: this->regions[regionID].getTreasures()) {
                if(treasure.getPosition() == position) {
                    throw std::invalid_argument(CATASTRO_IN_TREASURE_MSG);
                }
            }

            // Remove tile
            this->regions[regionID].removeTile(position);

            // Update maps
            this->boardStateMap[position.i][position.j] = this->terrainMap[position.i][position.j];
            this->regionMap[position.i][position.j] = NO_REGION_ID;

            // Remove all leaders that aren't adjacent to any temple
            removedLeaders = this->removeLeadersWithoutTemple(regionID);

            // Check for region break
            this->restructureRegion(regionID, {-1, -1});

        }

        // Add catastrophe to catastrophe's list
        this->catastrophes.push_back(position);

        // Update maps
        this->boardStateMap[position.i][position.j] = CATASTRO;
        this->regionMap[position.i][position.j] = NO_REGION_ID;

        return removedLeaders;

    }

    int Board::warLost(Leader leader, Position unificationPosition, std::string type) {

        // Get region ID
        int regionID = this->regionMap[unificationPosition.i][unificationPosition.j];

        // Sanity check
        if(this->regionMap[leader.getPosition().i][leader.getPosition().j] != regionID || this->boardStateMap[leader.getPosition().i][leader.getPosition().j] != LEADER) {
            throw std::logic_error(LEADER_MAP_ERROR_MSG);
        }

        // Create a list of positions on this side of the region
        std::vector<Position> positions;

        // Add unification position to the list
        positions.push_back(unificationPosition);

        // Start recursive search
        this->recursiveAdjacentSearch(leader.getPosition(), positions);

        // Iterate over all positions and remove supporters
        int count = 0;

        for(auto pos: positions) {
            bool remove = true;

            if(this->boardStateMap[pos.i][pos.j] == type && pos != unificationPosition) {
                if(type == TEMPLE) {
                    // Check if there's a treasure in the given position
                    for(auto treasure: this->regions[regionID].getTreasures()) {
                        if(treasure.getPosition() == pos) {
                            remove = false;
                        }
                    }
                    // Check if there's a leader adjacent to the temple
                    std::vector<std::string> adjPos =  checkAdjacentPositions(pos);
                    std::vector<Position> posMap = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
                    for(int i = 0; i < 4; i++) {
                        if(adjPos[i] == LEADER) {
                            std::vector<std::string> adjLeader = checkAdjacentPositions({pos.i + posMap[i].i, pos.j + posMap[i].j});
                            if(std::count(adjLeader.begin(), adjLeader.end(), TEMPLE) == 1) {
                                remove = false;
                            }
                        }
                    }
                }

                if(remove) {
                    this->removeTileFromTheBoard(pos);
                    count++;
                }
                
            }
        }

        // Remove leader from region
        this->regions[regionID].removeLeader(leader.getPlayerID(), leader.getType());

        // Update maps
        this->regionMap[leader.getPosition().i][leader.getPosition().j] = NO_REGION_ID;
        this->boardStateMap[leader.getPosition().i][leader.getPosition().j] = this->terrainMap[leader.getPosition().i][leader.getPosition().j];

        // Check for region break
        this->restructureRegion(regionID, unificationPosition);

        return count;

    }

    std::vector<Leader> Board::removeLeadersWithoutTemple(int regionID) {

        // Create list of removed leaders
        std::vector<Leader> removedLeaders;
        
        for(auto leader: this->regions[regionID].getLeaders()) {
            // Check if is adjacent to a temple
            std::vector<std::string> adjacentPieces = checkAdjacentPositions(leader.getPosition());
            bool atLeastOneAdjacentTemple = false;

            for(auto tile: adjacentPieces) {
                if(tile == TEMPLE) {
                    atLeastOneAdjacentTemple = true;
                    break;
                }
            }

            if(!atLeastOneAdjacentTemple) {
                // Save leader
                removedLeaders.push_back(leader);

                // Remove leader from region
                this->regions[regionID].removeLeader(leader.getPlayerID(), leader.getType());

                // Update maps
                this->boardStateMap[leader.getPosition().i][leader.getPosition().j] = this->terrainMap[leader.getPosition().i][leader.getPosition().j];
                this->regionMap[leader.getPosition().i][leader.getPosition().j] = NO_REGION_ID;

            }

        }

        return removedLeaders;

    }

    void Board::restructureRegion(int regionID, Position unificationPosition) {

        // Create a vector with all positions in the region
        std::vector<Position> regionPositionList;

        for(int i = 0; i < NUM_LINES; i++) {
            for(int j = 0; j < NUM_ROWS; j++) {
                if(this->regionMap[i][j] == regionID) {
                    regionPositionList.push_back({i, j});
                }
            }
        }

        // Extreme case: region is empty
        if(regionPositionList.size() == 0) {
            this->regions.erase(regionID);
            return;
        }
	        
        // Create temporary variables for recursive check
        Position initialPosition = regionPositionList[0];
        std::vector<Position> tempRegionList;
        std::vector<Position> totalRegionList;

        // Start recursive search
        while(totalRegionList.size() != regionPositionList.size()) {

            this->recursiveAdjacentSearch(initialPosition, tempRegionList);

            // Add all elements of temporary list to total list
            for(auto pos: tempRegionList) {
                totalRegionList.push_back(pos);
            }

            // Create new region
            int newRegionID = this->totalRegionCounter;
            this->totalRegionCounter++;
            Region region(newRegionID);

            // Add all elements for temporary list to the new region
            for(auto treasure: this->regions[regionID].getTreasures()) {
                if(std::count(tempRegionList.begin(), tempRegionList.end(), treasure.getPosition())) {
                    region.addTreasure(treasure);
                }
            }
            for(auto tile: this->regions[regionID].getTiles()) {
                if(std::count(tempRegionList.begin(), tempRegionList.end(), tile.getPosition())) {
                    region.addTile(tile);
                    // Update region map
                    this->regionMap[tile.getPosition().i][tile.getPosition().j] = newRegionID;
                    // Remove position from temporary list
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), tile.getPosition()), tempRegionList.end());
                }
            }
            for(auto leader: this->regions[regionID].getLeaders()) {
                if(std::count(tempRegionList.begin(), tempRegionList.end(), leader.getPosition())) {
                    region.addLeader(leader);
                    // Update region map
                    this->regionMap[leader.getPosition().i][leader.getPosition().j] = newRegionID;
                    // Remove position from temporary list
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), leader.getPosition()), tempRegionList.end());
                }
            }
            for(auto monument: this->regions[regionID].getMonuments()) {
                if(std::count(tempRegionList.begin(), tempRegionList.end(), monument.getPosition())) {
                    region.addMonument(monument);
                    // Update region map
                    this->regionMap[monument.getPosition().i][monument.getPosition().j] = newRegionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j] = newRegionID;
                    this->regionMap[monument.getPosition().i][monument.getPosition().j + 1] = newRegionID;
                    this->regionMap[monument.getPosition().i + 1][monument.getPosition().j + 1] = newRegionID;
                    // Remove position from temporary list
                    Position tempPos;
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), monument.getPosition()), tempRegionList.end());
                    tempPos = {monument.getPosition().i + 1, monument.getPosition().j};
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), tempPos), tempRegionList.end());
                    tempPos = {monument.getPosition().i, monument.getPosition().j + 1};
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), tempPos), tempRegionList.end());
                    tempPos = {monument.getPosition().i + 1, monument.getPosition().j + 1};
                    tempRegionList.erase(std::remove(tempRegionList.begin(), tempRegionList.end(), tempPos), tempRegionList.end());
                }
            }

            // Check if temporary list is empty
            if(tempRegionList.size() != 0) {
                throw std::logic_error(REC_SEARCH_ERROR_MSG);
            }

            // Check if new region is a kingdom
            if(region.getLeaders().size() > 0) {
                region.setIsKingdom(true);
            }

            // Add new region to the board
            this->regions[newRegionID] = region;

            // Check if new region is at war
            checkIfIsAtWar(newRegionID, unificationPosition);

            // Search for next initial position
            for(auto p: regionPositionList) {
		        if(std::count(totalRegionList.begin(), totalRegionList.end(), p) == 0) {
			        initialPosition = p;
                    break;
                }
	        }

        }

        // Sanity check
        if(!(std::is_permutation(totalRegionList.begin(), totalRegionList.end(), regionPositionList.begin()))) {
            throw std::logic_error(REC_SEARCH_ERROR_2_MSG);
        }

        // Remove original region from the board
        this->regions.erase(regionID);
        
    }

    void Board::recursiveAdjacentSearch(Position position, std::vector<Position>& tempRegionList) {

        // Add element to temporary list
        tempRegionList.push_back(position);

        // Check all occupied adjacent positions
        for(auto pos: this->checkAdjacentOccupiedPositions(position)) {
            // If position is not yet in the temporary list, call recursive methode
            if(std::count(tempRegionList.begin(), tempRegionList.end(), pos) == 0) {
                this->recursiveAdjacentSearch(pos, tempRegionList);
            }
        }

    }

    void Board::checkIfIsAtWar(int regionID, Position unificationPosition) {

        this->regions[regionID].setIsAtWar(false);

        if(this->regions[regionID].getIsKingdom()) {
            // Take all region leaders type
            std::vector<std::string> region_leaders;

            for(auto leader: this->regions[regionID].getLeaders()) {
                region_leaders.push_back(leader.getType());
            }

            std::sort(region_leaders.begin(), region_leaders.end());

            // Check if there's 2 leaders of the same type in the region
            if(std::adjacent_find(region_leaders.begin(), region_leaders.end()) != region_leaders.end()) {
                // Raise war flags
                this->regions[regionID].setIsAtWar(true);
                this->regions[regionID].setUnificationTilePosition(unificationPosition);
            }
        }

    }

    int Board::checkLeaderStrength(Leader leader, Position unificationPosition) {

        // Create a list of positions on this side of the region
        std::vector<Position> positions;

        // Leader to tile map
        std::unordered_map<std::string, std::string> leaderToTileMap = {{FARMER, FARM}, {PRIEST, TEMPLE}, {TRADER, MARKET}, {KING, SETTLEMENT}};

        // Add unification position to the list
        positions.push_back(unificationPosition);

        // Start recursive search
        this->recursiveAdjacentSearch(leader.getPosition(), positions);

        // Iterate over all positions and count supporters
        int count = 0;

        for(auto pos: positions) {
            if(this->boardStateMap[pos.i][pos.j] == leaderToTileMap[leader.getType()] && pos != unificationPosition) {
                count++;
            }
        }

        return count;

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

    void Board::setRegion(Region region) {
        this->regions[region.getRegionID()] = region;
    }

    std::vector<Monument> Board::getMonuments() {
        return this->monuments;
    }

    std::vector<Position> Board::getCatastrophes() {
        return this->catastrophes;
    }

    std::vector<Position> Board::getPossibleMonuments() {
        return this->possibleMonuments;
    }

    void Board::clearPossibleMonuments() {
        this->possibleMonuments.clear();
    }

}