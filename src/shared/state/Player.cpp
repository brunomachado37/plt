#include "Player.h"
#include "../constants.h"
#include "../messages.h"

#include <stdexcept>

namespace state {

    Player::Player(int id) {

        this->id = id;

        Leader king(KING, true, {-1, -1}, id);
        Leader priest(PRIEST, true, {-1, -1}, id);
        Leader farmer(FARMER, true, {-1, -1}, id);
        Leader trader(TRADER, true, {-1, -1}, id);

        this->leadersInHand = {{KING, king}, {PRIEST, priest}, {FARMER, farmer}, {TRADER, trader}};

    }

    void Player::addTileToHand(std::string type) {

        Tile tile(type, true, {-1, -1});
        this->tilesInHand.push_back(tile);

    }

    void Player::removeTileFromHand(std::string type) {

        for(int i = 0; i < HAND_LIMIT; i++) {
            if(this->tilesInHand[i].getType() == type) {
                this->tilesInHand.erase(this->tilesInHand.begin() + i);
                break;
            }
        }

        throw std::invalid_argument(TILE_NOT_IN_HAND_MSG);

    }


    std::vector<Tile> Player::getTilesInHand() {

        return this->tilesInHand;

    }

    std::unordered_map<std::string, Leader> Player::getLeadersInHand() {

        return this->leadersInHand;

    }

}