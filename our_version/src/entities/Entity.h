#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "../core/PhysicsBody.h"
#include <memory>

class Entity {
public:
    virtual ~Entity() = default;
    
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
    
    bool isActive() const { return is_active; }
    void setActive(bool active) { is_active = active; }
    
    PhysicsBody& getBody() { return body; }
    const PhysicsBody& getBody() const { return body; }

protected:
    sf::Sprite sprite;
    PhysicsBody body;
    bool is_active = true;
};

#endif // ENTITY_H
