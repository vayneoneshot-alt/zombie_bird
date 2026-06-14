#ifndef BIRD_H
#define BIRD_H

#include "../Entity.h"
#include "../../core/ResourceManager.h"

class Bird : public Entity {
public:
    Bird(sf::Vector2f pos, float radius);
    virtual ~Bird() = default;

    void launch(sf::Vector2f velocity);
    virtual void onAbility() = 0; // Pure virtual

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    bool isLaunched() const { return launched; }
    float getRadius() const { return radius; }

protected:
    bool launched = false;
    bool abilityUsed = false;
    float radius;
    float lifetime = 0.0f; // Track how long the bird has been flying
};

#endif // BIRD_H
