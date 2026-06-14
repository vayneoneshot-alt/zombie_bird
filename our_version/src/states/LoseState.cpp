#include "LoseState.h"
#include "MainMenuState.h"

LoseState::LoseState(StateManager& sm, sf::RenderWindow& win, int finalScore)
    : stateManager(sm), window(win), score(finalScore) {}

void LoseState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    background.setTexture(rm.getTexture("bg_menu"));
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);

    titleText.setFont(rm.getFont("main_font"));
    titleText.setString("OUT OF BIRDS!\nScore: " + std::to_string(score));
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Red);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(3);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width/2.0f, titleBounds.height/2.0f);
    titleText.setPosition(winSize.x / 2.0f, winSize.y * 0.4f);

    promptText.setFont(rm.getFont("main_font"));
    promptText.setString("Press ENTER for Menu");
    promptText.setCharacterSize(40);
    promptText.setFillColor(sf::Color::White);
    
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(promptBounds.width/2.0f, promptBounds.height/2.0f);
    promptText.setPosition(winSize.x / 2.0f, winSize.y * 0.7f);
}

void LoseState::onExit() {}

void LoseState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) {
        stateManager.changeState(std::make_unique<MainMenuState>(stateManager, window));
    }
}

void LoseState::update(float dt) {}

void LoseState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    renderWindow.draw(titleText);
    renderWindow.draw(promptText);
}
