#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <string>
#include <vector>

struct LevelData {
    std::string name;
    std::string background;
    std::vector<std::string> birds;
    
    struct PigData {
        float x, y;
    };
    std::vector<PigData> pigs;
    
    struct BlockData {
        std::string type;
        float x, y, w, h;
    };
    std::vector<BlockData> blocks;
};

class LevelLoader {
public:
    static LevelData load(const std::string& filepath);
};

#endif // LEVELLOADER_H
