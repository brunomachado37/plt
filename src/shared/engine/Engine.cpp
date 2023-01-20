#include "Engine.h"
#include "../constants.h"
#include "../messages.h"

#include <iostream>
#include <algorithm>
#include <limits>

namespace engine {

    Engine::Engine(int numPlayers) : numPlayers(numPlayers) {
        
        // Create empty state with given number of players
        state::State state(numPlayers);
        this->state = state;
        this->attackPendent = false;
        this->defensePendent = false;
        this->monumentPendent = false;

    }

    Engine::Engine(const engine::Engine& otherEngine) {

        std::unique_lock<std::mutex> lock_other(otherEngine.actionMutex);

        this->numPlayers = otherEngine.numPlayers;
        this->state = otherEngine.state;
        this->attackPendent = otherEngine.attackPendent;
        this->defensePendent = otherEngine.defensePendent;
        this->monumentPendent = otherEngine.monumentPendent;
        this->pendingActions = otherEngine.pendingActions;
        this->actionsLog = otherEngine.actionsLog;
        this->stateLog = otherEngine.stateLog;
        this->finalScore = otherEngine.finalScore;

    }

    Engine& Engine::operator=(Engine const& otherEngine) {

        if(&otherEngine != this) {

            std::unique_lock<std::mutex> lock_this(actionMutex, std::defer_lock);
            std::unique_lock<std::mutex> lock_other(otherEngine.actionMutex, std::defer_lock);

            std::lock(lock_this, lock_other);

            this->numPlayers = otherEngine.numPlayers;
            this->state = otherEngine.state;
            this->attackPendent = otherEngine.attackPendent;
            this->defensePendent = otherEngine.defensePendent;
            this->monumentPendent = otherEngine.monumentPendent;
            this->pendingActions = otherEngine.pendingActions;
            this->actionsLog = otherEngine.actionsLog;
            this->stateLog = otherEngine.stateLog;
            this->finalScore = otherEngine.finalScore;

        }

        return *this;

    }

    void Engine::init() {
        // Initialize game state
        this->state.init();

        // Initialize final scores array
        for(int i = 0; i < (int)this->state.getPlayers().size(); i++) {
            this->finalScore.push_back(STD_FINAL_SCORE);
        }

    }

    void Engine::play(std::shared_ptr<Action> action, bool explore) {

        this->pendingActions.push_back(std::make_pair(action, explore));

        if(this->recordEnabled) {

            Json::Value jsonAction = action->serialize();
		    this->record[JSON_ARRAY][this->record[JSON_ARRAY].size()] = jsonAction;

        }

    }

    void Engine::update() {

        while(!this->pendingActions.empty()) {

            // Retrieve action from the list and update the list
            std::shared_ptr<Action> action = pendingActions[0].first;
            bool explore = pendingActions[0].second;
            pendingActions.erase(pendingActions.begin());

            // Save previous state in stateLog
            std::shared_ptr<state::State> previousState = std::make_shared<state::State>(this->state);
            this->stateLog.push_back(previousState);

            // Save action in actions log
            this->actionsLog.push_back(action);

            // Return in case of pendencies
            if(checkPendencies(action))
                return;
            
            // Execute otherwise
            try {
                action->execute(this->state);
            }
            catch(const std::invalid_argument& e) {
                if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                    // Trigger end of the game
                    this->endGame(explore);
                    return;
                }
                else if(explore) {
                    throw;
                }
                else {
                    std::cout << e.what() << std::endl;
                    return;  
                }      
            }

            this->handleRecord();

            this->handlePendencies(action->getActionID(), explore);

        }

    }

    bool Engine::checkPendencies(std::shared_ptr<Action> action) {

        // Check if required solve conflict action was sent, in case of a conflict

        if(this->attackPendent) {
            if(action->getActionID() != ACTION_ID_ATTACK) {
                std::cout << INVALID_ACTION_ATTACK << std::endl;
                return true;
            }
        }
        else if(this->defensePendent) {
            if(action->getActionID() != ACTION_ID_DEFENSE) {
                std::cout << INVALID_ACTION_DEFENSE << std::endl;
                return true;
            }
        }
        else if(this->monumentPendent) {
            if(action->getActionID() != ACTION_ID_BUILD_MONUM) {
                std::cout << INVALID_ACTION_MONUMENT << std::endl;
                return true;
            }
        }

        return false;

    }

    void Engine::handleRecord() {

        // Save random conditions after action execution
        if(this->recordEnabled) {

            Json::Value jsonState = this->state.serialize();
            this->record[JSON_RANDOM_ARRAY][this->record[JSON_RANDOM_ARRAY].size()] = jsonState;

        }

        // Set random conditions when playing record
        if(this->playingRecord && !(this->receivedStateRecord.empty())) {

            Json::ArrayIndex index = 0;
            Json::Value dummieArray;

            this->setRandomConditions(this->receivedStateRecord[index]);
            this->receivedStateRecord.removeIndex(index, &dummieArray);

        }

    }

    void Engine::handlePendencies(int actionID, bool explore) {

        if(actionID == ACTION_ID_BUILD_MONUM) {
            this->monumentPendent = false;
        }
        if(actionID == ACTION_ID_ATTACK) {
            this->attackPendent  = false;
            this->defensePendent = true;
        }
        if(actionID == ACTION_ID_DEFENSE) {
            this->defensePendent = false;
        }

        if(actionID != ACTION_ID_ATTACK && actionID != ACTION_ID_BUILD_MONUM) {
            // Check for war or conflict
            this->checkForConflicts();
        }

        // Check for monuments to be built
        this->checkForMonuments();

        // Update game state if action was successful and there's no pendencies
        if(!(this->attackPendent || this->defensePendent || this->monumentPendent)) {
            this->endOfAction(explore);
        }

    }

    void Engine::rollback(bool actionLog) {

        // State receives previous state
        this->state = *stateLog.back();

        // Delete last saved state
        this->stateLog.pop_back();

        if(actionLog) {

            // Delete last saved action
            this->actionsLog.pop_back();

        }

        // Checks
        this->checkForConflicts();
        this->checkForMonuments();

    }


    void Engine::checkForConflicts() {

        unsigned int count = 0;

        // Iterate over all regions to identify conflits
        for(auto region: this->state.getBoard().getRegions()) {
            if(region.second.getIsAtWar() || region.second.getIsInRevolt()) {
                this->attackPendent = true;
            }
            else {
                count++;
            }
        }

        if(count == this->state.getBoard().getRegions().size())
            this->attackPendent = false;

    }

    void Engine::checkForMonuments() {

        if(this->state.getBoard().getPossibleMonuments().size() != 0) {
            this->monumentPendent = true;
        }
        else {
            this->monumentPendent = false;
        }

    }

    void Engine::distributeTreasures() {

        std::unordered_map<int, state::Region> regions = this->state.getBoard().getRegions();

        // Iterate over all regions
        for(auto region: regions) {
            // Check if region has more than 2 treasure in it
            if(region.second.getTreasures().size() > 1) {
                // Check if there's a trader in the region
                for(auto lead: region.second.getLeaders()) {
                    if(lead.getType() == TRADER) {
                        while(region.second.getTreasures().size() > 1) {
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
            while(player.second.getTilesInHand().size() < state::HAND_LIMIT) {
                try {
                    type = this->state.getRandomTileType();
                }
                catch(const std::invalid_argument& e) {
                    if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                        throw;
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

    void Engine::endOfAction(bool explore) {

        // Check for treasure distribution
        this->distributeTreasures();
        
        // Save active player ID
        int activePlayerID = this->state.getActivePlayerID();

        // If turn pass
        if(this->state.nextAction()) {
            // All player's draw tiles until hand limit
            try {
                this->fillPlayersHands();
            }
            catch(const std::invalid_argument& e) {
                if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                    // Trigger end of the game
                    this->endGame(explore);
                    return;
                }
            }

            // Monuments distribute victory points
            this->monumentsDistribute(activePlayerID);

            // Check for end of game
            if(this->state.getRemainingTreasures() <= 2) {
                // Trigger end of the game
                this->endGame(explore);
            }
        }

    }

    int Engine::evaluateState(int playerID) {

        int score = 0;

        // Get player's points
        std::unordered_map<std::string, int> playerPoints = this->state.getPlayers()[playerID].getVictoryPoints();

        if(this->gameNotOver()) {
            
            // Give 3 points for each treasure
            score += 3 * playerPoints[TREASURE];
            playerPoints.erase(TREASURE);

            // Sum everything
            for(auto type: playerPoints) {
                score += type.second;
            }

        }
        // If the game is over, consider final score
        else {

            // Distribute treasures
            while(playerPoints[TREASURE] > 0) {

                std::string lowerType;
                int lowerAmount = std::numeric_limits<int>::max();

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

            // Check minimum score of the player
            score = std::numeric_limits<int>::max();

            for(auto type: playerPoints) {
                if(type.second < score) {
                    score = type.second;
                }
            }
        }

        return score;

    }

    void Engine::endGame(bool explore) {

        std::lock_guard<std::mutex> lock(this->actionMutex);

        this->handleRecord();

        // Check if one of the end game conditions were reached
        if(this->state.getRemainingTreasures() > 2 && (this->state.getRemainingTiles()[FARM] + this->state.getRemainingTiles()[TEMPLE] + this->state.getRemainingTiles()[SETTLEMENT] + this->state.getRemainingTiles()[MARKET] != 0)) {
            throw std::logic_error(GAME_END_ERROR_MSG);
        }

        // Count players final score
        std::vector<std::unordered_map<std::string, int>> playersScore((int)this->state.getPlayers().size());

        for(auto player: this->state.getPlayers()) {
            // Get player's points
            std::unordered_map<std::string, int> playerPoints = player.second.getVictoryPoints();

            // Distribute treasures
            while(playerPoints[TREASURE] > 0) {

                std::string lowerType;
                int lowerAmount = std::numeric_limits<int>::max();

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
                this->finalScore[i] = std::numeric_limits<int>::max();
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
            winnerScore = std::numeric_limits<int>::min();

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

        if(!explore) {
            std::cout << GAME_OVER_MSG << std::endl;

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

    }

    bool Engine::gameNotOver() {

        std::lock_guard<std::mutex> lock(this->actionMutex);

        return (this->finalScore[0] == STD_FINAL_SCORE);

    }

    void Engine::finishGame() {

        std::lock_guard<std::mutex> lock(this->actionMutex);

        this->finalScore[0] = GAME_INTERRUPTED;

    }

    bool Engine::turnNotReached(int turn) {

        if(this->state.getTurn() < turn)
            return true;

        return false;

    }

    void Engine::enableRecord() {

        this->record[JSON_INITIAL] = this->state.serialize();
        this->recordEnabled = true;

    }

    void Engine::setRandomConditions(Json::Value& stateRecord) {

        this->state.setRandomConditions(stateRecord);

    }

    void Engine::enablePlayingRecord(Json::Value stateRecordArray) {

        this->receivedStateRecord = stateRecordArray;
        this->playingRecord = true;

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

        std::lock_guard<std::mutex> lock(this->actionMutex);

        return this->finalScore;

    }

    std::vector<std::shared_ptr<Action>> Engine::getActionsLog() {

        return this->actionsLog;

    }

    Json::Value Engine::getRecord() {

        return this->record;

    }

    Json::Value Engine::getLastPlayedAction() {

        return this->record[JSON_ARRAY][this->record[JSON_ARRAY].size() - 1];

    }

    void Engine::clearAction() {

        this->pendingActions.clear();

    }

}