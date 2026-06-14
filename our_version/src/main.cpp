#include <SFML/Graphics.hpp>
#include "core/ResourceManager.h"
#include "states/StateManager.h"
#include "states/MainMenuState.h"
#include <iostream>

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Happy Bird", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Load resources
    std::cout << "Loading resources...\n";
    ResourceManager& rm = ResourceManager::getInstance();
    rm.preloadAll();

    // Initialize State Manager
    StateManager stateManager;
    stateManager.pushState(std::make_unique<MainMenuState>(stateManager, window));

    sf::Clock clock;

    // Game loop
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            stateManager.handleEvent(event);
        }

        stateManager.update(dt.asSeconds());

        window.clear();
        stateManager.draw(window);
        window.display();
    }

    return 0;
}
