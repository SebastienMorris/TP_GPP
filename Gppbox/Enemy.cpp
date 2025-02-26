#include "Enemy.h"
#include "Game.hpp"


Enemy::Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite, sf::RectangleShape* deadSprite) :
Entity(standSprite, crouchSprite, EntityType::ENEMY),
deadSprite(deadSprite)
{
    
}


void Enemy::update(double dt)
{
    Entity::update(dt);
    
    if(!isDead)
    {
        Game& g = *Game::me;

        if(!takingDamage)
        {
            if(g.hasCollision(cx + moveDir, cy, height, *this))
                moveDir *= -1;

            move(moveDir);
        }
        else
        {
            if(!jumping)
                takingDamage = false;
        }

        if(damageAnim)
        {
            damageAnimTimer += dt;
            if(damageAnimTimer >= damageAnimDuration)
            {
                sprite->setFillColor(Color::Blue);
                damageAnimTimer = 0;
                damageAnim = false;
            }
        }
    }
}

void Enemy::damage(int damage, int damageDirection)
{
    takingDamage = true;
    sprite->setFillColor(Color::White);
    damageAnim = true;

    lifes -= damage;
    if(lifes <= 0)
    {
        die(damageDirection);
        return;
    }

    addForce(damageDirection * 30.0f, -20.0f);
}


void Enemy::die(int damageDirection)
{
    sprite = deadSprite;
    isDead = true;
    addForce(damageDirection * 60.0f, -30.0f);
}

