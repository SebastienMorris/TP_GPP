#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "C.hpp"

enum EntityType
{
    PLAYER,
    ENEMY
};

class Entity
{
public:
    EntityType type;
    
    sf::RectangleShape* sprite;

    sf::RectangleShape* standSprite;
    sf::RectangleShape* crouchSprite;

    int cx = 0;
    int cy = 0;

    float rx = 0.5f;
    float ry = 1.0f;

    float dx = 0.0f;
    float dy = 0.0f;

    float frx = 0.95f;
    float fry = 1.0f;

    float moveSpeed = 20.0f;
    float crouchMoveDecrease = 2.0f;

    float jumpForce = 40.0f;
    float crouchJumpDecrease = 2.0f;
    float gravStrength = 80.0f;
    float grav = 0.0f;
    
    bool jumping = false;
    bool dropping = false;
    bool crouching = false;
    bool attacking = false;
    bool lookingRight = true;

    bool isDead = false;
    
    int height = 2;

    
    Entity(sf::RectangleShape* standSprite, sf::RectangleShape* crouchSprite, EntityType type);

    
    virtual void update(double dt);
    virtual void draw(sf::RenderWindow& win);

    void setCoordPixel(float x, float y);
    void setCoordGrid(float x, float y);
    void syncPos();

    sf::Vector2i getPosPixel();

    void Jump();
    void setJumping(bool setJumping);
    void setDropping(bool setDropping);
    
    void move(float moveX);
    void crouch();
    void uncrouch();

    void addForce(float xVel, float yVel);
    
    bool im();
    void Reset();
};
