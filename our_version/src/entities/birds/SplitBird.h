#ifndef SPLITBIRD_H
#define SPLITBIRD_H

#include "Bird.h"
#include "BasicBird.h"
#include <vector>
#include <memory>

class SplitBird : public Bird {
public:
    SplitBird(sf::Vector2f pos);
    
    void onAbility() override;
    
    bool hasSplit() const { return split_triggered; }
    
    // Returns 2 new child birds to be added to the entity list
    std::vector<std::unique_ptr<BasicBird>> spawnChildren();

private:
    bool split_triggered = false;
};

#endif // SPLITBIRD_H
