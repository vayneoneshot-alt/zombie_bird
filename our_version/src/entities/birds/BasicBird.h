#ifndef BASICBIRD_H
#define BASICBIRD_H

#include "Bird.h"

class BasicBird : public Bird {
public:
    BasicBird(sf::Vector2f pos);
    
    void onAbility() override; // Basic bird has no special ability
};

#endif // BASICBIRD_H
