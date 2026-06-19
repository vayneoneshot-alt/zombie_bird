# Zombie Bird: OOP Final Project Architecture and Implementation Report

**Role**: Senior Software Engineer & Technical Lead
**Project**: Zombie Bird (Angry Birds Clone)
**Objective**: Codebase Analysis, OOP Architecture Review, and Team Task Allocation.

---

## 1. Codebase Review & OOP Analysis

The "Zombie Bird" codebase demonstrates a well-structured, modular C++ architecture. By strictly adhering to Object-Oriented Programming (OOP) principles, the game cleanly separates rendering, game logic, state management, and custom physics simulation. 

### Core Architecture and Design Patterns
- **State Pattern**: The game effectively uses a `StateManager` interacting with an abstract `IState` interface. This allows seamless transitions between `MainMenuState`, `LevelSelectState`, `GameplayState`, `WinState`, and `LoseState` without tightly coupling the states together.
- **Entity Hierarchy (Component-like Modeling)**: The game uses a robust inheritance hierarchy for game objects (`Bird`, `Pig`, `Block`), binding visual representation (SFML Sprites) to a custom physical representation (`PhysicsBody`).
- **Custom Physics Engine**: Rather than relying on external libraries like Box2D, the game implements a custom `PhysicsWorld` and `CollisionSystem` supporting Circle and OBB (Oriented Bounding Box) collision detection and resolution.

### The 4 Pillars of OOP Implemented

#### I. Encapsulation
The codebase actively restricts direct access to an object's internal state, exposing only what is necessary through public methods.
- **`PhysicsBody`**: Manages mass, inertia, and forces through controlled methods like `setMass()`, `setStatic()`, and `applyForce()`. While acting as a data container for the `PhysicsWorld` solver, it ensures physical properties like inverse mass and inertia are calculated safely and consistently.
- **Game Entities**: Classes like `Pig` and `Block` encapsulate their `health` and `isDestroyed` status. Damage is applied via specific interaction methods (e.g., assessing collision impulse), protecting the internal state from invalid modifications.

#### II. Inheritance
Inheritance is utilized to share common functionality and define strict "is-a" relationships, drastically reducing code duplication.
- **Entity Tree**: At the top level, `Entity` serves as the base class for all game objects. `Pig`, `Block`, and `Bird` inherit from `Entity`, acquiring standard update and draw capabilities.
- **Bird Subclasses**: The `Bird` class is further subclassed into `BasicBird`, `DashBird`, `ExplosiveBird`, and `SplitBird`. Each subclass inherits standard flight mechanics but introduces unique extended functionality.

#### III. Polymorphism
Polymorphism allows the game engine to treat different objects uniformly through base class pointers.
- **Runtime State Resolution**: The `StateManager` holds a stack of `std::unique_ptr<IState>`. When `update(dt)` or `draw(window)` is called, C++ dynamically dispatches the call to the currently active state (e.g., `GameplayState` or `MainMenuState`).
- **Dynamic Entities**: In `GameplayState`, birds are stored dynamically as `std::unique_ptr<Bird>`. The game loop iterates through them, calling `virtual void update(float dt)` and `virtual void draw(sf::RenderWindow& window)` without needing to know if they are `DashBird`, `SplitBird`, or `BasicBird`.
- **Bird Abilities**: The `virtual void onAbility()` method is overridden in subclasses. When the player clicks, the game calls this method on the active `Bird*`, and the correct behavior (Dashing, Exploding, or Splitting) executes dynamically.

#### IV. Abstraction
Abstraction hides complex implementation details behind simple, high-level interfaces.
- **`IState` Interface**: Exposes only high-level hooks (`onEnter`, `update`, `draw`, `handleEvent`). The core application loop does not need to know *how* a level is loaded; it only knows that the current state can be updated.
- **`CollisionSystem`**: The physics world simply passes two `PhysicsBody` pointers to the collision system. The complex mathematics of separating axis theorem (SAT) and manifold generation are completely abstracted away from the gameplay logic.

---

## 2. Complete Implementation Plan (Retroactive)

This outlines how the project was systematically built from scratch.

### Phase 1: Core Engine & Architecture Foundation
- **Step 1.1**: Set up CMake, SFML window initialization, and the main application loop (`main.cpp`).
- **Step 1.2**: Define the `IState` interface and implement the `StateManager` to handle pushing, popping, and transitioning between states.
- **Step 1.3**: Implement the `ResourceManager` to abstract the loading and caching of textures and fonts.

### Phase 2: Custom Physics Engine
- **Step 2.1**: Define the `PhysicsBody` struct to hold mass, velocity, forces, and shape data.
- **Step 2.2**: Implement `PhysicsWorld` to integrate physics bodies over time using Semi-Implicit Euler integration, applying gravity.
- **Step 2.3**: Build the `CollisionSystem`. Implement Circle-vs-Circle and Circle-vs-OBB detection.
- **Step 2.4**: Implement advanced Oriented Bounding Box (OBB) collision detection using SAT (Separating Axis Theorem) and impulse-based collision resolution (bounciness and friction).

### Phase 3: OOP Entity Modeling & Game Objects
- **Step 3.1**: Create the abstract `Entity` base class linking a `PhysicsBody` to an `sf::Sprite`.
- **Step 3.2**: Implement the `Block` and `Pig` classes, adding health, damage calculation based on collision impulse, and destruction logic.
- **Step 3.3**: Build the base `Bird` class with dragging and launching mechanics.
- **Step 3.4**: Leverage inheritance to implement polymorphic birds (`BasicBird`, `DashBird`, `ExplosiveBird`, `SplitBird`) with overridden `onAbility()` methods.

### Phase 4: Gameplay Mechanics & Level Loading
- **Step 4.1**: Implement the `Slingshot` logic (mouse dragging, rubber band rendering) and `TrajectoryLine` (parabolic flight path prediction).
- **Step 4.2**: Integrate a JSON parser (nlohmann/json) to dynamically load level layouts (blocks, pigs, bird inventory) from `.json` files.
- **Step 4.3**: Implement win/loss conditions inside `GameplayState` (checking if all pigs are dead or if birds are depleted).

### Phase 5: UI and Polish
- **Step 5.1**: Build `MainMenuState`, `LevelSelectState`, `WinState`, and `LoseState` to handle application flow. Implement interactive UI elements like the Level Selection buttons and an in-game Exit button.
- **Step 5.2**: Implement a 3-second cinematic delay upon winning before transitioning to the WinState.
- **Step 5.3**: Final tuning of physics parameters (gravity scale, block mass) and pre-settling logic to prevent initial jittering.

---

## 3. Team Task Allocation (5 Members)

To successfully execute the implementation plan, the workload was divided to emphasize individual strengths while ensuring all members heavily utilized and applied OOP principles in their designated domains.

| Team Member | Role | Assigned Phases | Specific OOP & Technical Responsibilities |
| :--- | :--- | :--- | :--- |
| **Trọng Nhân** | **Lead Architect & Core Engine Dev** | Phase 1 & 2 | **Abstraction**: Designed the `IState` interface and `StateManager`. Encapsulated the SFML render pipeline and designed the core `PhysicsWorld` integration loop. Set up the baseline architecture. |
| **Quang Minh** | **Physics & Collision Specialist** | Phase 2 | **Encapsulation**: Built the `CollisionSystem`. Encapsulated complex SAT math and manifold generation away from entities. Managed `PhysicsBody` state protections, ensuring strict boundaries on velocity and impulse modification. |
| **Tuấn Nguyên** | **Gameplay Programmer (Entities)** | Phase 3 & 4 | **Inheritance & Polymorphism**: Architected the `Entity` base class hierarchy. Developed the polymorphic subclasses for `DashBird`, `ExplosiveBird`, and `SplitBird`, dynamically overriding abilities and interactions. |
| **An Nam** | **Level Design & Systems Integrator** | Phase 4 | **Abstraction & Inheritance**: Implemented the JSON `LevelLoader`. Extended the `Entity` framework to handle dynamic instantiation and safely abstracted level parsing logic entirely away from the `GameplayState`. |
| **Trung Nguyên**| **UI/UX & State Programmer** | Phase 1 & 5 | **Polymorphism**: Implemented the various concrete game states (`MainMenuState`, `WinState`, etc.) that inherit from `IState`. Encapsulated UI components into scalable rendering classes. |
