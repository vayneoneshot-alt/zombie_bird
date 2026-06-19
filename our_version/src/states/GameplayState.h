#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include "IState.h"
#include "StateManager.h"
#include "../entities/birds/Bird.h"
#include "../entities/birds/BasicBird.h"
#include "../entities/birds/DashBird.h"
#include "../entities/birds/ExplosiveBird.h"
#include "../entities/birds/SplitBird.h"
#include "../entities/Pig.h"
#include "../entities/Block.h"
#include "../ui/Slingshot.h"
#include "../ui/TrajectoryLine.h"
#include "../level/LevelLoader.h"
#include "../core/PhysicsWorld.h"
#include <vector>
#include <memory>
#include <deque>

class GameplayState : public IState {
public:
    GameplayState(StateManager& sm, sf::RenderWindow& window, const std::string& levelFile);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    StateManager& stateManager;
    sf::RenderWindow& window;
    std::string levelFilePath;
    LevelData levelData;

    sf::Sprite background;
    
    std::deque<std::unique_ptr<Bird>> birdQueue;
    std::vector<std::unique_ptr<Bird>> activeBirds; // Launched birds
    std::vector<std::unique_ptr<Pig>> pigs;
    std::vector<std::unique_ptr<Block>> blocks;
    
    std::unique_ptr<Slingshot> slingshot;
    TrajectoryLine trajectoryLine;
    
    PhysicsWorld physicsWorld;
    PhysicsBody groundBody;
    
    float physicsActiveTimer = 0.0f;
    
    void checkCollisions();
    void resolveExplosions(const ExplosiveBird* source);
    bool checkWinLoss();
    void loadLevel();
};

#endif // GAMEPLAYSTATE_H
