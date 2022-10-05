#include "Scene.h"
#include "../shared/constants.h"
#include "../res.h"

#include <SFML/Graphics.hpp>
#include <iostream>

namespace render {

    Scene::Scene(sf::RenderWindow& window) {
        sf::Texture Image;
        Image.loadFromFile(BOARD_IMG);

        sf::Sprite Sprite;
        Sprite.setTexture(Image);
        Sprite.setScale(0.5, 0.5);

        window.draw(Sprite);

    }


    void Scene::display(sf::RenderWindow& window) {

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.display();
        }

    }


    void Scene::drawState(state::State state, sf::RenderWindow& window) {

        std::vector<std::vector<std::string>> boardStateMap = state.getBoardStateMap();

        for(int i = 0; i < (int)boardStateMap.size(); i++) {
            for(int j = 0; j < (int)boardStateMap[0].size(); j++) {
                if(boardStateMap[i][j] == TEMPLE || boardStateMap[i][j] == SPECIAL || boardStateMap[i][j] == MARKET || boardStateMap[i][j] == FARM || boardStateMap[i][j] == SETTLEMENT) {
                    this->drawTile({i, j}, boardStateMap[i][j]);
                }
            }
        }

    }


    void Scene::drawTile(state::Position position, std::string type) {
        std::cout << position.i << "    " << position.j << "    " << type << std::endl;
    }

}