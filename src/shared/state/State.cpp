#include "State.h"
#include "../constants.h"

#include <random>
#include <iostream>

namespace state {

    State::State(int numPlayers) : numPlayers(numPlayers) {
        Board b;
        this->board = b;

        for(int i = 0; i < numPlayers; i++) {
            Player player(i);

            for(int j = 0; j < HAND_LIMIT; j++) {
                player.addTileToHand(this->getRandomTileType());
            }

            this->players.push_back(player);
            
        }

    }


    std::string State::getRandomTileType() {
        float sum = this->remainingTiles[MARKET] + this->remainingTiles[FARM] + this->remainingTiles[TEMPLE] + this->remainingTiles[SETTLEMENT];
        float probGreen = this->remainingTiles[MARKET] / sum;
        float probBlack = this->remainingTiles[SETTLEMENT] / sum;
        float probRed = this->remainingTiles[TEMPLE] / sum;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1.0);

        float choice = dis(gen);

        if(choice < probGreen) {
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


    Board State::getBoard() {
        return this->board;
    }

}