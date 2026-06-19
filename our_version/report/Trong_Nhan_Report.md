# Detailed Architecture & Core Engine Report: Trọng Nhân

This section details the foundational object-oriented architecture and physics integration designed and implemented by **Trọng Nhân** during Phase 1 and 2 of the Zombie Bird project. The primary focus of this work was **Abstraction** and **Encapsulation** to create a scalable, decoupled game engine baseline.

## 1. Abstraction: The State Machine Architecture

To prevent a monolithic `main` loop filled with `if/else` statements handling different game screens (menus, gameplay, win screens), the engine utilizes a classic State Machine pattern driven by the `IState` interface.

### The `IState` Interface
The `IState` class is a pure abstract base class (an interface in C++) that defines the contract for any game state:
```cpp
class IState {
public:
    virtual ~IState() = default;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
```
**OOP Principle Application (Abstraction):** This interface completely abstracts the behavior of a game screen. The core application loop does not need to know *what* state is running; it only knows that the state can be initialized (`onEnter`), updated (`update`), drawn (`draw`), and cleaned up (`onExit`).

### The `StateManager`
The `StateManager` encapsulates the management of these states using a stack (`std::vector<std::unique_ptr<IState>> stack`). It acts as the centralized controller, exposing methods like `pushState`, `popState`, and `changeState` to allow seamless transitions. 
By utilizing `std::unique_ptr`, the system inherently manages memory lifecycle, ensuring state resources are automatically cleaned up when states are popped.

## 2. Encapsulation: The SFML Render Pipeline

The main application loop in `main.cpp` was meticulously encapsulated to ensure it remains lightweight and agnostic of game logic.

```cpp
while (window.isOpen()) {
    sf::Time dt = clock.restart();
    
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        stateManager.handleEvent(event);
    }

    stateManager.update(dt.asSeconds());

    window.clear();
    stateManager.draw(window);
    window.display();
}
```
**OOP Principle Application (Encapsulation):** All game-specific logic, input handling, and rendering calls are encapsulated entirely within the concrete states. The core loop acts only as a generic pipeline—delegating event polling, delta-time calculation, and render-buffer clearing/displaying to the `StateManager`. This strict boundary prevents the `main` loop from becoming bloated as new features are added.

## 3. Core `PhysicsWorld` Integration Loop

Rather than updating physics calculations per entity, the system utilizes a centralized `PhysicsWorld`. This is crucial for performance and accurate collision resolution.

### Semi-Implicit Euler Integration
The `PhysicsWorld::step(float dt, int iterations)` method is the heart of the engine's physics. It was designed to run a robust integration loop:
1. **Force Integration**: Iterates through all dynamic `PhysicsBody` objects and integrates external forces (gravity, acceleration) to update velocities. Forces are automatically cleared each frame.
2. **Broadphase / Narrowphase Delegation**: Iterates through pairs of bodies and delegates the complex SAT math to the encapsulated `CollisionSystem`.
3. **Pre-Step & Warm Starting**: Calculates effective masses (normal and tangent) and applies Baumgarte stabilization for penetration resolution. It heavily utilizes warm-starting (carrying over impulses from the previous frame) to maintain stable resting contacts.
4. **Iterative Solver**: Uses an iterative constraint solver over multiple iterations to resolve normal impulses (bouncing/pushing) and tangent impulses (friction).
5. **Velocity Integration**: Finally, uses the newly resolved velocities to update the spatial positions and rotations of the bodies.

**Decoupled Architecture:** The `PhysicsWorld` has absolutely no knowledge of SFML sprites or Game Entities (like `Bird` or `Pig`). It operates strictly on raw `PhysicsBody` pointers. Entities are responsible for passing their body references to the world and later synchronizing their graphical sprites to the body's transform.

---
### Summary of Impact
Trọng Nhân's architectural baseline ensured that the rest of the team could build gameplay features concurrently. By abstracting the game loop into states, and decoupling the physics integration from the game objects, the project maintained a strict adherence to SOLID principles from day one.
