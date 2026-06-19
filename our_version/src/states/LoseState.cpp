#include "LoseState.h"
#include "MainMenuState.h"

LoseState::LoseState(StateManager& sm, sf::RenderWindow& win)
    : stateManager(sm), window(win) {}

void LoseState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_lose"));
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);
}

void LoseState::onExit() {}

void LoseState::handleEvent(const sf::Event& e) {
    if ((e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) ||
        (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)) {
        stateManager.changeState(std::make_unique<MainMenuState>(stateManager, window));
    }
}

void LoseState::update(float dt) {}

void LoseState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
}
