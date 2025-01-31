#include "Scene.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"

#include <engine.h>
#include <ai.h>

#include <iostream>
#include <unistd.h>

namespace render {

    Scene::Scene(int playerID) {

        this->playerID = playerID;

        this->playerDraw = PlayerDraw();
        this->gameDraw = GameDraw();

    }


    void Scene::display(state::State state, sf::RenderWindow& window) {

        this->drawState(state, window);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                        std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    }
                }

                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.display();

        }

    }

    void Scene::run(engine::Engine engine, ai::AI* ai_1, ai::AI* ai_2, sf::RenderWindow& window) { 

        // Draw initial state
        engine.init();
        this->drawState(engine.getState(), window);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();

            // Check for end of game
            if(engine.gameNotOver()) {

                // If no defense is pending
                if(!engine.getDefensePendent()) {

                    if(engine.getState().getActivePlayerID() == 0) {
                        ai_1->run(engine);
                    }
                    else {
                        ai_2->run(engine);
                    }

                }
                else {

                    // Play defense
                    if(engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPlayerID() == 0) {
                        ai_2->run(engine);
                    }
                    else {
                        ai_1->run(engine);
                    }

                }

                // Update Engine
                engine.update();

                // Draw State
                this->drawState(engine.getState(), window);

            }
            // If game ended
            else {

                // Draw game over screen
                this->gameDraw.drawGameOver(engine.getFinalScore(), window);
                
            }

        }

    }

    void Scene::runRollback(engine::Engine engine, ai::AI* ai_1, ai::AI* ai_2, int numberOfTurns, sf::RenderWindow& window) { 

        // Draw initial state
        engine.init();
        this->drawState(engine.getState(), window);

        // Rollback flag
        bool rollback = false;

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();
            sleep(1);

            // Check for rollback
            if(engine.getState().getTurn() == numberOfTurns) {
                rollback = true;
            }

            // Check for end of game
            if(engine.gameNotOver()) {

                if(rollback) {
                    if(engine.getState().getTurn() > 0) {
                        engine.rollback();
                    }
                    // Check for end of the rollback
                    else if(engine.getState().getActionsDone() > 0) {
                        engine.rollback();
                    }
                }

                else {
                    // If no defense is pending
                    if(!engine.getDefensePendent()) {

                        if(engine.getState().getActivePlayerID() == 0) {
                            ai_1->run(engine);
                        }
                        else {
                            ai_2->run(engine);
                        }

                    }
                    else {

                        // Play defense
                        if(engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPlayerID() == 0) {
                            ai_2->run(engine);
                        }
                        else {
                            ai_1->run(engine);
                        }

                    }

                    engine.update();
                    
                }

                // Draw State
                this->drawState(engine.getState(), window);

            }
            // If game ended
            else {

                // Draw game over screen
                this->gameDraw.drawGameOver(engine.getFinalScore(), window);
                
            }

        }

    }

    void Scene::contest(ai::AI* ai_1, ai::AI* ai_2) { 

        // Save winner stats
        int ai1_count = 0;
        int ai2_count = 0;

        for(int n = 0; n < ROUNDS_CONTEST; n++) {
            // Create engine
            engine::Engine engine;

            // Initial state
            engine.init();

            while(engine.gameNotOver()) {

                // If no defense is pending
                if(!engine.getDefensePendent()) {

                    if(engine.getState().getActivePlayerID() == 0) {
                        ai_1->run(engine);
                    }
                    else {
                        ai_2->run(engine);
                    }

                }
                else {

                    // Play defense
                    if(engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPlayerID() == 0) {
                        ai_2->run(engine);
                    }
                    else {
                        ai_1->run(engine);
                    }

                }
            }

            // Check who's the winner
            std::vector<int> finalScore = engine.getFinalScore();

            // If is not a tie
            if(finalScore[0] != finalScore[1]) {

                int winner = 0;
                int winnerPoints = -1;

                for(int i = 0; i < (int)finalScore.size(); i++) {
                    
                    if(finalScore[i] > winnerPoints) {
                        winnerPoints = finalScore[i];
                        winner = i;
                    }

                }

                if(winner == 0)
                    ai1_count++;
                else if(winner == 1)
                    ai2_count++;

            }

        }

        std::cout << "From " << ROUNDS_CONTEST << " matches played, " << ai_1->getType() << " AI won " << ai1_count << " times while " << ai_2->getType() << " AI won " << ai2_count << " times" << std::endl;

    }

    void Scene::displayDemoEngine(engine::Engine engine, sf::RenderWindow& window) {

        // Count number of actions
        int countActions = 0;

        // Check players hands
        std::vector<state::Tile>    hand1;
        std::vector<state::Tile>    tiles;
        for(auto tile: engine.getState().getPlayers()[0].getTilesInHand())
            if(tile.getType() != FARM)
                hand1.push_back(tile);
            else
                tiles.push_back(tile);

        if(tiles.size() == 0)
            tiles = {engine.getState().getPlayers()[0].getTilesInHand()[5]};

        std::vector<state::Tile>    hand2;
        for(auto tile: engine.getState().getPlayers()[1].getTilesInHand())
            if(tile.getType() != FARM)
                hand2.push_back(tile);

        // Check condition for demo to run
        if(hand1.size() < 3 || hand2.size() < 3) {
            std::cout << EXIT_DEMO_MSG << std::endl;
            return;
        }

        // Count attacker supporters
        int attSup = 1;
        for(int t = 0; t < 2; t++)
            if(hand2[t].getType() == TEMPLE)
                attSup++;


        // Create list of actions
        state::Position pos = {6, 9};

        std::shared_ptr<engine::PlayLeader>         action1 = std::make_shared<engine::PlayLeader>(engine.getState().getPlayers()[0].getLeadersInHand()[KING], pos, 0);
        pos = {5, 8};
        std::shared_ptr<engine::PlayTile>           action2 = std::make_shared<engine::PlayTile>(hand1[0], pos, 0);
        pos = {2, 6};
        std::shared_ptr<engine::PlayLeader>         action3 = std::make_shared<engine::PlayLeader>(engine.getState().getPlayers()[1].getLeadersInHand()[PRIEST], pos, 1);
        pos = {2, 7};
        std::shared_ptr<engine::PlayTile>           action4 = std::make_shared<engine::PlayTile>(hand2[0], pos, 1);
        pos = {5, 7};
        std::shared_ptr<engine::PlayTile>           action5 = std::make_shared<engine::PlayTile>(hand1[1], pos, 0);
        pos = {4, 8};
        std::shared_ptr<engine::PlayTile>           action6 = std::make_shared<engine::PlayTile>(hand1[2], pos, 0);
        pos = {3, 7};
        std::shared_ptr<engine::PlayTile>           action7 = std::make_shared<engine::PlayTile>(hand2[1], pos, 1);
        pos = {4, 7};
        std::shared_ptr<engine::PlayCatastrophe>    action8 = std::make_shared<engine::PlayCatastrophe>(pos, 1);
        std::shared_ptr<engine::PlayDrawTiles>      action9 = std::make_shared<engine::PlayDrawTiles>(tiles, 0);
        pos = {6, 7};
        std::shared_ptr<engine::PlayLeader>         action10 = std::make_shared<engine::PlayLeader>(engine.getState().getPlayers()[0].getLeadersInHand()[PRIEST], pos, 0);
        pos = {3, 8};
        std::shared_ptr<engine::PlayTile>           action11 = std::make_shared<engine::PlayTile>(hand2[2], pos, 1);
        std::shared_ptr<engine::PlayAttack>         action12 = std::make_shared<engine::PlayAttack>(WAR, pos, 0, 1, PRIEST);
        std::shared_ptr<engine::PlayDefense>        action13 = std::make_shared<engine::PlayDefense>(action12->getConflictType(), action12->getPosition(), attSup, 0, 0, action12->getWarLeaderType());
        pos = {5, 8};
        std::shared_ptr<engine::PlayCatastrophe>    action14 = std::make_shared<engine::PlayCatastrophe>(pos, 1);
        pos = {4, 12};
        std::shared_ptr<engine::PlayMoveLeader>     action15 = std::make_shared<engine::PlayMoveLeader>(KING, pos, 0);

        std::vector<std::shared_ptr<engine::Action>> actions = {action1, action2, action3, action4, action5, action6, action7, action8, action9, action10, action11, action12, action13, action14, action15};
        int actions_size = actions.size();

        // Action ID to name of the action map
        std::vector<std::string> actionsMap = {"Play tile", "Play leader", "Play catastrophe", "Play draw tiles", "Play build monument", "Play attack", "Play defense", "Play move leader"};
        
        // Draw initial state
        engine.init();
        this->drawState(engine.getState(), window);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();
            sleep(1);

            if(countActions < actions_size) {
                engine.play(actions[countActions]);
                engine.update();
                this->drawState(engine.getState(), window);
                std::cout << "Command: " << actionsMap[actions[countActions]->getActionID()] << std::endl;
                countActions++;
            }

        }

    }

    void Scene::displayDemo(state::State state, sf::RenderWindow& window) {

        // Count number of left mouse clicks
        int countLeftClick = 0;
        bool click = false;

        this->drawState(state, window);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                        std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                        click = true;
                    }
                }

                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            // Display a different game state as the left mouse button is clicked
            if(click) {
                if(countLeftClick == 0) {
                    state.init();
                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 1) {
                    if(state.getPlayers()[0].getTilesInHand()[0].getType() != FARM) {
                        state::Board board = state.getBoard();
                        board.addTileToTheBoard(state.getPlayers()[0].getTilesInHand()[0], {9, 8});
                        state::Player player = state.getPlayers()[0];
                        player.removeTileFromHand(state.getPlayers()[0].getTilesInHand()[0].getType());
                        state.setPlayer(player);
                        state.setBoard(board);

                        state.nextAction();

                        this->drawState(state, window);
                    }
                    
                    countLeftClick++;
                }
                else if(countLeftClick == 2) {
                    state::Tile temple(TEMPLE, {-1, -1});
                    state::Board board = state.getBoard();
                    board.addTileToTheBoard(temple, {0, 0});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 3) {
                    state::Tile temple(TEMPLE, {-1, -1});
                    state::Board board = state.getBoard();
                    board.addTileToTheBoard(temple, {0, 1});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 4) {
                    state::Tile temple(TEMPLE, {-1, -1});
                    state::Board board = state.getBoard();
                    board.addTileToTheBoard(temple, {1, 0});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 5) {
                    state::Board board = state.getBoard();
                    board.addMonumentToTheBoard(board.getMonuments()[5], {0, 0});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 6) {
                    state::Tile temple(TEMPLE, {-1, -1});
                    state::Board board = state.getBoard();
                    board.addTileToTheBoard(temple, {4, 12});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 7) {
                    state::Board board = state.getBoard();
                    board.addLeaderToTheBoard(state.getPlayers()[1].getLeadersInHand()[TRADER], {5, 12});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 8) {
                    state::Board board = state.getBoard();
                    board.addCatastropheToTheBoard({4, 12});
                    state::Player player = state.getPlayers()[0];
                    player.useCatastropheTile();
                    state.setPlayer(player);
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 9) {
                    state::Board board = state.getBoard();
                    state::Tile temple(TEMPLE, {-1, -1});
                    board.addTileToTheBoard(temple, {9, 7});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 10) {
                    state::Board board = state.getBoard();
                    board.addLeaderToTheBoard(state.getPlayers()[0].getLeadersInHand()[FARMER], {10, 7});
                    state::Player player = state.getPlayers()[0];
                    player.removeLeaderFromHand(FARMER);
                    state.setPlayer(player);
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 11) {
                    state::Board board = state.getBoard();
                    state::Tile settlement(SETTLEMENT, {-1, -1});
                    board.addTileToTheBoard(settlement, {9, 4});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 12) {
                    state::Board board = state.getBoard();
                    board.addLeaderToTheBoard(state.getPlayers()[1].getLeadersInHand()[FARMER], {8, 5});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 13) {
                    state::Board board = state.getBoard();
                    state::Tile market(MARKET, {-1, -1});
                    board.addTileToTheBoard(market, {10, 4});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }
                else if(countLeftClick == 14) {
                    state::Board board = state.getBoard();
                    state::Tile market(MARKET, {-1, -1});
                    board.addTileToTheBoard(market, {9, 6});
                    state.setBoard(board);

                    state.nextAction();

                    this->drawState(state, window);
                    countLeftClick++;
                }

                click = false;

            }

            window.display();
        }

    }

    void Scene::drawState(state::State state, sf::RenderWindow& window) {

        // Draw board image (erasing anything previously draw)
        this->gameDraw.drawBoardImage(window);

        // Draw board elements
        this->gameDraw.drawBoardState(state.getBoard(), window);

        // Draw player info
        this->playerDraw.drawPlayerState(state.getPlayers()[this->playerID], window);

        // Draw game info
        this->gameDraw.drawGameInfo(state, window);

    }

    void Scene::drawGameOver(std::vector<int> finalScore, sf::RenderWindow& window) {
        
        this->gameDraw.drawGameOver(finalScore, window);

    }

}