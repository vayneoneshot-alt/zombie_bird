#ifndef SLINGSHOT_H
#define SLINGSHOT_H

#include <SFML/Graphics.hpp>
#include "../../core/ResourceManager.h"

class Slingshot {
public:
    Slingshot(sf::Vector2f center, float maxPullRadius);

    void onMousePress(sf::Vector2f mousePos);
    void onMouseMove(sf::Vector2f mousePos);
    bool onMouseRelease(); // Returns true if released while dragging
    
    sf::Vector2f getLaunchVelocity() const;
    sf::Vector2f getLaunchVelocityPreview() const;
    
    bool isDragging() const { return dragging; }
    sf::Vector2f getPullPosition() const { return pullPosition; }

    void draw(sf::RenderWindow& window);

private:
    sf::Vector2f center;
    float maxPullRadius;
    sf::Vector2f pullPosition;
    bool dragging = false;

    sf::Sprite sprite;
    sf::VertexArray bandLeft;
    sf::VertexArray bandRight;

    static constexpr float LAUNCH_POWER_SCALE = 12.0f;
    
    void updateBands();
};

#endif // SLINGSHOT_H
