#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Entity.h"

struct Bullet
{
    int cx;
    int cy;

    float rx;
    float ry;

    bool aimingRight;

    sf::CircleShape* sprite;

    void Initialise(int cx, int cy, float rx, float ry, bool aimingRight, sf::CircleShape* sprite)
    {
        this->cx = cx;
        this->cy = cy;

        this->rx = rx;
        this->ry = ry;

        this->aimingRight = aimingRight;

        this->sprite = sprite;
    }

    void syncPos()
    {
        sf::Vector2f pos = {(cx+rx) * C::GRID_SIZE, (cy+ry) * C::GRID_SIZE};

        sprite->setPosition(pos);
    }
};

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

    bool canShoot = true;
    float shootCooldown = 0.5f;
    float shootCooldownTimer = 0;
    float bulletSpeed = 40.0f;
    float bulletSize = 5.0f;
    std::vector<Bullet*> bullets;
    
    sf::CircleShape* muzzleFireSprite;

    
public:
    std::vector<sf::RectangleShape*> laserSprites;

    
    
private:
    void PollControllerInput(double dt);

    void Shoot();
    
    void fireLaser();
    void drawLaser(int x0, int y0, int x1, int y1);
    void createLaserPixel(int x, int y);
    void createLaser(int length);

    
public:
    Player(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite);
    
    void ProcessInput(sf::Event ev);
    void PollInput(double dt);

    void update(double dt) override;
    void draw(sf::RenderWindow& win) override;
};
