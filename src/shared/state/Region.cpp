#include "Region.h"
#include "../messages.h"
#include "../constants.h"

#include <unordered_map>
#include <stdexcept>

namespace state {

    Region::Region() {}

    Region::Region(int regionID) {

        this->regionID = regionID;
        this->isKingdom = false;
        this->isAtWar = false;
        this->isInRevolt = false;
        this->unificationTilePosition = {NO_UNIFICATION_POS, NO_UNIFICATION_POS};

    }

    void Region::addTile(std::string type, Position position) {

        Tile tile(type, position);
        this->tiles.push_back(tile);

    }

    void Region::addTile(Tile tile) {
        this->tiles.push_back(tile);
    }

    void Region::removeTile(Position position) {
        for(int i = 0; i < (int)this->tiles.size(); i++) {
            if(this->tiles[i].getPosition() == position) {
                this->tiles.erase(this->tiles.begin() + i);
                return;
            }
        }

        throw std::invalid_argument(TILE_NOT_IN_REGION_MSG);
    
    }

    void Region::addLeader(Leader leader) {
        this->leaders.push_back(leader);
    }

    Position Region::removeLeader(int playerID, std::string type) {
        for(int i = 0; i < (int)this->leaders.size(); i++) {
            if(this->leaders[i].getPlayerID() == playerID && this->leaders[i].getType() == type) {
                Position pos = this->leaders[i].getPosition();

                this->leaders.erase(this->leaders.begin() + i);

                // Check if this was the last leader in the region
                if(this->leaders.size() == 0) {
                    this->isKingdom = false;
                }

                return pos;
            }
        }

        throw std::invalid_argument(LEADER_NOT_IN_REGION_MSG);

    }

    void Region::addMonument(Monument monument) {

        this->monuments.push_back(monument);

    }

    void Region::addTreasure(Position position, bool isSpecial) {

        Treasure treasure(position, isSpecial);
        this->treasures.push_back(treasure);

    }
     
    void Region::addTreasure(Treasure treasure) {
        this->treasures.push_back(treasure);
    }

    void Region::removeTreasure(Position position) {
        for(int i = 0; i < (int)this->treasures.size(); i++) {
            if(this->treasures[i].getPosition() == position) {
                this->treasures.erase(this->treasures.begin() + i);
                return;
            }
        }

        throw std::invalid_argument(TREASURE_NOT_IN_REGION_MSG);

    }

    void Region::updateLeaderStrength(std::string tileType) {
        // Map relating leader to tile types
        std::unordered_map<std::string, std::string> tile_leader_map = {{SETTLEMENT, KING}, {TEMPLE, PRIEST}, {FARM, FARMER}, {MARKET, TRADER}};

        for(int i = 0; i < (int)this->leaders.size(); i++) {
            if(this->leaders[i].getType() == tile_leader_map[tileType]) {
                this->leaders[i].setStrength(this->leaders[i].getStrength() + 1);
            }
        }

    }

    void Region::updateAllLeadersStrength() {
        // Map relating leader to tile types
        std::unordered_map<std::string, std::string> tile_leader_map = {{SETTLEMENT, KING}, {TEMPLE, PRIEST}, {FARM, FARMER}, {MARKET, TRADER}};

        for(int i = 0; i < (int)this->leaders.size(); i++) {
            // Set leader strength to 0
            this->leaders[i].setStrength(0);

            // Add 1 strength for each correspondent tile
            for(auto tile: this->tiles) {
                if(this->leaders[i].getType() == tile_leader_map[tile.getType()]) {
                    this->leaders[i].setStrength(this->leaders[i].getStrength() + 1);
                }
            }
        }

    }

    int Region::getRegionID() {
        return this->regionID;
    }

    bool Region::getIsKingdom() {
        return this->isKingdom;
    }

    void Region::setIsKingdom(bool isKingdom) {
        this->isKingdom = isKingdom;
    }

    std::vector<Tile> Region::getTiles() {
        return this->tiles;
    }

    std::vector<Leader> Region::getLeaders() {
        return this->leaders;
    }

    std::vector<Monument> Region::getMonuments() {
        return this->monuments;
    }

    std::vector<Treasure> Region::getTreasures() {
        return this->treasures;
    }

    bool Region::getIsAtWar() {
        return this->isAtWar;
    }

    void Region::setIsAtWar(bool isAtWar) {
        this->isAtWar = isAtWar;
    }

    bool Region::getIsInRevolt() {
        return this->isInRevolt;
    }

    void Region::setIsInRevolt(bool isInRevolt) {
        this->isInRevolt = isInRevolt;
    }

    Position Region::getUnificationTilePosition() {
        return this->unificationTilePosition;
    }

    void Region::setUnificationTilePosition(Position unificationTilePosition) {
        this->unificationTilePosition = unificationTilePosition;
    }

}