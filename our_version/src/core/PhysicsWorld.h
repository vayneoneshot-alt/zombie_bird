#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "PhysicsBody.h"
#include <vector>
#include <map>
#include <memory>

struct CollisionEvent {
    PhysicsBody* bodyA;
    PhysicsBody* bodyB;
    float normalImpulse;
};

// Represents a single contact point constraint
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

class PhysicsWorld {
public:
    PhysicsWorld(sf::Vector2f gravity = {0.0f, 980.0f});
    
    void addBody(PhysicsBody* body);
    void removeBody(PhysicsBody* body);
    
    void setGravity(sf::Vector2f g) { gravity = g; }
    
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
