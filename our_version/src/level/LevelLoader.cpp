#include "LevelLoader.h"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

LevelData LevelLoader::load(const std::string& filepath) {
    LevelData data;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filepath << "\n";
        return data;
    }
    
    json j;
    file >> j;
    
    data.name = j.value("name", "Unknown Level");
    data.background = j.value("background", "lvl1");
    
    if (j.contains("birds")) {
        for (const auto& bird : j["birds"]) {
            data.birds.push_back(bird.get<std::string>());
        }
    }
    
    if (j.contains("pigs")) {
        for (const auto& pig : j["pigs"]) {
            data.pigs.push_back({pig["x"].get<float>(), pig["y"].get<float>()});
        }
    }
    
    if (j.contains("blocks")) {
        for (const auto& block : j["blocks"]) {
            data.blocks.push_back({
                block.value("type", "wood"),
                block["x"].get<float>(),
                block["y"].get<float>(),
                block["w"].get<float>(),
                block["h"].get<float>()
            });
        }
    }
    
    return data;
}
