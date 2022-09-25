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
    else if(string(argv[1]) == "help"){
        cout << "Tigris & Euphrates: An unofficial implementation" << endl;
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