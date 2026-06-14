#ifndef TRAJECTORYLINE_H
#define TRAJECTORYLINE_H

#include <SFML/Graphics.hpp>
#include <vector>

class TrajectoryLine {
public:
    TrajectoryLine();
    
    void calculate(sf::Vector2f startPos, sf::Vector2f initialVelocity, float gravity);
    void draw(sf::RenderWindow& window);

private:
    std::vector<sf::CircleShape> dots;
    int numSteps = 30;
    float timeStep = 0.05f;
};

#endif // TRAJECTORYLINE_H
