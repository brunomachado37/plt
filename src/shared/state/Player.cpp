#include "Player.h"
#include "../constants.h"
#include "../messages.h"

#include <stdexcept>

namespace state {

    Player::Player() {}

    Player::Player(int id) {

        this->id = id;

        Leader king(KING, {-1, -1}, id);
        Leader priest(PRIEST, {-1, -1}, id);
        Leader farmer(FARMER, {-1, -1}, id);
        Leader trader(TRADER, {-1, -1}, id);

        this->leadersInHand = {{KING, king}, {PRIEST, priest}, {FARMER, farmer}, {TRADER, trader}};

    }

    void Player::addTileToHand(std::string type) {
        if(this->tilesInHand.size() < HAND_LIMIT) {
            Tile tile(type, {-1, -1});
            this->tilesInHand.push_back(tile);
        }
        else {
            throw std::invalid_argument(MANY_TILE_IN_HAND_MSG);
        }

    }

    void Player::removeTileFromHand(std::string type) {

        for(int i = 0; i < HAND_LIMIT; i++) {
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
            leader.setPosition({-1, -1});
            leader.setStrength(0);
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