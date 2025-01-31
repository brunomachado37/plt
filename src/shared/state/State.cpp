#include "State.h"
#include "../constants.h"
#include "../messages.h"

#include <random>

namespace state {

    State::State(int numPlayers) : numPlayers(numPlayers) {
        Board b;
        this->board = b;

        std::random_device rd;
        auto g = std::mt19937(rd());

        this->gen = g;

        for(int i = 0; i < numPlayers; i++) {
            Player player(i);

            for(int j = 0; j < state::HAND_LIMIT; j++) {
                player.addTileToHand(this->getRandomTileType());
            }

            this->players[i] = player;
            
        }

    }


    void State::init() {
        this->board.init();
    }


    std::string State::getRandomTileType() {
        float sum = this->remainingTiles[MARKET] + this->remainingTiles[FARM] + this->remainingTiles[TEMPLE] + this->remainingTiles[SETTLEMENT];
        float probGreen = this->remainingTiles[MARKET] / sum;
        float probBlack = this->remainingTiles[SETTLEMENT] / sum;
        float probRed = this->remainingTiles[TEMPLE] / sum;

        
        std::uniform_real_distribution<> dis(0, 1.0);

        float choice = dis(gen);

        if(sum == 0.0) {
            throw OutOfTilesException(END_GAME_TILE);
        }
        else if(choice < probGreen) {
            this->remainingTiles[MARKET] -= 1;
            return MARKET;
        }
        else if(choice < probGreen + probBlack) {
            this->remainingTiles[SETTLEMENT] -= 1;
            return SETTLEMENT;
        }
        else if(choice < probGreen + probBlack + probRed) {
            this->remainingTiles[TEMPLE] -= 1;
            return TEMPLE;
        }
        else {
            this->remainingTiles[FARM] -= 1;
            return FARM;
        }
        
    }

    void State::nextTurn() {
        
        this->turn++;

        if(this->activePlayerID == (this->numPlayers - 1)) {
            this->activePlayerID = 0;
        }
        else {
            this->activePlayerID++;
        }

    }

    bool State::nextAction() {

        if(this->actionsDone == 1) {
            this->actionsDone = 0;
            this->nextTurn();
            // Return true if turn pass 
            return true;
        }
        else {
            this->actionsDone++;
            // Return false if turn doesn't pass
            return false;
        }

    }

    Json::Value State::serialize() {

        Json::Value stateInfo;

        for(auto player: this->players)
            stateInfo[JSON_PLAYER][player.first] = player.second.serializeHand();

        for(auto type: this->remainingTiles)
            stateInfo[JSON_TILES][type.first] = type.second;

        return stateInfo;

    }

    void State::setRandomConditions(Json::Value stateRecord) {

        // Deserialize remaining tiles
        this->remainingTiles[FARM] = stateRecord["RemainingTiles"][FARM].asInt();
        this->remainingTiles[MARKET] = stateRecord["RemainingTiles"][MARKET].asInt();
        this->remainingTiles[SETTLEMENT] = stateRecord["RemainingTiles"][SETTLEMENT].asInt();
        this->remainingTiles[TEMPLE] = stateRecord["RemainingTiles"][TEMPLE].asInt();

        // Deserialize player's hand
        for(int i = 0; i < (int)this->players.size(); i++)
            this->players[i].deserializeHand(stateRecord["PlayerHand"][i]);

    }

    Board State::getBoard() {
        return this->board;
    }

    void State::setBoard(Board board) {
        this->board = board;
    }

    std::unordered_map<int, Player> State::getPlayers() {
        return this->players;
    }

    void State::setPlayer(Player player) {
        this->players[player.getID()] = player;
    }

    std::unordered_map<std::string, int> State::getRemainingTiles() {
        return this->remainingTiles;
    }

    int State::getTurn() {
        return this->turn;
    }

    int State::getNumPlayers() {
        return this->numPlayers;
    }

    int State::getActionsDone() {
        return this->actionsDone;
    }

    int State::getActivePlayerID() {
        return this->activePlayerID;
    }

    int State::getRemainingTreasures() {
        return this->remainingTreasures;
    }

    void State::setRemainingTreasures(int remainingTreasures) {
        this->remainingTreasures = remainingTreasures;
    }

}