#pragma once

#include <vector>

#include "Entity.h"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"

#include "Particle.hpp"
#include "ParticleMan.hpp"

using namespace sf;

class HotReloadShader;
class Entity;
class Game {
public:
	
	sf::RenderWindow*				win = nullptr;

	sf::RectangleShape				bg;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;
	
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::RectangleShape> wallSprites;
	std::vector<Entity*> entities;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	bool editMode = true;
	bool placeWallEnemytoggle = false;

	static Game* me;

	Game(sf::RenderWindow * win);

	void cacheWalls();

	void processInput(sf::Event ev);
	bool wasPressedSpace = false;
	bool wasPressedLControl = false;
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);

	void draw(sf::RenderWindow& win);

	bool isWall(int cx, int cy);
	void im();

	void CreateEntity();

	bool hasCollision(float gx, float gy);
	bool hasCollision(float gx, float gy, int height);

	void EplaceWall(int mouseX, int mouseY);
	void EplaceEnemy(int mouseX, int mouseY);

	void Save(std::string filename);
	void Load(std::string filename);
};