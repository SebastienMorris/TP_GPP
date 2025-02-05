#include "Enemy.h"
#include "Game.hpp"


Enemy::Enemy(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite) : Entity(standSprite, crouchSprite)
{
    
}


void Enemy::update(double dt)
{
    Entity::update(dt);
    
    Game& g = *Game::me;

    if(g.hasCollision(cx + moveDir, cy, height, *this))
        moveDir *= -1;

    move(moveDir);
}

void Enemy::die()
{
    printf("fiiieifuhshkjfgs<lihn");
}

