#include "GameDraw.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"

namespace render {

    GameDraw::GameDraw() : Draw() {

        this->boardImage.loadFromFile(BOARD_IMG);
        this->monumentsImage.loadFromFile(MONUM_IMG);

        this->font.loadFromFile(FONT);

        this->imageMonumentsMap = {{std::string(GREEN) + std::string(RED), 0}, {std::string(RED) + std::string(BLUE), MONUMENT_SIZE}, {std::string(BLUE) + std::string(GREEN), MONUMENT_SIZE * 2}, {std::string(BLACK) + std::string(GREEN), MONUMENT_SIZE * 3}, {std::string(BLACK) + std::string(BLUE), MONUMENT_SIZE * 4}, {std::string(BLACK) + std::string(RED), MONUMENT_SIZE * 5}};

    }

    void GameDraw::drawBoardImage(sf::RenderWindow& window) {

        // Draw board image (erasing anything previously draw)
        sf::Sprite Sprite;
        Sprite.setTexture(this->boardImage);

        window.draw(Sprite);

    }

    void GameDraw::drawBoardState(state::Board board, sf::RenderWindow& window) {

        // Draw elements in every region
        for(auto region: board.getRegions()) {
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
        for(auto catastrophe: board.getCatastrophes()) {
            this->drawCatastrophe(catastrophe, window);
        }

        // Draw monuments outside of the board
        for(auto monument: board.getMonuments()) {
            this->drawMonumentOutside(monument, window);
        }

    }

    void GameDraw::drawGameInfo(state::State state, sf::RenderWindow& window) {

        // Count remaining tiles
        int remainingTiles = 0;

        for(auto type: state.getRemainingTiles()) {
            remainingTiles += type.second;
        }

        // Transform remaining tiles into a percentage multiple of 5
        remainingTiles = (((int)(((float)remainingTiles / (float)state::TOTAL_NUMBER_OF_TILES) * 100) / 5) * 5);

        // Create game info string
        std::string infoString = "Turn: " + std::to_string(state.getTurn()) + "       Active  Player: " + std::to_string(state.getActivePlayerID() + 1) + "      Actions  Done: " + std::to_string(state.getActionsDone()) + "\nTreasures Remaining: " + std::to_string(state.getRemainingTreasures()) + "       Tiles Remaining: " + std::to_string(remainingTiles) + " %";
       
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

    void GameDraw::drawGameOver(std::vector<int> finalScore, sf::RenderWindow& window) {

        // Create game over string
        std::string winnerString;
        std::string pointsString = FINAL_SCORE_7_MSG;
        int winner = 0;
        int winnerPoints = -1;

        for(int i = 0; i < (int)finalScore.size(); i++) {
            pointsString += FINAL_SCORE_1_MSG + std::to_string(i + 1) + FINAL_SCORE_5_MSG + std::to_string(finalScore[i]) + FINAL_SCORE_6_MSG;

            if(finalScore[i] > winnerPoints) {
                winnerPoints = finalScore[i];
                winner = i;
            }

        }


        // Create text with the winner info
        sf::Text winnerInfo;
        winnerInfo.setFont(this->font);
        winnerInfo.setCharacterSize(70);
        winnerInfo.setFillColor(sf::Color::Black);
        winnerInfo.setOutlineColor(sf::Color::White);
        winnerInfo.setOutlineThickness(1.5);

        if(finalScore[0] == finalScore[1]) {
            winnerString = TIE_2_MSG;
            winnerInfo.setString(winnerString);
            winnerInfo.setPosition(670, 300);
        }
        else {
            winnerString = FINAL_SCORE_1_MSG + std::to_string(winner + 1) + FINAL_SCORE_4_MSG;
            winnerInfo.setString(winnerString);
            winnerInfo.setPosition(730, 300);
        }


        // Create text with points info
        sf::Text pointsInfo;
        pointsInfo.setFont(this->font);
        pointsInfo.setString(pointsString);
        pointsInfo.setPosition(725, 400);
        pointsInfo.setCharacterSize(50);
        pointsInfo.setFillColor(sf::Color::Black);
        pointsInfo.setOutlineColor(sf::Color::White);
        pointsInfo.setOutlineThickness(1.3);


        // Draw sprites into the window
        window.draw(winnerInfo);
        window.draw(pointsInfo);

    }

    void GameDraw::drawTile(state::Tile tile, sf::RenderWindow& window) {

        // Convert i, j positions into x, y coordinates in the board  
        int x = tile.getPosition().j * SQUARE_SIZE + X_OFFSET;
        int y = tile.getPosition().i * SQUARE_SIZE + Y_OFFSET;

        // Draw tile
        Draw::drawTile(x, y, tile.getType(), window);

    }

    void GameDraw::drawLeader(state::Leader leader, sf::RenderWindow& window) {

        // Convert i, j positions into x, y coordinates in the board  
        int x = leader.getPosition().j * SQUARE_SIZE + X_OFFSET;
        int y = leader.getPosition().i * SQUARE_SIZE + Y_OFFSET;

        // Draw leader
        Draw::drawLeader(x, y, leader.getType(), leader.getPlayerID(), window);

    }

    void GameDraw::drawTreasure(state::Treasure treasure, sf::RenderWindow& window) {

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

    void GameDraw::drawMonument(state::Monument monument, sf::RenderWindow& window) {

        // Check if monument is built
        if(monument.getPosition().i == -1 || monument.getPosition().j == -1) {
            throw state::StateException(MONUMENT_POSITION_ERROR_MSG);
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

    void GameDraw::drawMonumentOutside(state::Monument monument, sf::RenderWindow& window) {

        // Check if monument is not built
        if(monument.getPosition().i != -1 || monument.getPosition().j != -1) {
            throw state::StateException(MONUMENT_POSITION_ERROR_MSG);
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

    void GameDraw::drawCatastrophe(state::Position position, sf::RenderWindow& window) {

        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Draw catastrophe
        Draw::drawTile(x, y, CATASTRO, window);

    }

    void GameDraw::drawUnificationTile(state::Position position, sf::RenderWindow& window) {

        // Convert i, j positions into x, y coordinates in the board  
        int x = position.j * SQUARE_SIZE + X_OFFSET;
        int y = position.i * SQUARE_SIZE + Y_OFFSET;

        // Draw tile
        Draw::drawTile(x, y, UNION, window);

    }

}