#include "TrajectoryLine.h"

TrajectoryLine::TrajectoryLine() {
    for (int i = 0; i < numSteps; ++i) {
        sf::CircleShape dot(3.0f);
        dot.setFillColor(sf::Color(255, 255, 255, 200)); // Semi-transparent white
        dot.setOrigin(3.0f, 3.0f);
        dots.push_back(dot);
    }
}

void TrajectoryLine::calculate(sf::Vector2f startPos, sf::Vector2f initialVelocity, float gravity) {
    for (int i = 0; i < numSteps; ++i) {
        float t = i * timeStep;
        float px = startPos.x + initialVelocity.x * t;
        float py = startPos.y + initialVelocity.y * t + 0.5f * gravity * t * t;
        dots[i].setPosition(px, py);
        
        // Fade out dots further away
        sf::Color color = dots[i].getFillColor();
        color.a = 255 * (1.0f - (float)i / numSteps);
        dots[i].setFillColor(color);
    }
}

void TrajectoryLine::draw(sf::RenderWindow& window) {
    for (const auto& dot : dots) {
        window.draw(dot);
    }
}
