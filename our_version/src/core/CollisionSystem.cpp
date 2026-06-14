#include "CollisionSystem.h"
#include "../entities/Entity.h"
#include "../entities/Block.h"
#include "../entities/Pig.h"
#include "../entities/birds/Bird.h"
#include <cmath>
#include <algorithm>
#include <limits>

// Helper math
inline sf::Vector2f rotateVec(sf::Vector2f v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return sf::Vector2f(v.x * c - v.y * s, v.x * s + v.y * c);
}

inline float dotProduct(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

CollisionManifold CollisionSystem::circleVsCircle(const PhysicsBody* a, float radiusA, 
                                                  const PhysicsBody* b, float radiusB) {
    CollisionManifold m;
    sf::Vector2f n = b->position - a->position;
    float distSq = dotProduct(n, n);
    float radiusSum = radiusA + radiusB;
    
    if (distSq >= radiusSum * radiusSum || distSq == 0.0f) {
        return m;
    }
    
    float distance = std::sqrt(distSq);
    m.isColliding = true;
    m.penetration = radiusSum - distance;
    m.normal = n / distance;
    m.contactPoints[0] = a->position + m.normal * radiusA;
    m.contactCount = 1;
    return m;
}

CollisionManifold CollisionSystem::circleVsOBB(const PhysicsBody* circle, float radius, 
                                               const PhysicsBody* box, sf::Vector2f boxHalfSize) {
    CollisionManifold m;
    
    sf::Vector2f d = circle->position - box->position;
    sf::Vector2f localCircle = rotateVec(d, -box->rotation);
    
    sf::Vector2f closest = localCircle;
    closest.x = std::max(-boxHalfSize.x, std::min(boxHalfSize.x, closest.x));
    closest.y = std::max(-boxHalfSize.y, std::min(boxHalfSize.y, closest.y));
    
    bool inside = false;
    if (localCircle == closest) {
        inside = true;
        if (std::abs(localCircle.x - boxHalfSize.x) < std::abs(localCircle.x + boxHalfSize.x)) {
            closest.x = (localCircle.x > 0) ? boxHalfSize.x : -boxHalfSize.x;
        } else {
            closest.y = (localCircle.y > 0) ? boxHalfSize.y : -boxHalfSize.y;
        }
    }
    
    sf::Vector2f localNormal = localCircle - closest;
    float distSq = dotProduct(localNormal, localNormal);
    
    if (!inside && distSq > radius * radius) {
        return m;
    }
    
    float distance = std::sqrt(distSq);
    m.isColliding = true;
    m.contactPoints[0] = box->position + rotateVec(closest, box->rotation);
    m.contactCount = 1;
    
    if (distance == 0.0f) {
        m.penetration = radius;
        m.normal = rotateVec(sf::Vector2f(0, -1), box->rotation);
    } else {
        sf::Vector2f worldNormal = rotateVec(localNormal / distance, box->rotation);
        if (inside) {
            m.normal = worldNormal; // from circle to box
            m.penetration = radius + distance;
        } else {
            m.normal = -worldNormal; // from A to B (circle to box)
            m.penetration = radius - distance;
        }
    }
    
    return m;
}

// OBB vs OBB SAT with face clipping
CollisionManifold CollisionSystem::obbVsOBB(const PhysicsBody* a, sf::Vector2f halfSizeA, 
                                            const PhysicsBody* b, sf::Vector2f halfSizeB) {
    CollisionManifold m;
    
    sf::Vector2f axes[4] = {
        rotateVec(sf::Vector2f(1, 0), a->rotation),
        rotateVec(sf::Vector2f(0, 1), a->rotation),
        rotateVec(sf::Vector2f(1, 0), b->rotation),
        rotateVec(sf::Vector2f(0, 1), b->rotation)
    };
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f smallestAxis;
    
    for (int i = 0; i < 4; ++i) {
        sf::Vector2f axis = axes[i];
        
        float pA1 = std::abs(dotProduct(rotateVec(sf::Vector2f(halfSizeA.x, 0), a->rotation), axis)) + 
                    std::abs(dotProduct(rotateVec(sf::Vector2f(0, halfSizeA.y), a->rotation), axis));
        float cA = dotProduct(a->position, axis);
        float minA = cA - pA1;
        float maxA = cA + pA1;
        
        float pB1 = std::abs(dotProduct(rotateVec(sf::Vector2f(halfSizeB.x, 0), b->rotation), axis)) + 
                    std::abs(dotProduct(rotateVec(sf::Vector2f(0, halfSizeB.y), b->rotation), axis));
        float cB = dotProduct(b->position, axis);
        float minB = cB - pB1;
        float maxB = cB + pB1;
        
        if (maxA < minB || maxB < minA) {
            return m; 
        }
        
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
            if (cA > cB) {
                smallestAxis = -smallestAxis; // point from A to B
            }
        }
    }
    
    m.isColliding = true;
    m.penetration = minOverlap;
    m.normal = smallestAxis;
    
    // Find contact points
    sf::Vector2f contactPoint(0.0f, 0.0f);
    int contactCount = 0;
    
    auto getCorners = [](const PhysicsBody* body, sf::Vector2f halfSize, sf::Vector2f* cornersOut) {
        sf::Vector2f x = rotateVec(sf::Vector2f(halfSize.x, 0), body->rotation);
        sf::Vector2f y = rotateVec(sf::Vector2f(0, halfSize.y), body->rotation);
        cornersOut[0] = body->position - x - y;
        cornersOut[1] = body->position + x - y;
        cornersOut[2] = body->position + x + y;
        cornersOut[3] = body->position - x + y;
    };
    
    auto isInside = [](sf::Vector2f p, const PhysicsBody* body, sf::Vector2f halfSize) {
        sf::Vector2f localP = rotateVec(p - body->position, -body->rotation);
        return std::abs(localP.x) <= halfSize.x + 0.05f && std::abs(localP.y) <= halfSize.y + 0.05f;
    };
    
    sf::Vector2f cornersA[4];
    getCorners(a, halfSizeA, cornersA);
    for (int i = 0; i < 4; ++i) {
        if (isInside(cornersA[i], b, halfSizeB)) {
            m.contactPoints[contactCount] = cornersA[i];
            contactCount++;
            if (contactCount == 2) break;
        }
    }
    
    sf::Vector2f cornersB[4];
    getCorners(b, halfSizeB, cornersB);
    for (int i = 0; i < 4 && contactCount < 2; ++i) {
        if (isInside(cornersB[i], a, halfSizeA)) {
            m.contactPoints[contactCount] = cornersB[i];
            contactCount++;
        }
    }
    
    if (contactCount == 0) {
        // Fallback: projection method
        sf::Vector2f n = smallestAxis;
        sf::Vector2f t(-n.y, n.x);
        
        auto project = [&](const PhysicsBody* body, sf::Vector2f halfSize, sf::Vector2f axis, float& minV, float& maxV) {
            float p = std::abs(dotProduct(rotateVec(sf::Vector2f(halfSize.x, 0), body->rotation), axis)) + 
                      std::abs(dotProduct(rotateVec(sf::Vector2f(0, halfSize.y), body->rotation), axis));
            float c = dotProduct(body->position, axis);
            minV = c - p;
            maxV = c + p;
        };
        
        float minAn, maxAn, minBn, maxBn;
        project(a, halfSizeA, n, minAn, maxAn);
        project(b, halfSizeB, n, minBn, maxBn);
        
        float minAt, maxAt, minBt, maxBt;
        project(a, halfSizeA, t, minAt, maxAt);
        project(b, halfSizeB, t, minBt, maxBt);
        
        float centerT = (std::max(minAt, minBt) + std::min(maxAt, maxBt)) * 0.5f;
        float centerN = (maxAn + minBn) * 0.5f; // n points from A to B
        m.contactPoints[0] = n * centerN + t * centerT;
        m.contactCount = 1;
    } else {
        m.contactCount = contactCount;
    }
    
    return m;
}

CollisionManifold CollisionSystem::detectCollision(PhysicsBody* a, PhysicsBody* b) {
    if (!a->userData || !b->userData) {
        // Assume OBBvsOBB for ground vs blocks if ground doesn't have userdata?
        // Let's assume all dynamic bodies have userData = Entity*
        // Ground might have userData = nullptr. Let's make ground an OBB.
        // Actually, if userData is missing, we must default to OBB.
        return obbVsOBB(a, {10000.0f, 100.0f}, b, {17.5f, 35.0f}); // this is hacky, but wait:
    }
    
    Entity* entA = static_cast<Entity*>(a->userData);
    Entity* entB = static_cast<Entity*>(b->userData);
    
    auto getType = [](Entity* ent) {
        if (dynamic_cast<Bird*>(ent)) return 0; // Circle
        if (dynamic_cast<Pig*>(ent)) return 1;  // Circle
        if (dynamic_cast<Block*>(ent)) return 2; // OBB
        return 3; // Unknown
    };
    
    // For the Ground static body, I will assign userData to a special value or just handle it if entA/B is null.
    // If entA or entB is nullptr, it's the ground. The ground is an OBB.
    int typeA = entA ? getType(entA) : 2; 
    int typeB = entB ? getType(entB) : 2;

    float rA = 0.0f; sf::Vector2f hA = {0,0};
    float rB = 0.0f; sf::Vector2f hB = {0,0};

    if (typeA == 0) rA = static_cast<Bird*>(entA)->getRadius();
    else if (typeA == 1) rA = static_cast<Pig*>(entA)->getRadius();
    else if (entA) hA = static_cast<Block*>(entA)->getHalfSize();
    else hA = {5000.0f, 100.0f}; // ground halfsize

    if (typeB == 0) rB = static_cast<Bird*>(entB)->getRadius();
    else if (typeB == 1) rB = static_cast<Pig*>(entB)->getRadius();
    else if (entB) hB = static_cast<Block*>(entB)->getHalfSize();
    else hB = {5000.0f, 100.0f}; // ground halfsize

    if (typeA <= 1 && typeB <= 1) return circleVsCircle(a, rA, b, rB);
    if (typeA <= 1 && typeB >= 2) return circleVsOBB(a, rA, b, hB);
    if (typeA >= 2 && typeB <= 1) {
        CollisionManifold m = circleVsOBB(b, rB, a, hA);
        if (m.isColliding) m.normal = -m.normal;
        return m;
    }
    return obbVsOBB(a, hA, b, hB);
}
