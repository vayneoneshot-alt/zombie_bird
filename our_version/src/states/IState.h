#ifndef ISTATE_H
#define ISTATE_H

#include <SFML/Graphics.hpp>

// An interface (contract) that every game screen (Menu, Gameplay, Win) must follow.
class IState {
public:
    virtual ~IState() = default;
    
    // Called exactly once when this screen first opens.
    virtual void onEnter() = 0;
    // Called exactly once when this screen is closed.
    virtual void onExit() = 0;
    // Called when the user presses a key or clicks the mouse.
    virtual void handleEvent(const sf::Event& e) = 0;
    // Called every frame to update game logic and animations.
    virtual void update(float dt) = 0;
    // Called every frame to draw pictures on the screen.
    virtual void draw(sf::RenderWindow& window) = 0;
};

#endif // ISTATE_H
