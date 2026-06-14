#ifndef EXPLOSIVEBIRD_H
#define EXPLOSIVEBIRD_H

#include "Bird.h"

class ExplosiveBird : public Bird {
public:
    ExplosiveBird(sf::Vector2f pos);
    
    void onAbility() override;
    
    bool hasExploded() const { return exploded; }
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    float getExplosionRadius() const { return 150.0f; }
    float getExplosionForce() const { return 25000.0f; }

private:
    bool exploded = false;
    sf::CircleShape explosionEffect;
    float explosionTimer = 0.0f;
};

#endif // EXPLOSIVEBIRD_H
