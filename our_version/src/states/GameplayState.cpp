#include "GameplayState.h"
#include "WinState.h"
#include "LoseState.h"
#include "../core/CollisionSystem.h"
#include <iostream>
#include <cmath>

GameplayState::GameplayState(StateManager& sm, sf::RenderWindow& win, const std::string& levelFile)
    : stateManager(sm), window(win), levelFilePath(levelFile) {}

void GameplayState::onEnter() {
    auto& rm = ResourceManager::getInstance();
    
    // Default background
    background.setTexture(rm.getTexture("bg_lvl1"));
    sf::Vector2u winSize = window.getSize();
    
    slingshot = std::make_unique<Slingshot>(sf::Vector2f(250.0f, winSize.y - 200.0f), 100.0f);
    
    scoreText.setFont(rm.getFont("main_font"));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.0f, 20.0f);
    scoreText.setString("Score: 0");
    
    groundBody.position = {window.getSize().x / 2.0f, window.getSize().y - 30.0f + 100.0f}; // groundY is win.y - 30. halfHeight is 100.
    groundBody.setStatic(true);
    groundBody.friction = 0.96f;
    groundBody.restitution = 0.2f;
    groundBody.userData = nullptr;
    physicsWorld.addBody(&groundBody);

    loadLevel();
}

void GameplayState::loadLevel() {
    levelData = LevelLoader::load(levelFilePath);
    
    auto& rm = ResourceManager::getInstance();
    if (levelData.background == "lvl2") background.setTexture(rm.getTexture("bg_lvl2"));
    else if (levelData.background == "lvl3") background.setTexture(rm.getTexture("bg_lvl3"));
    
    sf::Vector2u texSize = background.getTexture()->getSize();
    sf::Vector2u winSize = window.getSize();
    background.setScale((float)winSize.x / texSize.x, (float)winSize.y / texSize.y);

    // Load Birds — queue birds sit at the bottom-left waiting area
    int queueIndex = 0;
    for (const auto& birdType : levelData.birds) {
        // First bird goes to slingshot; rest sit on the ground waiting
        sf::Vector2f pos;
        if (queueIndex == 0) {
            pos = slingshot->getPullPosition();
        } else {
            pos = sf::Vector2f(60.0f + (queueIndex - 1) * 55.0f, winSize.y - 60.0f);
        }
        if (birdType == "dash") {
            birdQueue.push_back(std::make_unique<DashBird>(pos));
        } else if (birdType == "explosive") {
            birdQueue.push_back(std::make_unique<ExplosiveBird>(pos));
        } else if (birdType == "split") {
            birdQueue.push_back(std::make_unique<SplitBird>(pos));
        } else {
            birdQueue.push_back(std::make_unique<BasicBird>(pos));
        }
        queueIndex++;
    }
    // Sync first bird sprite to slingshot position (body is already set above)
    if (!birdQueue.empty()) {
        birdQueue.front()->getBody().position = slingshot->getPullPosition();
        birdQueue.front()->getSprite().setPosition(slingshot->getPullPosition());
    }
    
    // Load Pigs
    for (const auto& pigData : levelData.pigs) {
        auto pig = std::make_unique<Pig>(sf::Vector2f(pigData.x, pigData.y));
        pig->getBody().userData = pig.get();
        physicsWorld.addBody(&pig->getBody());
        pigs.push_back(std::move(pig));
    }
    
    // Load Blocks
    for (const auto& blockData : levelData.blocks) {
        auto block = std::make_unique<Block>(
            sf::Vector2f(blockData.x, blockData.y),
            sf::Vector2f(blockData.w, blockData.h),
            Block::Material::WOOD
        );
        block->getBody().userData = block.get();
        physicsWorld.addBody(&block->getBody());
        blocks.push_back(std::move(block));
    }
}

void GameplayState::onExit() {
}

void GameplayState::handleEvent(const sf::Event& e) {
    bool hasActiveFlying = false;
    for (const auto& b : activeBirds) {
        if (b->isActive()) hasActiveFlying = true;
    }

    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(e.mouseButton.x, e.mouseButton.y);
        
        // Use bird ability if we click and a bird is already flying
        if (hasActiveFlying) {
            for (auto it = activeBirds.rbegin(); it != activeBirds.rend(); ++it) {
                if ((*it)->isActive()) {
                    (*it)->onAbility();
                    break;
                }
            }
        } else if (!birdQueue.empty()) {
            slingshot->onMousePress(mousePos);
        }
    }
    else if (e.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(e.mouseMove.x, e.mouseMove.y);
        slingshot->onMouseMove(mousePos);
    }
    else if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (slingshot->isDragging() && !birdQueue.empty()) {
            sf::Vector2f launchVel = slingshot->getLaunchVelocity();
            slingshot->onMouseRelease();
            
            auto bird = std::move(birdQueue.front());
            birdQueue.pop_front();
            
            bird->launch(launchVel);
            bird->getBody().userData = bird.get();
            physicsWorld.addBody(&bird->getBody());
            activeBirds.push_back(std::move(bird));
            
            // Move next bird to slingshot (if any)
            if (!birdQueue.empty()) {
                sf::Vector2f slingshotPos = slingshot->getPullPosition();
                birdQueue.front()->getBody().position = slingshotPos;
                birdQueue.front()->getSprite().setPosition(slingshotPos);
            }
        }
    }
    // Also use spacebar for ability
    else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
        if (!activeBirds.empty()) {
            activeBirds.back()->onAbility();
        }
    }
}

void GameplayState::update(float dt) {
    // Ground collision level
    float groundY = window.getSize().y - 30.0f;

    // Update Slingshot position tracking for current bird
    if (slingshot->isDragging() && !birdQueue.empty()) {
        sf::Vector2f pullPos = slingshot->getPullPosition();
        birdQueue.front()->getBody().position = pullPos;
        birdQueue.front()->getSprite().setPosition(pullPos);
    }

    // SplitBird child spawning check
    std::vector<std::unique_ptr<Bird>> newChildren;
    for (auto& bird : activeBirds) {
        if (SplitBird* sb = dynamic_cast<SplitBird*>(bird.get())) {
            if (sb->hasSplit()) {
                auto children = sb->spawnChildren();
                for (auto& child : children) {
                    child->getBody().userData = child.get();
                    physicsWorld.addBody(&child->getBody());
                    newChildren.push_back(std::move(child));
                }
            }
        }
    }
    for (auto& child : newChildren) {
        activeBirds.push_back(std::move(child));
    }

    // Update Entities (just syncs sprites now)
    for (auto& bird : activeBirds) {
        bird->update(dt);
    }
    for (auto& pig : pigs) {
        pig->update(dt);
    }
    for (auto& block : blocks) {
        block->update(dt);
    }

    auto events = physicsWorld.step(dt, 10);
    
    // Process Collision events for damage
    for (const auto& ev : events) {
        Entity* entA = static_cast<Entity*>(ev.bodyA->userData);
        Entity* entB = static_cast<Entity*>(ev.bodyB->userData);
        
        float impact = std::abs(ev.normalImpulse);
        
        if (entA && entB) {
            bool isBirdA = dynamic_cast<Bird*>(entA) != nullptr;
            bool isBirdB = dynamic_cast<Bird*>(entB) != nullptr;
            bool isPigA = dynamic_cast<Pig*>(entA) != nullptr;
            bool isPigB = dynamic_cast<Pig*>(entB) != nullptr;
            bool isBlockA = dynamic_cast<Block*>(entA) != nullptr;
            bool isBlockB = dynamic_cast<Block*>(entB) != nullptr;
            
            if ((isBirdA && isPigB) || (isBirdB && isPigA)) {
                Pig* pig = isPigA ? static_cast<Pig*>(entA) : static_cast<Pig*>(entB);
                if (!pig->isDead()) {
                    pig->receiveDamage(impact * 50.0f);
                    if (pig->isDead()) score += 100;
                }
            }
            else if ((isBirdA && isBlockB) || (isBirdB && isBlockA)) {
                Block* block = isBlockA ? static_cast<Block*>(entA) : static_cast<Block*>(entB);
                if (!block->isDestroyed()) {
                    block->receiveDamage(impact);
                    if (block->isDestroyed()) score += 50;
                }
            }
            else if ((isPigA && isBlockB) || (isPigB && isBlockA)) {
                Pig* pig = isPigA ? static_cast<Pig*>(entA) : static_cast<Pig*>(entB);
                if (!pig->isDead() && impact > 150.0f) {
                    pig->receiveDamage(impact);
                    if (pig->isDead()) score += 100;
                }
            }
        }
    }
    
    // Cleanup destroyed bodies from physics world
    for (auto& pig : pigs) {
        if (pig->isDead() && pig->getBody().userData != nullptr) {
            physicsWorld.removeBody(&pig->getBody());
            pig->getBody().userData = nullptr;
        }
    }
    for (auto& block : blocks) {
        if (block->isDestroyed() && block->getBody().userData != nullptr) {
            physicsWorld.removeBody(&block->getBody());
            block->getBody().userData = nullptr;
        }
    }
    for (auto& bird : activeBirds) {
        if (ExplosiveBird* eb = dynamic_cast<ExplosiveBird*>(bird.get())) {
            if (eb->hasExploded() && eb->isActive()) {
                resolveExplosions(eb);
                eb->setActive(false);
                physicsWorld.removeBody(&eb->getBody());
                eb->getBody().userData = nullptr;
            }
        }
        if (!bird->isActive() && bird->getBody().userData != nullptr) {
            physicsWorld.removeBody(&bird->getBody());
            bird->getBody().userData = nullptr;
        }
    }

    // Only check win/loss if at least one bird has been launched
    if (!activeBirds.empty()) {
        if (checkWinLoss()) return; // Early exit to prevent use-after-free crash
    }

    scoreText.setString("Score: " + std::to_string(score));
}

void GameplayState::checkCollisions() {
    // Deprecated, handled by PhysicsWorld now.
}

void GameplayState::resolveExplosions(const ExplosiveBird* eb) {
    sf::Vector2f center = eb->getBody().position;
    float radiusSq = eb->getExplosionRadius() * eb->getExplosionRadius();
    
    for (auto& pig : pigs) {
        if (pig->isDead()) continue;
        sf::Vector2f diff = pig->getBody().position - center;
        if (diff.x * diff.x + diff.y * diff.y < radiusSq) {
            pig->receiveDamage(eb->getExplosionForce()); // Instant kill usually
            if (pig->isDead()) score += 100;
        }
    }
    
    for (auto& block : blocks) {
        if (block->isDestroyed()) continue;
        sf::Vector2f diff = block->getBody().position - center;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        if (distSq < radiusSq) {
            // Apply massive force
            float dist = std::sqrt(distSq);
            if (dist > 0.01f) {
                sf::Vector2f dir = diff / dist;
                block->getBody().applyForce(dir * eb->getExplosionForce() * (1.0f - dist/eb->getExplosionRadius()));
                block->receiveDamage(1000.0f); // Massive damage
                if (block->isDestroyed()) score += 50;
            }
        }
    }
}

bool GameplayState::checkWinLoss() {
    bool allPigsDead = true;
    for (const auto& pig : pigs) {
        if (!pig->isDead()) {
            allPigsDead = false;
            break;
        }
    }
    
    if (allPigsDead) {
        std::cout << "Win! Returning to menu...\n";
        stateManager.changeState(std::make_unique<WinState>(stateManager, window, score));
        return true;
    } else {
        bool outOfBirds = birdQueue.empty();
        bool allBirdsStopped = true;
        for (const auto& bird : activeBirds) {
            if (bird->isActive()) {
                allBirdsStopped = false;
                break;
            }
        }
        
        if (outOfBirds && allBirdsStopped) {
            std::cout << "Lose! Returning to menu...\n";
            stateManager.changeState(std::make_unique<LoseState>(stateManager, window, score));
            return true;
        }
    }
    return false;
}

void GameplayState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    slingshot->draw(renderWindow);
    
    if (slingshot->isDragging() && !birdQueue.empty()) {
        trajectoryLine.calculate(slingshot->getPullPosition(), slingshot->getLaunchVelocityPreview(), 600.0f);
        trajectoryLine.draw(renderWindow);
    }
    
    for (auto& block : blocks) block->draw(renderWindow);
    for (auto& pig : pigs) pig->draw(renderWindow);
    for (auto& bird : activeBirds) bird->draw(renderWindow);
    
    // Draw birds in queue
    for (auto& bird : birdQueue) {
        bird->draw(renderWindow);
    }
    
    renderWindow.draw(scoreText);
}
