
#include <imgui.h>
#include <array>
#include <vector>

#include "C.hpp"
#include "Game.hpp"

#include "HotReloadShader.hpp"

Game* Game::me = 0;

static int cols = C::SCREEN_WIDTH / C::GRID_SIZE;
static int lastLine = C::SCREEN_HEIGHT / C::GRID_SIZE - 1;

Game::Game(sf::RenderWindow * win) {
	this->win = win;
	me = this;
	bg = sf::RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));

	bool isOk = tex.loadFromFile("res/bg_stars.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	bg.setTexture(&tex);
	bg.setSize(sf::Vector2f(C::SCREEN_WIDTH, C::SCREEN_HEIGHT));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");
	
	for (int i = 0; i < C::SCREEN_WIDTH / C::GRID_SIZE; ++i) 
		walls.push_back( Vector2i(i, lastLine) );

	walls.push_back(Vector2i(0, lastLine-1));
	walls.push_back(Vector2i(0, lastLine-2));
	walls.push_back(Vector2i(0, lastLine-3));

	walls.push_back(Vector2i(cols-1, lastLine - 1));
	walls.push_back(Vector2i(cols-1, lastLine - 2));
	walls.push_back(Vector2i(cols-1, lastLine - 3));

	walls.push_back(Vector2i(cols >>2, lastLine - 2));
	walls.push_back(Vector2i(cols >>2, lastLine - 3));
	walls.push_back(Vector2i(cols >>2, lastLine - 4));
	walls.push_back(Vector2i((cols >> 2) + 1, lastLine - 4));
	cacheWalls();

	auto sprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE * 2.0f});
	sprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2.0f);
	auto playerEnt =  new Entity(sprite);
	playerEnt->setCoordPixel(C::GRID_SIZE*5, C::SCREEN_HEIGHT - C::GRID_SIZE*2);
	entities.push_back(playerEnt);
}

void Game::cacheWalls()
{
	wallSprites.clear();
	for (Vector2i & w : walls) {
		sf::RectangleShape rect(Vector2f(16,16));
		rect.setPosition((float)w.x * C::GRID_SIZE, (float)w.y * C::GRID_SIZE);
		rect.setFillColor(sf::Color(0x07ff07ff));
		wallSprites.push_back(rect);
	}
}

void Game::processInput(sf::Event ev) {
	if (ev.type == sf::Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	if (ev.type == sf::Event::KeyReleased) {
		if(ev.key.code == Keyboard::E)
		{
			
		}
	}
}


static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double dt) {

	float lateralSpeed = 8.0;
	float maxSpeed = 40.0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		if(entities.size())
		{
			auto mainChar = entities[0];
			if(mainChar)
				mainChar->move(false);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		if(entities.size())
		{
			auto mainChar = entities[0];
			if(mainChar)
				mainChar->move(true);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		if(entities.size())
		{
			auto mainChar = entities[0];
			if(mainChar && !mainChar->jumping)
			{
				mainChar->setJumping(true);
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		if (!wasPressed) {
			onSpacePressed();
			wasPressed = true;
		}
	}
	else {
		wasPressed = false;
	}

}

static sf::VertexArray va;
static RenderStates vaRs;
static std::vector<sf::RectangleShape> rects;

int blendModeIndex(sf::BlendMode bm) {
	if (bm == sf::BlendAlpha) return 0;
	if (bm == sf::BlendAdd) return 1;
	if (bm == sf::BlendNone) return 2;
	if (bm == sf::BlendMultiply) return 3;
	return 4;
};

void Game::update(double dt) {
	pollInput(dt);

	g_time += dt;
	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);

	entities[0]->update(dt);
}

 void Game::draw(sf::RenderWindow & win) {
	if (closing) return;

	sf::RenderStates states = sf::RenderStates::Default;
	sf::Shader * sh = &bgShader->sh;
	states.blendMode = sf::BlendAdd;
	states.shader = sh;
	states.texture = &tex;
	sh->setUniform("texture", tex);
	//sh->setUniform("time", g_time);
	win.draw(bg, states);

	beforeParts.draw(win);

	for (sf::RectangleShape & r : wallSprites)
		win.draw(r);

	for (sf::RectangleShape& r : rects) 
		win.draw(r);

	for(Entity* entity : entities)
	{
		win.draw(*entity->sprite);
	}
	

	afterParts.draw(win);
}

void Game::onSpacePressed() {
	
}


bool Game::isWall(int cx, int cy)
{
	for (Vector2i & w : walls) {
		if (w.x == cx && w.y == cy)
			return true;
	}
	return false;
}

void Game::im()
{
	for(auto entity : entities)
	{
		entity->im();
	}
}

bool Game::hasCollision(float gx, float gy, int width, int height)
{
	if (gx < 0)
		return true;

	auto wallRightX = (C::SCREEN_WIDTH / C::GRID_SIZE);
	if (gx >= wallRightX)
		return true;

	for(auto wall : walls)
	{
		if(wall.x == (int)gx && wall.y == (int)gy)
			return true;

		if((wall.x == (int)(gx - width/2) || wall.x == (int)(gx + width/2)) && wall.y == (int)(gy - height/2))
			return true;
	}

	return false;
}




