#include "Pig.h"

Pig::Pig(sf::Vector2f pos) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("pig"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Auto scale to radius 20.0f
    float targetRadius = 20.0f;
    sprite.setScale(targetRadius * 2.0f / bounds.width, targetRadius * 2.0f / bounds.height);
    radius = targetRadius;
    
    body.position = pos;
    body.mass = 1.5f;
    body.inertia = 0.5f * body.mass * radius * radius;
    body.restitution = 0.3f;
    body.friction = 0.8f;
    body.isStatic = false;
    
    sprite.setPosition(body.position);
}

void Pig::update(float dt) {
    if (is_active && !isDead()) {
        body.applyForce(sf::Vector2f(0.0f, 980.0f * body.mass)); // Gravity
        body.integrate(dt);
        sprite.setPosition(body.position);
        sprite.setRotation(body.rotation * 180.0f / 3.14159265f);
    }
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
