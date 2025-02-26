
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
	editView = &this->win->getDefaultView();
	playView = new sf::View(*editView);
	playView->zoom(0.5f);
	
	win->setView(*editView);
	
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
	
	CreatePlayer(5, 60);

	Save("ResetSaveFile.txt");
}

Game::~Game()
{
	delete playView;
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

	if(editMode)
	{
		if(ev.type == sf::Event::MouseButtonPressed)
		{
			if(ev.mouseButton.button == Mouse::Right)
			{
				currentMouseX = ev.mouseButton.x / C::GRID_SIZE;
				currentMouseY = ev.mouseButton.y / C::GRID_SIZE;
			
				if(placeWallEnemytoggle)
					EplaceEnemy(ev.mouseButton.x, ev.mouseButton.y);
				else
					EplaceWall(ev.mouseButton.x, ev.mouseButton.y);
				mouseRight = true;
			}
		}

		if(ev.type == sf::Event::MouseButtonReleased)
		{
			if(ev.mouseButton.button == Mouse::Right)
			{
				mouseRight = false;
			}
		}

		if(ev.type == sf::Event::MouseMoved)
		{
			int mouseX = ev.mouseMove.x / C::GRID_SIZE;
			int mouseY= ev.mouseMove.y / C::GRID_SIZE;
			
			if(mouseRight)
			{
				if(mouseX != currentMouseX || mouseY != currentMouseY)
				{
					if(placeWallEnemytoggle)
						EplaceEnemy(ev.mouseMove.x, ev.mouseMove.y);
					else
						EplaceWall(ev.mouseMove.x, ev.mouseMove.y);

					currentMouseX = mouseX;
					currentMouseY = mouseY;
				}
			}
		}
	}
	else
	{
		player->ProcessInput(ev);
	}
}


static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double dt) {
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

	if(editMode)
	{
		if(sf::Mouse::isButtonPressed(Mouse::Right))
		{
			
		}
	}
	else
	{
		player->PollInput(dt);
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
	{
		for(auto ent : entities)
			ent->update(dt);

		Vector3 secondOrderRes = secondOrderDynamics.Calculate(dt, {(player->cx + player->rx) * C::GRID_SIZE, (player->cy + player->ry) * C::GRID_SIZE, 0}, {player->dx, player->dy, 0});
		//playView->setCenter(player->getPosPixel().x, player->getPosPixel().y)
		playView->setCenter(secondOrderRes.x, secondOrderRes.y);
		win->setView(*playView);

	}
	else
	{
		win->setView(*editView);
	}
}

 void Game::draw(sf::RenderWindow& win) {
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

	for (sf::RectangleShape& r : wallSprites)
		win.draw(r);

	for (sf::RectangleShape& r : rects) 
		win.draw(r);

	for(Entity* entity : entities)
	{
		entity->draw(win);
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

void Game::CreatePlayer(int spawnX, int spawnY)
{
	auto standSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE * 2.0f});
	standSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2.0f);

	auto crouchSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE});
	crouchSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE);
	
	auto playerEnt =  new Player(standSprite, crouchSprite);
	playerEnt->setCoordPixel(spawnX * C::GRID_SIZE + C::GRID_SIZE * 0.5f, spawnY * C::GRID_SIZE);
	player = playerEnt;
	entities.push_back(playerEnt);
}

void Game::CreateEnemy(int spawnX, int spawnY)
{
	auto standSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE * 2.0f});
	standSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2.0f);
	standSprite->setFillColor(sf::Color::Blue);

	auto crouchSprite = new sf::RectangleShape({C::GRID_SIZE, C::GRID_SIZE});
	crouchSprite->setOrigin(C::GRID_SIZE * 0.5f, C::GRID_SIZE);
	crouchSprite->setFillColor(sf::Color::Blue);

	auto deadSprite = new sf::RectangleShape({C::GRID_SIZE * 2.0f, C::GRID_SIZE});
	deadSprite->setOrigin(C::GRID_SIZE, C::GRID_SIZE);
	deadSprite->setFillColor(sf::Color::Blue);

	auto enemyEnt = new Enemy(standSprite, crouchSprite, deadSprite);
	enemyEnt->setCoordPixel(spawnX * C::GRID_SIZE + C::GRID_SIZE * 0.5f, (spawnY + 1) * C::GRID_SIZE);
	enemies.push_back(enemyEnt);
	entities.push_back(enemyEnt);
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


bool Game::hasCollision(float gx, float gy, int height, Entity& self)
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

	for(auto ent : entities)
	{
		if(ent->isDead)
			continue;
		
		if(ent != &self)
		{
			if((int)gx == ent->cx && ((int)gy == ent->cy ||(int)gy == ent->cy -(ent->height - 1)))
				return true;

			if((int)gx == ent->cx && ((int)gy - (height - 1) == ent->cy ||(int)gy - (height - 1) == ent->cy -(ent->height - 1)))
				return true;
		}
	}

	return false;
}

Enemy* Game::hasCollisionEnemy(float gx, float gy)
{
	for(auto enemy : enemies)
	{
		if(enemy->isDead)
			continue;
			
		if((int)gx == enemy->cx && ((int)gy == enemy->cy ||(int)gy == enemy->cy -(enemy->height - 1)))
			return enemy;
	}
	return nullptr;
}

void Game::CamShake()
{
	secondOrderDynamics.StartShake(camShakeDuration, camShakeFrequency, camShakeAmplitude);
}



void Game::EplaceWall(int mouseX, int mouseY)
{
	int gridX = mouseX / C::GRID_SIZE;
	int gridY = mouseY / C::GRID_SIZE;
	
	if(eraseMode)
	{
		for(auto wall : walls)
		{
			if(gridX == wall.x && gridY == wall.y)
			{
				walls.erase(std::find(walls.begin(), walls.end(), wall));
				cacheWalls();
				return;
			}
		}
	}
	else
	{
		if(ECheckEntity(gridX, gridY, 0, 0, 1))
			return;
		
		walls.push_back(Vector2i(gridX, gridY));
	}
	cacheWalls();
}

void Game::EplaceEnemy(int mouseX, int mouseY)
{
	int gridX = mouseX / C::GRID_SIZE;
	int gridY = mouseY / C::GRID_SIZE;

	if(eraseMode)
	{
		for(auto enemy : enemies)
		{
			if(gridX == enemy->cx && (gridY == enemy->cy || gridY == enemy->cy - (enemy->height - 1)))
			{
				enemies.erase(std::find(enemies.begin(), enemies.end(), enemy));
				entities.erase(std::find(entities.begin(), entities.end(), enemy));
				return;
			}
		}
	}
	else
	{
		if(ECheckEntity(gridX, gridY, 1, 1, 2))
			return;
		
		CreateEnemy(gridX, gridY);
	}
}

bool Game::ECheckEntity(int gx, int gy, int xRange, int yRange, int height)
{
	for(auto wall : walls)
	{
		if(gx == wall.x && (gy == wall.y || gy - (height-1) == wall.y))
			return true;
	}
	
	for(int x = gx-xRange; x <= gx+xRange; x++)
	{
		for(int y = gy-yRange-(height-1); y < gy+yRange+(height-1); y++)
		{
			if(x == player->cx && (y == player->cy || y == player->cy - (player->height - 1)))
				return true;
			
			for(auto enemy : enemies)
			{
				if(x == enemy->cx && (y == enemy->cy || y == enemy->cy - (enemy->height - 1)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


void Game::Save(std::string filename)
{
	ofstream saveFile;
	saveFile.open(filename);
	if(!saveFile.is_open())
		return;
	
	for(auto wall : walls)
	{
		saveFile << std::to_string(wall.x) << " " << std::to_string(wall.y) << " 0" << endl;
	}

	for(auto enemy : enemies)
	{
		saveFile << std::to_string(enemy->cx) << " " << std::to_string(enemy->cy) << " 1" << endl;
	}

	saveFile << std::to_string(player->cx) << " " << std::to_string(player->cy) << " 2" << endl;
	
	saveFile.close();
}

void Game::Load(std::string filename)
{
	walls.clear();

	for(auto ent : entities)
		delete ent;
	entities.clear();
	enemies.clear();
	
	std::string line;
	ifstream loadFile;
	loadFile.open(filename);
	if(!loadFile.is_open())
		return;

	while(getline(loadFile, line))
	{
		int x = -1;
		int y = -1;
		int id = -1;

		std::string number;
		for(char c : line)
		{
			if(c == ' ')
			{
				if(x == -1)
				{
					x = stoi(number);
				}
				else if(y == -1)
				{
					y = stoi(number);
				}
				number = "";
			}
			else
				number += c;
		}
		id = stoi(number);


		switch (id)
		{
			case 0:
				walls.push_back(Vector2i(x,y));
				break;

			case 1:
				CreateEnemy(x, y);
				break;

			case 2:
				CreatePlayer(x, y);
		}
	}
	loadFile.close();
	cacheWalls();
}


void Game::im()
{
	if(ImGui::CollapsingHeader("Editor", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* modeLabel = editMode ? "Current Mode: EDIT" : "Current Mode: PLAY";
		if(ImGui::Button(modeLabel))
		{
			editMode = !editMode;
			if(!editMode)
				secondOrderDynamics.Reset(camFrequency, camDampening, camBouciness, {(player->cx + player->rx) * C::GRID_SIZE, (player->cy + player->ry) * C::GRID_SIZE, 0});
		}
		if(editMode)
		{
			ImGui::Indent(0);
			const char* placeLabel = placeWallEnemytoggle ? "Current Item: ENEMY" : "Current Item: WALL";
			if(ImGui::Button(placeLabel))
			{
				placeWallEnemytoggle = !placeWallEnemytoggle;
				eraseMode = false;
			}

			const char* eraseLabel = eraseMode ? "Erase Mode: ON" : "Erase mode: OFF";
			if(ImGui::Button(eraseLabel))
			{
				eraseMode = !eraseMode;
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
		else
		{
			ImGui::Indent(0);
			ImGui::DragFloat("cam frequency", &camFrequency, 0.1f, 0, 5);
			ImGui::DragFloat("cam dampening", &camDampening, 0.01f, 0, 1);
			ImGui::DragFloat("cam bounciness", &camBouciness, 0.1f, 0, 10);

			ImGui::DragFloat("cam shake duration", &camShakeDuration, 0.1f, 0, 10);
			ImGui::DragFloat("cam shake frequency", &camShakeFrequency, 0.1f, 0, 10);
			ImGui::DragFloat("cam shake amplitude", &camShakeAmplitude, 0.1f, 0, 10);
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






