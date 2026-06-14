#ifndef ISTATE_H
#define ISTATE_H

#include <SFML/Graphics.hpp>

class IState {
public:
    virtual ~IState() = default;
    
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

#endif // ISTATE_H
