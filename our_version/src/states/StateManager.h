#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "IState.h"
#include <memory>
#include <vector>

// Manages different screens in the game using a stack (like a pile of pages).
class StateManager {
public:
    // Adds a new screen on top (like opening a pause menu).
    void pushState(std::unique_ptr<IState> state);
    // Removes the top screen and goes back to the previous one.
    void popState();
    // Closes the current screen and completely replaces it with a new one.
    void changeState(std::unique_ptr<IState> state);

    // Passes mouse/keyboard events to the top screen.
    void handleEvent(const sf::Event& e);
    // Runs the logic for the top screen.
    void update(float dt);
    // Draws the top screen.
    void draw(sf::RenderWindow& window);

private:
    std::vector<std::unique_ptr<IState>> stack; // The pile of screens
};

#endif // STATEMANAGER_H
