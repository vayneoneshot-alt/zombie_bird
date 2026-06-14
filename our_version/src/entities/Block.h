#ifndef BLOCK_H
#define BLOCK_H

#include "../Entity.h"
#include "../../core/ResourceManager.h"

class Block : public Entity {
public:
    enum class Material { WOOD };
    
    Block(sf::Vector2f pos, sf::Vector2f size, Material mat = Material::WOOD);
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    void receiveDamage(float impactForce);
    bool isDestroyed() const { return health <= 0.0f; }
    sf::Vector2f getHalfSize() const { return halfSize; }

private:
    Material material;
    float maxHealth;
    float health;
    int crackStage = 0; // 0 = intact, 1 = cracked
    sf::Vector2f halfSize;
    
    void updateTexture();
};

#endif // BLOCK_H
