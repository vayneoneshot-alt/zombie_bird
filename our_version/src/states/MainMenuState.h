#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "IState.h"
#include "StateManager.h"
#include "../core/ResourceManager.h"

class MainMenuState : public IState {
public:
    MainMenuState(StateManager& sm, sf::RenderWindow& window);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    StateManager& stateManager;
    sf::RenderWindow& window;
    sf::Sprite background;
};

#endif // MAINMENUSTATE_H
