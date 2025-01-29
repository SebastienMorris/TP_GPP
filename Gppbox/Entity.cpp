#include "Entity.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include "Game.hpp"
#include "imgui-SFML.h"
#include "imgui.h"


Entity::Entity(sf::RectangleShape* sprite)
{
    this->sprite = sprite;
}

void Entity::update(double dt)
{
    Game& g = *Game::me;
    
    double rate = 1.0f / dt;
    double dfr =  60.0f / rate;

    dy += grav * dt;

    if(dx < 0)
    {
        if(g.hasCollision(cx + rx - 1, cy + ry, width, height) && rx < 0.45f)
        {
            
            dx = 0;
            rx = 0.45f;
        }
    }
    else if(dx > 0)
    {
        if(g.hasCollision(cx + rx + 1, cy + ry, width, height) && rx > 0.55f)
        {
            dx = 0;
            rx = 0.55f;
        }
    }
    
    dx = dx * pow(frx, dfr);
    dy = dy * pow(fry, dfr);
    
    rx += dx * dt;
    ry += dy * dt;

    if(rx > 1.0f)
    {
        if(! g.hasCollision(cx+rx, cy+ry, width, height))
        {
            rx--;
            cx++;
        }
        else
        {
            dx = 0;
            rx = 0.55f;
        }
    }
    if(rx < 0)
    {
        if(! g.hasCollision(cx+rx, cy+ry, width, height))
        {
            rx++;
            cx--;
        }
        else
        {
            dx = 0;
            rx = 0.45f;
        }
    }

    if(jumping)
    {
        if(dy > 0)
        {
            if(g.hasCollision(cx + rx, cy + ry, width, height))
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
        }

        if(dy < 0)
        {
            if(g.hasCollision(cx + rx, cy + ry - 2, width, height))
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
        }
    }
    
    if(!dropping)
    {
        if(!jumping)
        {
            if(!g.hasCollision(cx + rx, cy + ry + 1, width, height) || ry < 0.99f && ry > 0)
            {
                dropping = true;
                grav = 80;
            }
        }
    }

    if(dropping)
    {
        if(g.hasCollision(cx + rx, cy + ry, width, height))
        {
            dropping = false;
            grav = 0;
            ry = 0.99f;
            dy = 0;
        }
        else if(ry > 1)
        {
            ry--;
            cy++;
        }
    }

    syncPos();
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

void Entity::setJumping(bool setJumping)
{
    if(dropping)
        return;
    
    if(jumping && setJumping)
        return;
    
    if(setJumping)
    {
        dy -= 40;
        grav = 80.0f;
        jumping = true;
        dropping = false;
    }
    else
    {
        grav = 0;
        jumping = false;
    }
}

void Entity::move(bool moveRight)
{
    if(moveRight)
        dx += moveSpeed;
    else
        dx -= moveSpeed;
}

    

bool Entity::im()
{
    using namespace ImGui;

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
    
    chg |= DragFloat("move speed", &moveSpeed, 0.001f, 0, 2);
    chg |= DragFloat("gravity", &grav, 0.001f, -2, 2);

    Value("jumping", jumping);
    Value("dropping", dropping);

    if(Button("reset"))
    {
        cx = 3;
        cy = 54;
        rx = 0.5f;
        ry = 0.99f;
        dx = dy = 0;
        setJumping(false);
        dropping = false;
    }

    return chg|chgCoord;
}



