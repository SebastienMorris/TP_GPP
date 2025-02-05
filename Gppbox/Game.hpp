#pragma once

#include <vector>

#include "Enemy.h"
#include "Entity.h"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"

#include "Particle.hpp"
#include "ParticleMan.hpp"

#include "Player.h"

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
	std::vector<Enemy*> enemies;

	Player* player;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	bool editMode = true;
	bool placeWallEnemytoggle = false;

	static Game* me;

	Game(sf::RenderWindow * win);

	void cacheWalls();

	void processInput(sf::Event ev);
	bool wasPressedSpace = false;
	void pollInput(double dt);
	void onSpacePressed();

	void update(double dt);

	void draw(sf::RenderWindow& win);

	bool isWall(int cx, int cy);
	void im();

	void CreatePlayer(int spawnX, int spawnY);
	void CreateEnemy(int spawnX, int spawnY);

	bool hasCollision(float gx, float gy);
	bool hasCollision(float gx, float gy, int height, Entity& self);
	Enemy* hasCollisionEnemy(float gx, float gy);

	void EplaceWall(int mouseX, int mouseY);
	void EplaceEnemy(int mouseX, int mouseY);

	void Save(std::string filename);
	void Load(std::string filename);
};