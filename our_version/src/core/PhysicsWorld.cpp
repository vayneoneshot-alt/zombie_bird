#include "PhysicsWorld.h"
#include "CollisionSystem.h"
#include <algorithm>

PhysicsWorld::PhysicsWorld(sf::Vector2f gravity) : gravity(gravity) {}

void PhysicsWorld::addBody(PhysicsBody* body) {
    if (std::find(bodies.begin(), bodies.end(), body) == bodies.end()) {
        bodies.push_back(body);
    }
}

void PhysicsWorld::removeBody(PhysicsBody* body) {
    auto it = std::find(bodies.begin(), bodies.end(), body);
    if (it != bodies.end()) {
        bodies.erase(it);
    }
}

std::vector<CollisionEvent> PhysicsWorld::step(float dt, int iterations) {
    std::vector<CollisionEvent> events;
    if (dt <= 0.0f) return events;
    
    float inv_dt = 1.0f / dt;

    // 1. Integrate forces
    for (auto* body : bodies) {
        if (body->isStatic) continue;
        // Gravity is handled directly in world now, not in Entity update
        body->velocity += gravity * body->gravityScale * dt;
        body->velocity += body->acceleration * dt;
        body->angularVelocity += body->angularAcceleration * dt;
        
        // Reset forces
        body->acceleration = {0.0f, 0.0f};
        body->angularAcceleration = 0.0f;
    }

    // 2. Broadphase & Narrowphase (Collision Detection)
    // We rebuild constraints each frame. For a full engine, you'd match old contacts for warm starting.
    std::vector<ContactConstraint> newContacts;
    
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            PhysicsBody* a = bodies[i];
            PhysicsBody* b = bodies[j];
            
            if (a->isStatic && b->isStatic) continue;
            
            // Assuming broadphase is done, we do narrowphase
            CollisionManifold m = CollisionSystem::detectCollision(a, b);
            
            if (m.isColliding) {
                for (int k = 0; k < m.contactCount; ++k) {
                    ContactConstraint c;
                    c.a = a;
                    c.b = b;
                    c.normal = m.normal; // Points from A to B
                    c.contactPoint = m.contactPoints[k];
                    c.penetration = m.penetration;
                    newContacts.push_back(c);
                }
            }
        }
    }
    
    // Match old contacts for warm starting (simplified distance-based matching)
    for (auto& newC : newContacts) {
        for (const auto& oldC : currentContacts) {
            if ((newC.a == oldC.a && newC.b == oldC.b) || (newC.a == oldC.b && newC.b == oldC.a)) {
                // If contact points are close, carry over the impulse
                float dx = newC.contactPoint.x - oldC.contactPoint.x;
                float dy = newC.contactPoint.y - oldC.contactPoint.y;
                if (dx*dx + dy*dy < 4.0f) {
                    newC.normalImpulse = oldC.normalImpulse;
                    newC.tangentImpulse = oldC.tangentImpulse;
                    break;
                }
            }
        }
    }
    currentContacts = newContacts;

    // 3. Pre-Step (Calculate Effective Mass & Apply Warm Starting)
    const float allowedPenetration = 0.01f;
    const float baumgarte = 0.2f;

    for (auto& c : currentContacts) {
        c.rA = c.contactPoint - c.a->position;
        c.rB = c.contactPoint - c.b->position;
        
        float rnA = c.rA.x * c.normal.y - c.rA.y * c.normal.x; // crossProduct(rA, normal)
        float rnB = c.rB.x * c.normal.y - c.rB.y * c.normal.x; // crossProduct(rB, normal)
        
        float kNormal = c.a->invMass + c.b->invMass + 
                        c.a->invInertia * rnA * rnA + 
                        c.b->invInertia * rnB * rnB;
        c.massNormal = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;
        
        sf::Vector2f tangent(-c.normal.y, c.normal.x);
        float rtA = c.rA.x * tangent.y - c.rA.y * tangent.x;
        float rtB = c.rB.x * tangent.y - c.rB.y * tangent.x;
        
        float kTangent = c.a->invMass + c.b->invMass + 
                         c.a->invInertia * rtA * rtA + 
                         c.b->invInertia * rtB * rtB;
        c.massTangent = kTangent > 0.0f ? 1.0f / kTangent : 0.0f;
        
        // Bias velocity for penetration resolution
        c.bias = -baumgarte * inv_dt * std::min(0.0f, -c.penetration + allowedPenetration);
        
        // Apply warm starting
        sf::Vector2f P = c.normal * c.normalImpulse + tangent * c.tangentImpulse;
        if (!c.a->isStatic) {
            c.a->velocity -= P * c.a->invMass;
            c.a->angularVelocity -= (c.rA.x * P.y - c.rA.y * P.x) * c.a->invInertia;
        }
        if (!c.b->isStatic) {
            c.b->velocity += P * c.b->invMass;
            c.b->angularVelocity += (c.rB.x * P.y - c.rB.y * P.x) * c.b->invInertia;
        }
    }

    // 4. Iterative Solver
    for (int iter = 0; iter < iterations; ++iter) {
        for (auto& c : currentContacts) {
            sf::Vector2f vA = c.a->velocity + sf::Vector2f(-c.a->angularVelocity * c.rA.y, c.a->angularVelocity * c.rA.x);
            sf::Vector2f vB = c.b->velocity + sf::Vector2f(-c.b->angularVelocity * c.rB.y, c.b->angularVelocity * c.rB.x);
            sf::Vector2f rv = vB - vA;
            
            // Solve Normal Constraint
            float vn = rv.x * c.normal.x + rv.y * c.normal.y;
            // Include restitution (bounciness) if velocity is high enough
            float restitution = std::min(c.a->restitution, c.b->restitution);
            float bounce = 0.0f;
            if (vn < -10.0f) {
                bounce = -restitution * vn;
            }
            
            float dPn = c.massNormal * (-vn + c.bias + bounce);
            
            // Clamp normal impulse
            float pn0 = c.normalImpulse;
            c.normalImpulse = std::max(pn0 + dPn, 0.0f);
            dPn = c.normalImpulse - pn0;
            
            sf::Vector2f Pn = c.normal * dPn;
            if (!c.a->isStatic) {
                c.a->velocity -= Pn * c.a->invMass;
                c.a->angularVelocity -= (c.rA.x * Pn.y - c.rA.y * Pn.x) * c.a->invInertia;
            }
            if (!c.b->isStatic) {
                c.b->velocity += Pn * c.b->invMass;
                c.b->angularVelocity += (c.rB.x * Pn.y - c.rB.y * Pn.x) * c.b->invInertia;
            }
            
            // Solve Tangent (Friction) Constraint
            vA = c.a->velocity + sf::Vector2f(-c.a->angularVelocity * c.rA.y, c.a->angularVelocity * c.rA.x);
            vB = c.b->velocity + sf::Vector2f(-c.b->angularVelocity * c.rB.y, c.b->angularVelocity * c.rB.x);
            rv = vB - vA;
            
            sf::Vector2f tangent(-c.normal.y, c.normal.x);
            float vt = rv.x * tangent.x + rv.y * tangent.y;
            float dPt = c.massTangent * (-vt);
            
            // Clamp friction
            float friction = std::min(c.a->friction, c.b->friction);
            float maxPt = friction * c.normalImpulse;
            float pt0 = c.tangentImpulse;
            c.tangentImpulse = std::max(-maxPt, std::min(pt0 + dPt, maxPt));
            dPt = c.tangentImpulse - pt0;
            
            sf::Vector2f Pt = tangent * dPt;
            if (!c.a->isStatic) {
                c.a->velocity -= Pt * c.a->invMass;
                c.a->angularVelocity -= (c.rA.x * Pt.y - c.rA.y * Pt.x) * c.a->invInertia;
            }
            if (!c.b->isStatic) {
                c.b->velocity += Pt * c.b->invMass;
                c.b->angularVelocity += (c.rB.x * Pt.y - c.rB.y * Pt.x) * c.b->invInertia;
            }
        }
    }

    // Generate Collision Events for Gameplay
    for (const auto& c : currentContacts) {
        if (c.normalImpulse > 0.0f) {
            CollisionEvent ev;
            ev.bodyA = c.a;
            ev.bodyB = c.b;
            ev.normalImpulse = c.normalImpulse;
            events.push_back(ev);
        }
    }

    // 5. Integrate Velocities
    for (auto* body : bodies) {
        if (body->isStatic) continue;
        body->position += body->velocity * dt;
        body->rotation += body->angularVelocity * dt;
    }

    return events;
}
