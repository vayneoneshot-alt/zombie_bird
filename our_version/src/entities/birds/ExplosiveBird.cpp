#include "ExplosiveBird.h"

ExplosiveBird::ExplosiveBird(sf::Vector2f pos) : Bird(pos, 12.5f) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("bird_explosive"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Auto scale to radius 20.0f
    float targetRadius = 20.0f;
    sprite.setScale(targetRadius * 2.0f / bounds.width, targetRadius * 2.0f / bounds.height);
    radius = targetRadius;
    
    explosionEffect.setRadius(getExplosionRadius());
    explosionEffect.setOrigin(getExplosionRadius(), getExplosionRadius());
    explosionEffect.setFillColor(sf::Color(255, 100, 0, 150));
}

void ExplosiveBird::onAbility() {
    if (launched && !abilityUsed && !body.isStatic) {
        abilityUsed = true;
        exploded = true;
        explosionTimer = 0.5f; // Explosion visible for 0.5s
        
        // The actual explosion logic (applying force to nearby objects)
        // will be handled by GameplayState which has access to all entities
    }
}

void ExplosiveBird::update(float dt) {
    Bird::update(dt);
    
    if (exploded && explosionTimer > 0.0f) {
        explosionTimer -= dt;
        explosionEffect.setPosition(body.position);
        
        // Fade out
        float alpha = (explosionTimer / 0.5f) * 150.0f;
        explosionEffect.setFillColor(sf::Color(255, 100, 0, (sf::Uint8)alpha));
        
        if (explosionTimer <= 0.0f) {
            is_active = false; // Disappear after explosion
        }
    }
}

void ExplosiveBird::draw(sf::RenderWindow& window) {
    if (!exploded) {
        Bird::draw(window);
    } else if (explosionTimer > 0.0f) {
        window.draw(explosionEffect);
    }
}
