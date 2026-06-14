#include "BasicBird.h"

BasicBird::BasicBird(sf::Vector2f pos) : Bird(pos, 10.0f) { // ~10px radius
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("bird_basic"));
    
    // Set origin to center for rotation and physics
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setScale(0.2f, 0.2f);
    
    // Adjust radius based on scale
    radius = (bounds.width / 2.0f) * 0.2f;
}

void BasicBird::onAbility() {
    // Basic bird does nothing on tap
    if (!abilityUsed) {
        abilityUsed = true;
    }
}
