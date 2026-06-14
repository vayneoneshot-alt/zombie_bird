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

    // Load Birds
    for (const auto& birdType : levelData.birds) {
        sf::Vector2f pos(100.0f - birdQueue.size() * 30.0f, winSize.y - 50.0f);
        if (birdType == "dash") {
            birdQueue.push_back(std::make_unique<DashBird>(pos));
        } else if (birdType == "explosive") {
            birdQueue.push_back(std::make_unique<ExplosiveBird>(pos));
        } else if (birdType == "split") {
            birdQueue.push_back(std::make_unique<SplitBird>(pos));
        } else {
            birdQueue.push_back(std::make_unique<BasicBird>(pos));
        }
    }
    
    // Move first bird to slingshot
    if (!birdQueue.empty()) {
        birdQueue.front()->getBody().position = slingshot->getPullPosition();
    }
    
    // Load Pigs
    for (const auto& pigData : levelData.pigs) {
        pigs.push_back(std::make_unique<Pig>(sf::Vector2f(pigData.x, pigData.y)));
    }
    
    // Load Blocks
    for (const auto& blockData : levelData.blocks) {
        blocks.push_back(std::make_unique<Block>(
            sf::Vector2f(blockData.x, blockData.y),
            sf::Vector2f(blockData.w, blockData.h),
            Block::Material::WOOD
        ));
    }
}

void GameplayState::onExit() {
}

void GameplayState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(e.mouseButton.x, e.mouseButton.y);
        
        // Use bird ability if we click and a bird is already flying
        if (!activeBirds.empty()) {
            activeBirds.back()->onAbility();
        } else if (!birdQueue.empty()) {
            slingshot->onMousePress(mousePos);
        }
    }
    else if (e.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(e.mouseMove.x, e.mouseMove.y);
        slingshot->onMouseMove(mousePos);
    }
    else if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (slingshot->onMouseRelease() && !birdQueue.empty()) {
            auto bird = std::move(birdQueue.front());
            birdQueue.pop_front();
            
            sf::Vector2f launchVel = slingshot->getLaunchVelocity();
            bird->launch(launchVel);
            activeBirds.push_back(std::move(bird));
            
            // Move next bird to slingshot (if any)
            if (!birdQueue.empty()) {
                birdQueue.front()->getBody().position = sf::Vector2f(250.0f, window.getSize().y - 200.0f);
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
        birdQueue.front()->getBody().position = slingshot->getPullPosition();
    }

    // SplitBird child spawning check
    std::vector<std::unique_ptr<BasicBird>> newChildren;
    for (auto& bird : activeBirds) {
        if (SplitBird* sb = dynamic_cast<SplitBird*>(bird.get())) {
            if (sb->hasSplit()) {
                auto children = sb->spawnChildren();
                for (auto& child : children) {
                    newChildren.push_back(std::move(child));
                }
            }
        }
    }
    for (auto& child : newChildren) {
        activeBirds.push_back(std::move(child));
    }

    // Update Entities
    for (auto& bird : activeBirds) {
        bird->update(dt);
        if (bird->getBody().position.y + bird->getBounds().height/2.0f > groundY) {
            bird->getBody().position.y = groundY - bird->getBounds().height/2.0f;
            bird->getBody().velocity.y *= -bird->getBody().restitution;
            bird->getBody().velocity.x *= 0.98f; // Ground friction
        }
    }
    for (auto& pig : pigs) {
        pig->update(dt);
        if (pig->getBody().position.y + pig->getRadius() > groundY) {
            pig->getBody().position.y = groundY - pig->getRadius();
            pig->getBody().velocity.y *= -pig->getBody().restitution;
            pig->getBody().velocity.x *= 0.95f;
        }
    }
    for (auto& block : blocks) {
        block->update(dt);
        if (block->getBody().position.y + block->getHalfSize().y > groundY) {
            block->getBody().position.y = groundY - block->getHalfSize().y;
            block->getBody().velocity.y *= -block->getBody().restitution;
            block->getBody().velocity.x *= 0.90f;
        }
    }

    checkCollisions();
    checkWinLoss();

    scoreText.setString("Score: " + std::to_string(score));
}

void GameplayState::checkCollisions() {
    // Bird vs Pig
    for (auto& bird : activeBirds) {
        if (!bird->isActive()) continue;
        
        // Check if bird is explosive and exploded
        if (ExplosiveBird* eb = dynamic_cast<ExplosiveBird*>(bird.get())) {
            if (eb->hasExploded() && eb->isActive()) { // Just exploded
                resolveExplosions(eb);
                eb->setActive(false); // only apply explosion once
                continue;
            }
        }
        
        for (auto& pig : pigs) {
            if (!pig->isDead()) {
                CollisionManifold m = CollisionSystem::circleVsCircle(
                    bird->getBody(), bird->getRadius(),
                    pig->getBody(), pig->getRadius()
                );
                CollisionSystem::resolveCollision(bird->getBody(), pig->getBody(), m);
                if (m.isColliding) {
                    pig->receiveDamage(std::abs(m.impactForce) * 50.0f);
                    if (pig->isDead()) score += 100;
                }
            }
        }
        
        // Bird vs Block
        for (auto& block : blocks) {
            if (!block->isDestroyed()) {
                CollisionManifold m = CollisionSystem::circleVsAABB(
                    bird->getBody(), bird->getRadius(),
                    block->getBody(), block->getHalfSize()
                );
                CollisionSystem::resolveCollision(bird->getBody(), block->getBody(), m);
                if (m.isColliding) {
                    block->receiveDamage(std::abs(m.impactForce));
                    if (block->isDestroyed()) score += 50;
                }
            }
        }
    }
    
    // Block vs Block (Stacking)
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i]->isDestroyed()) continue;
        for (size_t j = i + 1; j < blocks.size(); ++j) {
            if (blocks[j]->isDestroyed()) continue;
            
            CollisionManifold m = CollisionSystem::aabbVsAABB(
                blocks[i]->getBody(), blocks[i]->getHalfSize(),
                blocks[j]->getBody(), blocks[j]->getHalfSize()
            );
            CollisionSystem::resolveCollision(blocks[i]->getBody(), blocks[j]->getBody(), m);
        }
    }
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

void GameplayState::checkWinLoss() {
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
    } else {
        bool outOfBirds = birdQueue.empty();
        bool allBirdsStopped = true;
        for (const auto& bird : activeBirds) {
            float speed = std::abs(bird->getBody().velocity.x) + std::abs(bird->getBody().velocity.y);
            if (speed > 10.0f || bird->isActive()) {
                allBirdsStopped = false;
                break;
            }
        }
        
        if (outOfBirds && allBirdsStopped) {
            std::cout << "Lose! Returning to menu...\n";
            stateManager.changeState(std::make_unique<LoseState>(stateManager, window, score));
        }
    }
}

void GameplayState::draw(sf::RenderWindow& renderWindow) {
    renderWindow.draw(background);
    slingshot->draw(renderWindow);
    
    if (slingshot->isDragging() && !birdQueue.empty()) {
        trajectoryLine.calculate(slingshot->getPullPosition(), slingshot->getLaunchVelocityPreview(), 980.0f);
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
