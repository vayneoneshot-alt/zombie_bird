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
    loadTexture("bg_lvl1", "Data/object/level1.png");
    loadTexture("bg_lvl2", "Data/object/level2.png");
    loadTexture("bg_lvl3", "Data/object/level3.png");
    
    // Birds
    loadTexture("bird_basic", "Data/object/red.png");
    loadTexture("bird_dash", "Data/object/yellow.png");
    loadTexture("bird_explosive", "Data/object/black.png");
    loadTexture("bird_split", "Data/object/blue.png");
    
    // Pig
    loadTexture("pig", "Data/object/pig.png");
    
    // UI Level Buttons
    loadTexture("btn_level1", "Data/object/level1.png");
    loadTexture("btn_level2", "Data/object/level2.png");
    loadTexture("btn_level3", "Data/object/level3.png");
    
    // Slingshot
    loadTexture("slingshot", "Data/Images/slingshot.png");
    
    // Blocks
    loadTexture("block_wood_intact", "Data/Images/kenney_physicspack/PNG/Wood elements/elementWood047.png");
    loadTexture("block_wood_cracked", "Data/Images/kenney_physicspack/PNG/Wood elements/elementWood049.png");
}
