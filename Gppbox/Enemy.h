#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    int moveDir = 1;
    
public:


    
private:

    
public:

    Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite);
    void update(double dt) override;

    void die();
};
