#include "Region.h"

namespace state {

    Region::Region(int regionID) {

        this->regionID = regionID;

    }

    void Region::Region::addPiece(std::string type, std::string color, Position position) {

        if(type == "tile") {
            Piece tile(color, false, position);
            this->pieces.push_back(tile);
        }

    }

}