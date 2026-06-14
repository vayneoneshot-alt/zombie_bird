#include "StateManager.h"

void StateManager::pushState(std::unique_ptr<IState> state) {
    stack.push_back(std::move(state));
    stack.back()->onEnter();
}

void StateManager::popState() {
    if (!stack.empty()) {
        stack.back()->onExit();
        stack.pop_back();
    }
}

void StateManager::changeState(std::unique_ptr<IState> state) {
    popState();
    pushState(std::move(state));
}

void StateManager::handleEvent(const sf::Event& e) {
    if (!stack.empty()) {
        stack.back()->handleEvent(e);
    }
}

void StateManager::update(float dt) {
    if (!stack.empty()) {
        stack.back()->update(dt);
    }
}

void StateManager::draw(sf::RenderWindow& window) {
    if (!stack.empty()) {
        stack.back()->draw(window);
    }
}
