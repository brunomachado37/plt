#include "PlayerDraw.h"
#include "../shared/constants.h"
#include "../shared/messages.h"
#include "../resourse.h"

namespace render {

    PlayerDraw::PlayerDraw() : Draw() {}

    void PlayerDraw::drawPlayerState(state::Player player, sf::RenderWindow& window) {
        
        this->drawTiles(player.getTilesInHand(), window);
        this->drawLeaders(player.getLeadersInHand(), window);
        this->drawVictoryPoints(player.getVictoryPoints(), window);
        this->drawCatastrophes(player.getCatastropheTiles(), window);
    
    }

    void PlayerDraw::drawTiles(std::vector<state::Tile> tiles, sf::RenderWindow& window) {

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

            // Draw tile
            Draw::drawTile(x, y, tiles[i].getType(), window);

        }

    }

    void PlayerDraw::drawLeaders(std::unordered_map<std::string,state::Leader> leaders, sf::RenderWindow& window) {

        int i = 0;
        
        for(auto leader: leaders) {
            // Sanity check
            if(leader.second.getPosition().i != -1 || leader.second.getPosition().j != -1) {
                throw std::logic_error(LEADER_POSITION_ERROR_MSG);
            }

            // Calculate x, y coordinates outside of the board
            int x = LEADER_OUTSIDE_X + (LEADER_SIZE + LEADER_ESPACE_X) * i;
            int y = LEADER_OUTSIDE_Y;

            // Draw leader
            Draw::drawLeader(x, y, leader.second.getType(), leader.second.getPlayerID(), window);

            i++;

        }

    }

    void PlayerDraw::drawCatastrophes(int remainingCatastrophes, sf::RenderWindow& window) {

        for(int i = 0; i < remainingCatastrophes; i++) {
            // Take x, y coordinates outside of the board
            int x = CATASTROPHE_OUTSIDE_X + i * (CATASTROPHE_ESPACE + TILE_SIZE);
            int y = CATASTROPHE_OUTSIDE_Y;

            Draw::drawTile(x, y, CATASTRO, window);
        
        }

    }

    void PlayerDraw::drawVictoryPoints(std::unordered_map<std::string, int> points, sf::RenderWindow& window) {

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

}