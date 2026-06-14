#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "PhysicsBody.h"

struct CollisionManifold {
    bool isColliding = false;
    float penetration = 0.0f;
    sf::Vector2f normal;
    sf::Vector2f contactPoints[2];
    int contactCount = 0;
};

class CollisionSystem {
public:
    static CollisionManifold circleVsCircle(const PhysicsBody* a, float radiusA, 
                                            const PhysicsBody* b, float radiusB);
                                            
    static CollisionManifold circleVsOBB(const PhysicsBody* circle, float radius, 
                                         const PhysicsBody* box, sf::Vector2f boxHalfSize);
                                         
    static CollisionManifold obbVsOBB(const PhysicsBody* a, sf::Vector2f halfSizeA, 
                                      const PhysicsBody* b, sf::Vector2f halfSizeB);

    static CollisionManifold detectCollision(PhysicsBody* a, PhysicsBody* b);
};

#endif // COLLISIONSYSTEM_H
