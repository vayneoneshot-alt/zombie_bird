#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "PhysicsBody.h"
#include <SFML/Graphics.hpp>

struct CollisionManifold {
    bool isColliding = false;
    sf::Vector2f normal = {0.0f, 0.0f};
    float penetration = 0.0f;
    float impactForce = 0.0f;
};

class CollisionSystem {
public:
    // Broad phase / narrow phase combinations
    static CollisionManifold circleVsCircle(const PhysicsBody& a, float radiusA, 
                                            const PhysicsBody& b, float radiusB);
                                            
    static CollisionManifold circleVsAABB(const PhysicsBody& circle, float radius, 
                                          const PhysicsBody& box, sf::Vector2f boxHalfSize);
                                          
    static CollisionManifold aabbVsAABB(const PhysicsBody& a, sf::Vector2f halfSizeA, 
                                        const PhysicsBody& b, sf::Vector2f halfSizeB);

    // Resolve collision via impulses
    static void resolveCollision(PhysicsBody& a, PhysicsBody& b, CollisionManifold& manifold);
};

#endif // COLLISIONSYSTEM_H
