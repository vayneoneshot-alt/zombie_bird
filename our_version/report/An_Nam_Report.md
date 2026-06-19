# Detailed Level Design & Integration Report: An Nam

This section details the level loading architecture and dynamic instantiation systems engineered by **An Nam** during Phase 4 of the Zombie Bird project. The primary focus of this work was **Abstraction** and **Inheritance** integration, bridging data-driven design with the C++ runtime.

## 1. Abstraction: The JSON `LevelLoader`

Hardcoding levels in C++ is inflexible and scales poorly. To solve this, a data-driven approach was implemented using JSON files, parsed via the `nlohmann/json` library.

### Abstracting Level Data
The `LevelLoader` is a static utility class designed to completely abstract the file I/O and parsing logic away from the main game loop.

```cpp
class LevelLoader {
public:
    static LevelData load(const std::string& filepath);
};
```
**OOP Principle Application (Abstraction):** The `GameplayState` only asks the `LevelLoader` to load a file path and receives a clean, strongly-typed `LevelData` struct in return. All the complexities of file streams, JSON tokenization, error handling, and array parsing are entirely abstracted away. 

## 2. Extending the Framework via Dynamic Instantiation

The parsed data must be mapped to the `Entity` framework. The level integration logic uses the JSON strings to dynamically instantiate polymorphic subclasses at runtime.

### Handling Dynamic Instantiation
When the `GameplayState` reads the array of bird strings (e.g., `["basic", "dash", "explosive"]`), it relies on the inheritance hierarchy to securely create objects:

```cpp
// Dynamic instantiation mapping string identifiers to polymorphic classes
if (birdType == "dash") {
    birdQueue.push_back(std::make_unique<DashBird>(pos));
} else if (birdType == "explosive") {
    birdQueue.push_back(std::make_unique<ExplosiveBird>(pos));
} else if (birdType == "split") {
    birdQueue.push_back(std::make_unique<SplitBird>(pos));
} else {
    birdQueue.push_back(std::make_unique<BasicBird>(pos));
}
```
**OOP Principle Application (Inheritance & Integration):** By utilizing the `std::unique_ptr<Bird>` queue, the level loading system cleanly leverages Tuấn Nguyên's inheritance framework. The data specifies the *type*, and the loader instantiates the *subclass*, pushing it into a generic container for the gameplay loop to consume seamlessly.

---
### Summary of Impact
An Nam's data-driven `LevelLoader` abstracted the tedious setup logic out of the game states. By bridging JSON data with the polymorphic entity framework, the project allowed level designers to create and balance entirely new levels without touching or recompiling a single line of C++ code.
