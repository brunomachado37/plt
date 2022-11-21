#include "Scene.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"
#include "../../shared/engine.h"

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

    void Scene::displayDemoEngine(engine::Engine engine, sf::RenderWindow& window) {

        // Count number of actions
        int countActions = 0;

        // Check players hands
        std::vector<state::Tile>    hand1;
        for(auto tile: engine.getState().getPlayers()[0].getTilesInHand())
            if(tile.getType() != FARM)
                hand1.push_back(tile);

        std::vector<state::Tile>    hand2;
        for(auto tile: engine.getState().getPlayers()[1].getTilesInHand())
            if(tile.getType() != FARM)
                hand2.push_back(tile);

        // Create list of actions
        engine::PlayLeader          action1(engine.getState().getPlayers()[0].getLeadersInHand()[KING], {6, 9}, 0);
        engine::PlayTile            action2(hand1[0], {5, 8}, 0);
        engine::PlayLeader          action3(engine.getState().getPlayers()[1].getLeadersInHand()[TRADER], {2, 6}, 1);
        engine::PlayTile            action4(hand2[0], {2, 7}, 1);
        engine::PlayTile            action5(hand1[1], {5, 7}, 0);
        engine::PlayTile            action6(hand1[2], {1, 7}, 0);
        engine::PlayTile            action7(hand2[1], {3, 7}, 1);
        engine::PlayTile            action8(hand2[2], {3, 8}, 1);
        engine::PlayCatastrophe     action9({4, 7}, 0);
        std::vector<state::Tile>    tile = {engine.getState().getPlayers()[0].getTilesInHand()[5]};
        engine::PlayDrawTiles       action10(tile, 0);
        engine::PlayLeader          action11(engine.getState().getPlayers()[1].getLeadersInHand()[PRIEST], {1, 5}, 1);
        engine::PlayTile            action12(hand2[3], {4, 8}, 1);

        // Monument test
        engine::PlayTile            action13(state::Tile(TEMPLE, {-1, -1}), {0, 0}, 0);
        engine::PlayTile            action14(state::Tile(TEMPLE, {-1, -1}), {0, 1}, 0);
        engine::PlayTile            action15(state::Tile(TEMPLE, {-1, -1}), {1, 0}, 1);
        engine::PlayBuildMonument   action16(true, engine.getState().getBoard().getMonuments()[1], {0, 0}, 1);

        // Revolt test
        engine::PlayLeader          action17(engine.getState().getPlayers()[0].getLeadersInHand()[PRIEST], {6, 9}, 0);
        engine::PlayTile            action18(hand1[0], {5, 8}, 0);
        engine::PlayTile            action19(hand2[0], {5, 7}, 1);
        engine::PlayLeader          action20(engine.getState().getPlayers()[1].getLeadersInHand()[PRIEST], {6, 7}, 1);
        engine::PlayAttack          action21(REVOLT, {6, 7}, 0, 1, "");
        engine::PlayDefense         action22(REVOLT, {6, 7}, action21.getSupporters(), 1, 0, "");
        engine::PlayTile            action23(hand1[1], {7, 8}, 0);

        // War test
        engine::PlayAttack          action24(WAR, {4, 8}, 0, 1, PRIEST);
        engine::PlayDefense         action25(WAR, {4, 8}, action24.getSupporters(), 0, 0, PRIEST);

        // Move test
        engine::PlayMoveLeader      action26(TRADER, {6, 7}, 1);

        //std::vector<engine::Action*> actions = {&action1, &action2, &action3, &action4, &action5, &action6, &action7, &action8, &action9, &action10, &action11, &action12};
        //std::vector<engine::Action*> actions = {&action13, &action14, &action15, &action16};
        //std::vector<engine::Action*> actions = {&action17, &action18, &action19, &action20, &action21, &action22, &action23};
        //std::vector<engine::Action*> actions = {&action1, &action2, &action3, &action4, &action5, &action6, &action7, &action8, &action9, &action10, &action11, &action12, &action24, &action25};
        std::vector<engine::Action*> actions = {&action1, &action2, &action3, &action4, &action5, &action6, &action7, &action8, &action9, &action10, &action11, &action26};

        int actions_size = actions.size();
        
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
                this->drawState(engine.getState(), window);
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

}