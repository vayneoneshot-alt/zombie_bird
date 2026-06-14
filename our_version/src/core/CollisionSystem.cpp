#include "CollisionSystem.h"
#include <cmath>
#include <algorithm>

CollisionManifold CollisionSystem::circleVsCircle(const PhysicsBody& a, float radiusA, 
                                                  const PhysicsBody& b, float radiusB) {
    CollisionManifold m;
    
    sf::Vector2f n = b.position - a.position;
    float distSq = n.x * n.x + n.y * n.y;
    float radiusSum = radiusA + radiusB;
    
    if (distSq >= radiusSum * radiusSum || distSq == 0.0f) {
        return m;
    }
    
    float distance = std::sqrt(distSq);
    
    m.isColliding = true;
    m.penetration = radiusSum - distance;
    m.normal = n / distance; // Normalize
    
    return m;
}

CollisionManifold CollisionSystem::circleVsAABB(const PhysicsBody& circle, float radius, 
                                                const PhysicsBody& box, sf::Vector2f boxHalfSize) {
    CollisionManifold m;
    
    // Vector from box to circle
    sf::Vector2f n = circle.position - box.position;
    
    // Closest point on box to center of circle
    sf::Vector2f closest = n;
    
    // Clamp point to edges of the AABB
    closest.x = std::max(-boxHalfSize.x, std::min(boxHalfSize.x, closest.x));
    closest.y = std::max(-boxHalfSize.y, std::min(boxHalfSize.y, closest.y));
    
    bool inside = false;
    
    // Circle is inside the AABB
    if (n == closest) {
        inside = true;
        // Find closest axis
        if (std::abs(n.x) > std::abs(n.y)) {
            if (closest.x > 0) closest.x = boxHalfSize.x;
            else closest.x = -boxHalfSize.x;
        } else {
            if (closest.y > 0) closest.y = boxHalfSize.y;
            else closest.y = -boxHalfSize.y;
        }
    }
    
    sf::Vector2f normal = n - closest;
    float distSq = normal.x * normal.x + normal.y * normal.y;
    
    if (distSq > radius * radius && !inside) {
        return m; // No collision
    }
    
    float distance = std::sqrt(distSq);
    
    m.isColliding = true;
    if (inside) {
        m.normal = -normal / distance;
        m.penetration = radius - distance;
    } else {
        m.normal = normal / distance;
        m.penetration = radius - distance;
    }
    
    return m;
}

CollisionManifold CollisionSystem::aabbVsAABB(const PhysicsBody& a, sf::Vector2f halfSizeA, 
                                              const PhysicsBody& b, sf::Vector2f halfSizeB) {
    CollisionManifold m;
    
    sf::Vector2f n = b.position - a.position;
    
    float x_overlap = halfSizeA.x + halfSizeB.x - std::abs(n.x);
    if (x_overlap > 0) {
        float y_overlap = halfSizeA.y + halfSizeB.y - std::abs(n.y);
        if (y_overlap > 0) {
            m.isColliding = true;
            if (x_overlap < y_overlap) {
                if (n.x < 0) m.normal = sf::Vector2f(-1, 0);
                else m.normal = sf::Vector2f(1, 0);
                m.penetration = x_overlap;
            } else {
                if (n.y < 0) m.normal = sf::Vector2f(0, -1);
                else m.normal = sf::Vector2f(0, 1);
                m.penetration = y_overlap;
            }
        }
    }
    return m;
}

void CollisionSystem::resolveCollision(PhysicsBody& a, PhysicsBody& b, CollisionManifold& manifold) {
    if (!manifold.isColliding) return;
    
    // 1. Positional correction (prevent sinking)
    const float percent = 0.8f; // usually 0.2 to 0.8
    const float slop = 0.01f; // usually 0.01 to 0.1
    float invMassA = a.isStatic ? 0.0f : 1.0f / a.mass;
    float invMassB = b.isStatic ? 0.0f : 1.0f / b.mass;
    float totalInvMass = invMassA + invMassB;
    
    if (totalInvMass == 0.0f) return;
    
    float correctionAmount = std::max(manifold.penetration - slop, 0.0f) / totalInvMass * percent;
    sf::Vector2f correction = manifold.normal * correctionAmount;
    
    if (!a.isStatic) a.position -= correction * invMassA;
    if (!b.isStatic) b.position += correction * invMassB;
    
    // 2. Impulse resolution
    sf::Vector2f rv = b.velocity - a.velocity;
    float velAlongNormal = rv.x * manifold.normal.x + rv.y * manifold.normal.y;
    
    // Do not resolve if velocities are separating
    if (velAlongNormal > 0) return;
    
    float e = std::min(a.restitution, b.restitution);
    
    float j = -(1 + e) * velAlongNormal;
    j /= totalInvMass;
    
    sf::Vector2f impulse = manifold.normal * j;
    
    if (!a.isStatic) a.velocity -= impulse * invMassA;
    if (!b.isStatic) b.velocity += impulse * invMassB;
    
    // Store impact force for damage calculation later
    manifold.impactForce = j;
}
