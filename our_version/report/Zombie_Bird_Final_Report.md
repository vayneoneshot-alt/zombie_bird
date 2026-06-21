# Zombie Bird: OOP Final Project Report

## Chapter 1: Project Introduction

### 1.1 Project Overview
"Zombie Bird" is a 2D physics-based puzzle game inspired by the popular game Angry Birds. The project focuses on building a modular C++ game architecture from scratch, featuring a custom physics engine, interactive UI, and data-driven level design. The game strictly adheres to Object-Oriented Programming (OOP) principles to cleanly separate rendering, game logic, state management, and physics simulation.

### 1.2 Objectives and Motivation
The primary objective of this project is to practically apply the four pillars of OOP (Encapsulation, Inheritance, Polymorphism, and Abstraction) in a complex software system. By building a custom physics engine and game architecture rather than relying on external libraries like Box2D or Unity, the team aimed to gain a deep understanding of software engineering patterns, collision mathematics, and game loop architecture.

### 1.3 Scope and Limitations
The scope of the project encompasses:
- A custom 2D physics engine supporting Circle and Oriented Bounding Box (OBB) collision detection using the Separating Axis Theorem (SAT).
- A robust entity hierarchy with polymorphic behaviors (e.g., different bird types).
- A state-driven application flow (Main Menu, Level Selection, Gameplay, Win/Lose states).
- Dynamic level loading using JSON files.

Limitations include a simplified physics integration model (Semi-Implicit Euler) and a finite set of entity interactions compared to a commercial engine.

### 1.4 Report Structure
This report is structured into four chapters. Chapter 2 covers the theoretical background and OOP concepts applied. Chapter 3 details the project design, system architecture, and specific implementation phases. Chapter 4 evaluates the results, discusses challenges, and outlines future improvements.

---

## Chapter 2: Theoretical Background

### 2.1 Object-Oriented Programming (OOP) Concepts

#### 2.1.1 Abstraction
Abstraction is heavily utilized to hide complex implementation details:
- **`IState` Interface**: Exposes only high-level hooks (`onEnter`, `update`, `draw`, `handleEvent`), abstracting the core loop from the specific logic of each game screen (Trọng Nhân).
- **`LevelLoader`**: Abstracted file I/O and JSON tokenization away from the gameplay logic. The game only requests a filepath and receives a clean `LevelData` struct (An Nam).

#### 2.1.2 Encapsulation
The codebase restricts direct access to internal states to ensure data integrity:
- **`PhysicsBody`**: Manages mass, inertia, and forces. External classes must use `applyForce()`, ensuring calculations remain predictable (Quang Minh).
- **`CollisionSystem`**: The complex SAT math and manifold generation are completely encapsulated within a stateless utility class, shielding the rest of the engine.
- **UI Components**: Rendering logic for the `Slingshot` and `TrajectoryLine` (calculating the parabolic arc) is hidden inside their respective classes, keeping `GameplayState` clean (Trung Nguyên).

#### 2.1.3 Inheritance
Inheritance defines strict "is-a" relationships and drastically reduces code duplication:
- **`Entity` Hierarchy**: `Entity` serves as the abstract base class linking a `PhysicsBody` to an `sf::Sprite`. `Block`, `Pig`, and `Bird` all inherit from `Entity` (Tuấn Nguyên).
- **Subclassing**: `Bird` is further specialized into `BasicBird`, `DashBird`, `ExplosiveBird`, and `SplitBird`, sharing standard flight mechanics but introducing unique features.

#### 2.1.4 Polymorphism
Polymorphism allows dynamic behavior resolution at runtime:
- **Game States**: The `StateManager` holds a stack of `std::unique_ptr<IState>`, dispatching `update()` and `draw()` dynamically to `MainMenuState`, `GameplayState`, etc. (Trung Nguyên).
- **Dynamic Abilities**: The input handler simply calls the virtual `onAbility()` method on a `Bird*`. The C++ runtime dynamically executes the specific behavior (Dashing, Exploding, Splitting) based on the actual subclass (Tuấn Nguyên).

### 2.2 Software Development Methodology
The project was developed in distinct iterative phases (Core Engine -> Physics -> Entities -> Gameplay/Levels -> UI/Polish), with tasks divided among the five team members based on their specialized domains, ensuring modular development and integration.

### 2.3 Technologies and Tools Used
- **Programming Language**: C++ (chosen for performance and explicit memory management).
- **Libraries**:
  - **SFML**: Simple and Fast Multimedia Library used for window management, 2D graphics rendering, and event handling.
  - **nlohmann/json**: A modern C++ library used for parsing level data.
- **Build System**: CMake.

---

## Chapter 3: Project Design and Implementation

### 3.1 Requirements Analysis
- **Functional Requirements**: The system must allow users to launch birds from a slingshot, apply physics (gravity, collision, impulses) to interactive entities, track win/lose conditions, and transition between menus and levels.
- **Non-Functional Requirements**: The physics simulation must run smoothly without frame drops. Collision detection must be accurate to prevent clipping.

### 3.2 System Architecture
The application uses a **State Pattern** driven by a `StateManager` interacting with an abstract `IState` interface. This seamlessly manages transitions. Underneath, it utilizes an **Entity Hierarchy** that acts similarly to a component model, binding visual sprites to a custom `PhysicsWorld` and `CollisionSystem`.

### 3.3 Class Design and Data Structures
- **`Entity`**: The foundational class wrapping `PhysicsBody` and `sf::Sprite`.
- **`PhysicsBody`**: Contains critical state data (velocity, mass, rotation) but is decoupled from game logic, holding only a `void* userData` pointer back to its owner.

### 3.4 Implementation Details

#### 3.4.1 Core Engine and Physics System (Phase 1 & 2)
Trọng Nhân built the baseline architecture (`StateManager`, `PhysicsWorld` integration loop). Quang Minh implemented the custom physics engine. The `CollisionSystem` handles Circle-vs-Circle and Circle-vs-OBB detection, utilizing the Separating Axis Theorem (SAT) for OBB collisions and impulse-based resolution with friction and bounciness.

#### 3.4.2 Entity and Gameplay Logic (Phase 3)
Tuấn Nguyên engineered the polymorphic entity framework. By overriding `onAbility()`, specialized behaviors like the `DashBird` (instant velocity boost) and `ExplosiveBird` (delayed area-of-effect impulse) were cleanly integrated without cluttering the main loop with type-checking.

#### 3.4.3 Level Integration and Dynamic Instantiation (Phase 4)
An Nam developed the data-driven level system. The JSON parser maps string identifiers (e.g., "dash") to dynamic instantiations of the polymorphic subclasses (e.g., `std::make_unique<DashBird>()`), pushing them into a generic container for seamless gameplay loop consumption.

#### 3.4.4 UI/UX and State Implementations (Phase 5)
Trung Nguyên encapsulated complex UI rendering into classes like `Slingshot` and `TrajectoryLine`. Concrete game states (`LevelSelectState`, `WinState`) were implemented to polish the user experience, including dynamic button generation and a 3-second cinematic delay upon winning.

### 3.5 Testing and Quality Assurance
Testing largely focused on unit-level validation of the physics engine. A major challenge was ensuring the `PhysicsBody` state protections prevented invalid modifications. The SAT implementation required rigorous mathematical verification to ensure collision normals and penetration depths were computed accurately for arbitrary shapes.

---

## Chapter 4: Results and Conclusion

### 4.1 Results and Evaluation
The completed "Zombie Bird" project successfully demonstrates a fully functional, physics-driven puzzle game built from scratch. The custom physics engine handles rigid body dynamics effectively, and the polymorphic design allows for a highly extensible system where new bird types or level components can be added with minimal friction.

### 4.2 Project Limitations and Challenges
- **Physics Stability**: Implementing a stable custom physics engine was mathematically challenging. Initial issues with object jittering required tuning of physics parameters (gravity scale, block mass) and pre-settling logic.
- **Complex SAT Math**: Managing the manifold generation for OBB collisions was highly intensive and required strict encapsulation to prevent errors from bleeding into gameplay logic.

### 4.3 Future Improvements
- **Physics Engine Enhancement**: The collision detection algorithm could be swapped or upgraded to use GJK (Gilbert-Johnson-Keerthi) for better performance and support for arbitrary convex polygons.
- **Feature Expansion**: Adding more complex entities, varied bird abilities, and introducing particle effects to enhance visual feedback.

### 4.4 Conclusion
The "Zombie Bird" project stands as a successful exercise in advanced Object-Oriented Programming and software architecture. Through the collaborative efforts of Trọng Nhân (Lead Architect), Quang Minh (Physics), Tuấn Nguyên (Entities), An Nam (Level Design), and Trung Nguyên (UI/UX), the team built a robust, scalable game engine. By strictly adhering to encapsulation, inheritance, polymorphism, and abstraction, the resulting codebase is maintainable, highly modular, and serves as a strong foundation for future software engineering endeavors.
