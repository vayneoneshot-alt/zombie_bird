#include "MainMenuState.h"
#include "LevelSelectState.h"
#include <iostream>

MainMenuState::MainMenuState(StateManager& sm, sf::RenderWindow& window)
    : stateManager(sm), window(window) {}

void MainMenuState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_menu"));
    
    // Scale background to window size
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);

    titleText.setFont(rm.getFont("main_font"));
    titleText.setString("HAPPY BIRD");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(3);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width/2.0f, titleBounds.top + titleBounds.height/2.0f);
    titleText.setPosition(winSize.x / 2.0f, winSize.y / 4.0f);

    promptText.setFont(rm.getFont("main_font"));
    promptText.setString("Press ENTER to Start");
    promptText.setCharacterSize(40);
    promptText.setFillColor(sf::Color::Yellow);
    promptText.setOutlineColor(sf::Color::Black);
    promptText.setOutlineThickness(2);

    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(promptBounds.left + promptBounds.width/2.0f, promptBounds.top + promptBounds.height/2.0f);
    promptText.setPosition(winSize.x / 2.0f, winSize.y * 3.0f / 4.0f);
}

void MainMenuState::onExit() {
}

void MainMenuState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) {
        stateManager.changeState(std::make_unique<LevelSelectState>(stateManager, window));
    }
}

void MainMenuState::update(float dt) {
    // Pulse animation for prompt text
    static float time = 0;
    time += dt;
    float scale = 1.0f + 0.05f * sin(time * 5.0f);
    promptText.setScale(scale, scale);
}

void MainMenuState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    renderWindow.draw(titleText);
    renderWindow.draw(promptText);
}
