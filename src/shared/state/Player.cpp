#include "Player.h"
#include "../constants.h"
#include "../messages.h"

#include <stdexcept>

namespace state {

    Player::Player() {}

    Player::Player(int id) {

        this->id = id;

        Leader king(KING, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, id);
        Leader priest(PRIEST, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, id);
        Leader farmer(FARMER, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, id);
        Leader trader(TRADER, {NOT_IN_MAP_ID, NOT_IN_MAP_ID}, id);

        this->leadersInHand = {{KING, king}, {PRIEST, priest}, {FARMER, farmer}, {TRADER, trader}};

    }

    void Player::addTileToHand(std::string type) {
        if(this->tilesInHand.size() < HAND_LIMIT) {
            Tile tile(type, {NOT_IN_MAP_ID, NOT_IN_MAP_ID});
            this->tilesInHand.push_back(tile);
        }
        else {
            throw std::invalid_argument(MANY_TILE_IN_HAND_MSG);
        }

    }

    void Player::removeTileFromHand(std::string type) {

        for(int i = 0; i < (int)this->tilesInHand.size(); i++) {
            if(this->tilesInHand[i].getType() == type) {
                this->tilesInHand.erase(this->tilesInHand.begin() + i);
                return;
            }
        }

        throw std::invalid_argument(TILE_NOT_IN_HAND_MSG);

    }

    void Player::addLeaderToHand(Leader leader) {
        if(leader.getPlayerID() != this->id) {
            throw std::invalid_argument(LEADER_WRONG_ID_MSG);
        }
        if(this->leadersInHand.find(leader.getType()) == this->leadersInHand.end()) {
            leader.setPosition({NOT_IN_MAP_ID, NOT_IN_MAP_ID});
            this->leadersInHand[leader.getType()] = leader;
        }
        else {
            throw std::invalid_argument(LEADER_IN_HAND_MSG);
        }

    }

    void Player::removeLeaderFromHand(std::string type) {
        if(this->leadersInHand.erase(type) == 0) {
            throw std::invalid_argument(LEADER_NOT_IN_HAND_MSG);
        }
    }

    void Player::addVictoryPoints(std::string color, int amount) {
        this->victoryPoints[color] += amount;
    }


    Json::Value Player::serializeHand() {

        Json::Value hand;
        int i = -1;

        for(auto tile: this->tilesInHand)
            hand[++i] = tile.getType();

        return hand;

    }

    void Player::deserializeHand(Json::Value hand) {

        this->tilesInHand.clear();

        for(auto tileType: hand)
            addTileToHand(tileType.asString());

    }

    std::vector<Tile> Player::getTilesInHand() {

        return this->tilesInHand;

    }

    std::unordered_map<std::string, Leader> Player::getLeadersInHand() {

        return this->leadersInHand;

    }

    int Player::getID() {
        return this->id;
    }

    std::unordered_map<std::string, int> Player::getVictoryPoints() {
        return this->victoryPoints;
    }

    int Player::getCatastropheTiles() {
        return this->catastropheTiles;
    }

    void Player::useCatastropheTile() {
        if(this->catastropheTiles > 0) {
            this->catastropheTiles--;
        }
        else {
            throw std::invalid_argument(NO_CATAST_LEFT_MSG);
        }
    }

}