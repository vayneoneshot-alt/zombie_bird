#ifndef LEVELSELECTSTATE_H
#define LEVELSELECTSTATE_H

#include "IState.h"
#include "StateManager.h"
#include "../core/ResourceManager.h"

class LevelSelectState : public IState {
public:
    LevelSelectState(StateManager& sm, sf::RenderWindow& window);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    StateManager& stateManager;
    sf::RenderWindow& window;
    sf::Sprite background;
    
    struct LevelButton {
        sf::RectangleShape shape;
        sf::Text text;
        std::string levelFile;
    };
    
    std::vector<LevelButton> buttons;
};

#endif // LEVELSELECTSTATE_H
