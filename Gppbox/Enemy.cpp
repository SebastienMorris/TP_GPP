#include "Enemy.h"
#include "Game.hpp"


Enemy::Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite) : Entity(standSprite, crouchSprite, EntityType::ENEMY)
{
    
}


void Enemy::update(double dt)
{
    Entity::update(dt);
    
    Game& g = *Game::me;

    if(g.hasCollision(cx + moveDir, cy, height, *this))
        moveDir *= -1;

    move(moveDir);

    if(takeDamage)
    {
        damageAnimTimer += dt;
        if(damageAnimTimer >= damageAnimDuration)
        {
            sprite->setFillColor(Color::Blue);
            damageAnimTimer = 0;
            takeDamage = false;
        }
    }
}

void Enemy::die()
{
    sprite->setFillColor(Color::White);
    takeDamage = true;
}

