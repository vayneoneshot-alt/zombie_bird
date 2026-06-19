# Detailed Gameplay Entities Report: Tuấn Nguyên

This section details the architectural design of the game objects engineered by **Tuấn Nguyên** during Phase 3 and 4 of the Zombie Bird project. The primary focus of this work was **Inheritance** and **Polymorphism** to create a robust, extensible hierarchy of interactive entities.

## 1. Inheritance: The `Entity` Base Class Hierarchy

To avoid code duplication across different physical objects in the game (birds, pigs, blocks), a unified base class was architected.

### The `Entity` Interface
The `Entity` class acts as an abstract base class that encapsulates the shared relationship between a `PhysicsBody` and a graphical `sf::Sprite`.

```cpp
class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    
    PhysicsBody& getBody() { return body; }
    sf::Sprite& getSprite() { return sprite; }

protected:
    PhysicsBody body;
    sf::Sprite sprite;
};
```
**OOP Principle Application (Inheritance):** All interactive objects (`Bird`, `Pig`, `Block`) inherit from `Entity`. They inherit the base physics body and sprite, ensuring that the main loop can blindly iterate over their respective collections and call `update()` or `draw()` leveraging the base class contract.
## 2. Polymorphism: The `Bird` Subclasses

The true power of the object-oriented design is showcased in the polymorphic design of the bird abilities.

### Dynamic Ability Overriding
A base `Bird` class inherits from `Entity` and defines a virtual `onAbility()` method. From this, several specialized subclasses were derived:

- **`BasicBird`**: The standard bird with no special ability.
- **`DashBird`**: Overrides `onAbility()` to apply an instant horizontal velocity boost, neutralizing gravity temporarily.
- **`ExplosiveBird`**: Contains a delayed fuse logic, overriding its interaction logic to trigger a massive area-of-effect impulse force on surrounding blocks and pigs.
- **`SplitBird`**: Overrides `onAbility()` to dynamically instantiate and spawn new `Bird` instances (children) mid-flight with offset velocity vectors.

```cpp
// In GameplayState.cpp, polymorphism in action:
for (auto it = activeBirds.rbegin(); it != activeBirds.rend(); ++it) {
    if ((*it)->isActive()) {
        (*it)->onAbility(); // Dynamically calls Dash, Explode, or Split!
        break;
    }
}
```

**OOP Principle Application (Polymorphism):** The input handler in `GameplayState` simply calls `onAbility()` on a base `Bird` pointer. Through dynamic dispatch (vtable), the C++ runtime resolves the correct specific ability to execute. This makes adding new bird types incredibly simple—just create a new subclass and override the method.

---
### Summary of Impact
Tuấn Nguyên's intelligent use of inheritance and polymorphism resulted in a highly extensible entity framework. This allowed for the rapid development of diverse gameplay mechanics (multiple bird types) without cluttering the main game loop with complex `switch` or `if/else` type-checking logic.
