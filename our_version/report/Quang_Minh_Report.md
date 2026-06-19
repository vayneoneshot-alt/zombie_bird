# Detailed Physics & Collision Report: Quang Minh

This section details the collision detection framework and physics data protection engineered by **Quang Minh** during Phase 2 of the Zombie Bird project. The primary focus of this work was **Encapsulation**, effectively hiding the complex mathematics of the Separating Axis Theorem (SAT) and safeguarding the physics states of objects.

## 1. Encapsulation: The `CollisionSystem`

The most mathematically intensive part of any physics engine is collision detection. Instead of allowing entities to check collisions amongst themselves, this logic was encapsulated into a dedicated, stateless `CollisionSystem` utility class.

### Hiding Complex Mathematics (SAT)
The `CollisionSystem` exposes simple, high-level methods like `detectCollision(PhysicsBody* a, PhysicsBody* b)`. However, internally, it encapsulates highly complex mathematical routines, including:
- **Separating Axis Theorem (SAT)**: Projecting shapes onto axes to find the minimum translation vector (penetration depth).
- **Manifold Generation**: Calculating the exact contact points and contact normals between two colliding Oriented Bounding Boxes (OBBs) or Circles.

```cpp
// External interface is clean and simple:
CollisionManifold m = CollisionSystem::detectCollision(a, b);

// Internal encapsulation hides SAT complexities:
float overlapX = obbOverlap(a, aHalfSize, b, bHalfSize, a->rotation, b->rotation, sf::Vector2f(1, 0));
```
**OOP Principle Application (Encapsulation):** By encapsulating these algorithms inside `CollisionSystem.cpp`, the rest of the codebase (like the `PhysicsWorld` and the `GameplayState`) is completely shielded from vector projection math. If the collision detection algorithm were to be swapped out in the future (e.g., to GJK), no other part of the engine would need to change.

## 2. Managing `PhysicsBody` State Protections

The `PhysicsBody` struct was designed to encapsulate the physical properties of an entity (mass, velocity, forces, shape) away from its graphical representation.

### Strict Boundaries on Modification
While `PhysicsBody` acts as a data container, it employs encapsulation principles to ensure that its physical state is manipulated securely through defined interfaces:
- **Forces and Impulses**: Rather than directly altering the velocity vector from external classes, forces are applied via methods like `applyForce()` or resolved by the `PhysicsWorld`'s iterative solver.
- **Velocity Clamping**: The collision resolution ensures strict boundaries on velocity and impulse modification (e.g., using Baumgarte stabilization and clamping friction impulses to prevent unstable infinite energy gain).

### Decoupling Logic
Because `PhysicsBody` only holds a `void* userData` pointer back to its owner, the physics solver has zero dependencies on the actual game logic. 

---
### Summary of Impact
Quang Minh's focus on encapsulation ensured that the mathematical complexity of a custom physics engine did not bleed into the gameplay code. By isolating the collision detection and securely managing the `PhysicsBody` states, the engine achieved stability and maintainability.
