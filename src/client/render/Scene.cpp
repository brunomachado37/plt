#include "Scene.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"

#include <iostream>

namespace render {

    Scene::Scene(int playerID) {

        this->playerID = playerID;

        this->boardImage.loadFromFile(BOARD_IMG);
        this->tilesImage.loadFromFile(TILES_IMG);
        this->leadersImage.loadFromFile(LEADERS_IMG);
        this->monumentsImage.loadFromFile(MONUM_IMG);
        this->pointsImage.loadFromFile(POINTS_IMG);

        this->font.loadFromFile(FONT);

        this->imageTilesMap = {{TEMPLE, TILE_SIZE * 4}, {MARKET, TILE_SIZE * 3}, {FARM, TILE_SIZE * 6}, {SETTLEMENT, TILE_SIZE}, {CATASTRO, TILE_SIZE * 2}, {MONUMENT, 0}, {UNION, TILE_SIZE * 5}};
        this->imageLeadersMap = {{KING, 0}, {FARMER, LEADER_SIZE}, {TRADER, LEADER_SIZE * 2}, {PRIEST, LEADER_SIZE * 3}};
        this->imagePointsMap = {{TREASURE, 0}, {BLUE, POINT_SIZE_W}, {RED, POINT_SIZE_W * 2}, {BLACK, POINT_SIZE_W * 3}, {GREEN, POINT_SIZE_W * 4}};
        this->imageMonumentsMap = {{std::string(GREEN) + std::string(RED), 0}, {std::string(RED) + std::string(BLUE), MONUMENT_SIZE}, {std::string(BLUE) + std::string(GREEN), MONUMENT_SIZE * 2}, {std::string(BLACK) + std::string(GREEN), MONUMENT_SIZE * 3}, {std::string(BLACK) + std::string(BLUE), MONUMENT_SIZE * 4}, {std::string(BLACK) + std::string(RED), MONUMENT_SIZE * 5}};

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

        // Draw board image (erasing anything previously draw)
        sf::Sprite Sprite;
        Sprite.setTexture(this->boardImage);
        window.draw(Sprite);

        // Draw elements in every region
        for(auto region: state.getBoard().getRegions()) {
            for(auto tile: region.second.getTiles()) {
                this->drawTile(tile, window);
            }
            for(auto leader: region.second.getLeaders()) {
                this->drawLeader(leader, window);
            }
            for(auto monument: region.second.getMonuments()) {
                this->drawMonument(monument, window);
            }
            for(auto treasure: region.second.getTreasures()) {
                this->drawTreasure(treasure, window);
            }
            if(region.second.getIsAtWar()) {
                this->drawUnificationTile(region.second.getUnificationTilePosition(), window);
            }
        }

        // Draw catastrophes
        for(auto catastrophe: state.getBoard().getCatastrophes()) {
            this->drawCatastrophe(catastrophe, window);
        }

        // Draw monuments outside of the board
        for(auto monument: state.getBoard().getMonuments()) {
            this->drawMonumentOutside(monument, window);
        }

        // Draw player info
        this->drawTilesInHand(state.getPlayers()[this->playerID].getTilesInHand(), window);
        this->drawLeadersInHand(state.getPlayers()[this->playerID].getLeadersInHand(), window);
        this->drawVictoryPoints(state.getPlayers()[this->playerID].getVictoryPoints(), window);
        this->drawCatastropheInHand(state.getPlayers()[this->playerID].getCatastropheTiles(), window);

        // Draw game info
        this->drawGameInfo(state, window);

    }


    void Scene::drawTile(state::Position position, std::string type, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[type], 0, TILE_SIZE, TILE_SIZE));
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
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[tile.getType()], 0, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

    void Scene::drawTilesInHand(std::vector<state::Tile> tiles, sf::RenderWindow& window) {

        for(int i = 0; i < (int)tiles.size(); i++) {
            // Sanity check
            if(tiles[i].getPosition().i != -1 || tiles[i].getPosition().j != -1) {
                throw std::logic_error(TILE_POSITION_ERROR_MSG);
            }

            // Calculate x, y coordinates outside of the board
            int x, y;

            if(i < 3) {
                x = TILE_OUTSIDE_X + (TILE_SIZE + TILE_ESPACE_X) * i;
                y = TILE_OUTSIDE_Y;
            }
            else {
                x = TILE_OUTSIDE_X + (TILE_SIZE + TILE_ESPACE_X) * (i - 3);
                y = TILE_OUTSIDE_Y + TILE_SIZE + TILE_ESPACE_Y;
            }

            // Create a sprite in the x, y position with the respect "type" Texture
            sf::Sprite sprite;
            sprite.setTexture(this->tilesImage);
            sprite.setTextureRect(sf::IntRect(this->imageTilesMap[tiles[i].getType()], 0, TILE_SIZE, TILE_SIZE));
            sprite.setPosition(x, y);

            // Draw tile sprite into the window
            window.draw(sprite);

        }

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

        // Draw leader sprite into the window
        window.draw(sprite);

    }

    void Scene::drawLeadersInHand (std::unordered_map<std::string,state::Leader> leaders, sf::RenderWindow& window) {

        int i = 0;
        
        for(auto leader: leaders) {
            // Sanity check
            if(leader.second.getPosition().i != -1 || leader.second.getPosition().j != -1) {
                throw std::logic_error(LEADER_POSITION_ERROR_MSG);
            }

            // Calculate x, y coordinates outside of the board
            int x = LEADER_OUTSIDE_X + (LEADER_SIZE + LEADER_ESPACE_X) * i;
            int y = LEADER_OUTSIDE_Y;

            // Create a sprite in the x, y position with the respect "type" Texture
            sf::Sprite sprite;
            sprite.setTexture(this->leadersImage);
            sprite.setTextureRect(sf::IntRect(leader.second.getPlayerID() * LEADER_SIZE, this->imageLeadersMap[leader.second.getType()], LEADER_SIZE, LEADER_SIZE));
            sprite.setPosition(x, y);

            // Draw tile sprite into the window
            window.draw(sprite);

            i++;

        }

    }

    void Scene::drawTreasure(state::Treasure treasure, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = treasure.getPosition().j * SQUARE_SIZE + X_OFFSET + TREASURE_OFFSET;
        int y = treasure.getPosition().i * SQUARE_SIZE + Y_OFFSET + TREASURE_OFFSET;

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->pointsImage);
        sprite.setTextureRect(sf::IntRect(TREASURE_SQUARE, this->imagePointsMap[TREASURE], POINT_SIZE_H, POINT_SIZE_H));
        sprite.setScale(0.7, 0.7);
        sprite.setPosition(x, y);

        // Draw treasure sprite into the window
        window.draw(sprite);

    }

    void Scene::drawMonument(state::Monument monument, sf::RenderWindow& window) {

        // Check if monument is built
        if(monument.getPosition().i == -1 || monument.getPosition().j == -1) {
            throw std::logic_error(MONUMENT_POSITION_ERROR_MSG);
        }

        // Convert i, j positions into x, y coordinates in the board  
        int x = monument.getPosition().j * SQUARE_SIZE + X_OFFSET + MONUMENT_OFFSET_X;
        int y = monument.getPosition().i * SQUARE_SIZE + Y_OFFSET + MONUMENT_OFFSET_Y;

        // Create monument border tiles sprites and draw them
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 2; j++) {
                // Set tile position
                int x = (monument.getPosition().j + j) * SQUARE_SIZE + X_OFFSET;
                int y = (monument.getPosition().i + i) * SQUARE_SIZE + Y_OFFSET;

                // Create tile sprite
                sf::Sprite sprite;
                sprite.setTexture(this->tilesImage);
                sprite.setTextureRect(sf::IntRect(this->imageTilesMap[MONUMENT], 0, TILE_SIZE, TILE_SIZE));
                sprite.setPosition(x, y);

                // Set sprite rotation
                if(i == 0 && j == 0) {
                    sprite.move({(float)TILE_SIZE, (float)TILE_SIZE});
                    sprite.rotate(180.f);
                } 
                else if(i == 0 && j == 1) {
                    sprite.move({0, (float)TILE_SIZE});
                    sprite.rotate(270.f);
                }
                else if(i == 1 && j == 0) {
                    sprite.move({(float)TILE_SIZE, 0});
                    sprite.rotate(90.f);
                }

                window.draw(sprite);
            }
        }

        // Create monument sprite
        sf::Sprite sprite;
        sprite.setTexture(this->monumentsImage);
        sprite.setTextureRect(sf::IntRect(this->imageMonumentsMap[monument.getColor1() + monument.getColor2()], 0, MONUMENT_SIZE, MONUMENT_SIZE));
        sprite.setPosition(x, y);

        // Draw monument sprite into the window
        window.draw(sprite);

    }

    void Scene::drawMonumentOutside(state::Monument monument, sf::RenderWindow& window) {
        // Check if monument is not built
        if(monument.getPosition().i != -1 || monument.getPosition().j != -1) {
            throw std::logic_error(MONUMENT_POSITION_ERROR_MSG);
        }

        // Get monument image info
        int imageX = this->imageMonumentsMap[monument.getColor1() + monument.getColor2()];
        int x, y;

        // Set position outside the board
        if(imageX < MONUMENT_SIZE * 3) {
            x = MONUMENT_OUTSIDE_X + imageX + (MONUMENT_ESPACE_X * imageX / MONUMENT_SIZE);
            y = MONUMENT_OUTSIDE_Y;
        }
        else {
            x = MONUMENT_OUTSIDE_X + imageX + (MONUMENT_ESPACE_X * (imageX - (MONUMENT_SIZE * 3)) / MONUMENT_SIZE) - (MONUMENT_SIZE * 3);
            y = MONUMENT_OUTSIDE_Y + MONUMENT_SIZE + MONUMENT_ESPACE_Y;
        }

        // Create a sprite in the "not built monuments" area
        sf::Sprite sprite;
        sprite.setTexture(this->monumentsImage);
        sprite.setTextureRect(sf::IntRect(imageX, 0, MONUMENT_SIZE, MONUMENT_SIZE));
        sprite.setPosition(x, y);

        // Draw monument sprite into the window
        window.draw(sprite);

    }

    void Scene::drawVictoryPoints(std::unordered_map<std::string, int> points, sf::RenderWindow& window) {

        std::vector<std::string> points_map = {BLUE, RED, BLACK, GREEN, TREASURE};

        for(int i = 0; i < (int)points_map.size(); i++) {

            // Calculate x, y coordinates outside of the board
            int x = POINTS_OUTSIDE_X + (POINT_SIZE_W + POINTS_ESPACE_X) * i;
            int y = POINTS_OUTSIDE_Y;

            // Create a sprite in the x, y position with the respective "type" Texture
            sf::Sprite sprite;
            sprite.setTexture(this->pointsImage);
            sprite.setTextureRect(sf::IntRect(this->imagePointsMap[points_map[i]], 0, POINT_SIZE_W, POINT_SIZE_H));
            sprite.setPosition(x, y);

            // Create text with the number of points
            sf::Text numberOfPoints;
            numberOfPoints.setFont(this->font);
            numberOfPoints.setString(std::to_string(points[points_map[i]]));
            numberOfPoints.setPosition(x + POINT_SIZE_W + POINTS_ESPACE_NUM, y - POINTS_ESPACE_NUM);
            numberOfPoints.setCharacterSize(37);
            numberOfPoints.setFillColor(sf::Color::Black);
            numberOfPoints.setOutlineColor(sf::Color::White);
            numberOfPoints.setOutlineThickness(1);

            // Draw sprites into the window
            window.draw(sprite);
            window.draw(numberOfPoints);

        }

    }

    void Scene::drawCatastrophe(state::Position position, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with catastrophe texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[CATASTRO], 0, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);
    }

    void Scene::drawCatastropheInHand(int remainingCatastrophes, sf::RenderWindow& window) {

        for(int i = 0; i < remainingCatastrophes; i++) {
            // Take x, y coordinates outside of the board
            int x = CATASTROPHE_OUTSIDE_X + i * (CATASTROPHE_ESPACE + TILE_SIZE);
            int y = CATASTROPHE_OUTSIDE_Y;

            // Create a sprite in the x, y position with the respective "type" Texture
            sf::Sprite sprite;
            sprite.setTexture(this->tilesImage);
            sprite.setTextureRect(sf::IntRect(this->imageTilesMap[CATASTRO], 0, TILE_SIZE, TILE_SIZE));
            sprite.setPosition(x, y);

            // Draw sprites into the window
            window.draw(sprite);
        }

    }

    void Scene::drawUnificationTile(state::Position position, sf::RenderWindow& window) {
        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Create a sprite in the x, y position with catastrophe texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[UNION], 0, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);
    }

    void Scene::drawGameInfo(state::State state, sf::RenderWindow& window) {

        // Count remaining tiles
        int remainingTiles = 0;

        for(auto type: state.getRemainingTiles()) {
            remainingTiles += type.second;
        }

        // Transform remaining tiles into a percentage multiple of 5
        remainingTiles = (((((float)remainingTiles / (float)TOTAL_NUMBER_OF_TILES) * 100) / 5) * 5);

        // Create game info string
        std::string infoString = "Turn: " + std::to_string(state.getTurn()) + "         Active  Player: " + std::to_string(state.getActivePlayerID()) + "        Actions  Done: " + std::to_string(state.getActionsDone()) + "\nTreasures Remaining: " + std::to_string(state.getRemainingTreasures()) + "       Tiles Remaining: " + std::to_string(remainingTiles) + " %";
       
        // Create text with the game info
        sf::Text gameInfo;
        gameInfo.setFont(this->font);
        gameInfo.setString(infoString);
        gameInfo.setPosition(GAME_INFO_BASE_X, GAME_INFO_BASE_Y);
        gameInfo.setCharacterSize(37);
        gameInfo.setFillColor(sf::Color::Black);
        gameInfo.setOutlineColor(sf::Color::White);
        gameInfo.setOutlineThickness(1);

        // Draw sprites into the window
        window.draw(gameInfo);

    }

}