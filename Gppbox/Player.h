#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Entity.h"

class Player : public Entity
{

private:
    bool wasPressedCrouch = false;
    bool wasPressedLaser = false;

    int laserRange = 20;
    float laserPixelSize = 8.0f;

    bool firingLaser = false;
    
    bool showMuzzle = false;
    float muzzleLifetime = 0.1f;
    float muzzleLifetimeTimer = 0.0f;

    sf::CircleShape* muzzleFireSprite;

    
public:
    std::vector<sf::RectangleShape*> laserSprites;

    
    
private:
    void PollControllerInput(double dt);

    void drawLaser(int x0, int y0, int x1, int y1);
    void createLaserPixel(int x, int y);

    
public:
    using Entity::Entity;
    Player(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite);
    
    void ProcessInput(sf::Event ev);
    void PollInput(double dt);

    void update(double dt) override;
    void draw(sf::RenderWindow& win) override;

    void fireLaser();
};
