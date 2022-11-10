#include "Scene.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"

#include <iostream>

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
                    player.removeLeaderFromHand(KING);
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