#include "Engine.h"
#include "../constants.h"
#include "../messages.h"

#include <iostream>
#include <algorithm>

namespace engine {

    Engine::Engine(int numPlayers) : numPlayers(numPlayers) {
        
        // Create empty state with given number of players
        state::State state(numPlayers);
        this->state = state;
        this->attackPendent = false;
        this->defensePendent = false;
        this->monumentPendent = false;

    }

    Engine::~Engine() {

        while(!this->actionsLog.empty()) delete this->actionsLog.back(), this->actionsLog.pop_back();

    }

    void Engine::init() {
        // Initialize game state
        this->state.init();

        // Initialize final scores array
        for(int i = 0; i < (int)this->state.getPlayers().size(); i++) {
            this->finalScore.push_back(STD_FINAL_SCORE);
        }

    }

    void Engine::play(Action* action) {

        // Check if required solve conflict action was sent, in case of a conflict
        if(this->attackPendent && (action->getActionID() != ACTION_ID_ATTACK)) {
            std::cout << INVALID_ACTION_ATTACK << std::endl;
            return;
        }
        if(this->defensePendent && (action->getActionID() != ACTION_ID_DEFENSE)) {
            std::cout << INVALID_ACTION_DEFENSE << std::endl;
            return;
        }
        if(this->monumentPendent && (action->getActionID() != ACTION_ID_BUILD_MONUM)) {
            std::cout << INVALID_ACTION_MONUMENT << std::endl;
            return;
        }


        // If not, execute action
        try {

            action->execute(this->state);

        }
        catch(const std::invalid_argument& e) {
            if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                // Trigger end of the game
                this->endGame();
                return;
            }
            else {
                std::cout << e.what() << std::endl;
                return;  
            }          
        }

        if(action->getActionID() == ACTION_ID_BUILD_MONUM) {
            this->monumentPendent = false;
        }
        if(action->getActionID() == ACTION_ID_ATTACK) {
            this->attackPendent  = false;
            this->defensePendent = true;
        }
        if(action->getActionID() == ACTION_ID_DEFENSE) {
            this->defensePendent = false;
        }

        if(action->getActionID() != ACTION_ID_ATTACK && action->getActionID() != ACTION_ID_BUILD_MONUM) {
            // Check for war or conflict
            this->checkForConflicts();

            // Check for monuments to be built
            this->checkForMonuments();
        }

        // Update game state if action was successful and there's no pendencies
        if(!(this->attackPendent || this->defensePendent || this->monumentPendent)) {
            this->endOfAction();
        }

        // Save action in actions log
        this->actionsLog.push_back(action);

    }

    void Engine::checkForConflicts() {

        // Iterate over all regions to identify conflits
        for(auto region: this->state.getBoard().getRegions()) {
            if(region.second.getIsAtWar() || region.second.getIsInRevolt()) {
                this->attackPendent = true;
            }
        }

    }

    void Engine::checkForMonuments() {

        if(this->state.getBoard().getPossibleMonuments().size() != 0) {
            this->monumentPendent = true;
        }

    }

    void Engine::distributeTreasures() {

        // Iterate over all regions
        for(auto region: this->state.getBoard().getRegions()) {
            // Check if region has more than 2 treasure in it
            if(this->state.getBoard().getRegions()[region.first].getTreasures().size() > 1) {
                // Check if there's a trader in the region
                for(auto lead: this->state.getBoard().getRegions()[region.first].getLeaders()) {
                    if(lead.getType() == TRADER) {
                        while(this->state.getBoard().getRegions()[region.first].getTreasures().size() > 1) {
                            // Give correspondent player a treasure
                            state::Player player = this->state.getPlayers()[lead.getPlayerID()];
                            player.addVictoryPoints(TREASURE, 1);
                            this->state.setPlayer(player);

                            // Remove treasure from the region
                            state::Board board = this->state.getBoard();
                            region.second.removeTreasure();
                            board.setRegion(region.second);
                            this->state.setBoard(board);

                            // Update treasures remaining
                            this->state.setRemainingTreasures(this->state.getRemainingTreasures() - 1);
                        }

                        break;
                        
                    }
                }
            }
        }

    }

    void Engine::fillPlayersHands() {

        // Define string variable
        std::string type;

        // Fill all players hands
        for(auto player: this->state.getPlayers()) {
            // Draw tile until fill player's hand
            while(player.second.getTilesInHand().size() < HAND_LIMIT) {
                try {
                    type = this->state.getRandomTileType();
                }
                catch(const std::invalid_argument& e) {
                    if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                        // Trigger end of the game
                        this->endGame();
                    }
                }

                player.second.addTileToHand(type);

            }
            // Update player state
            this->state.setPlayer(player.second);
        }

    }

    void Engine::monumentsDistribute(int activePlayerID) {

        // Leader to color map
        std::unordered_map<std::string, std::string> leaderToColorMap = {{FARMER, BLUE}, {PRIEST, RED}, {TRADER, GREEN}, {KING, BLACK}};

        // Iterate over all regions
        for(auto reg: this->state.getBoard().getRegions()) {
            // Iterate over all monuments in the region
            for(auto monu: reg.second.getMonuments()) {
                // Iterate over all leaders in the region
                for(auto lead: reg.second.getLeaders()) {
                    // Check if active player has a leader of the same color of the monument
                    if(lead.getPlayerID() == activePlayerID) {
                        if(leaderToColorMap[lead.getType()] == monu.getColor1() || leaderToColorMap[lead.getType()] == monu.getColor2()) {
                            state::Player player = this->state.getPlayers()[activePlayerID];
                            player.addVictoryPoints(leaderToColorMap[lead.getType()], 1);
                            this->state.setPlayer(player);
                        }
                    }
                }

            }
        }

    }

    void Engine::endOfAction() {

        // Check for treasure distribution
        this->distributeTreasures();
        
        // Save active player ID
        int activePlayerID = this->state.getActivePlayerID();

        // If turn pass
        if(this->state.nextAction()) {
            // All player's draw tiles until hand limit
            this->fillPlayersHands();

            // Monuments distribute victory points
            this->monumentsDistribute(activePlayerID);

            // Check for end of game
            if(this->state.getRemainingTreasures() <= 2) {
                // Trigger end of the game
                this->endGame();
            }
        }

    }

    void Engine::endGame() {
        // Check if one of the end game conditions were reached
        if(!(this->state.getRemainingTreasures() > 2 || (this->state.getRemainingTiles()[FARM] + this->state.getRemainingTiles()[TEMPLE] + this->state.getRemainingTiles()[SETTLEMENT] + this->state.getRemainingTiles()[MARKET] != 0))) {
            throw std::logic_error(GAME_END_ERROR_MSG);
        }

        std::cout << GAME_OVER_MSG << std::endl;

        // Count players final score
        std::vector<std::unordered_map<std::string, int>> playersScore((int)this->state.getPlayers().size());

        for(auto player: this->state.getPlayers()) {
            // Get player's points
            std::unordered_map<std::string, int> playerPoints = player.second.getVictoryPoints();

            // Distribute treasures
            while(playerPoints[TREASURE] > 0) {

                std::string lowerType;
                int lowerAmount = 99999;

                for(auto type: playerPoints) {
                    if(type.first != TREASURE) {
                        if(type.second < lowerAmount) {
                            lowerAmount = type.second;
                            lowerType = type.first;
                        }
                    }
                }

                playerPoints[lowerType]++;
                playerPoints[TREASURE]--;
            
            }

            // Save player score
            playerPoints.erase(TREASURE);
            playersScore[player.second.getID()] = playerPoints;

        }

        // Define winner
        bool tie = true;
        int winner;
        int winnerScore;

        while(tie) {
            // Check if game ended in a tie
            if(playersScore[0].size() == 0) {
                break;
            }

            // Count minimum score
            for(int i = 0; i < (int)playersScore.size(); i++) {
                // Check minimum score of the player
                this->finalScore[i] = 9999999;
                std::string minimumScoreType;

                for(auto type: playersScore[i]) {
                    if(type.second < this->finalScore[i]) {
                        this->finalScore[i] = type.second;
                        minimumScoreType = type.first;
                    }
                }

                playersScore[i].erase(minimumScoreType);

            }

            // Check if there's a winner
            winnerScore = -1;

            // Find best minimum score
            for(int i = 0; i < (int)this->finalScore.size(); i++) {
                if(this->finalScore[i] > winnerScore) {
                    winnerScore = this->finalScore[i];
                    winner = i;
                }
            }

            // Check if there's a tie
            if(std::count(this->finalScore.begin(), this->finalScore.end(), winnerScore) > 1) {
                tie = true;
            }
            else {
                tie = false;
            }
        }

        if(tie) {
            std::cout << TIE_MSG << std::endl;
        }
        else {
            for(int i = 0; i < (int)this->finalScore.size(); i++) {
                std::cout << FINAL_SCORE_1_MSG << i + 1 << FINAL_SCORE_2_MSG << this->finalScore[i] << FINAL_SCORE_3_MSG << std::endl;
            }

            std::cout << FINAL_SCORE_1_MSG << winner + 1 << FINAL_SCORE_4_MSG << std::endl;

        }

    }

    state::State Engine::getState() {

        return this->state;

    }

    bool Engine::getDefensePendent() {

        return this->defensePendent;

    }

    bool Engine::getAttackPendent() {

        return this->attackPendent;
        
    }

    bool Engine::getMonumentPendent() {

        return this->monumentPendent;
        
    }

    std::vector<int> Engine::getFinalScore() {

        return this->finalScore;

    }

    std::vector<Action*> Engine::getActionsLog() {

        return this->actionsLog;

    }

}