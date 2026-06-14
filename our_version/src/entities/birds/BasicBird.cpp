#include "BasicBird.h"

BasicBird::BasicBird(sf::Vector2f pos) : Bird(pos, 10.0f) { // ~10px radius
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("bird_basic"));
    
    // Set origin to center for rotation and physics
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Auto scale to radius 20.0f
    float targetRadius = 20.0f;
    sprite.setScale(targetRadius * 2.0f / bounds.width, targetRadius * 2.0f / bounds.height);
    radius = targetRadius;
}

void BasicBird::onAbility() {
    // Basic bird does nothing on tap
    if (!abilityUsed) {
        abilityUsed = true;
    }
}
