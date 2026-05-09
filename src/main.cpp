#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace std;




int main(){


    cout << "Hello" << endl;
    sf::Window window(sf::VideoMode({800, 600}), "My WIndow", sf::Style::Default, sf::State::Fullscreen);

    while(window.isOpen()){

        while(const optional event = window.pollEvent()){

            if(event->is<sf::Event::Closed>()){
                window.close();
            }

        }

        window.display();
    }

    return 0;
}