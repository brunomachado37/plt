#include "Player.h"

namespace state {

    Player::Player(int id) {

        this->id = id;

        Leader king("king", true, {-1, -1}, id);
        Leader priest("priest", true, {-1, -1}, id);
        Leader farmer("farmer", true, {-1, -1}, id);
        Leader trader("trader", true, {-1, -1}, id);

        this->leadersInHand = {king, priest, farmer, trader};

    }

    void Player::addTileToHand(std::string type) {

        Tile tile(type, true, {-1, -1});
        this->tilesInHand.push_back(tile);

    }

}