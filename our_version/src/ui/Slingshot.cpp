#include "Slingshot.h"
#include <cmath>

Slingshot::Slingshot(sf::Vector2f c, float maxRadius) 
    : center(c), maxPullRadius(maxRadius), pullPosition(c) {
    
    auto& rm = ResourceManager::getInstance();
    sprite.setTexture(rm.getTexture("slingshot"));
    
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setPosition(center.x, center.y + bounds.height/2.0f); // Center is top of slingshot
    
    bandLeft.setPrimitiveType(sf::Lines);
    bandLeft.resize(2);
    bandRight.setPrimitiveType(sf::Lines);
    bandRight.resize(2);
    
    updateBands();
}

void Slingshot::onMousePress(sf::Vector2f mousePos) {
    // Check if mouse is near the center
    sf::Vector2f diff = mousePos - center;
    if (diff.x * diff.x + diff.y * diff.y < 2500.0f) { // 50px radius
        dragging = true;
        pullPosition = mousePos;
        updateBands();
    }
}

void Slingshot::onMouseMove(sf::Vector2f mousePos) {
    if (dragging) {
        sf::Vector2f diff = mousePos - center;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        
        if (distSq > maxPullRadius * maxPullRadius) {
            float dist = std::sqrt(distSq);
            pullPosition = center + (diff / dist) * maxPullRadius;
        } else {
            pullPosition = mousePos;
        }
        updateBands();
    }
}

bool Slingshot::onMouseRelease() {
    if (dragging) {
        dragging = false;
        pullPosition = center;
        updateBands();
        return true;
    }
    return false;
}

sf::Vector2f Slingshot::getLaunchVelocityPreview() const {
    if (!dragging) return sf::Vector2f(0, 0);
    sf::Vector2f pullVector = center - pullPosition;
    return pullVector * LAUNCH_POWER_SCALE;
}

sf::Vector2f Slingshot::getLaunchVelocity() const {
    return getLaunchVelocityPreview();
}

void Slingshot::updateBands() {
    sf::Color bandColor(48, 22, 8); // Dark brown
    
    bandLeft[0].position = sf::Vector2f(center.x - 15.0f, center.y);
    bandLeft[0].color = bandColor;
    bandLeft[1].position = pullPosition;
    bandLeft[1].color = bandColor;
    
    bandRight[0].position = sf::Vector2f(center.x + 15.0f, center.y);
    bandRight[0].color = bandColor;
    bandRight[1].position = pullPosition;
    bandRight[1].color = bandColor;
}

void Slingshot::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    if (dragging) {
        window.draw(bandLeft);
        window.draw(bandRight);
    }
}
