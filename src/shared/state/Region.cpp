#include "Region.h"

namespace state {

    Region::Region(int regionID) {

        this->regionID = regionID;

    }

    void Region::addTile(std::string type, Position position) {

        Tile tile(type, false, position);
        this->tiles.push_back(tile);

    }

    void Region::addTreasure(Position position, bool isSpecial) {

        Treasure treasure(position, isSpecial);
        this->treasures.push_back(treasure);

    }

}