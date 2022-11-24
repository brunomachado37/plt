#include "Engine.h"
#include "../constants.h"
#include "../messages.h"

#include <iostream>

namespace engine {

    Engine::Engine(int numPlayers) : numPlayers(numPlayers) {
        
        // Create empty state with given number of players
        state::State state(numPlayers);
        this->state = state;
        this->attackPendent = false;
        this->defensePendent = false;
        this->monumentPendent = false;

    }

    void Engine::init() {
        // Initialize game state
        this->state.init();

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

        }
        catch(const std::invalid_argument& e) {
            if(std::string(e.what()) == std::string(END_GAME_TILE)) {
                // Trigger end of the game
            }
            else {
                std::cout << e.what() << std::endl;
                return;  
            }          
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
        for(auto reg: this->state.getBoard().getRegions()) {
            // Check if region has more than 2 treasure in it
            if(reg.second.getTreasures().size() > 1) {
                // Check if there's a trader in the region
                for(auto lead: reg.second.getLeaders()) {
                    if(lead.getType() == TRADER) {
                        // Give correspondent player a treasure
                        state::Player player = this->state.getPlayers()[lead.getPlayerID()];
                        player.addVictoryPoints(TREASURE, 1);
                        this->state.setPlayer(player);

                        // Remove treasure from the region
                        reg.second.removeTreasure();
                        state::Board board = this->state.getBoard();
                        board.setRegion(reg.second);
                        this->state.setBoard(board);
                    }
                }
            }
        }

    }

    void Engine::fillPlayersHands() {

        // Fill all players hands
        for(auto player: this->state.getPlayers()) {
            // Draw tile until fill player's hand
            while(player.second.getTilesInHand().size() < HAND_LIMIT) {
                player.second.addTileToHand(this->state.getRandomTileType());
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
        }

    }

    state::State Engine::getState() {

        return this->state;

    }

}