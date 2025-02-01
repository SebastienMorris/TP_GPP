
#include <imgui.h>
#include <array>

#include "C.hpp"
#include "Game.hpp"

#include <fstream>

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
	Save("ResetSaveFile.txt");
	cacheWalls();

	CreateEntity();
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

	if(ev.type == sf::Event::MouseButtonPressed)
	{
		if(ev.mouseButton.button == Mouse::Left)
		{
			if(editMode)
				if(placeWallEnemytoggle)
					EplaceEnemy(ev.mouseButton.x, ev.mouseButton.y);
				else
					EplaceWall(ev.mouseButton.x, ev.mouseButton.y);
		}
	}
}


static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double dt) {

	if(!editMode)
	{
		float lateralSpeed = 8.0;
		float maxSpeed = 40.0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
			if(entities.size())
			{
				auto mainChar = entities[0];
				if(mainChar)
					mainChar->move(-1.0f);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			if(entities.size())
			{
				auto mainChar = entities[0];
				if(mainChar)
					mainChar->move(1.0f);
			}
		}

		if(sf::Joystick::hasAxis(0, Joystick::Axis::X))
		{
			float moveX = sf::Joystick::getAxisPosition(0, Joystick::Axis::X) / 100;
			if(moveX >= 0.1f || moveX <= -0.1f) //deadzone
			{
				if(entities.size())
				{
					auto mainChar = entities[0];
					if(mainChar)
						mainChar->move(moveX);
				}
			}
		}

		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Joystick::isButtonPressed(0, 0)) {
			if(entities.size())
			{
				auto mainChar = entities[0];
				if(mainChar && !mainChar->jumping)
				{
					mainChar->setJumping(true);
				}
			}
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Joystick::isButtonPressed(0, 1))
		{
			if(!wasPressedLControl)
			{
				auto mainChar = entities[0];
				if(mainChar)
				{
					mainChar->crouch();
				}
				wasPressedLControl = true;
			}
		}
		else
		{
			if(wasPressedLControl)
			{
				auto mainChar = entities[0];
				if(mainChar)
				{
					mainChar->uncrouch();
				}
				wasPressedLControl = false;
			}
		}
	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) {

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
			if (!wasPressedSpace) {
				onSpacePressed();
				wasPressedSpace = true;
			}
		}
		else {
			wasPressedSpace = false;
		}
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

	if(!editMode)
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

void Game::CreateEntity()
{
	auto standSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE * 2.0f});
	standSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2.0f);

	auto crouchSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE});
	crouchSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE);
	
	auto playerEnt =  new Entity(standSprite, crouchSprite);
	playerEnt->setCoordPixel(C::GRID_SIZE*5, C::SCREEN_HEIGHT - C::GRID_SIZE*2);
	entities.push_back(playerEnt);
}


bool Game::hasCollision(float gx, float gy)
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
	}

	return false;
}


bool Game::hasCollision(float gx, float gy, int height)
{
	if (gx < 0)
		return true;

	auto wallRightX = C::SCREEN_WIDTH / C::GRID_SIZE;
	if (gx >= wallRightX)
		return true;

	for(auto wall : walls)
	{
		if(wall.x == (int)gx && wall.y == (int)gy)
			return true;

		if(wall.x == (int)gx && wall.y == (int)(gy - (height - 1)))
			return true;
	}

	return false;
}

void Game::EplaceWall(int mouseX, int mouseY)
{
	int gridX = mouseX / C::GRID_SIZE;
	int gridY = mouseY / C::GRID_SIZE;

	for(auto wall : walls)
	{
		if(gridX == wall.x && gridY == wall.y)
		{
			walls.erase(std::find(walls.begin(), walls.end(), wall));
			cacheWalls();
			return;
		}
	}
	
	walls.push_back(Vector2i(gridX, gridY));
	cacheWalls();
}

void Game::EplaceEnemy(int mouseX, int mouseY)
{
	
}

void Game::Save(std::string filename)
{
	ofstream saveFile;
	saveFile.open(filename);
	if(!saveFile.is_open())
		return;
	
	for(auto wall : walls)
	{
		saveFile << std::to_string(wall.x) << "/" << std::to_string(wall.y) << endl;
	}
	saveFile.close();
}

void Game::Load(std::string filename)
{
	walls.clear();
	std::string line;
	ifstream loadFile;
	loadFile.open(filename);
	if(!loadFile.is_open())
		return;

	while(getline(loadFile, line))
	{
		int x = -1;
		int y = -1;

		std::string number;
		for(char c : line)
		{
			if(c == '/')
			{
				x = stoi(number);
				number = "";
			}
			else
				number += c;
		}
		y = stoi(number);
		walls.push_back(Vector2i(x,y));
	}
	loadFile.close();
	cacheWalls();
}


void Game::im()
{
	if(ImGui::CollapsingHeader("Editor", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* modeLabel = editMode ? "Switch Play Mode" : "Switch Edit Mode";
		if(ImGui::Button(modeLabel))
		{
			editMode = !editMode;
		}
		if(editMode)
		{
			ImGui::Indent(0);
			const char* placeLabel = placeWallEnemytoggle ? "Switch Place Wall" : "Switch Place Enemy";
			if(ImGui::Button(placeLabel))
			{
				placeWallEnemytoggle = !placeWallEnemytoggle;
			}

			if(ImGui::Button("Save"))
			{
				Save("SaveFile.txt");
			}

			if(ImGui::Button("Load"))
			{
				Load("SaveFile.txt");
			}

			if(ImGui::Button("Reset"))
			{
				Load("ResetSaveFile.txt");
			}
		}
	}
	ImGui::Unindent(0.5f);
	if(!editMode)
	{
		for(auto entity : entities)
		{
			entity->im();
		}
	}
}






