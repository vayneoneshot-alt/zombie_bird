#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "IState.h"
#include <memory>
#include <vector>

class StateManager {
public:
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);

    void handleEvent(const sf::Event& e);
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    std::vector<std::unique_ptr<IState>> stack;
};

#endif // STATEMANAGER_H
