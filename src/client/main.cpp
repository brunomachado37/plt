#include <iostream>
#include <cstdlib>

// Les lignes suivantes ne servent qu'à vérifier que la compilation avec SFML fonctionne
#include <SFML/Graphics.hpp>

void testSFML() {
    sf::Texture texture;
}

// Fin test SFML

#include <state.h>

using namespace std;
using namespace state;

int main(int argc, char* argv[]){

    if(string(argv[1]) == "hello"){
        //Exemple exemple;
        //exemple.setX(53);
        //testSFML();

        cout << "Tigris & Euphrates: An unofficial implementation" << endl;

        return EXIT_SUCCESS;
    }
    else if(string(argv[1]) == "state"){
        State testState;
        
        return EXIT_SUCCESS;
    }
    else if(string(argv[1]) == "render"){
        sf::RenderWindow window(sf::VideoMode(1900, 1023), "Tigris & Euphrates");

        sf::Texture Image;
        if(!Image.loadFromFile("res/boardTable.png")) {
            cerr << "Error reading the board image" << endl;
            return EXIT_FAILURE;
        }

        sf::Sprite Sprite;
        Sprite.setTexture(Image);
        Sprite.setScale(0.5, 0.5);

        window.draw(Sprite);

        while (window.isOpen()) {

            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.display();

        }

        return EXIT_SUCCESS;

    }
    else if(string(argv[1]) == "help"){
        cout << "Tigris & Euphrates: An unofficial implementation" << endl << endl;
        cout << "Available arguments:" << endl;
        cout << "hello      Print an introduction message" << endl;

        return EXIT_SUCCESS;
    }
    else{
        cout << "Unknown argument: " << argv[1] << endl;
        cout << "Use the 'help' argument to check availble options." << endl;

        return EXIT_SUCCESS;
    }

}