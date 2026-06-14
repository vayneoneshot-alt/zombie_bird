#include "SplitBird.h"
#include <cmath>

SplitBird::SplitBird(sf::Vector2f pos) : Bird(pos, 10.0f) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("bird_split"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setScale(0.2f, 0.2f);
    
    radius = (bounds.width / 2.0f) * 0.2f;
}

void SplitBird::onAbility() {
    if (launched && !abilityUsed && !body.isStatic) {
        abilityUsed = true;
        split_triggered = true;
    }
}

std::vector<std::unique_ptr<BasicBird>> SplitBird::spawnChildren() {
    std::vector<std::unique_ptr<BasicBird>> children;
    
    if (split_triggered) {
        split_triggered = false; // Only spawn once
        
        float speed = std::sqrt(body.velocity.x * body.velocity.x + body.velocity.y * body.velocity.y);
        if (speed > 0.0f) {
            float angle = std::atan2(body.velocity.y, body.velocity.x);
            
            // Spawn 2 children at +/- 20 degrees (0.35 radians)
            for (float offset : {-0.35f, 0.35f}) {
                auto child = std::make_unique<BasicBird>(body.position);
                sf::Vector2f newVel(
                    std::cos(angle + offset) * speed,
                    std::sin(angle + offset) * speed
                );
                child->launch(newVel);
                children.push_back(std::move(child));
            }
        }
    }
    
    return children;
}
