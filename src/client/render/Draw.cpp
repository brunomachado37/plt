#include "Draw.h"
#include "../shared/constants.h"
#include "../resourse.h"


namespace render {

    Draw::Draw() {

        this->tilesImage.loadFromFile(TILES_IMG);
        this->leadersImage.loadFromFile(LEADERS_IMG);
        this->pointsImage.loadFromFile(POINTS_IMG);

        this->font.loadFromFile(FONT);

        this->imageTilesMap = {{TEMPLE, TILE_SIZE * 4}, {MARKET, TILE_SIZE * 3}, {FARM, TILE_SIZE * 6}, {SETTLEMENT, TILE_SIZE}, {CATASTRO, TILE_SIZE * 2}, {MONUMENT, 0}, {UNION, TILE_SIZE * 5}};
        this->imageLeadersMap = {{KING, 0}, {FARMER, LEADER_SIZE}, {TRADER, LEADER_SIZE * 2}, {PRIEST, LEADER_SIZE * 3}};
        this->imagePointsMap = {{TREASURE, 0}, {BLUE, POINT_SIZE_W}, {RED, POINT_SIZE_W * 2}, {BLACK, POINT_SIZE_W * 3}, {GREEN, POINT_SIZE_W * 4}};
     
    }

    void Draw::drawTile(int x, int y, std::string type, sf::RenderWindow& window) {

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->tilesImage);
        sprite.setTextureRect(sf::IntRect(this->imageTilesMap[type], 0, TILE_SIZE, TILE_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

    void Draw::drawLeader(int x, int y, std::string type, int playerID, sf::RenderWindow& window) {

        // Create a sprite in the x, y position with the respect "type" Texture
        sf::Sprite sprite;
        sprite.setTexture(this->leadersImage);
        sprite.setTextureRect(sf::IntRect(playerID * LEADER_SIZE, this->imageLeadersMap[type], LEADER_SIZE, LEADER_SIZE));
        sprite.setPosition(x, y);

        // Draw tile sprite into the window
        window.draw(sprite);

    }

}