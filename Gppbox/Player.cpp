#include "Player.h"

#include <iostream>
#include <SFML/System/Sleep.hpp>

#include "Game.hpp"

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
        setJumping(true);
    
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
            for(auto laser : laserSprites)
                delete laser;
            laserSprites.clear();
            firingLaser = false;
            attacking = false;
            wasPressedLaser = false;
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
            setJumping(true);
        
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
                        hitEnemy->die();
                
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
                        hitEnemy->die();

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
    
    for(auto laser : laserSprites)
    {
        win.draw(*laser);
    }
    
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

    canShoot = false;
}

void Player::fireLaser(double dt)
{
    Game& g = *Game::me;

    attacking = true;
    
    for(auto laser : laserSprites)
        delete laser;
    laserSprites.clear();
    
    int x0 = lookingRight ? (cx+rx+0.5f) * C::GRID_SIZE : (cx+rx-0.5f) * C::GRID_SIZE;
    int y0 = (cy+ry - height/2) * C::GRID_SIZE;

    int laserLength = laserRange;
    for(int i=0; i<laserRange; i++)
    {
        int j = lookingRight ? i : -i;
        if(g.hasCollision(cx + j, cy - 1, 1, *this))
        {
            Enemy* hitEnemy = g.hasCollisionEnemy(cx + j, cy - 1);
            if(hitEnemy)
            {
                hitEnemy->die();
            }
            
            laserLength = i - 1;
            break;
        }
    }
    
    int laserRangePixel = laserLength * C::GRID_SIZE;
    if(!lookingRight)
        laserRangePixel *= -1; 
     
    drawLaser(x0, y0, x0 + laserRangePixel, y0);
    
    addForce((lookingRight ? -20 : 20) * dt, 0);
    
    firingLaser = true;
}

void Player::drawLaser(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    if(dx != 0)
    {
        int D = 2 * dy - abs(dx);
        int y = y0;
        for(int i=0; dx < 0 ? i>dx : i<dx; dx < 0 ? i-- : i++)
        {
            createLaserPixel(x0 + i, y0);
            if(D > 0)
            { 
            y++;
                D -= 2 * abs(dx);
            }
            D += 2 * dy;
        }
    }
}

void Player::createLaserPixel(int x, int y)
{
    auto laserPixel = new sf::RectangleShape({1.0f, laserPixelSize});
    laserPixel->setOrigin(0.5f, laserPixelSize * 0.5f);
    laserPixel->setPosition(x, y);
    laserPixel->setFillColor(sf::Color::Red);
    laserSprites.push_back(laserPixel);
}

void Player::createLaser(int length)
{
    auto laser = new sf::RectangleShape({(float)length, laserPixelSize});
}








