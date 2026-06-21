#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "PhysicsBody.h"

// CollisionManifold acts like an accident report. It stores if two things hit, how deep they overlapped, which way to push them apart, and exactly where they touched.
struct CollisionManifold {
    bool isColliding = false;
    float penetration = 0.0f;
    sf::Vector2f normal;
    sf::Vector2f contactPoints[2];
    int contactCount = 0;
};

// CollisionSystem is an independent helper class. It holds math formulas to check if different shapes overlap, without knowing about game logic.
class CollisionSystem {
public:
    // Checks if two circles overlap by comparing the distance between their centers to their combined radius.
    static CollisionManifold circleVsCircle(const PhysicsBody* a, float radiusA, 
                                            const PhysicsBody* b, float radiusB);
                                            
    // Checks if a circle hits a rotated rectangle (OBB). It temporarily rotates the world to make the box straight for easier calculation.
    static CollisionManifold circleVsOBB(const PhysicsBody* circle, float radius, 
                                         const PhysicsBody* box, sf::Vector2f boxHalfSize);
                                         
    // Checks if two rotated rectangles (OBBs) hit each other using the Separating Axis Theorem (checking 4 different viewing angles).
    static CollisionManifold obbVsOBB(const PhysicsBody* a, sf::Vector2f halfSizeA, 
                                      const PhysicsBody* b, sf::Vector2f halfSizeB);

    // The automatic sorting station. It looks at two objects and picks the right math formula (circle vs box, etc.) to check their collision.
    static CollisionManifold detectCollision(PhysicsBody* a, PhysicsBody* b);
};

#endif // COLLISIONSYSTEM_H
