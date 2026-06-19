#include "WinState.h"
#include "MainMenuState.h"

WinState::WinState(StateManager& sm, sf::RenderWindow& win)
    : stateManager(sm), window(win) {}

void WinState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_win"));
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);
}

void WinState::onExit() {}

void WinState::handleEvent(const sf::Event& e) {
    if ((e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) ||
        (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)) {
        stateManager.changeState(std::make_unique<MainMenuState>(stateManager, window));
    }
}

void WinState::update(float dt) {}

void WinState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
}
