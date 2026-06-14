#ifndef LOSESTATE_H
#define LOSESTATE_H

#include "IState.h"
#include "StateManager.h"
#include "../core/ResourceManager.h"

class LoseState : public IState {
public:
    LoseState(StateManager& sm, sf::RenderWindow& window, int finalScore);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    StateManager& stateManager;
    sf::RenderWindow& window;
    int score;
    sf::Sprite background;
    sf::Text titleText;
    sf::Text promptText;
};

#endif // LOSESTATE_H
