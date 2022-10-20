#include "Scene.h"
#include "../shared/constants.h"
#include "../resourse.h"

#include <SFML/Graphics.hpp>
#include <iostream>

namespace render {

    Scene::Scene(sf::RenderWindow& window) {
        this->boardImage.loadFromFile(BOARD_IMG);
        this->tilesImage.loadFromFile(TILES_IMG);
        this->leadersImage.loadFromFile(LEADERS_IMG);
        this->monumentsImage.loadFromFile(MONUM_IMG);
        this->pointsImage.loadFromFile(POINTS_IMG);

        this->imageTilesMap = {{TEMPLE, {TILE_SIZE * 4, 0}}, {MARKET, {TILE_SIZE * 3, 0}}, {FARM, {TILE_SIZE * 6, 0}}, {SETTLEMENT, {TILE_SIZE, 0}}, {CATASTRO, {TILE_SIZE * 2, 0}}, {MONUM_BOARD, {0, 0}}, {UNION, {TILE_SIZE * 5, 0}}};
        this->imageLeadersMap = {{KING, 0}, {FARMER, LEADER_SIZE}, {TRADER, LEADER_SIZE * 2}, {PRIEST, LEADER_SIZE * 3}};

        sf::Sprite Sprite;
        Sprite.setTexture(this->boardImage);
        // Sprite.setScale(0.5, 0.5);

        window.draw(Sprite);

    }


    void Scene::display(sf::RenderWindow& window) {

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


    void Scene::drawState(state::State state, sf::RenderWindow& window) {

        state::Board board = state.getBoard();
        std::vector<std::vector<std::string>> boardStateMap = board.getBoardStateMap();
        std::unordered_map<int, state::Region> regions = board.getRegions();

        for(auto region: regions) {
            for(auto tile: region.second.getTiles()) {
                this->drawTile(tile, window);
            }
            for(auto leader: region.second.getLeaders()) {
                this->drawLeader(leader, window);
            }
        }

    }


    void Scene::drawTile(state::Position position, std::string type, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[type].i, this->imageTilesMap[type].j, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

    void Scene::drawTile(state::Tile tile, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = tile.getPosition().j * SQUARE_SIZE + X_OFFSET;
        int y = tile.getPosition().i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[tile.getType()].i, this->imageTilesMap[tile.getType()].j, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

    void Scene::drawLeader(state::Leader leader, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = leader.getPosition().j * SQUARE_SIZE + X_OFFSET;
        int y = leader.getPosition().i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->leadersImage);

        // Set rect position based on player ID and leader type
        sprite.setTextureRect(sf::IntRect(leader.getPlayerID() * LEADER_SIZE, this->imageLeadersMap[leader.getType()], LEADER_SIZE, LEADER_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

}