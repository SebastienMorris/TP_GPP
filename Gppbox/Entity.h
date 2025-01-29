#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "C.hpp"

class Entity
{
public:
    sf::RectangleShape* sprite;

    int cx = 0;
    int cy = 0;

    float rx = 0.5f;
    float ry = 1.0f;

    float dx = 0.0f;
    float dy = 0.0f;

    float frx = 0.95f;
    float fry = 1.0f;

    float moveSpeed = 0.5f;

    float grav = 0.0f;
    bool jumping = false;
    bool dropping = false;

    int width = 1;
    int height = 2;

    
    Entity(sf::RectangleShape* sprite);

    
    void update(double dt);

    void setCoordPixel(float x, float y);
    void setCoordGrid(float x, float y);
    void syncPos();

    sf::Vector2i getPosPixel();

    void setJumping(bool setJumping);

    void move(bool moveRight);
    
    bool im();
};
