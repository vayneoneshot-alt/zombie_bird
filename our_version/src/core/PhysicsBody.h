#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include <SFML/Graphics.hpp>

struct PhysicsBody {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    
    float mass = 1.0f;
    float restitution = 0.3f; // Bounciness (0 = no bounce, 1 = perfect bounce)
    float friction = 0.8f;
    
    bool isStatic = false;
    bool isOnGround = false;

    void applyForce(sf::Vector2f force) {
        if (!isStatic && mass > 0.0f) {
            acceleration += force / mass;
        }
    }

    void integrate(float dt) {
        if (!isStatic) {
            velocity += acceleration * dt;
            position += velocity * dt;
            acceleration = {0.0f, 0.0f}; // Reset acceleration each frame
        }
    }
};

#endif // PHYSICSBODY_H
