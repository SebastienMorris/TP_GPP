#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    int moveDir = 1;

    bool takeDamage = false;
    float damageAnimDuration = 0.1f;
    float damageAnimTimer = 0.0f;
    
public:


    
private:

    
public:

    Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite);
    void update(double dt) override;

    void die();
};
