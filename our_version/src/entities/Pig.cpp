#include "Pig.h"

Pig::Pig(sf::Vector2f pos) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("pig"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Auto scale to radius 17.5f
    float targetRadius = 17.5f;
    sprite.setScale(targetRadius * 2.0f / bounds.width, targetRadius * 2.0f / bounds.height);
    radius = targetRadius;
    
    body.position = pos;
    body.setMass(1.5f);
    body.setInertia(0.5f * body.mass * radius * radius);
    body.restitution = 0.3f;
    body.friction = 0.8f;
    body.setStatic(false);
    
    sprite.setPosition(body.position);
}

void Pig::update(float dt) {
    if (isDead()) return;
    
    // Physics is handled by PhysicsWorld
    sprite.setPosition(body.position);
    sprite.setRotation(body.rotation * 180.0f / 3.14159f);
}

void Pig::draw(sf::RenderWindow& window) {
    if (is_active && !isDead()) {
        window.draw(sprite);
    }
}

void Pig::receiveDamage(float amount) {
    health -= amount;
    if (health <= 0.0f) {
        is_active = false;
    }
}
