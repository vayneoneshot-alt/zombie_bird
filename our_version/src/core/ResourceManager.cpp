#include "ResourceManager.h"

sf::Texture& ResourceManager::getTexture(const std::string& id) {
    return textures.at(id);
}

sf::Font& ResourceManager::getFont(const std::string& id) {
    return fonts.at(id);
}

void ResourceManager::loadTexture(const std::string& id, const std::string& filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        std::cerr << "Failed to load texture: " << filepath << "\n";
    } else {
        textures[id] = std::move(texture);
    }
}

void ResourceManager::loadFont(const std::string& id, const std::string& filepath) {
    sf::Font font;
    if (!font.loadFromFile(filepath)) {
        std::cerr << "Failed to load font: " << filepath << "\n";
    } else {
        fonts[id] = std::move(font);
    }
}

void ResourceManager::preloadAll() {
    loadFont("main_font", "Data/Fonts/OpenSans-Bold.ttf");
    
    // Backgrounds
    loadTexture("bg_menu", "Data/Images/menu.jpg");
    loadTexture("bg_lvl1", "Data/Images/lvl1.png");
    loadTexture("bg_lvl2", "Data/Images/lvl2.png");
    loadTexture("bg_lvl3", "Data/Images/lvl3.png");
    
    // Birds
    loadTexture("bird_basic", "Data/Images/kenney_animalpackredux/PNG/Round (outline)/parrot.png");
    loadTexture("bird_dash", "Data/Images/kenney_animalpackredux/PNG/Round (outline)/duck.png");
    loadTexture("bird_explosive", "Data/Images/kenney_animalpackredux/PNG/Round (outline)/chick.png");
    loadTexture("bird_split", "Data/Images/kenney_animalpackredux/PNG/Round (outline)/owl.png");
    
    // Pig
    loadTexture("pig", "Data/Images/kenney_animalpackredux/PNG/Round (outline)/pig.png");
    
    // Slingshot
    loadTexture("slingshot", "Data/Images/slingshot.png");
    
    // Blocks
    loadTexture("block_wood_intact", "Data/Images/kenney_physicspack/PNG/Wood elements/elementWood047.png");
    loadTexture("block_wood_cracked", "Data/Images/kenney_physicspack/PNG/Wood elements/elementWood049.png");
}
