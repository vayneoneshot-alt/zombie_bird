#include "Bird.h"

Bird::Bird(sf::Vector2f pos, float r) : radius(r) {
    body.position = pos;
    body.mass = 1.0f;
    body.restitution = 0.4f;
    body.friction = 0.8f;
    body.isStatic = true; // Static until launched
    sprite.setPosition(body.position); // Sync sprite immediately
}

void Bird::launch(sf::Vector2f velocity) {
    if (!launched) {
        launched = true;
        body.isStatic = false;
        body.velocity = velocity;
    }
}

void Bird::update(float dt) {
    if (launched && !body.isStatic) {
        body.applyForce(sf::Vector2f(0.0f, 980.0f * body.mass)); // Gravity
        body.integrate(dt);
        
        lifetime += dt;
        if (lifetime > 5.0f) {
            setActive(false);
        }
    }
    
    // Update sprite position to match physics body
    sprite.setPosition(body.position);
}

void Bird::draw(sf::RenderWindow& window) {
    if (is_active) {
        window.draw(sprite);
    }
}
