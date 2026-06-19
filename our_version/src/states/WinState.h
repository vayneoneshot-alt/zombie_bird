#ifndef WINSTATE_H
#define WINSTATE_H

#include "IState.h"
#include "StateManager.h"
#include "../core/ResourceManager.h"

class WinState : public IState {
public:
    WinState(StateManager& sm, sf::RenderWindow& window);

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

#endif // WINSTATE_H
