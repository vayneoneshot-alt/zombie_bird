#include "MainMenuState.h"
#include "LevelSelectState.h"
#include <iostream>
#include <cmath>

MainMenuState::MainMenuState(StateManager& sm, sf::RenderWindow& window)
    : stateManager(sm), window(window) {}

void MainMenuState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_menu"));
    
    // Scale background to window size
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);
}

void MainMenuState::onExit() {
}

void MainMenuState::handleEvent(const sf::Event& e) {
    if ((e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) ||
        (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)) {
        stateManager.changeState(std::make_unique<LevelSelectState>(stateManager, window));
    }
}

void MainMenuState::update(float dt) {
    // No update logic needed
}

void MainMenuState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
}
