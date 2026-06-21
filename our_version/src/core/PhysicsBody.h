#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include <SFML/Graphics.hpp>

// PhysicsBody stores all the physical properties of an object (like a Bird or Block) so the physics engine can move it around properly.
struct PhysicsBody {
    sf::Vector2f position; // Where the object is in the world.
    sf::Vector2f velocity; // How fast and which direction it is currently moving.
    sf::Vector2f acceleration; // How fast it is speeding up right now.
    
    float mass = 1.0f; // How heavy it is.
    float invMass = 1.0f; // 1 divided by mass. Used to make math faster. If 0, the object cannot be moved.
    float inertia = 1000.0f; // Moment of inertia. How hard it is to make it spin.
    float invInertia = 0.001f; // 1 divided by inertia.
    
    float restitution = 0.3f; // Bounciness (0 = no bounce, 1 = perfect bounce).
    float friction = 0.8f; // How slippery it is.
    float gravityScale = 1.0f; // Multiplier for gravity. 0 means it floats.
    
    float rotation = 0.0f; // The current angle (in radians).
    float angularVelocity = 0.0f; // How fast it is currently spinning.
    float angularAcceleration = 0.0f; // How fast it is starting to spin.
    
    bool isStatic = false; // If true, this object cannot move at all (like the ground).
    bool isOnGround = false; // True if it is currently touching the floor.
    
    void* userData = nullptr; // A pointer to link back to the Entity. Like a nametag to find the owner.

    // Safely sets the mass and automatically calculates the inverse mass. Protects against division-by-zero errors.
    void setMass(float m) {
        mass = m;
        if (mass > 0.0f && !isStatic) {
            invMass = 1.0f / mass;
        } else {
            mass = 0.0f;
            invMass = 0.0f;
        }
    }

    // Safely sets the inertia (spin resistance) and automatically calculates inverse inertia.
    void setInertia(float i) {
        inertia = i;
        if (inertia > 0.0f && !isStatic) {
            invInertia = 1.0f / inertia;
        } else {
            inertia = 0.0f;
            invInertia = 0.0f;
        }
    }

    // Makes the object immovable or movable. If static, it forces mass to 0 so the physics engine won't move it.
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

    // Pushes the object with a given force. If the push is off-center (contactPoint), it also makes the object spin (torque).
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
