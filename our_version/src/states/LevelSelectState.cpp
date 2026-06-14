#include "LevelSelectState.h"
#include "GameplayState.h"
#include <iostream>

LevelSelectState::LevelSelectState(StateManager& sm, sf::RenderWindow& window)
    : stateManager(sm), window(window) {}

void LevelSelectState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_menu"));
    
    // Scale background to window size
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);

    titleText.setFont(rm.getFont("main_font"));
    titleText.setString("SELECT LEVEL");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(3);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width/2.0f, titleBounds.top + titleBounds.height/2.0f);
    titleText.setPosition(winSize.x / 2.0f, winSize.y * 0.15f);

    // Create buttons
    for (int i = 0; i < 3; ++i) {
        LevelButton btn;
        btn.rect.setSize(sf::Vector2f(300, 100));
        btn.rect.setFillColor(sf::Color(100, 150, 200, 200));
        btn.rect.setOutlineColor(sf::Color::White);
        btn.rect.setOutlineThickness(2);
        
        btn.rect.setOrigin(150, 50);
        btn.rect.setPosition(winSize.x / 2.0f, winSize.y * 0.4f + i * 150.0f);
        
        btn.text.setFont(rm.getFont("main_font"));
        btn.text.setString("Level " + std::to_string(i + 1));
        btn.text.setCharacterSize(40);
        btn.text.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = btn.text.getLocalBounds();
        btn.text.setOrigin(textBounds.left + textBounds.width/2.0f, textBounds.top + textBounds.height/2.0f);
        btn.text.setPosition(btn.rect.getPosition());
        
        btn.levelFile = "Data/Levels/level" + std::to_string(i + 1) + ".json";
        
        buttons.push_back(btn);
    }
}

void LevelSelectState::onExit() {
}

void LevelSelectState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(e.mouseButton.x, e.mouseButton.y);
        for (const auto& btn : buttons) {
            if (btn.rect.getGlobalBounds().contains(mousePos)) {
                std::cout << "Loading " << btn.levelFile << "\n";
                stateManager.changeState(std::make_unique<GameplayState>(stateManager, window, btn.levelFile));
            }
        }
    }
}

void LevelSelectState::update(float dt) {
    // Highlight hovered button
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto& btn : buttons) {
        if (btn.rect.getGlobalBounds().contains(mousePos)) {
            btn.rect.setFillColor(sf::Color(150, 200, 250, 255));
        } else {
            btn.rect.setFillColor(sf::Color(100, 150, 200, 200));
        }
    }
}

void LevelSelectState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    renderWindow.draw(titleText);
    for (const auto& btn : buttons) {
        renderWindow.draw(btn.rect);
        renderWindow.draw(btn.text);
    }
}
