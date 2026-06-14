#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include <SFML/Graphics.hpp>

struct PhysicsBody {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    
    float mass = 1.0f;
    float invMass = 1.0f;
    float inertia = 1000.0f; // Moment of inertia
    float invInertia = 0.001f;
    
    float restitution = 0.3f; // Bounciness (0 = no bounce, 1 = perfect bounce)
    float friction = 0.8f;
    float gravityScale = 1.0f;
    
    float rotation = 0.0f; // radians
    float angularVelocity = 0.0f;
    float angularAcceleration = 0.0f;
    
    bool isStatic = false;
    bool isOnGround = false;
    
    void* userData = nullptr; // For linking back to Entity

    void setMass(float m) {
        mass = m;
        if (mass > 0.0f && !isStatic) {
            invMass = 1.0f / mass;
        } else {
            mass = 0.0f;
            invMass = 0.0f;
        }
    }

    void setInertia(float i) {
        inertia = i;
        if (inertia > 0.0f && !isStatic) {
            invInertia = 1.0f / inertia;
        } else {
            inertia = 0.0f;
            invInertia = 0.0f;
        }
    }

    void setStatic(bool staticFlag) {
        isStatic = staticFlag;
        if (isStatic) {
            invMass = 0.0f;
            invInertia = 0.0f;
        } else {
            setMass(mass);
            setInertia(inertia);
        }
    }

    void applyForce(sf::Vector2f force, sf::Vector2f contactPoint = {0,0}) {
        if (!isStatic && mass > 0.0f) {
            acceleration += force * invMass;
            // If contactPoint is not 0,0, apply torque (simplified)
            if (contactPoint.x != 0 || contactPoint.y != 0) {
                sf::Vector2f r = contactPoint - position;
                float torque = r.x * force.y - r.y * force.x;
                angularAcceleration += torque * invInertia;
            }
        }
    }
    
    // integrate(dt) removed, handled by PhysicsWorld
};

#endif // PHYSICSBODY_H
