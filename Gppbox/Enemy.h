#pragma once
#include "Entity.h"

class Enemy : public Entity
{
private:
    int moveDir = 1;

    int lifes = 3;

    bool takingDamage = false;
    
    bool damageAnim = false;
    float damageAnimDuration = 0.1f;
    float damageAnimTimer = 0.0f;

    sf::RectangleShape* deadSprite;
    
public:
    

    
private:

    
public:
  
    Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite, sf::RectangleShape* deadSprite);
    void update(double dt) override;

    void damage(int damage, int damageDirection);
    void die(int damageDirection);
};
