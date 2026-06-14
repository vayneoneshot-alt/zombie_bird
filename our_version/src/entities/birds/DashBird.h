#ifndef DASHBIRD_H
#define DASHBIRD_H

#include "Bird.h"

class DashBird : public Bird {
public:
    DashBird(sf::Vector2f pos);
    
    void onAbility() override;
};

#endif // DASHBIRD_H
