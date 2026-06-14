#include "Pig.h"

Pig::Pig(sf::Vector2f pos) {
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("pig"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setScale(0.45f, 0.45f);
    
    radius = (bounds.width / 2.0f) * 0.45f;
    
    body.position = pos;
    body.mass = 1.5f;
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
