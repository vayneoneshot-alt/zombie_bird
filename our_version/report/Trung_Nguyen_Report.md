# Detailed UI/UX & State Programming Report: Trung Nguyên

This section details the user interface architecture and state implementations engineered by **Trung Nguyên** during Phase 1 and 5 of the Zombie Bird project. The primary focus of this work was leveraging **Polymorphism** to govern the game flow and **Encapsulation** to build modular graphical elements.

## 1. Polymorphism: Concrete Game States

Building upon Trọng Nhân's `IState` interface, concrete implementations of the game screens needed to be developed. 

### Overriding the State Contract
Several concrete classes were created to manage distinct phases of the application:
- **`MainMenuState`**: Handles the title screen and entry point.
- **`LevelSelectState`**: Manages dynamic button generation to load specific JSON levels.
- **`WinState` / `LoseState`**: Handles end-game logic and transition routing.
- **Gameplay Flow Elements**: Implemented an in-game Exit button and a 3-second cinematic delay upon winning before transitioning states.

```cpp
class LevelSelectState : public IState {
public:
    void onEnter() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
};
```
**OOP Principle Application (Polymorphism):** Because all of these classes inherit from `IState` and override its pure virtual functions, the central `StateManager` can treat them identically. When the player clicks "Level 1", the transition is executed polymorphically by passing a `std::unique_ptr<GameplayState>` up to the manager.

## 2. Encapsulation: Scalable UI Components

Rendering user interface elements (like buttons, trajectory lines, and slingshots) requires careful management of vertex arrays, shapes, colors, and math.

### Encapsulating Rendering Logic
Instead of bloating the states with raw SFML rendering commands, complex UI logic was encapsulated into dedicated classes:

- **`Slingshot` Class**: Encapsulates the dragging mechanics, bounds checking (max pull distance), and the rendering of the visual rubber bands. The math to calculate launch vectors is hidden behind `getLaunchVelocity()`.
- **`TrajectoryLine` Class**: Encapsulates a mathematical projectile prediction loop. It computes the parabolic arc based on current velocity and gravity, constructing an `sf::VertexArray` internally.
- **UI Button Structs**: Encapsulates the bounding box (`sf::RectangleShape`) and text (`sf::Text`), allowing for clean bounds-checking during mouse events (e.g., Level Selection and the in-game Exit button).

**OOP Principle Application (Encapsulation):** By wrapping UI elements into self-contained classes, the `GameplayState` only needs to call `slingshot->draw(window)` and `trajectoryLine.calculate(...)`. The internal mathematics and visual state changes (like hovering or clicking) are completely hidden from the consumer.

---
### Summary of Impact
Trung Nguyên's implementation of the polymorphic game states brought the architecture to life, creating a seamless user flow. By encapsulating complex UI rendering into modular classes like the `Slingshot`, the gameplay code remained clean, highly readable, and solely focused on game logic rather than visual geometry.
