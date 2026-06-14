#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include <SFML/Graphics.hpp>

struct PhysicsBody {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    
    float mass = 1.0f;
    float inertia = 1000.0f; // Moment of inertia
    float restitution = 0.3f; // Bounciness (0 = no bounce, 1 = perfect bounce)
    float friction = 0.8f;
    
    float rotation = 0.0f; // radians
    float angularVelocity = 0.0f;
    float angularAcceleration = 0.0f;
    
    bool isStatic = false;
    bool isOnGround = false;

    void applyForce(sf::Vector2f force, sf::Vector2f contactPoint = {0,0}) {
        if (!isStatic && mass > 0.0f) {
            acceleration += force / mass;
            // If contactPoint is not 0,0, apply torque (simplified)
            if (contactPoint.x != 0 || contactPoint.y != 0) {
                sf::Vector2f r = contactPoint - position;
                float torque = r.x * force.y - r.y * force.x;
                angularAcceleration += torque / inertia;
            }
        }
    }

    void integrate(float dt) {
        if (!isStatic) {
            velocity += acceleration * dt;
            position += velocity * dt;
            acceleration = {0.0f, 0.0f}; // Reset acceleration each frame
            
            angularVelocity += angularAcceleration * dt;
            rotation += angularVelocity * dt;
            angularAcceleration = 0.0f;
        }
    }
};

#endif // PHYSICSBODY_H
