#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "PhysicsBody.h"
#include <vector>
#include <map>
#include <memory>

// An event sent to the game when two objects hit each other, so the game knows to deal damage or play a sound.
struct CollisionEvent {
    PhysicsBody* bodyA;
    PhysicsBody* bodyB;
    float normalImpulse; // How hard they hit each other.
};

// Holds all the math details about exactly how two objects are touching, so the physics engine knows how to push them apart smoothly.
struct ContactConstraint {
    PhysicsBody* a;
    PhysicsBody* b;
    sf::Vector2f normal;
    sf::Vector2f contactPoint;
    float penetration;
    
    // Warm starting
    float normalImpulse = 0.0f;
    float tangentImpulse = 0.0f;
    
    // Pre-calculated values for the solver
    float massNormal = 0.0f;
    float massTangent = 0.0f;
    float bias = 0.0f;
    sf::Vector2f rA, rB;
};

// The main physics engine. It keeps track of all objects, applies gravity, moves them, and stops them from passing through each other.
class PhysicsWorld {
public:
    PhysicsWorld(sf::Vector2f gravity = {0.0f, 980.0f});
    
    // Adds a physical object to the world so it can fall and bounce.
    void addBody(PhysicsBody* body);
    // Removes an object from the world (e.g., when a pig is destroyed).
    void removeBody(PhysicsBody* body);
    
    // Changes how strong gravity is.
    void setGravity(sf::Vector2f g) { gravity = g; }
    
    // Moves the physics world forward in time by a small amount (dt). Returns a list of what crashed into what.
    std::vector<CollisionEvent> step(float dt, int iterations = 10);

private:
    std::vector<PhysicsBody*> bodies;
    sf::Vector2f gravity;
    
    // Keep track of contacts between frames for warm starting
    // We map a pair of bodies to a list of contact constraints.
    // For simplicity, we just rebuild constraints each frame but preserve impulses if contacts match closely.
    std::vector<ContactConstraint> currentContacts;
};

#endif // PHYSICSWORLD_H
