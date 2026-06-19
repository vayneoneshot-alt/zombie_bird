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

    // Create buttons
    for (int i = 0; i < 3; ++i) {
        LevelButton btn;
        
        btn.shape.setSize(sf::Vector2f(240.0f, 60.0f));
        btn.shape.setOrigin(120.0f, 30.0f);
        btn.shape.setPosition(winSize.x / 2.0f, winSize.y * 0.45f + 160.0f + i * 80.0f);
        
        sf::Color themeColor;
        if (i == 0) {
            themeColor = sf::Color(0, 150, 0); // Dark Green
        } else if (i == 1) {
            themeColor = sf::Color(180, 180, 0); // Dark Yellow
        } else {
            themeColor = sf::Color(180, 0, 0); // Dark Red
        }
        
        btn.shape.setFillColor(sf::Color(20, 20, 20, 230)); // Dark background like template
        btn.shape.setOutlineColor(themeColor);
        btn.shape.setOutlineThickness(3.0f);
        
        btn.text.setFont(rm.getFont("main_font"));
        btn.text.setString("Level " + std::to_string(i + 1));
        btn.text.setCharacterSize(30);
        btn.text.setFillColor(themeColor);
        
        sf::FloatRect textBounds = btn.text.getLocalBounds();
        btn.text.setOrigin(textBounds.left + textBounds.width/2.0f, textBounds.top + textBounds.height/2.0f);
        btn.text.setPosition(btn.shape.getPosition());
        
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
            if (btn.shape.getGlobalBounds().contains(mousePos)) {
                std::cout << "Loading " << btn.levelFile << "\n";
                stateManager.changeState(std::make_unique<GameplayState>(stateManager, window, btn.levelFile));
            }
        }
    }
}

void LevelSelectState::update(float dt) {
    // Highlight hovered button
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (int i = 0; i < buttons.size(); ++i) {
        auto& btn = buttons[i];
        sf::Color themeColor;
        if (i == 0) themeColor = sf::Color(0, 150, 0);
        else if (i == 1) themeColor = sf::Color(180, 180, 0);
        else themeColor = sf::Color(180, 0, 0);

        if (btn.shape.getGlobalBounds().contains(mousePos)) {
            btn.shape.setFillColor(sf::Color(50, 50, 50, 255)); // Lighter on hover
        } else {
            btn.shape.setFillColor(sf::Color(20, 20, 20, 230));
        }
    }
}

void LevelSelectState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    for (const auto& btn : buttons) {
        renderWindow.draw(btn.shape);
        renderWindow.draw(btn.text);
    }
}
