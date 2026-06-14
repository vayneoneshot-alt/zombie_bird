#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    sf::Texture& getTexture(const std::string& id);
    sf::Font& getFont(const std::string& id);

    void loadTexture(const std::string& id, const std::string& filepath);
    void loadFont(const std::string& id, const std::string& filepath);
    void preloadAll();

private:
    ResourceManager() = default;
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
};

#endif // RESOURCEMANAGER_H
