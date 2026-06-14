#ifndef PIG_H
#define PIG_H

#include "../Entity.h"
#include "../../core/ResourceManager.h"

class Pig : public Entity {
public:
    Pig(sf::Vector2f pos);
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    void receiveDamage(float amount);
    bool isDead() const { return health <= 0.0f; }
    float getRadius() const { return radius; }

private:
    float health = 50.0f;
    float radius;
};

#endif // PIG_H
