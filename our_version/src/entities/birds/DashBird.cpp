#include "DashBird.h"
#include <cmath>

DashBird::DashBird(sf::Vector2f pos) : Bird(pos, 10.0f) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("bird_dash"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Auto scale to radius 20.0f
    float targetRadius = 20.0f;
    sprite.setScale(targetRadius * 2.0f / bounds.width, targetRadius * 2.0f / bounds.height);
    radius = targetRadius;
}

void DashBird::onAbility() {
    if (launched && !abilityUsed && !body.isStatic) {
        abilityUsed = true;
        
        // Dash: Boost velocity in the current moving direction
        float speed = std::sqrt(body.velocity.x * body.velocity.x + body.velocity.y * body.velocity.y);
        if (speed > 0.0f) {
            sf::Vector2f dir = body.velocity / speed;
            body.velocity += dir * 800.0f; // Add a flat amount of speed
        }
    }
}
