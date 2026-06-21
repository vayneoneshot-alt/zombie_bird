# Final Report: Zombie Bird

## CHAPTER 1
## PROJECT OVERVIEW

### 1.1 Introduction
"Zombie Bird" is a 2D physics-based puzzle game inspired by the mechanics of the popular game Angry Birds. The project focuses on building a modular C++ game architecture from scratch, featuring a custom physics engine, an interactive user interface, and data-driven level design. The game is developed strictly adhering to Object-Oriented Programming (OOP) principles to cleanly separate rendering, game logic, state management, and physics simulation.

### 1.2 Objectives
The primary objective of this project is to practically apply the four pillars of OOP—Abstraction, Encapsulation, Inheritance, and Polymorphism—in the development of a complex software system. By choosing to build a custom physics engine and game architecture rather than relying on commercial or external libraries like Box2D or Unity, the team aimed to gain a profound, hands-on understanding of software engineering patterns, collision mathematics, and game loop architecture.

### 1.3 Scope and Limitations
**Scope:**
- Development of a custom 2D physics engine supporting Circle and Oriented Bounding Box (OBB) collision detection using the Separating Axis Theorem (SAT).
- A robust entity hierarchy with polymorphic behaviors, allowing for multiple bird types with unique abilities.
- A state-driven application flow encompassing Main Menu, Level Selection, Gameplay, Win, and Lose states.
- Dynamic level loading system utilizing JSON files for data-driven game design.

**Limitations:**
- The physics engine uses a simplified integration model (Semi-Implicit Euler), which may result in minor instabilities compared to more advanced integrators.
- The system supports a finite set of entity interactions and shapes compared to a fully-featured commercial physics engine.
- Collision resolution does not handle stacking complex polygons, limiting the physical interactions to circles and rectangular blocks.

---

## CHAPTER 2
## THEORETICAL BACKGROUND

### 2.1 Applied Object-Oriented Programming (OOP) Concepts
The "Zombie Bird" codebase actively utilizes the four core concepts of OOP to ensure a maintainable and scalable architecture.

#### 2.1.1 Abstraction
Abstraction is heavily utilized to hide complex implementation details behind simple, high-level interfaces:
- **`IState` Interface**: Exposes only high-level hooks (`onEnter`, `update`, `draw`, `handleEvent`). The core application loop does not need to know how a specific screen works; it simply updates and renders the current state.
- **JSON `LevelLoader`**: This static utility class abstracts file I/O and JSON tokenization away from the gameplay logic. The `GameplayState` only asks for a filepath and receives a clean, strongly-typed `LevelData` struct in return.
- **`CollisionSystem`**: The complex mathematics of the Separating Axis Theorem (SAT) and manifold generation are completely abstracted away from the gameplay logic and entities.

#### 2.1.2 Encapsulation
The codebase restricts direct access to internal object states, protecting them from invalid modifications and ensuring data integrity:
- **`PhysicsBody`**: Manages physical properties like mass, inertia, and forces. External classes must interact via controlled methods like `applyForce()` or `setMass()`, rather than manipulating vectors directly. Inverse mass and inertia are automatically recalculated inside these methods.
- **Game Entities**: Classes like `Pig` and `Block` encapsulate their health and destruction logic. Damage is applied via specific interaction methods based on collision impulses, hiding the health state.
- **UI Components**: Rendering logic for the `Slingshot` (dragging mechanics, launch vector calculation) and `TrajectoryLine` (calculating the parabolic arc) is hidden inside their respective classes, keeping the main `GameplayState` exceptionally clean.

#### 2.1.3 Inheritance
Inheritance defines strict "is-a" relationships to share common functionality and significantly reduce code duplication:
- **`Entity` Hierarchy**: To avoid code duplication across different physical objects, an abstract `Entity` base class was created to link a `PhysicsBody` with an `sf::Sprite`. `Block`, `Pig`, and `Bird` all inherit from `Entity`, ensuring the main loop can blindly iterate over them.
- **Bird Subclassing**: The `Bird` class acts as a base for multiple specialized derived classes, such as `BasicBird`, `DashBird`, `ExplosiveBird`, and `SplitBird`. Each inherits standard flight mechanics but introduces unique extended functionality.

#### 2.1.4 Polymorphism
Polymorphism allows the game engine to treat different objects uniformly through base class pointers, enabling dynamic behavior resolution at runtime:
- **Runtime State Resolution**: The `StateManager` holds a stack of `std::unique_ptr<IState>`. When `update(dt)` is called, C++ dynamically dispatches the call to the active state, such as `MainMenuState` or `GameplayState`.
- **Dynamic Abilities**: The input handler in `GameplayState` simply calls a virtual `onAbility()` method on a base `Bird` pointer. Through dynamic dispatch, the C++ runtime resolves the correct specific ability to execute (e.g., dashing, exploding, or splitting). This allows new bird types to be added seamlessly.
- **Dynamic Instantiation**: The JSON parser maps string identifiers to dynamic instantiations of the polymorphic subclasses (e.g., `std::make_unique<DashBird>()`), pushing them into a generic container for the gameplay loop.

### 2.2 Software Development Methodology
The project was developed using an iterative, modular approach. The workload was divided into distinct phases: Core Engine setup, Custom Physics Development, OOP Entity Modeling, Gameplay/Level Integration, and finally UI/Polish. Tasks were allocated among the team members based on specialized domains—ranging from architecture and physics to UI and level design—ensuring that all members actively applied OOP principles within their respective areas of focus.

### 2.3 Technologies and Tools Used
- **Programming Language**: C++ (chosen for high performance, explicit memory management, and robust OOP support).
- **Graphics & Window Management**: **SFML** (Simple and Fast Multimedia Library), used for window creation, 2D graphics rendering, and input event handling.
- **Data Parsing**: **nlohmann/json**, a modern C++ library utilized for parsing level data from JSON files.
- **Build System**: **CMake**, ensuring cross-platform compilation and dependency management.

---

## CHAPTER 3
## SYSTEM DESIGN AND IMPLEMENTATION

### 3.1 Requirements Analysis
- **Functional Requirements**: The system must allow users to launch birds using a drag-and-release slingshot mechanic, apply physics (gravity, collision, impulses) to interactive entities, track win/lose conditions, and transition smoothly between menus and levels.
- **Non-Functional Requirements**: The physics simulation must run at a consistent framerate to ensure smooth gameplay. Collision detection algorithms must be highly accurate to prevent object clipping or tunnel-through at high velocities.

### 3.2 System Architecture
The application is governed by a **State Pattern** driven by a `StateManager` interacting with an abstract `IState` interface, cleanly managing transitions between the application's different screens. Underneath, the gameplay relies on a robust **Entity Hierarchy** that acts similarly to a component model, structurally binding visual assets (`sf::Sprite`) to a custom `PhysicsWorld` and `CollisionSystem`.

### 3.3 Class Design and Data Structures
- **`Entity`**: The foundational abstract class that tightly binds a `PhysicsBody` instance to an `sf::Sprite`. It defines virtual `update()` and `draw()` methods for subclasses.
- **`PhysicsBody`**: A struct containing critical physical state data (velocity, mass, rotation, inertia). It is decoupled from game logic, holding only a `void* userData` pointer back to its owner entity to facilitate collision callbacks.
- **Data Structures**: `std::vector` and `std::unique_ptr` are used extensively to manage dynamic collections of game objects and states. This ensures automatic memory management and safe polymorphic behavior when iterating over abstract base class pointers.

### 3.4 Implementation Details

#### 3.4.1 Engine and Physics System Development
Trọng Nhân built the baseline architecture, establishing the `StateManager` and the core `PhysicsWorld` integration loop. Quang Minh engineered the custom physics engine. The `CollisionSystem` handles Circle-vs-Circle and Circle-vs-OBB detection, utilizing the Separating Axis Theorem (SAT) for OBB collisions. It resolves collisions by calculating penetration depths and applying impulse-based resolution with configurable friction and bounciness parameters.

#### 3.4.2 Entity and Gameplay Development
Tuấn Nguyên engineered the polymorphic entity framework. By overriding the `onAbility()` method, specialized behaviors like the `DashBird` (instant velocity boost) and `ExplosiveBird` (delayed area-of-effect impulse) were cleanly integrated. The core gameplay loop iterates over abstract `Entity` pointers, updating and drawing them without the need for complex type-checking.

#### 3.4.3 Dynamic Level System
An Nam developed the data-driven level system. A JSON parser (`nlohmann/json`) reads `.json` files to map string identifiers (e.g., "dash") to dynamic instantiations of the polymorphic subclasses (e.g., `std::make_unique<DashBird>()`). These objects are safely pushed into generic containers, abstracting level design away from the C++ source code.

#### 3.4.4 User Interface and State Management
Trung Nguyên encapsulated complex UI rendering into dedicated classes like `Slingshot` and `TrajectoryLine`. Concrete game states, including `LevelSelectState` and `WinState`, were implemented to polish the user experience. Features such as dynamic button generation and a cinematic delay upon winning ensure a smooth application flow.

### 3.5 Testing and Quality Assurance
Testing largely focused on unit-level validation of the physics engine and collision responses. A major challenge was ensuring the `PhysicsBody` state protections successfully prevented invalid modifications from external logic. Furthermore, the SAT implementation required rigorous mathematical verification to guarantee that collision normals and penetration depths were computed accurately under all edge cases.

---

## CHAPTER 4
## RESULTS AND CONCLUSION

### 4.1 Achievements and Results
The completed "Zombie Bird" project successfully demonstrates a fully functional, physics-driven puzzle game built entirely from scratch. The custom physics engine robustly handles rigid body dynamics, while the polymorphic design allows for a highly extensible system where new bird types and gameplay mechanics were integrated seamlessly. The project stands as a practical testament to the team's ability to apply advanced OOP design patterns to create a scalable, modular game architecture.

### 4.2 Challenges and Limitations
- **Physics Stability**: Ensuring the stability of the custom physics engine was mathematically challenging. Initial issues with object jittering required careful tuning of physics parameters (e.g., gravity scale, block mass) and the implementation of pre-settling logic.
- **Complex SAT Mathematics**: Managing the manifold generation for OBB collisions was computationally intensive and prone to edge-case bugs. It required strict encapsulation to prevent errors from bleeding into general gameplay logic.
- **Current Limitations**: The engine relies on Semi-Implicit Euler integration, which can occasionally introduce instability under extreme impulses. Furthermore, the game is currently limited to convex circle and rectangular shapes.

### 4.3 Future Development
- **Physics Engine Enhancements**: Upgrading the collision detection algorithm from SAT to GJK (Gilbert-Johnson-Keerthi) to improve performance and expand support to arbitrary convex polygons.
- **Gameplay Expansion**: Adding more complex interactive entities, new varied bird abilities, and introducing particle effect systems to enhance visual feedback.
- **Level Editor**: Building an in-game level editor to visually generate the `.json` level files, allowing for rapid level iteration without manual JSON manipulation.

### 4.4 Conclusion
The "Zombie Bird" project was a highly successful exercise in advanced Object-Oriented Programming and software architecture. Through the collaborative efforts of Trọng Nhân (Lead Architect), Quang Minh (Physics), Tuấn Nguyên (Entities), An Nam (Level Design), and Trung Nguyên (UI/UX), the team built a robust, data-driven game engine. By strictly adhering to the core pillars of encapsulation, inheritance, polymorphism, and abstraction, the resulting codebase is maintainable, highly modular, and serves as an excellent foundation for future software engineering endeavors.
