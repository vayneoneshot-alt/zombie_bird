#include "Block.h"

Block::Block(sf::Vector2f pos, sf::Vector2f size, Material mat) 
    : material(mat), halfSize(size / 2.0f) {
    
    body.position = pos;
    body.mass = 2.0f;
    body.restitution = 0.1f;
    body.friction = 0.9f;
    body.isStatic = false;
    
    maxHealth = 200.0f; // Threshold for breaking
    health = maxHealth;
    
    updateTexture();
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Scale sprite to match desired physical size
    sprite.setScale(size.x / bounds.width, size.y / bounds.height);
    sprite.setPosition(body.position);
}

void Block::updateTexture() {
    auto& rm = ResourceManager::getInstance();
    if (material == Material::WOOD) {
        if (crackStage == 0) {
            sprite.setTexture(rm.getTexture("block_wood_intact"));
        } else {
            sprite.setTexture(rm.getTexture("block_wood_cracked"));
        }
    }
}

void Block::update(float dt) {
    if (is_active && !isDestroyed()) {
        body.applyForce(sf::Vector2f(0.0f, 980.0f * body.mass)); // Gravity
        body.integrate(dt);
        sprite.setPosition(body.position);
    }
}

void Block::draw(sf::RenderWindow& window) {
    if (is_active && !isDestroyed()) {
        window.draw(sprite);
    }
}

void Block::receiveDamage(float impactForce) {
    // Only take damage if impact is strong enough
    const float damageThreshold = 150.0f; 
    
    if (impactForce > damageThreshold) {
        health -= (impactForce - damageThreshold);
        
        if (health <= maxHealth * 0.5f && crackStage == 0) {
            crackStage = 1;
            updateTexture();
        }
        
        if (health <= 0.0f) {
            is_active = false;
        }
    }
}
