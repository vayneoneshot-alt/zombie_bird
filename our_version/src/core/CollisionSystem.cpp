#include "CollisionSystem.h"
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

inline float crossProduct(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.y - a.y * b.x;
}

inline sf::Vector2f crossProduct(float a, sf::Vector2f v) {
    return sf::Vector2f(-a * v.y, a * v.x);
}

CollisionManifold CollisionSystem::circleVsCircle(const PhysicsBody& a, float radiusA, 
                                                  const PhysicsBody& b, float radiusB) {
    CollisionManifold m;
    sf::Vector2f n = b.position - a.position;
    float distSq = dotProduct(n, n);
    float radiusSum = radiusA + radiusB;
    
    if (distSq >= radiusSum * radiusSum || distSq == 0.0f) {
        return m;
    }
    
    float distance = std::sqrt(distSq);
    m.isColliding = true;
    m.penetration = radiusSum - distance;
    m.normal = n / distance;
    m.contactPoint = a.position + m.normal * radiusA;
    return m;
}

CollisionManifold CollisionSystem::circleVsOBB(const PhysicsBody& circle, float radius, 
                                               const PhysicsBody& box, sf::Vector2f boxHalfSize) {
    CollisionManifold m;
    
    // Transform circle center into box's local space
    sf::Vector2f d = circle.position - box.position;
    sf::Vector2f localCircle = rotateVec(d, -box.rotation);
    
    // Find closest point on box
    sf::Vector2f closest = localCircle;
    closest.x = std::max(-boxHalfSize.x, std::min(boxHalfSize.x, closest.x));
    closest.y = std::max(-boxHalfSize.y, std::min(boxHalfSize.y, closest.y));
    
    bool inside = false;
    if (localCircle == closest) {
        inside = true;
        // Circle center is inside the box, push it out to the closest edge
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
    m.contactPoint = box.position + rotateVec(closest, box.rotation);
    
    if (distance == 0.0f) {
        m.penetration = radius;
        m.normal = rotateVec(sf::Vector2f(0, -1), box.rotation); // Arbitrary normal
    } else {
        sf::Vector2f worldNormal = rotateVec(localNormal / distance, box.rotation);
        if (inside) {
            m.normal = worldNormal; // from circle to box
            m.penetration = radius + distance;
        } else {
            m.normal = -worldNormal; // we want normal from A to B (circle to box)
            m.penetration = radius - distance;
        }
    }
    
    return m;
}

// OBB vs OBB SAT
CollisionManifold CollisionSystem::obbVsOBB(const PhysicsBody& a, sf::Vector2f halfSizeA, 
                                            const PhysicsBody& b, sf::Vector2f halfSizeB) {
    CollisionManifold m;
    
    sf::Vector2f axes[4] = {
        rotateVec(sf::Vector2f(1, 0), a.rotation),
        rotateVec(sf::Vector2f(0, 1), a.rotation),
        rotateVec(sf::Vector2f(1, 0), b.rotation),
        rotateVec(sf::Vector2f(0, 1), b.rotation)
    };
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f smallestAxis;
    
    for (int i = 0; i < 4; ++i) {
        sf::Vector2f axis = axes[i];
        
        // Project A
        float pA1 = std::abs(dotProduct(rotateVec(sf::Vector2f(halfSizeA.x, 0), a.rotation), axis)) + 
                    std::abs(dotProduct(rotateVec(sf::Vector2f(0, halfSizeA.y), a.rotation), axis));
        float cA = dotProduct(a.position, axis);
        float minA = cA - pA1;
        float maxA = cA + pA1;
        
        // Project B
        float pB1 = std::abs(dotProduct(rotateVec(sf::Vector2f(halfSizeB.x, 0), b.rotation), axis)) + 
                    std::abs(dotProduct(rotateVec(sf::Vector2f(0, halfSizeB.y), b.rotation), axis));
        float cB = dotProduct(b.position, axis);
        float minB = cB - pB1;
        float maxB = cB + pB1;
        
        if (maxA < minB || maxB < minA) {
            return m; // Separating axis found
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
    
    // Approximate contact point: halfway between centers along the normal
    m.contactPoint = a.position + (b.position - a.position) * 0.5f;
    
    return m;
}

void CollisionSystem::resolveCollision(PhysicsBody& a, PhysicsBody& b, CollisionManifold& manifold) {
    if (!manifold.isColliding) return;
    
    // 1. Positional correction
    const float percent = 0.8f;
    const float slop = 0.01f;
    float invMassA = a.isStatic ? 0.0f : 1.0f / a.mass;
    float invMassB = b.isStatic ? 0.0f : 1.0f / b.mass;
    float totalInvMass = invMassA + invMassB;
    if (totalInvMass == 0.0f) return;
    
    float correctionAmount = std::max(manifold.penetration - slop, 0.0f) / totalInvMass * percent;
    sf::Vector2f correction = manifold.normal * correctionAmount;
    
    if (!a.isStatic) a.position -= correction * invMassA;
    if (!b.isStatic) b.position += correction * invMassB;
    
    // 2. Impulse resolution
    sf::Vector2f rA = manifold.contactPoint - a.position;
    sf::Vector2f rB = manifold.contactPoint - b.position;
    
    sf::Vector2f vA = a.velocity + crossProduct(a.angularVelocity, rA);
    sf::Vector2f vB = b.velocity + crossProduct(b.angularVelocity, rB);
    sf::Vector2f rv = vB - vA;
    
    float velAlongNormal = dotProduct(rv, manifold.normal);
    if (velAlongNormal > 0) return;
    
    float e = std::min(a.restitution, b.restitution);
    if (std::abs(velAlongNormal) < 25.0f) {
        e = 0.0f;
        if (std::abs(manifold.normal.y) > 0.8f) {
            // Propagate resting state from bottom to top to prevent mid-air slow falls
            if (manifold.normal.y > 0 && std::abs(b.velocity.y) < 5.0f) { // A is above B, B is resting
                a.velocity.y = 0.0f;
                a.velocity.x *= 0.1f; // Massive horizontal friction
                a.angularVelocity *= 0.1f;
            } else if (manifold.normal.y < 0 && std::abs(a.velocity.y) < 5.0f) { // B is above A, A is resting
                b.velocity.y = 0.0f;
                b.velocity.x *= 0.1f; // Massive horizontal friction
                b.angularVelocity *= 0.1f;
            }
            // Recalculate relative velocity after stabilization
            vA = a.velocity + crossProduct(a.angularVelocity, rA);
            vB = b.velocity + crossProduct(b.angularVelocity, rB);
            rv = vB - vA;
            velAlongNormal = dotProduct(rv, manifold.normal);
        }
    }
    
    float invInertiaA = a.isStatic ? 0.0f : 1.0f / a.inertia;
    float invInertiaB = b.isStatic ? 0.0f : 1.0f / b.inertia;
    
    float rA_cross_n = crossProduct(rA, manifold.normal);
    float rB_cross_n = crossProduct(rB, manifold.normal);
    
    float invMassSum = invMassA + invMassB + 
                       (rA_cross_n * rA_cross_n) * invInertiaA + 
                       (rB_cross_n * rB_cross_n) * invInertiaB;
                       
    float j = -(1 + e) * velAlongNormal / invMassSum;
    sf::Vector2f impulse = manifold.normal * j;
    
    if (!a.isStatic) {
        a.velocity -= impulse * invMassA;
        a.angularVelocity -= crossProduct(rA, impulse) * invInertiaA;
    }
    if (!b.isStatic) {
        b.velocity += impulse * invMassB;
        b.angularVelocity += crossProduct(rB, impulse) * invInertiaB;
    }
    
    manifold.impactForce = j;
    
    // 3. Friction resolution
    sf::Vector2f tangent = rv - (velAlongNormal * manifold.normal);
    float tangentLen = std::sqrt(dotProduct(tangent, tangent));
    if (tangentLen > 0.0001f) {
        tangent /= tangentLen;
        float rA_cross_t = crossProduct(rA, tangent);
        float rB_cross_t = crossProduct(rB, tangent);
        float invMassSumT = invMassA + invMassB + 
                            (rA_cross_t * rA_cross_t) * invInertiaA + 
                            (rB_cross_t * rB_cross_t) * invInertiaB;
                            
        float jt = -dotProduct(rv, tangent) / invMassSumT;
        float mu = std::min(a.friction, b.friction);
        
        sf::Vector2f frictionImpulse;
        if (std::abs(jt) < j * mu) {
            frictionImpulse = jt * tangent;
        } else {
            frictionImpulse = -j * mu * tangent;
        }
        
        if (!a.isStatic) {
            a.velocity -= frictionImpulse * invMassA;
            a.angularVelocity -= crossProduct(rA, frictionImpulse) * invInertiaA;
        }
        if (!b.isStatic) {
            b.velocity += frictionImpulse * invMassB;
            b.angularVelocity += crossProduct(rB, frictionImpulse) * invInertiaB;
        }
    }
}
