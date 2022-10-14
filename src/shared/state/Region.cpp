#include "Region.h"

namespace state {

    Region::Region() {}

    Region::Region(int regionID) {

        this->regionID = regionID;

    }

    void Region::addTile(std::string type, Position position) {

        Tile tile(type, false, position);
        this->tiles.push_back(tile);

    }

    void Region::addTile(Tile tile) {
        this->tiles.push_back(tile);
    }

    void Region::addLeader(Leader leader) {
        this->leaders.push_back(leader);
    }

    void Region::addTreasure(Position position, bool isSpecial) {

        Treasure treasure(position, isSpecial);
        this->treasures.push_back(treasure);

    }

    int Region::getRegionID() {
        return this->regionID;
    }

    std::vector<Tile> Region::getTiles() {
        return this->tiles;
    }

    std::vector<Leader> Region::getLeaders() {
        return this->leaders;
    }

}