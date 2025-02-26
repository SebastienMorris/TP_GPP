#include "Entity.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include "Game.hpp"
#include "imgui-SFML.h"
#include "imgui.h"


Entity::Entity(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite, EntityType entityType) :
sprite(standSprite), standSprite(standSprite), crouchSprite(crouchSprite), type(entityType)
{
    
}

void Entity::update(double dt)
{
    Game& g = *Game::me;
    
    double rate = 1.0f / dt;
    double dfr =  60.0f / rate;

    dy += grav * dt;
    
    dx = dx * pow(frx, dfr);
    dy = dy * pow(fry, dfr);
    
    rx += dx * dt;
    ry += dy * dt;

    do
    {
        if(g.hasCollision(cx - 1, cy, height, *this) && rx <= 0.5f)
        {
            dx = 0;
            rx = 0.5f;
        }
        if(rx < 0)
        {
            rx++;
            cx--;
        }
    }while(rx < 0);
    
    do
    {
        if(g.hasCollision(cx + 1, cy, height, *this) && rx >= 0.5f)
        {
            dx = 0;
            rx = 0.5f;
        }
        if(rx > 1)
        {
            rx--;
            cx++;
        }
    }while(rx > 1);

    if(jumping)
    {
        do
        {
            if(g.hasCollision(cx, cy + 1, height, *this) && ry >= 0.99f)
            {
                setJumping(false);
                ry = 0.99f;
                dy = 0;
            }
            else if(ry > 1)
            {
                ry--;
                cy++;
            }
        }while (ry > 1);
        
        do
        {
            if(g.hasCollision(cx, cy - 2, height, *this) && ry < 0.01f)
            {
                setJumping(false);
                ry = 0.01f;
                dy = 0;
            }
            else if (ry < 0)
            {
                ry++;
                cy--;
            }
        }while(ry < 0);
    }
    
    if(!dropping)
    {
        if(!jumping)
        {
            if(!g.hasCollision(cx, cy + 1, height, *this) || ry < 0.99f && ry > 0)
            {
                setDropping(true);
            }
        }
    }

    if(dropping)
    {
        do
        {
            if(g.hasCollision(cx, cy + 1, height, *this) && ry > 0.99f)
            {
                setDropping(false);
                ry = 0.99f;
                dy = 0;
            }
            else if(ry > 1)
            {
                ry--;
                cy++;
            }
        }while(ry > 1);
    }

    syncPos();
}

void Entity::draw(sf::RenderWindow& win)
{
    win.draw(*sprite);
}


void Entity::setCoordPixel(float x, float y)
{
    cx = (int)x/C::GRID_SIZE;
    cy = (int)y/C::GRID_SIZE;
    rx = (x - cx*C::GRID_SIZE) / C::GRID_SIZE;
    ry = (y - cy*C::GRID_SIZE) / C::GRID_SIZE;

    syncPos();
}

void Entity::setCoordGrid(float x, float y)
{
    cx = (int)x;
    cy = (int)y;

    rx = x - cx;
    ry = y - cy;

    syncPos();
}

void Entity::syncPos()
{
    sf::Vector2f pos = {(cx+rx) * C::GRID_SIZE, (cy+ry) * C::GRID_SIZE};

    sprite->setPosition(pos);
}

sf::Vector2i Entity::getPosPixel()
{
    return sf::Vector2i((cx+rx) * C::GRID_SIZE, (cy+ry) * C::GRID_SIZE);
}

void Entity::Jump()
{
    if(dropping)
        return;
    
    if(jumping)
        return;
    
    dy -= jumpForce;
    setJumping(true);
}


void Entity::setJumping(bool setJumping)
{
    if(dropping)
        return;
    
    if(jumping && setJumping)
        return;
    
    if(setJumping)
    {
        grav = gravStrength;
        jumping = true;
        dropping = false;
    }
    else
    {
        grav = 0;
        jumping = false;
    }
}

void Entity::setDropping(bool setDropping)
{
    if(setDropping)
    {
        dropping = true;
        grav = gravStrength;
    }
    else
    {
        grav = 0;
        dropping = false;
    }
}


void Entity::move(float moveX)
{
    dx = moveX * moveSpeed;
    if(!attacking)
    {
        if(moveX > 0)
            lookingRight = true;
        else
            lookingRight = false;
    }
}


void Entity::crouch()
{
    if(crouching)
        return;

    if(jumping || dropping)
        return;
    
    sprite = crouchSprite;
    moveSpeed /= crouchMoveDecrease;
    jumpForce /= crouchJumpDecrease;
    height /= 2;
    crouching = true;
}

void Entity::uncrouch()
{
    if(!crouching)
        return;
    
    sprite = standSprite;
    moveSpeed *= crouchMoveDecrease;
    jumpForce *= crouchJumpDecrease;
    height *= 2;
    crouching = false;
}

void Entity::addForce(float xVel, float yVel)
{
    dx += xVel;
    dy += yVel;
    if(dy != 0)
        setJumping(true);
}


bool Entity::im()
{
    using namespace ImGui;

    if(ImGui::CollapsingHeader("Character", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool chg = false;
        Value("cx", cx);
        Value("cy", cy);

        Value("rx", rx);
        Value("ry", ry);

        sf::Vector2i pix = getPosPixel();
        chg |= DragInt2("rx/ry", &pix.x, 1.0f, -2000, 2000);

        if(chg)
        {
            setCoordPixel(pix.x, pix.y);
        }

        sf::Vector2f coord = {cx + rx, cy + ry};
        bool chgCoord = DragFloat2("coord x/y", &coord.x, 1.0f, -2000, 2000);
        if(chgCoord)
        {
            setCoordGrid(coord.x, coord.y);
        
        }

        chg |= DragFloat2("dx/dy", &dx, 0.01f, -20,20);
        chg |= DragFloat2("frx/fry", &frx, 0.001f, 0, 1);
    
        chg |= DragFloat("move speed", &moveSpeed, 0.1f, 0, 50);
        chg |= DragFloat("gravity", &grav, 0.001f, -2, 2);

        Value("jumping", jumping);
        Value("dropping", dropping);
        Value("crouching", crouching);
        Value("height", height);

        if(Button("reset"))
        {
            cx = 3;
            cy = 54;
            rx = 0.5f;
            ry = 0.99f;
            dx = dy = 0;
            setJumping(false);
            setDropping(false);
        }

        return chg|chgCoord;
    }
}

void Entity::Reset()
{
    cx = 3;
    cy = 54;
    rx = 0.5f;
    ry = 0.99f;
    dx = dy = 0;
    setJumping(false);
    setDropping(false);
    uncrouch();
}



