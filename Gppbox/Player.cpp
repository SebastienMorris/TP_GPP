#include "Player.h"

#include <iostream>
#include <SFML/System/Sleep.hpp>

#include "Game.hpp"

#include "TweenEngine.h"

Player::Player(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite) : Entity(standSprite, crouchSprite, EntityType::PLAYER)
{
    muzzleFireSprite = new sf::CircleShape(C::GRID_SIZE);
    muzzleFireSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE * 0.5f);
    muzzleFireSprite->setFillColor(sf::Color::Yellow);
}


void Player::ProcessInput(sf::Event ev)
{
    
}

void Player::PollInput(double dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
        move(-1.0f);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        move(1.0f);
		
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) 
        Jump();
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
    {
        if(!wasPressedCrouch)
        {
            crouch();
            wasPressedCrouch = true;
        }
    }
    else
    {
        if(wasPressedCrouch)
        {
            uncrouch();
            wasPressedCrouch = false;
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
    {
        fireLaser(dt);
        //Shoot();
        wasPressedLaser = true;
    }
    else
    {
        if(wasPressedLaser)
        {
            *laserSprite = sf::RectangleShape({0.0f, 0.0f});
            firingLaser = false;
            attacking = false;
            wasPressedLaser = false;
            laserLength = 0;
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
    {
        Shoot(dt);
        wasPressedShoot = true;
    }
    else
    {
        if(wasPressedShoot)
        {
            attacking = false;
            wasPressedShoot = false;
        }
    }

    PollControllerInput(dt);
}

void Player::PollControllerInput(double dt)
{
    if(sf::Joystick::isConnected(0))
    {
        if(sf::Joystick::hasAxis(0, sf::Joystick::Axis::X))
        {
            float moveX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) / 100;
            if(moveX >= 0.1f || moveX <= -0.1f) //deadzone
            {
                move(moveX);
            }
        }
        
        if (sf::Joystick::isButtonPressed(0, 0)) 
            Jump();
        
        if(sf::Joystick::isButtonPressed(0, 1))
        {
            if(!wasPressedCrouch)
            {
                crouch();
                wasPressedCrouch = true;
            }
        }
        else
        {
            if(wasPressedCrouch)
            {
                uncrouch();
                wasPressedCrouch = false;
            }
        }
    }
}

void Player::update(double dt)
{
    Entity::update(dt);

    Game& g = *Game::me;

    if(showMuzzle)
    {
        if(lookingRight)
            muzzleFireSprite->setPosition((cx + rx + 0.5f) * C::GRID_SIZE, (cy + ry - (float)height/2.0f) * C::GRID_SIZE);
        else
            muzzleFireSprite->setPosition((cx + rx - 1.5f) * C::GRID_SIZE, (cy + ry - (float)height/2.0f) * C::GRID_SIZE);
        
        muzzleLifetimeTimer += dt;
        if(muzzleLifetimeTimer >= muzzleLifetime)
        {
            showMuzzle = false;
            muzzleLifetimeTimer = 0;
        }
    }

    if(!canShoot)
    {
        shootCooldownTimer += dt;
        if(shootCooldownTimer >= shootCooldown)
        {
            canShoot = true;
            shootCooldownTimer = 0;
        }
    }

    for(auto bullet : bullets)
    {
        if(bullet->aimingRight)
            bullet->rx += bulletSpeed * dt;
        else
            bullet->rx -= bulletSpeed * dt;
        
        do
        {
            if(g.hasCollision(bullet->cx + 1, bullet->cy, 1, *this))
            {
                if(!bullet->destroy)
                {
                    Enemy* hitEnemy = g.hasCollisionEnemy(bullet->cx + 1, bullet->cy);
                    if(hitEnemy)
                    {
                        int damageDir = hitEnemy->cx - cx;
                        if(damageDir > 0)
                            damageDir = 1;
                        else if(damageDir < 0)
                            damageDir = -1;
                        
                        hitEnemy->damage(1, damageDir);
                    }
                
                    bulletsToDestroy.push_back(bullet);
                    bullet->destroy = true;
                }
            }
            if(bullet->rx > 1.0f)
            {
                bullet->cx++;
                bullet->rx--;
            }
        }while(bullet->rx > 1.0f);

        do
        {
            if(g.hasCollision(bullet->cx - 1, bullet->cy, 1, *this))
            {
                if(!bullet->destroy)
                {
                    Enemy* hitEnemy = g.hasCollisionEnemy(bullet->cx - 1, bullet->cy);
                    if(hitEnemy)
                    {
                        int damageDir = hitEnemy->cx - cx;
                        if(damageDir > 0)
                            damageDir = 1;
                        else if(damageDir < 0)
                            damageDir = -1;
                        hitEnemy->damage(1, damageDir);
                    }

                    bulletsToDestroy.push_back(bullet);
                    bullet->destroy = true;
                }
            }
            
            if(bullet->rx < 0.0f)
            {
                bullet->cx--;
                bullet->rx++;
            }
        }while (bullet->rx < 0.0f);
        bullet->syncPos();
    }

    if(!bulletsToDestroy.empty())
    {
        for(auto btd : bulletsToDestroy)
        {
            
            if(!bullets.empty())
                bullets.erase(std::find(bullets.begin(), bullets.end(), btd));

            delete btd;
        }
        bulletsToDestroy.clear();
    }
}

void Player::draw(sf::RenderWindow& win)
{
    Entity::draw(win);
    
    if(laserSprite)
        win.draw(*laserSprite);
    
    for(auto bullet : bullets)
    {
        win.draw(*bullet->sprite);
    }

    if(showMuzzle)
    {
        win.draw(*muzzleFireSprite);
    }
}

void Player::Shoot(double dt)
{
    Game& g = *Game::me;
    if(!canShoot)
        return;

    attacking = true;
    
    showMuzzle = true;

    auto bulletSprite = new sf::CircleShape({bulletSize});
    bulletSprite->setOrigin(0.5f, laserPixelSize * 0.5f);
    bulletSprite->setPosition(cx + rx, cy + ry - (float)height/2.0f);
    bulletSprite->setFillColor(sf::Color::Magenta);

    auto bullet = new Bullet;
    bullet->Initialise(cx, cy, rx, ry - (float)height/2.0f, lookingRight, bulletSprite);
    bullets.push_back(bullet);

    addForce((lookingRight ? -200 : 200) * dt, 0);
    g.CamShake();

    canShoot = false;
}

void Player::fireLaser(double dt)
{
    Game& g = *Game::me;

    if(!attacking)
    {
        addForce((lookingRight ? -200 : 200) * dt, 0);
        g.CamShake();
    }

    attacking = true;
    
    *laserSprite = sf::RectangleShape({0.0f, 0.0f});
    
    int x0 = lookingRight ? (cx+rx+0.5f) * C::GRID_SIZE : (cx+rx-0.5f) * C::GRID_SIZE;
    int y0 = (cy+ry - height/2) * C::GRID_SIZE;

    float laserNewLength = laserRange;
    
    for(int i=0; i<=laserRange; i++)
    {
        int j = lookingRight ? i : -i;
        if(g.hasCollision(cx + j, cy - 1, 1, *this))
        {
            Enemy* hitEnemy = g.hasCollisionEnemy(cx + j, cy - 1);
            if(hitEnemy)
            {
                int damageDir = hitEnemy->cx - cx;
                if(damageDir > 0)
                    damageDir = 1;
                else if(damageDir < 0)
                    damageDir = -1;
                
                hitEnemy->damage(1, damageDir);
            }
            
            laserNewLength = i - 1;
            laserNewLength += lookingRight ? -(rx - 0.5f) : (rx-0.5f);
            break;
        }
    }
    
    float laserRangePixel = laserNewLength * C::GRID_SIZE;
    if(!lookingRight)
        laserRangePixel *= -1; 

    drawAnimLaser((float)x0, (float)x0 + laserRangePixel, y0, dt);
    
    firingLaser = true;
}

void Player::drawAnimLaser(float x0, float x1, int y, double dt)
{
    float dist = x1 - x0;
    if(dist == 0.0f)
        return;
    
    float progress = laserLength / dist;
    
    float newProgress = clamp(progress + dt * (laserAnimSpeed / abs(dist)), 0.0, 1.0);
    laserLength = TweenEngine::Lerp(0, dist, newProgress);
    createLaserSprite(laserLength, y);
    
}


void Player::createLaserSprite(float length, int y)
{
    auto spr =  sf::RectangleShape({length , laserPixelSize});
    spr.setOrigin(0.5f, laserPixelSize * 0.5f);
    spr.setPosition(lookingRight ? (cx+rx+0.5f) * C::GRID_SIZE : (cx+rx-0.5f) * C::GRID_SIZE, y);
    spr.setFillColor(sf::Color::Red);
    *laserSprite = spr;
}

void Player::createLaser(int length)
{
    auto laser = new sf::RectangleShape({(float)length, laserPixelSize});
}








