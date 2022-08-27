#include "Assignment1.h"
#include "GL\glew.h"
#include "Application.h"
#include "SceneManager.h"
#include <sstream>
#include <cmath>

const float Assignment1::ROTATION_SPEED = 5.f;
const float Assignment1::MAX_ROTATION_SPEED = 0.7f;
const float Assignment1::GRAVITY_CONSTANT = 1.f;
Assignment1::Assignment1()

	:cSoundController(NULL)
{
}

Assignment1::~Assignment1()
{
	if (cSoundController)
	{
		cSoundController = NULL;
	}
}

static void Wrap(float& val, float bound)
{
	if (val < 0)
	{
		val += bound;
	}
	else if (val > bound)
	{
		val -= bound;
	}
}

void Assignment1::Init()
{
	SceneBase::Init();
	cSoundController = CSoundController::GetInstance();
	//Calculating aspect ratio
	m_worldHeight = 600.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	shipSpeed = 13.f;
	prevElapsedAsteroid = prevElapsedBullet = elapsedTime = waveTimer = prevElapsedCard = keyDelay = prevHealthRegen = tripleShotTimer = 0.0;
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i != 100; i++)
	{
		m_goList.push_back(new GameObject(GameObject::GO_GHOST));
	}

	//Exercise 2b: Initialize m_hp and m_score
	m_hp = 100;

	m_money = 100;

	waveCount = 1;

	gravity = -4;
	storystate = 1;
	hpFactor = moneyFactor = 1;
	bonusMoney = 1;
	iFrames = 0;
	fireTimer = 0;
	ringauraTimer = 0;
	explosionTimer = 0;

	shoppointer = 0;
	shopactive = false;
	fireRate = 5;
	fireRateCost = 100;
	damageUpCost = 300;
	cardCost = 100;
	ringCost = 200;
	bombCost = 100;
	molotovCost = 50;
	arrowCost = 100;
	flamingarrowCost = 1000;
	healthRegenCost = 250;

	tempSpawnCount = 0;
	tempWormCount = 0;
	WormMax = 2;
	spawnrate = 1;
	killcount = 0;
	enemycount = 0;

	shootCount = 0;
	bossState = 0;
	laserAngle = 0;



	basicBulletDamage = 1;
	healthRegen = false;
	regenlvl = 0;
	healthRegenAmount = 0;
	cardRate = 1;
	cardlvl = 0;
	ringlvl = 0;
	ringAOE = 6.0f;
	bomblvl = 0;
	bombRate = 0.75;
	molotovlvl = 0;
	molotovRate = 0.3;
	molotovAmount = 1;
	arrowlvl = 0;
	arrowRate = 0.5;
	arrowAmount = 6;
	flamingarrowlvl = 0;
	flamingarrowCost = 1000;
	fireratelvl = 0;
	damagelvl = 0;

	doubleBullet = false;
	tripleShot = false;
	flamingarrowUse = false;
	bombUse = false;
	arrowUse = false;
	molotovUse = false;
	cardUse = false;
	isAlive = true;
	gameStart = false;
	bossspawned = false;

	movementLastPressed = ' ';

	asteroidCount = 0;
	maxEnemyCount = 12;


	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_HERO);
	m_ship->active = true;
	m_ship->scale.Set(7, 7, 1);
	m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2);
	m_ship->vel.Set(1, 0, 0);
	m_ship->direction.Set(1, 0, 0);
	m_ship->hp = m_hp;
	m_ship->maxHP = m_hp;
	m_ship->mass = 0.1f;
	m_ship->prevEnemyBullet = 0;


	// Attract powerups
	GameObject* hole = FetchGO();
	hole->type = GameObject::GO_BLACKHOLE;
	hole->scale.Set(5, 5, 1);
	hole->mass = 1000;
	hole->pos = m_ship->pos;

	m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;

	cSoundController->LoadSound(FileSystem::getPath("Sound\\AMainMenu.ogg"), 1, true, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\AShop.ogg"), 2, true, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\AGamePlay1.ogg"), 3, true, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\AGamePlay2.ogg"), 4, true, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Explosion.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Magic.ogg"), 6, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Slash.ogg"), 7, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Grunt.wav"), 8, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\GameOver.ogg"), 9, true);

	//shop purchase
	cSoundController->LoadSound(FileSystem::getPath("Sound\\PurchaseRing.ogg"), 10, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\SwordSlash.wav"), 11, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\WormRoar.wav"), 12, true);

	cSoundController->LoadSound(FileSystem::getPath("Sound\\ABossTheme.ogg"), 13, true, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\FwooshFire.ogg"), 14, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Punch.ogg"), 15, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\Coins.ogg"), 16, true);

	// Shrek sfx
	cSoundController->LoadSound(FileSystem::getPath("Sound\\hey.wav"), 17, true);
	cSoundController->LoadSound(FileSystem::getPath("Sound\\bye-bye.wav"), 18, true);

	// Death sound
	cSoundController->LoadSound(FileSystem::getPath("Sound\\screaming-2.wav"), 19, true);

	// World map generation on game start
	int obstacleCount = 150;
	int chestCount = 10;

	int obstacleIndex = 0;
	float obstacleX, obstacleY;
	while (obstacleIndex < 150)
	{
		obstacleX = Math::RandFloatMinMax(3, m_worldWidth - 3);
		obstacleY = Math::RandFloatMinMax(3, m_worldHeight - 3);
		if (obstacleIndex > 0)
		{
			// Checks for surrounding area to spawn safely in
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->type == GameObject::GO_CHEST ||
					go->type == GameObject::GO_TREE)
				{
					// Prevent checking with itself
					if (go->pos.x != obstacleX &&
						go->pos.y != obstacleY)
					{

						// Spawn Chest far away from each other
						if (obstacleIndex < chestCount)
						{

							if (go->type == GameObject::GO_CHEST)
							{
								if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 34000.f &&
									go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 34000.f)
								{
									break;
								}
							}
							else
							{
								if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f &&
									m_ship->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f)
								{
									break;
								}
							}
						}
						else
						{
							// Preventing spawning obstacle inside another obstacle or player
							if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f ||
								m_ship->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f)
							{
								break;
							}
						}
					}
				}
				// Position does not interfere with other objects, spawn successful
				if (it + 1 >= m_goList.end())
				{
					GameObject* newObstacle = FetchGO();
					if (obstacleIndex < chestCount)
					{
						newObstacle->type = GameObject::GO_CHEST;
						newObstacle->pos.Set(obstacleX, obstacleY, 1);
						newObstacle->scale.Set(10, 8, 1);
					}
					else
					{
						newObstacle->type = GameObject::GO_TREE;
						newObstacle->pos.Set(obstacleX, obstacleY, 1);
						newObstacle->scale.Set(14, 16, 1);
					}
					newObstacle->direction = 0;
					newObstacle->vel = 0;
					newObstacle->timer = 0;
					newObstacle->hitboxSizeDivider = 3;
					newObstacle->moneyDrop = 300;
					obstacleIndex++;
					break;
				}
			}
		}
		else
		{
			GameObject* newObstacle = FetchGO();
			if (obstacleIndex < chestCount)
			{
				newObstacle->type = GameObject::GO_CHEST;
				newObstacle->pos.Set(obstacleX, obstacleY, 1);
				newObstacle->scale.Set(12, 12, 1);
			}
			else
			{
				newObstacle->type = GameObject::GO_TREE;
				newObstacle->pos.Set(obstacleX, obstacleY, 1);
				newObstacle->scale.Set(14, 16, 1);
			}
			newObstacle->direction = 0;
			newObstacle->vel = 0;
			newObstacle->hitboxSizeDivider = 3;
			newObstacle->timer = 0;
			newObstacle->moneyDrop = 300;
			obstacleIndex++;
		}
	}
	SceneBase::menuType = M_MAIN;
}

GameObject* Assignment1::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active) {
			continue;
		}
		go->active = true;
		return go;
	}

	//Get Size before adding 10
	int prevSize = m_goList.size();
	for (int i = 0; i < 100; ++i) {
		m_goList.push_back(new GameObject(GameObject::GO_GHOST));
	}
	m_goList.at(prevSize)->active = true;
	return m_goList.at(prevSize);
}

void Assignment1::RestartGame()
{
	for (GameObject* go : m_goList)
	{
		if (go->active)
			go->active = false;
	}
	//Exercise 2b: Initialize m_hp and m_score

	m_hp = 100;
	m_money = 100;

	waveCount = 1;
	gravity = -4;
	storystate = 1;
	hpFactor = moneyFactor = 1;
	bonusMoney = 1;
	iFrames = 0;
	fireTimer = 0;
	ringauraTimer = 0;
	explosionTimer = 0;
	chestTimer = 0;

	shoppointer = 0;
	shopactive = false;
	fireRate = 5;
	fireRateCost = 100;
	damageUpCost = 300;
	cardCost = 100;
	ringCost = 200;
	bombCost = 100;
	molotovCost = 50;
	arrowCost = 100;
	flamingarrowCost = 1000;
	healthRegenCost = 250;

	tempSpawnCount = 0;
	tempWormCount = 0;
	WormMax = 2;
	spawnrate = 1;
	killcount = 0;
	enemycount = 0;

	shootCount = 0;
	bossState = 0;
	laserAngle = 0;



	basicBulletDamage = 1;
	healthRegen = false;
	healthRegenAmount = 0;
	cardRate = 1;
	cardlvl = 0;
	ringlvl = 0;
	ringAOE = 6.0f;
	bomblvl = 0;
	bombRate = 0.75;
	molotovlvl = 0;
	molotovRate = 0.3;
	molotovAmount = 1;
	arrowlvl = 0;
	arrowRate = 0.5;
	arrowAmount = 6;
	flamingarrowlvl = 0;
	flamingarrowCost = 1000;
	fireratelvl = 0;
	damagelvl = 0;
	gameOverTimer = 0;

	doubleBullet = false;
	tripleShot = false;
	flamingarrowUse = false;
	ringUse = false;
	bombUse = false;
	arrowUse = false;
	molotovUse = false;
	cardUse = false;
	isAlive = true;
	gameStart = false;
	bossspawned = false;
	bombChoose = false;
	upgradescreen = false;
	arrowChoose = false;
	cardChoose = false;


	movementLastPressed = ' ';

	asteroidCount = 0;
	maxEnemyCount = 12;


	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_HERO);
	m_ship->active = true;
	m_ship->scale.Set(9, 9, 1);
	m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2);
	m_ship->vel.Set(1, 0, 0);
	m_ship->direction.Set(1, 0, 0);
	m_ship->hp = m_hp;
	m_ship->maxHP = m_hp;
	m_ship->mass = 0.1f;

	// Attract powerups
	GameObject* hole = FetchGO();
	hole->type = GameObject::GO_BLACKHOLE;
	hole->scale.Set(5, 5, 1);
	hole->mass = 1000;
	hole->pos = m_ship->pos;
	hole->vel.SetZero();

	//spawn and reset enemy station and turrets

	// World map generation on game start
	int obstacleCount = 150;
	int chestCount = 10;

	int obstacleIndex = 0;
	float obstacleX, obstacleY;
	while (obstacleIndex < 150)
	{
		obstacleX = Math::RandFloatMinMax(3, m_worldWidth - 3);
		obstacleY = Math::RandFloatMinMax(3, m_worldHeight - 3);
		if (obstacleIndex > 0)
		{
			// Checks for surrounding area to spawn safely in
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->type == GameObject::GO_CHEST ||
					go->type == GameObject::GO_TREE)
				{
					// Prevent checking with itself
					if (go->pos.x != obstacleX &&
						go->pos.y != obstacleY)
					{

						// Spawn Chest far away from each other
						if (obstacleIndex < chestCount)
						{

							if (go->type == GameObject::GO_CHEST)
							{
								if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 34000.f &&
									go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 34000.f)
								{
									break;
								}
							}
							else
							{
								if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f &&
									m_ship->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f)
								{
									break;
								}
							}
						}
						else
						{
							// Preventing spawning obstacle inside another obstacle or player
							if (go->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f ||
								m_ship->pos.DistanceSquared(Vector3(obstacleX, obstacleY, 1)) < 400.0f)
							{
								break;
							}
						}
					}
				}
				// Position does not interfere with other objects, spawn successful
				if (it + 1 >= m_goList.end())
				{
					GameObject* newObstacle = FetchGO();
					if (obstacleIndex < chestCount)
					{
						newObstacle->type = GameObject::GO_CHEST;
						newObstacle->pos.Set(obstacleX, obstacleY, 1);
						newObstacle->scale.Set(10, 8, 1);
					}
					else
					{
						newObstacle->type = GameObject::GO_TREE;
						newObstacle->pos.Set(obstacleX, obstacleY, 1);
						newObstacle->scale.Set(14, 16, 1);
					}
					newObstacle->direction = 0;
					newObstacle->vel = 0;
					newObstacle->timer = 0;
					newObstacle->hitboxSizeDivider = 3;
					newObstacle->moneyDrop = 300;
					obstacleIndex++;
					break;
				}
			}
		}
		else
		{
			GameObject* newObstacle = FetchGO();
			if (obstacleIndex < chestCount)
			{
				newObstacle->type = GameObject::GO_CHEST;
				newObstacle->pos.Set(obstacleX, obstacleY, 1);
				newObstacle->scale.Set(12, 12, 1);
			}
			else
			{
				newObstacle->type = GameObject::GO_TREE;
				newObstacle->pos.Set(obstacleX, obstacleY, 1);
				newObstacle->scale.Set(14, 16, 1);
			}
			newObstacle->direction = 0;
			newObstacle->vel = 0;
			newObstacle->hitboxSizeDivider = 3;
			newObstacle->timer = 0;
			newObstacle->moneyDrop = 300;
			obstacleIndex++;
		}
	}
}

void Assignment1::SpawnWorm()
{
	if ((waveCount == 10 || waveCount == 15 || waveCount > 20) && tempWormCount < 1)
	{
		for (int i = 0; i < WormMax; ++i)
		{
			GameObject* prevBody;
			int segmentCount = 30;
			for (int i = 0; i < segmentCount; i++)
			{
				GameObject* go = FetchGO();
				if (i == 0)
				{
					go->type = GameObject::GO_WORMHEAD;
					go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
					go->scale.Set(10, 10, 1);
					go->vel.Set(2, 2, 0);
					go->timer = 10;

					go->enemyDamage = 35;

					go->prevNode = nullptr;
					go->nextNode = nullptr;
					go->hitboxSizeDivider = 1.8;
				}
				else if (i == segmentCount - 1)
				{
					go->type = GameObject::GO_WORMTAIL;
					go->pos = prevBody->pos;
					go->scale.Set(10, 10, 1);
					go->vel.Set(0, 0, 0);

					go->enemyDamage = 20;

					go->prevNode = prevBody;
					go->prevNode->nextNode = go;
					go->nextNode = nullptr;
					go->hitboxSizeDivider = 3;
				}
				else
				{

					if (prevBody->type == GameObject::GO_WORMHEAD ||
						prevBody->type == GameObject::GO_WORMBODY2)
					{
						go->type = GameObject::GO_WORMBODY1;
					}
					else
					{
						go->type = GameObject::GO_WORMBODY2;
					}

					go->pos = prevBody->pos;
					go->vel.Set(0, 0, 0);
					go->scale.Set(10, 10, 1);

					go->enemyDamage = 20;

					go->prevNode = prevBody;
					go->prevNode->nextNode = go;
					go->nextNode = nullptr;
					go->hitboxSizeDivider = 3;
				}
				go->hp = 33 * sqrt(waveCount - 9);
				go->maxHP = go->hp;
				go->prevEnemyBullet = elapsedTime;
				go->speedFactor = 1;


				go->facingLeft = true;
				go->reachTarget = true;
				prevBody = go;
			}
		}
		tempWormCount++;
	}
}

void Assignment1::SpawnBoss()
{
	if ((waveCount == 20 || waveCount == 25 || waveCount > 25) && tempSpawnCount < 1 && bossspawned == false)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_BOSS;
		go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
		go->vel.Set(Math::RandFloatMinMax(-20, 20), Math::RandFloatMinMax(-20, 20), 0);
		go->scale.Set(30, 30, 1);
		go->hp = 10000;
		go->maxHP = go->hp;
		go->prevEnemyBullet = elapsedTime;
		go->speedFactor = 1;
		go->hitboxSizeDivider = 8;
		go->enemyDamage = 25;
		go->facingLeft = true;
		tempSpawnCount++;
		enemycount++;

		bossspawned = true;
	}
}

void Assignment1::UpdateMenu()
{
	if (menuType == M_NONE)
		m_speed = 1;
	else
		m_speed = 0;

	if (Application::IsKeyReleased(VK_ESCAPE))
	{
		SceneBase::menuType = M_PAUSE;
	}

	if (SceneBase::restartGame)
	{
		RestartGame();
		SceneBase::menuType = M_NONE;
		SceneBase::restartGame = false;
		SceneManager::activeScene = S_ASSIGNMENT1;
	}

	if (SceneBase::resetGame)
	{
		RestartGame();
		SceneBase::resetGame = false;
		SceneBase::menuType = M_MAIN;
		SceneManager::activeScene = S_ASSIGNMENT1;
	}
	if (Application::IsKeyReleased(VK_ESCAPE))
		SceneBase::menuType = M_PAUSE;

	// For shop
	if (menuType == M_UPGRADE && selectorIndex != 8)
	{
		if (Application::IsKeyReleased(VK_LEFT) && selectorIndex - 4 >= 0)
			selectorIndex -= 4;
		if (Application::IsKeyReleased(VK_RIGHT) && selectorIndex + 4 <= 7)
			selectorIndex += 4;

		// Exit shop option

		if (Application::IsKeyReleased(VK_DOWN))
		{
			if (selectorIndex == 3)
			{
				selectorIndex = 8;
			}
			else
			{
				selectorIndex++;
			}
		}
		else if (Application::IsKeyReleased(VK_UP))
		{
			if (selectorIndex != 4)
			{
				selectorIndex--;
			}
		}
		//	selectorIndex--;
		//if (Application::IsKeyReleased(VK_DOWN) && selectorIndex == 3)
		//	selectorIndex = 8;
		//else if (Application::IsKeyReleased(VK_UP) && selectorIndex != 4)
		//	selectorIndex++;
		//else if (Application::IsKeyReleased(VK_DOWN) && selectorIndex == 7)
		//	selectorIndex = 8;
		//else if (Application::IsKeyReleased(VK_DOWN))
		//	selectorIndex++;
	}
	// Other menu types
	else
	{
		if (Application::IsKeyReleased(VK_DOWN))
			selectorIndex++;
		else if (Application::IsKeyReleased(VK_UP))
			selectorIndex--;
	}



	//if (Application::IsKeyReleased(VK_RIGHT))
	//	colourIndex++;
	//else if (Application::IsKeyReleased(VK_LEFT))
	//	colourIndex--;

	if (Application::IsKeyReleased(VK_RETURN))
	{
		switch (menuType)
		{
		case M_MAIN:
			UpdateMainMenu(m_speed);

			break;
		case M_PAUSE:
			UpdatePauseMenu(m_speed);
			break;
		case M_GAMEOVER:
			UpdateGameOver(m_speed);
			break;
		case M_CHOOSE:
			UpdateChoose(m_speed);
			break;
		case M_ARROW:
			UpdateCArrow(m_speed);
			break;
		case M_BOMB:
			UpdateCBomb(m_speed);
			break;
		case M_CARD:
			UpdateCCard(m_speed);
			break;
		case M_CONTROL:
			UpdateControl(m_speed);
			break;
		case M_UPGRADE:

			// Shop leaving sfx
			if (selectorIndex == 8 && menuType == M_UPGRADE)
			{
				cSoundController->PlaySoundByID(18);
			}
			UpdateUpgrade(m_speed);
			break;
		case M_BOSSCONTROL:
			UpdateBossControl(m_speed);
			break;
		case M_SHOPCONTROL:
			UpdateShopControl(m_speed);
			break;
		}
	}
}

void Assignment1::Update(double dt)
{
	//main menu
	if (menuType == M_MAIN)
	{
		cSoundController->StopSoundByID(9);
		cSoundController->StopSoundByID(2);
		cSoundController->StopSoundByID(3);
		cSoundController->StopSoundByID(4);
		cSoundController->StopSoundByID(7);
		cSoundController->StopSoundByID(8);
		cSoundController->StopSoundByID(13);

		cSoundController->PlaySoundByID(1);
	}
	//shop
	if (menuType == M_UPGRADE)
	{
		cSoundController->StopSoundByID(9);
		cSoundController->StopSoundByID(1);
		cSoundController->StopSoundByID(3);
		cSoundController->StopSoundByID(4);
		cSoundController->StopSoundByID(7);
		cSoundController->StopSoundByID(8);
		cSoundController->StopSoundByID(13);

		cSoundController->PlaySoundByID(2);
	}
	if (keyDelay > 0)
	{
		keyDelay -= 1.0 * dt;
	}
	else if (bombChoose == true)
	{
		keyDelay = 0.3;
		if (m_money >= bombCost)
		{
			if (bombCost < 110)
			{
				bomblvl++;
				bombUse = true;

			}
			else
			{
				if (bomblvl <= 8)
				{
					bombRate += 0.35;
					bomblvl++;

				}
			}
			if (bomblvl <= 8)
			{
				m_money -= bombCost;
				bombCost += 55;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}

		}
		bombChoose = false;

	}
	else if (cardChoose == true)
	{
		keyDelay = 0.3;
		if (m_money >= cardCost)
		{
			if (cardCost < 110)
			{
				cardlvl++;
				cardUse = true;

			}
			else
			{
				if (cardlvl <= 8)
				{
					cardRate += 0.5;
					cardlvl++;

				}
			}

			if (cardlvl <= 8)
			{
				m_money -= cardCost;
				cardCost += 50;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
		}
		cardChoose = false;
	}
	else if (arrowChoose == true)
	{
		keyDelay = 0.3;
		if (m_money >= arrowCost)
		{

			if (arrowlvl == 9 && molotovlvl == 9 && flamingarrowCost == 1000)
			{
				flamingarrowUse = true;
				flamingarrowlvl++;
				m_money -= flamingarrowCost;
				flamingarrowCost += 1000;
				cSoundController->PlaySoundByID(12);
			}

			else if (arrowCost < 100)
			{
				arrowUse = true;
				arrowlvl++;
			}

			else if (arrowCost > 30)
			{
				if (arrowlvl <= 4)
				{
					arrowAmount++;
					arrowlvl++;
				}
				else if (arrowlvl <= 8)
				{
					arrowRate += 0.15;
					arrowlvl++;
				}
			}

			if (arrowlvl <= 8)
			{
				m_money -= arrowCost;
				arrowCost += 50;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
		}
		arrowChoose = false;
	}
	else if (healthUpgrade == true)
	{
		keyDelay = 0.3;
		if (m_money >= healthRegenCost)
		{
			if (healthRegenCost < 400)
			{
				healthRegen = true;
				regenlvl++;
			}
			else if (regenlvl <= 2)
			{
				healthRegenAmount++;
				regenlvl++;
			}

			if (regenlvl <= 2)
			{
				m_money -= healthRegenCost;
				healthRegenCost += 300;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
		}
		healthUpgrade = false;
	}
	else if (ringUpgrade == true)
	{
		keyDelay = 0.3;
		if (m_money >= ringCost)
		{
			if (ringCost < 300)
			{
				ringUse = true;

				ringlvl++;
			}
			else
			{
				if (ringlvl <= 4)
				{
					ringAOE += 2.0;
					ringlvl++;
				}
			}
			if (ringlvl <= 4)
			{
				m_money -= ringCost;
				ringCost += 150;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
			ringUpgrade = false;
		}
	}
	else if (molotovUpgrade == true)
	{
		keyDelay = 0.3;
		if (m_money >= molotovCost)
		{
			if (molotovCost < 60)
			{
				molotovUse = true;
				molotovlvl++;
			}
			else
			{
				if (molotovlvl <= 3)
				{
					molotovAmount++;
					molotovlvl++;
				}
				else if (molotovlvl <= 8)
				{
					molotovRate += 0.15;
					molotovlvl++;
				}
			}
			if (molotovlvl <= 8)
			{
				m_money -= molotovCost;
				molotovCost += 35;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}

		}
		molotovUpgrade = false;
	}

	if (firerateUpgrade == true)
	{
		keyDelay = 0.3;
		if (m_money >= fireRateCost)
		{
			if (fireratelvl <= 8)
			{
				fireRate += 0.5;
				m_money -= fireRateCost;
				fireRateCost += 400;
				fireratelvl++;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
		}
		firerateUpgrade = false;
	}
	if (damageUpgrade == true)
	{
		keyDelay = 0.3;
		if (m_money >= damageUpCost)
		{
			if (damagelvl <= 8)
			{
				m_money -= damageUpCost;
				basicBulletDamage++;
				damageUpCost += 200 * (damagelvl + 1);
				damagelvl++;
				cSoundController->StopSoundByID(10);
				cSoundController->PlaySoundByID(10);
			}
		}
		damageUpgrade = false;
	}


	SceneBase::Update(dt);
	UpdateMenu();
	deltaTime = dt;


	//dont update anything if in menu
	if (menuType != M_NONE)
		return;


	SceneBase::Update(dt);
	elapsedTime += dt;
	if (isAlive == true)
	{
		cSoundController->PlaySoundByID(1);
	}
	//Calculating aspect ratio
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Fire
	FireSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_FIRE]);
	FireSprite->PlayAnimation("Fire", -1, 1.0f);
	FireSprite->Update(dt);

	// Explosion
	ExplosionSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLOSION]);
	ExplosionSprite->PlayAnimation("Explode", -1, 1.0f);
	ExplosionSprite->Update(dt);

	// Barrier
	BarrierSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_RINGAURA]);
	BarrierSprite->PlayAnimation("Aura", -1, 2.0f);
	BarrierSprite->Update(dt);

	// Purple Shot
	PurpleShot = dynamic_cast<SpriteAnimation*>(meshList[GEO_LASER]);
	PurpleShot->PlayAnimation("Purple Projectile", -1, 2.0f);
	PurpleShot->Update(dt);

	ChestSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_CHEST]);
	// Chest Opening Animation
	if (chestTimer <= 0)
	{
		ChestSprite->PlayAnimation("IDLE", -1, 1.0f);
	}
	ChestSprite->Update(dt);

	Chestparticlesprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_CHEST_PARTICLE]);
	// Chest Opening Animation
	if (chestTimer <= 0)
	{
		Chestparticlesprite->PlayAnimation("OPEN", -1, 1.0f);
	}
	Chestparticlesprite->Update(dt);


	// Enter to begin game
	if (!gameStart)
	{
		if (keyDelay > 0)
		{
			keyDelay -= 1.0 * dt;
		}
		if (Application::IsKeyReleased(' '))
		{
			if (keyDelay <= 0)
			{
				keyDelay = 1.5;
				storystate += 1;
			}

		}
	}

	else if (gameStart && upgradescreen == false && isAlive == true)
	{

		// Hero
		HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERO]);
		HeroSprite->PlayAnimation("IDLE", -1, 0.5f);
		HeroSprite->Update(dt);

		HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERO_LEFT]);
		HeroSprite->PlayAnimation("IDLE", -1, 0.5f);
		HeroSprite->Update(dt);

		// Shrek shopkeeper
		ShrekSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_SHREK]);
		ShrekSprite->PlayAnimation("IDLE", -1, 0.8f);
		ShrekSprite->Update(dt);

		ShrekSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_SHREK_LEFT]);
		ShrekSpriteLeft->PlayAnimation("IDLE", -1, 0.8f);
		ShrekSpriteLeft->Update(dt);

		// Ghost
		GhostSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_GHOST]);
		GhostSprite->PlayAnimation("IDLE", -1, 0.5f);
		GhostSprite->Update(dt);

		GhostSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_GHOST_LEFT]);
		GhostSpriteLeft->PlayAnimation("IDLE", -1, 0.5f);
		GhostSpriteLeft->Update(dt);

		// Nightmare
		NightmareSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_NIGHTMARE]);
		NightmareSprite->PlayAnimation("IDLE", -1, 0.3f);
		NightmareSprite->Update(dt);

		NightmareSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_NIGHTMARE_LEFT]);
		NightmareSpriteLeft->PlayAnimation("IDLE", -1, 0.3f);
		NightmareSpriteLeft->Update(dt);

		// Flame Demon
		FdemonSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_FDEMON]);
		FdemonSprite->PlayAnimation("IDLE", -1, 0.5f);
		FdemonSprite->Update(dt);

		FdemonSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_FDEMON_LEFT]);
		FdemonSpriteLeft->PlayAnimation("IDLE", -1, 0.5f);
		FdemonSpriteLeft->Update(dt);

		// B Demon (dunno what b stands for)
		BdemonSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BDEMON]);
		BdemonSprite->PlayAnimation("IDLE", -1, 1.0f);
		BdemonSprite->Update(dt);

		BdemonSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BDEMON_LEFT]);
		BdemonSpriteLeft->PlayAnimation("IDLE", -1, 1.0f);
		BdemonSpriteLeft->Update(dt);

		// Exploding Enemy 
		ExploderSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLODER]);
		ExploderSprite->PlayAnimation("IDLE", -1, 1.0f);
		ExploderSprite->Update(dt);

		ExploderSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLODER_LEFT]);
		ExploderSpriteLeft->PlayAnimation("IDLE", -1, 1.0f);
		ExploderSpriteLeft->Update(dt);


		if (bossspawned == false && isAlive == true)
		{
			cSoundController->StopSoundByID(1);
			cSoundController->StopSoundByID(2);
			cSoundController->StopSoundByID(3);
			cSoundController->StopSoundByID(13);

			cSoundController->PlaySoundByID(4);
		}
		else if (bossspawned == true && isAlive == true)
		{
			cSoundController->StopSoundByID(1);
			cSoundController->StopSoundByID(2);
			cSoundController->StopSoundByID(3);
			cSoundController->StopSoundByID(4);

			cSoundController->PlaySoundByID(13);
		}


		// ************************* CURSOR CODE ****************************************
		{

			Application::GetCursorPos(&worldPosX, &worldPosY);

			// Converting to world space

			worldPosX /= 10;
			worldPosY /= 10;

			// CHANGE BELOW if m_worldheight/m_worldwidth is changed
			// eg. m_worldHeight is 100
			//     then set below values to 1
			//
			worldPosX *= 1.5;
			worldPosY *= 1.5;

			worldPosX += camera.position.x;
			worldPosY -= camera.position.y;


			worldPosY = m_worldHeight - (worldPosY)-450;
			//worldPosX += 5;

			m_ship->angle = atan2(m_ship->pos.y - worldPosY, m_ship->pos.x - worldPosX);
			m_ship->angle = (m_ship->angle / Math::PI) * 180.0;


			camera.position.x = m_ship->pos.x - m_worldWidth * 0.125;
			camera.position.y = m_ship->pos.y - m_worldHeight * 0.125;
			camera.target.x = m_ship->pos.x - m_worldWidth * 0.125;
			camera.target.y = m_ship->pos.y - m_worldHeight * 0.125;


			//camera.position.x = Math::Clamp(camera.position.x, 0.f, m_worldWidth / 2);
			//camera.position.y = Math::Clamp(camera.position.y, 0.f, m_worldHeight / 2);
			//camera.target.x = Math::Clamp(camera.target.x, 0.f, m_worldWidth / 2);
			//camera.target.y = Math::Clamp(camera.target.y, 0.f, m_worldHeight / 2);
		}
		// **************************************************************************



		m_force = Vector3(0, 0, 0);
		m_ship->vel = Vector3(0, 0, 0);

		m_force.SetZero();
		m_torque.SetZero();
		//Exercise 6: set m_force values based on WASD
		if (Application::IsKeyPressed('W'))
		{
			m_ship->direction = Vector3(0, 1, 0);
			m_ship->vel = Vector3(0, 20, 0);
			/*m_force += m_ship->direction * 4.f;*/
		}
		if (Application::IsKeyPressed('A'))
		{
			m_ship->direction = Vector3(-1, 0, 0);
			m_ship->vel = Vector3(-20, 0, 0);
			heroFacingLeft = true;
			//m_force += m_ship->direction * ROTATION_SPEED;
			//m_torque += Vector3(-m_ship->scale.x, -m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
			//if (movementLastPressed == 'D')
			//{
			//	m_ship->angularVelocity = 0;
			//}
			//movementLastPressed = 'A';
		}
		if (Application::IsKeyPressed('S'))
		{
			m_ship->direction = Vector3(0, -1, 0);
			m_ship->vel = Vector3(0, -20, 0);

		}
		if (Application::IsKeyPressed('D'))
		{
			m_ship->direction = Vector3(1, 0, 0);
			m_ship->vel = Vector3(20, 0, 0);
			heroFacingLeft = false;
			//m_force += m_ship->direction * ROTATION_SPEED;
			//m_torque += Vector3(-m_ship->scale.x, m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
			//if (movementLastPressed == 'A')
			//{
			//	m_ship->angularVelocity = 0;
			//}
			//movementLastPressed = 'D';
		}

		if (Application::IsKeyPressed('W') ||
			Application::IsKeyPressed('A') ||
			Application::IsKeyPressed('S') ||
			Application::IsKeyPressed('D'))
		{
			// Running Animations
			if (heroFacingLeft == true)
			{
				HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERORUN_LEFT]);
				HeroSprite->PlayAnimation("Run", -1, 1.0f);
				HeroSprite->Update(dt);
			}
			else
			{
				HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERORUN]);
				HeroSprite->PlayAnimation("Run", -1, 1.0f);
				HeroSprite->Update(dt);
			}
		}

		// Wave count increases after a certain period
		float diff = elapsedTime - waveTimer;
		if (diff > 25)
		{
			waveCount++;
			if (waveCount < 20)
			{
				hpFactor += 0.2;
				moneyFactor += 0.35;
				bonusMoney ++;
				maxEnemyCount += 4;
			}
			else if (waveCount > 20)
			{
				WormMax = 4;
				maxEnemyCount = 100;
				moneyFactor = 6;
				hpFactor += 1;
				spawnrate = 0.5;
			}
			waveTimer = elapsedTime;
		}


		// Randomised enemy spawns
		diff = elapsedTime - prevElapsedAsteroid;
		//only spawn if current object count is less than enemy cap
		if (diff > spawnrate && enemycount < maxEnemyCount)
		{
			for (int i = 0; i < 2; ++i)
			{
				GameObject* go = FetchGO();
				int randomEnemy = rand() % 100;

				//wave 1 to 2
				if (waveCount <= 2)
				{
					go->type = GameObject::GO_GHOST;
					go->hp = round(1 * hpFactor);
					go->scale.Set(10, 10, 1);
					go->hitboxSizeDivider = 3.5;
					go->enemyDamage = 2;
				}
				//wave 3
				else if (waveCount == 3)
				{
					for (int j = 0; j < 35; ++j)
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(2 * hpFactor);
						go->scale.Set(14, 14, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 5;
					}
				}
				//wave 4
				else if (waveCount == 4)
				{
					if (randomEnemy <= 5)
					{
						go->type = GameObject::GO_BDEMON;
						go->hp = round(5 * hpFactor);
						go->scale.Set(14, 14, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 5;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(2 * hpFactor);
						go->scale.Set(10, 10, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 4;
					}
				}
				//special wave 5
				else if (waveCount == 5)
				{
					if (randomEnemy <= 5)
					{

						go->type = GameObject::GO_BDEMON;
						go->hp = round(5 * hpFactor);
						go->scale.Set(14, 14, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 5;
					}
					else if (5 < randomEnemy && randomEnemy <= 15)
					{
						go->type = GameObject::GO_NIGHTMARE;
						go->hp = round(10 * hpFactor);
						go->scale.Set(20, 18, 1);
						go->hitboxSizeDivider = 4.5;
						go->enemyDamage = 10;
					}
					else if (15 < randomEnemy && randomEnemy <= 45)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(15, 15, 1);
						go->hp = round(3 * hpFactor);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 10;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(2 * hpFactor);
						go->scale.Set(10, 10, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 2;
					}
				}

				//wave 6 to 9
				else if (5 < waveCount <= 9)
				{
					if (randomEnemy <= 5)
					{
						go->type = GameObject::GO_BDEMON;
						go->hp = round(6 * hpFactor);
						go->scale.Set(14, 14, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 5;

					}
					else if (5 < randomEnemy && randomEnemy <= 10)
					{
						go->type = GameObject::GO_NIGHTMARE;
						go->hp = round(10 * hpFactor);
						go->scale.Set(20, 18, 1);
						go->hitboxSizeDivider = 4.5;
						go->enemyDamage = 10;
					}
					else if (10 < randomEnemy && randomEnemy <= 25)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(15, 15, 1);
						go->hp = round(3 * hpFactor);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 10;
					}
					else if (25 < randomEnemy && randomEnemy <= (40 + waveCount * 1.2))
					{
						go->type = GameObject::GO_EXPLODER;
						go->hp = round(2 * hpFactor);
						go->scale.Set(15, 15, 1);
						go->hitboxSizeDivider = 4.5;
						go->enemyDamage = 15;
						go->angle = 0;
						go->maxHP = go->hp;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(1 * hpFactor);
						go->scale.Set(15, 15, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 2;
					}
				}
				//special wave 10
				else if (waveCount == 10)
				{
					if (randomEnemy <= 35)
					{

						go->type = GameObject::GO_EXPLODER;
						go->hp = round(1 * hpFactor);
						go->scale.Set(20, 20, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 15;

					}
					else if (35 < randomEnemy && randomEnemy <= 50)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(10, 10, 1);
						go->hp = round(4 * hpFactor);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 10;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(1 * hpFactor);
						go->scale.Set(15, 15, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 2;
					}
				}
				else if (10 < waveCount <= 20)
				{
					if (randomEnemy <= 10)
					{
						go->type = GameObject::GO_EXPLODER;
						go->hp = round(1 * hpFactor);
						go->scale.Set(16, 16, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 25;
					}
					else if (10 < randomEnemy && randomEnemy <= 30)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(25, 25, 1);
						go->hp = round(8 * hpFactor);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 15;
					}
					else if (30 < randomEnemy <= 45)
					{
						go->type = GameObject::GO_BDEMON;
						go->hp = round(12 * hpFactor);
						go->scale.Set(10, 10, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 15;

					}
					else if (45 < randomEnemy && randomEnemy <= 65)
					{
						go->type = GameObject::GO_NIGHTMARE;
						go->hp = round(15 * hpFactor);
						go->scale.Set(20, 18, 1);
						go->hitboxSizeDivider = 4.5;
						go->enemyDamage = 20;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(1 * hpFactor);
						go->scale.Set(15, 15, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 4;
					}
				}
				else if (waveCount > 20)
				{
					if (randomEnemy <= 10)
					{

						go->type = GameObject::GO_EXPLODER;
						go->hp = round(1 * hpFactor);
						go->scale.Set(16, 16, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 25;

					}
					else if (10 < randomEnemy && randomEnemy <= 20)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(25, 25, 1);
						go->hp = round(8 * hpFactor);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 15;
					}
					else if (20 < randomEnemy <= 25)
					{
						go->type = GameObject::GO_BDEMON;
						go->hp = round(8 * hpFactor);
						go->scale.Set(10, 10, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 15;

					}
					else if (25 < randomEnemy && randomEnemy <= 35)
					{
						go->type = GameObject::GO_NIGHTMARE;
						go->hp = round(20 * hpFactor);
						go->scale.Set(20, 18, 1);
						go->hitboxSizeDivider = 4.5;
						go->enemyDamage = 20;
					}
					else if (36 < randomEnemy <= 42)
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(4000 * hpFactor);
						go->scale.Set(35, 35, 1);
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 50;
					}
					else if (42 < randomEnemy <= 44)
					{
						SpawnBoss();
						SpawnWorm();
					}
					else if (44 < randomEnemy <= 48)
					{
						SpawnWorm();
					}
					else if (48 < randomEnemy <= 54)
					{
						go->type = GameObject::GO_FLAMEDEMON;
						go->scale.Set(45, 45, 1);
						go->hp = round(500);
						go->maxHP = go->hp;
						go->prevEnemyBullet = elapsedTime;
						go->speedFactor = 1;
						go->hitboxSizeDivider = 2.8;
						go->enemyDamage = 35;
					}
					else if (54 < randomEnemy <= 58)
					{

						go->type = GameObject::GO_EXPLODER;
						go->hp = round(1 * hpFactor);
						go->scale.Set(25, 25, 1);
						go->prevEnemyBullet = 0.0;
						go->hitboxSizeDivider = 3;
						go->enemyDamage = 55;
					}
					else
					{
						go->type = GameObject::GO_GHOST;
						go->hp = round(2 * hpFactor);
						go->scale.Set(15, 15, 1);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 4;
					}
				}

				if (randomEnemy < 100 && waveCount >= 1 && shopactive == false)
				{
					go->type = GameObject::GO_SHOP;
					go->scale.Set(10, 10, 1);
					go->prevEnemyBullet = elapsedTime;
					go->hitboxSizeDivider = 0.75;
					go->enemyDamage = 0;
					go->timer = 49;
					shopactive = true;
				}

				if (waveCount == 10 || waveCount == 15)
				{
					SpawnWorm();
				}
				else if (waveCount == 20 || waveCount == 25)
				{
					SpawnBoss();
				}

				go->angle = 0;
				go->maxHP = go->hp;

				// Spawning from edge of world
				int random = rand() % 4;

				if (go->type == GameObject::GO_SHOP)
				{
					switch (random)
					{
					case 0:
						go->pos.Set(Math::RandFloatMinMax(4, m_worldWidth - 4), Math::RandFloatMinMax(4, m_worldHeight - 4), go->pos.z);
						break;
					case 1:
						go->pos.Set(Math::RandFloatMinMax(4, m_worldWidth - 4), Math::RandFloatMinMax(4, m_worldHeight - 4), go->pos.z);
						break;
					case 2:
						go->pos.Set(Math::RandFloatMinMax(4, m_worldWidth - 4), Math::RandFloatMinMax(4, m_worldHeight - 4), go->pos.z);
						break;
					case 3:
						go->pos.Set(Math::RandFloatMinMax(4, m_worldWidth - 4), Math::RandFloatMinMax(4, m_worldHeight - 4), go->pos.z);
						break;
					}
				}
				else if (0 < waveCount <= 10 || waveCount > 20)
				{
					// Spawning outside camera
					switch (random)
					{
					case 0:
						go->pos.Set(m_ship->pos.x + (m_ship->pos.x - camera.position.x), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						break;
					case 1:
						go->pos.Set(camera.position.x, Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						break;
					case 2:
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), m_ship->pos.y + (m_ship->pos.y - camera.position.y), go->pos.z);
						break;
					case 3:
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), go->pos.y < camera.position.y, go->pos.z);
						break;

					}
				}
				else if (11 <= waveCount < 20)
				{
					//map spawn
					switch (random)
					{
					case 0:
						go->pos.Set(m_worldWidth + 1, Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						break;
					case 1:
						go->pos.Set(0 - 1, Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						break;
					case 2:
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), m_worldHeight + 1, go->pos.z);
						break;
					case 3:
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), 0 - 1, go->pos.z);
						break;
					}
				}
				/*				go->direction.Set(0.1, 0.1, 0);
								go->vel = go->direction*/;
				go->vel = 0;
				prevElapsedAsteroid = elapsedTime;
				enemycount++;
			}
		}
		//allow worm to spawn again
		if ((waveCount == 11 || waveCount == 16 || waveCount > 20) && tempWormCount >= 1)
		{
			tempWormCount--;
		}
		if ((waveCount == 21 || waveCount > 25) && tempSpawnCount >= 1)
		{
			tempSpawnCount--;
		}
		// Attack with Primary Weapon
		if (Application::IsMousePressed(0))
		{

			// Attack Animations
			if (worldPosX < m_ship->pos.x)
			{
				HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HEROATTACK_LEFT]);
				HeroSprite->PlayAnimation("Attack", -1, 1 / fireRate);
				HeroSprite->Update(dt);
				heroFacingLeft = true;
			}
			else
			{
				HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HEROATTACK]);
				HeroSprite->PlayAnimation("Attack", -1, 1 / fireRate);
				HeroSprite->Update(dt);
				heroFacingLeft = false;
			}

			//Exercise 15: limit the spawn rate of bullets
			float diff = elapsedTime - prevElapsedBullet;
			if (diff > 1 / fireRate)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_BULLET;
				if (tripleShot)
				{
					go->pos = m_ship->pos;

					go->direction = m_ship->pos - Vector3(worldPosX, worldPosY, m_ship->pos.z);
					go->direction = go->direction.Normalized();
					go->vel = -(go->direction * BULLET_SPEED * 1.5);
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

					GameObject* go = FetchGO();
					go->type = GameObject::GO_BULLET;
					go->pos = m_ship->pos;
					go->pos.y += 4;
					go->pos.z += 1;

					go->direction = m_ship->pos - Vector3(worldPosX, worldPosY, m_ship->pos.z);
					go->direction = go->direction.Normalized();
					go->vel = -(go->direction * BULLET_SPEED * 1.5);
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

					go = FetchGO();
					go->type = GameObject::GO_BULLET;
					go->pos = m_ship->pos;
					go->pos.x += 4;
					go->pos.z += 1;

					go->direction = m_ship->pos - Vector3(worldPosX, worldPosY, m_ship->pos.z);
					go->direction = go->direction.Normalized();
					go->vel = -(go->direction * BULLET_SPEED * 1.5);
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

				}
				else
				{
					go->pos = m_ship->pos;
					go->scale.Set(4.0f, 4.0f, 4.0f);


					go->direction = m_ship->pos - Vector3(worldPosX, worldPosY, m_ship->pos.z);
					go->direction = go->direction.Normalized();
					go->vel = -(go->direction * BULLET_SPEED * 1.5);
					go->angle = m_ship->angle;
				}
				prevElapsedBullet = elapsedTime;
			}
		}

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		//Mouse Section
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
			std::cout << "LBUTTON DOWN" << std::endl;

		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
			std::cout << "LBUTTON UP" << std::endl;
		}
		static bool bRButtonState = false;
		if (!bRButtonState && Application::IsMousePressed(1))
		{
			bRButtonState = true;
			std::cout << "RBUTTON DOWN" << std::endl;
		}
		else if (bRButtonState && !Application::IsMousePressed(1))
		{
			bRButtonState = false;
			std::cout << "RBUTTON UP" << std::endl;
		}


		Vector3 acceleration = m_force * (1.0f / m_ship->mass);

		//Velocity
		m_ship->vel += acceleration * dt * shipSpeed;
		//Exercise 10: Cap Velocity magnitude (FLOAT) using MAX_SPEED (FLOAT)
		if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
		{
			m_ship->vel.Normalize() *= MAX_SPEED;
		}
		m_ship->vel += acceleration * dt * shipSpeed;
		m_ship->pos += m_ship->vel * dt * shipSpeed;


		// Bound player within screen
		if (m_ship->pos.x > m_worldWidth)
		{
			m_ship->pos.x = m_worldWidth;
		}
		else if (m_ship->pos.x < 0)
		{
			m_ship->pos.x = 0;
		}

		if (m_ship->pos.y > m_worldHeight)
		{
			m_ship->pos.y = m_worldHeight;
		}
		else if (m_ship->pos.y < 0)
		{
			m_ship->pos.y = 0;
		}






		//********************************************************************************************************
		// Collision Detection
		//*********************************************************************************************************
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;

			if (go->active)
			{


				go->pos += go->vel * dt * m_speed;
				if (go->pos.x > m_ship->pos.x)
				{
					go->facingLeft = true;
				}
				else
				{
					go->facingLeft = false;
				}


				if (go->type == GameObject::GO_GHOST ||	               // Checks for Player vs Enemy Collision (Taking DMG)
					go->type == GameObject::GO_FLAMEDEMON ||
					go->type == GameObject::GO_BDEMON ||
					go->type == GameObject::GO_NIGHTMARE ||
					go->type == GameObject::GO_ENEMYBULLET ||
					go->type == GameObject::GO_LASER ||
					go->type == GameObject::GO_BOSS ||
					go->type == GameObject::GO_TRIPLESHOT ||
					go->type == GameObject::GO_HEAL ||
					go->type == GameObject::GO_WORMHEAD ||
					go->type == GameObject::GO_WORMBODY1 ||
					go->type == GameObject::GO_WORMBODY2 ||
					go->type == GameObject::GO_WORMTAIL ||
					go->type == GameObject::GO_SHOP ||
					go->type == GameObject::GO_EXPLODER ||
					go->type == GameObject::GO_TREE ||
					go->type == GameObject::GO_CHEST
					)
				{
					Collision(go);
				}

				else if (go->type == GameObject::GO_RINGAURA || go->type == GameObject::GO_FIRE || go->type == GameObject::GO_EXPLOSION)
				{
					//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{

							if (go2->type == GameObject::GO_GHOST ||
								go2->type == GameObject::GO_FLAMEDEMON ||
								go2->type == GameObject::GO_BDEMON ||
								go2->type == GameObject::GO_NIGHTMARE ||
								go2->type == GameObject::GO_ENEMYBULLET ||
								go2->type == GameObject::GO_BOSS ||
								go2->type == GameObject::GO_WORMHEAD ||
								go2->type == GameObject::GO_WORMBODY1 ||
								go2->type == GameObject::GO_WORMBODY2 ||
								go2->type == GameObject::GO_WORMTAIL ||
								go2->type == GameObject::GO_EXPLODER ||
								go2->type == GameObject::GO_TREE)
							{
								HitEnemy(go, go2);
							}

						}

					}
				}



				//Exercise 16: unspawn bullets when they leave screen
				else if (go->type == GameObject::GO_BULLET || go->type == GameObject::GO_CARD || go->type == GameObject::GO_BOMB || go->type == GameObject::GO_MOLOTOV || go->type == GameObject::GO_ARROW || go->type == GameObject::GO_FLAMINGARROW)
				{
					if (go->pos.x > m_worldWidth
						|| go->pos.x <0
						|| go->pos.y > m_worldHeight
						|| go->pos.y < 0)
					{
						go->active = false;
						continue;
					}

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{

							if (go2->type == GameObject::GO_GHOST ||	  // Checks for Player WEAPON vs Enemy Collision (Dealing DMG)
								go2->type == GameObject::GO_FLAMEDEMON ||
								go2->type == GameObject::GO_BDEMON ||
								go2->type == GameObject::GO_NIGHTMARE ||
								go2->type == GameObject::GO_BOSS ||
								go2->type == GameObject::GO_WORMHEAD ||
								go2->type == GameObject::GO_WORMBODY1 ||
								go2->type == GameObject::GO_WORMBODY2 ||
								go2->type == GameObject::GO_WORMTAIL ||
								go2->type == GameObject::GO_EXPLODER ||
								go2->type == GameObject::GO_TREE
								)
							{
								HitEnemy(go, go2);
							}

						}

					}
				}

				// Player projectles despawn outside the camera view
				if (go->type == GameObject::GO_BULLET ||
					go->type == GameObject::GO_BOMB ||
					go->type == GameObject::GO_CARD ||
					go->type == GameObject::GO_FIRE ||
					go->type == GameObject::GO_MOLOTOV ||
					go->type == GameObject::GO_FLAMINGARROW ||
					go->type == GameObject::GO_ARROW ||
					go->type == GameObject::GO_HEAL ||
					go->type == GameObject::GO_TRIPLESHOT)
				{

					if (go->pos.x > m_ship->pos.x + (m_ship->pos.x - camera.position.x)
						|| go->pos.x < camera.position.x
						|| go->pos.y > m_ship->pos.y + (m_ship->pos.y - camera.position.y)
						|| go->pos.y < camera.position.y)
					{
						go->active = false;

					}
				}

				// Magnet effect for powerups
				if (go->type == GameObject::GO_BLACKHOLE)
				{
					for (int j = 0; j < m_goList.size(); ++j)
					{
						if (m_goList[j]->active)
						{
							if (m_goList[j]->type == GameObject::GO_HEAL ||
								m_goList[j]->type == GameObject::GO_TRIPLESHOT)
							{
								if (m_goList[j]->pos.DistanceSquared(go->pos) < 400.0f)
								{
									//1 Close Destroy the object (absorb its mass)
									if (m_goList[j]->pos.DistanceSquared(go->pos) < 1.0f)
									{
										go->mass += m_goList[j]->mass;
										m_goList[j]->active = false;
									}
									//2 Not Close Enough ... affect the object through force
									else
									{
										float sign = (go->type == GameObject::GO_WHITEHOLE) ? -1 : 1;
										Vector3 dir = sign * (go->pos - m_goList[j]->pos).Normalized();
										float force = CalculateAdditionalForce(m_goList[j], go);
										m_goList[j]->vel += 10.f / m_goList[j]->mass * dir * force
											* dt * m_speed;
									}
								}
							}
							else if (m_goList[j]->type == GameObject::GO_EXPLODER)
							{
								//1 Close Destroy the object (absorb its mass)
								if (m_goList[j]->pos.DistanceSquared(go->pos) > 10000.f)
								{
									//go->mass += m_goList[j]->mass;
									//m_goList[j]->active = false;
									m_goList[j]->direction = m_ship->pos - Vector3(m_goList[j]->pos.x, m_goList[j]->pos.y, m_goList[j]->pos.z);
									m_goList[j]->direction = m_goList[j]->direction.Normalized();
									m_goList[j]->vel = (m_goList[j]->direction * 10);
								}
								//2 Not Close Enough ... affect the object through force
								else
								{
									float sign = (go->type == GameObject::GO_WHITEHOLE) ? -1 : 1;
									Vector3 dir = sign * (go->pos - m_goList[j]->pos).Normalized();
									float force = CalculateAdditionalForce(m_goList[j], go);
									m_goList[j]->vel += 70.f / m_goList[j]->mass * dir * force * dt * m_speed;


									//if (m_goList[j]->pos.x < m_ship->pos.x + (m_ship->pos.x - camera.position.x)
									//	&& m_goList[j]->pos.x > camera.position.x
									//	&& m_goList[j]->pos.y < m_ship->pos.y + (m_ship->pos.y - camera.position.y)
									//	&& m_goList[j]->pos.y > camera.position.y)
									//{

									//}
								}
							}
						}
					}
				}
			}
		}


		// Player Item Usage
		if (cardUse == true)
		{
			diff = elapsedTime - prevElapsedCard;
			if (diff > 1 / cardRate)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_CARD;
				go->pos = m_ship->pos;
				go->vel = m_ship->direction * BULLET_SPEED;
				go->scale.Set(6.0f, 4.0f, 4.0f);
				go->angle = m_ship->angle;
				go->hitboxSizeDivider = 3;

				prevElapsedCard = elapsedTime;
			}
		}

		if (bombUse == true)
		{
			diff = elapsedTime - prevElapsedBomb;
			if (diff > 1 / bombRate)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_BOMB;
				go->pos = m_ship->pos;
				go->vel = Vector3(Math::RandFloatMinMax(-1, 1), 2, 0) * BULLET_SPEED;
				go->scale.Set(5.0f, 5.0f, 4.0f);
				go->angle = m_ship->angle;
				go->hitboxSizeDivider = 3;
				prevElapsedBomb = elapsedTime;
			}
		}

		if (ringUse == true)
		{

			GameObject* go = FetchGO();
			go->type = GameObject::GO_RINGAURA;
			go->pos = m_ship->pos;
			BarrierSprite->PlayAnimation("Aura", -1, 4.0f);

			ringUse = false;
		}

		if (flamingarrowUse == false)
		{
			if (arrowUse == true)
			{
				diff = elapsedTime - prevElapsedArrow;
				if (diff > 1 / arrowRate)
				{
					for (int i = 0; i < arrowAmount; i++)
					{
						GameObject* go = FetchGO();
						go->type = GameObject::GO_ARROW;
						go->pos = m_ship->pos;
						go->direction = m_ship->pos - Vector3(worldPosX + Math::RandFloatMinMax(10, -10), worldPosY + Math::RandFloatMinMax(10, -10), m_ship->pos.z);
						go->direction = go->direction.Normalized();
						go->vel = -(go->direction * BULLET_SPEED * 0.8);
						go->scale.Set(6.0f, 6.0f, 6.0f);
						go->angle = m_ship->angle + 45;
						prevElapsedArrow = elapsedTime;
					}
				}
			}
		}

		if (flamingarrowUse == true)
		{
			diff = elapsedTime - prevElapsedArrow;
			if (diff > 1 / arrowRate)
			{
				for (int i = 0; i < arrowAmount; i++)
				{
					GameObject* go = FetchGO();
					go->type = GameObject::GO_FLAMINGARROW;
					go->pos = m_ship->pos;
					go->direction = m_ship->pos - Vector3(worldPosX + Math::RandFloatMinMax(10, -10), worldPosY + Math::RandFloatMinMax(10, -10), m_ship->pos.z);
					go->direction = go->direction.Normalized();
					go->vel = -(go->direction * BULLET_SPEED * 0.8);
					go->scale.Set(6.0f, 6.0f, 6.0f);
					go->angle = m_ship->angle + 45;
					prevElapsedArrow = elapsedTime;
				}
			}
		}

		if (flamingarrowUse == false)
		{
			if (molotovUse == true)
			{
				diff = elapsedTime - prevElapsedMolotov;
				if (diff > 1 / molotovRate)
				{
					for (int i = 0; i < molotovAmount; i++)
					{
						GameObject* go = FetchGO();
						go->type = GameObject::GO_MOLOTOV;
						go->pos = m_ship->pos;
						go->vel = Vector3(Math::RandFloatMinMax(-1, 1), Math::RandFloatMinMax(-1, 1), 0) * BULLET_SPEED;
						go->scale.Set(5.0f, 5.0f, 4.0f);
						go->angle = m_ship->angle;
						go->hitboxSizeDivider = 3;
						prevElapsedMolotov = elapsedTime;
					}
				}
			}
		}


		if (shopactive == true && shoppointer < 1)
		{
			GameObject* go = FetchGO();
			go->type = GameObject::GO_SHOP_POINTER;
			go->scale.Set(10, 10, 10);
			go->pos = m_ship->pos;
			go->pos.y = m_ship->pos.y - 10;
			shoppointer++;
		}


		// ************************************* Homing Card Code *******************************************
		float closestDis = 999999;
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active && go->type == GameObject::GO_CARD)
			{
				// Check for enemy targets
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					// Proceed if enemy type
					if (go2->type == GameObject::GO_GHOST ||	  // Checks for Player WEAPON vs Enemy Collision (Dealing DMG)
						go2->type == GameObject::GO_FLAMEDEMON ||
						go2->type == GameObject::GO_BDEMON ||
						go2->type == GameObject::GO_NIGHTMARE ||
						go2->type == GameObject::GO_BOSS ||
						go2->type == GameObject::GO_WORMHEAD ||
						go2->type == GameObject::GO_WORMBODY1 ||
						go2->type == GameObject::GO_WORMBODY2 ||
						go2->type == GameObject::GO_WORMTAIL
						)
					{
						if (go2->active && it != it2)
						{

							// This checks current distances between each other,
							// will update target based on closest distance
							float currentDis = go->pos.DistanceSquared(go2->pos);
							if (currentDis < closestDis)

							{
								closestDis = currentDis;
								float rad = (go->scale.x + go2->scale.x * 10) * (go->scale.x + go2->scale.x * 10);
								if (currentDis < rad)
								{

									go->direction = go->pos - go2->pos;
									go->direction = -go->direction.Normalized();
									go->vel = go->direction * BULLET_SPEED * 0.5;

									go->angle = atan2(go2->pos.y - go->pos.y, go2->pos.x - go->pos.x) + 45;
									go->angle = (go->angle / Math::PI) * 180.0 - 90.0f;
								}
							}
						}
					}

				}
			}

			if (go->type == GameObject::GO_FIRE && go->active == true)
			{
				go->timer -= 1 * dt;
				if (fireTimer <= 0)
				{
					fireTimer = 0.75;
				}
				if (go->timer < 0)
				{
					go->active = false;
				}
			}

			if (go->type == GameObject::GO_SHOP && go->active == true)
			{
				go->timer -= 1 * dt;
				cout << go->timer << endl;
				if (go->timer <= 0)
				{
					go->active = false;
					shopactive = false;

				}
			}

			if (go->type == GameObject::GO_CHEST_PARTICLE && go->active == true)
			{
				go->timer -= 1 * dt;
				if (go->timer < 0)
				{
					go->active = false;
				}
			}
		}
		//*****************************************************************************************************************


		//************************************ ENEMY ATTACKS *****************************************************************************
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* enemy = (GameObject*)*it;
			if (enemy->active)
			{
				// Flame Demon: Periodially dashes to player
				if (enemy->type == GameObject::GO_FLAMEDEMON)
				{
					float diff = elapsedTime - enemy->prevEnemyBullet;
					if (diff > 3)
					{
						enemy->speedFactor = 10;
						enemy->prevEnemyBullet = elapsedTime;
					}
					else
					{
						if (enemy->speedFactor > 1)
						{
							enemy->speedFactor -= 5 * dt;
							if (enemy->speedFactor < 1)
							{
								enemy->speedFactor = 1;
							}
						}
					}

					// Move towards player
					if (enemy->speedFactor <= 1)
					{
						enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
						enemy->direction = enemy->direction.Normalized();
					}
					enemy->vel = (enemy->direction * 6 * enemy->speedFactor);
				}

				// BOSS ENEMY
				else if (enemy->type == GameObject::GO_BOSS)
				{
					NightborneSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSS_LEFT]);
					NightborneSpriteLeft->PlayAnimation("Move Left", -1, 0.8f);
					NightborneSpriteLeft->Update(dt);


					NightborneSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSS]);
					NightborneSprite->PlayAnimation("Move Right", -1, 0.8f);
					NightborneSprite->Update(dt);

					NightborneSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSSATTACK]);
					NightborneSprite->PlayAnimation("Attack Right", 3, 0.65f);
					NightborneSprite->Update(dt);

					NightborneSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSSATTACK_LEFT]);
					NightborneSpriteLeft->PlayAnimation("Attack Left", 3, 0.65f);
					NightborneSpriteLeft->Update(dt);


					// Move towards player
					if (enemy->speedFactor <= 1)
					{
						enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
						enemy->direction = enemy->direction.Normalized();
					}
					enemy->vel = (enemy->direction * 6 * enemy->speedFactor);

					float diff = elapsedTime - enemy->prevEnemyBullet;
					switch (bossState)
					{
					case 0:
						if (diff > 1)
						{
							float bulletCount = 8;
							float angle = 360 / bulletCount;;
							for (int i = 0; i < 8; ++i)
							{
								GameObject* go2 = FetchGO();
								go2->type = GameObject::GO_ENEMYBULLET;
								go2->scale.Set(4.0f, 4.0f, 4.0f);
								go2->pos = enemy->pos;
								go2->angle = angle * i + Math::RandFloatMinMax(0, 50);
								go2->enemyDamage = 13;
								go2->hitboxSizeDivider = 2;


								go2->direction = RotateVector(go2->pos, go2->angle * dt * shipSpeed);
								go2->direction = go2->direction.Normalized();


								go2->vel = go2->direction * BULLET_SPEED * 0.8;

							}
							shootCount++;
							enemy->prevEnemyBullet = elapsedTime;

							if (shootCount == 3)
							{
								bossState = 1;
								shootCount = 0;
								enemy->hitboxSizeDivider = 3;
								NightborneSprite->Reset();
								NightborneSpriteLeft->Reset();
							}
						}
						break;
					case 1:

						enemy->enemyDamage = 35;
						if (diff > 0.6)
						{
							enemy->speedFactor = 21;
							enemy->prevEnemyBullet = elapsedTime;
							enemy->direction += Vector3(Math::RandFloatMinMax(-0.5, 0.5), Math::RandFloatMinMax(-0.5, 0.5), 0);
							cSoundController->StopSoundByID(11);
							cSoundController->PlaySoundByID(11);

							shootCount++;
						}
						else
						{
							if (enemy->speedFactor > 1)
							{
								enemy->speedFactor -= 42 * dt;
								if (enemy->speedFactor < 1)
								{
									enemy->speedFactor = 1;
									if (shootCount == 3)
									{
										bossState = 2;
										shootCount = 0;
										enemy->hitboxSizeDivider = 8;
										NightborneSprite->Reset();
										NightborneSpriteLeft->Reset();
									}
								}
							}
						}


						break;
					case 2:
						enemy->enemyDamage = 20;
						if (diff > 1)
						{
							enemy->prevEnemyBullet = elapsedTime - 0.96;
							enemy->vel = 0;
							GameObject* go2 = FetchGO();
							go2->type = GameObject::GO_LASER;
							go2->scale.Set(2.0f, 2.0f, 4.0f);
							go2->pos = enemy->pos;
							go2->angle = laserAngle;
							go2->enemyDamage = 13;
							go2->hitboxSizeDivider = 6;
							go2->timer = 10;

							laserAngle += 6;

							go2->direction = RotateVector(go2->pos, go2->angle * dt * shipSpeed);
							go2->direction = go2->direction.Normalized();

							go2->angle = atan2(go2->direction.y, go2->direction.x) - 90;
							go2->angle = (go2->angle / Math::PI) * 180.0 - 90.0f;


							go2->vel = go2->direction * BULLET_SPEED * 0.8;

							if (laserAngle >= 350)
							{
								laserAngle = 0;
								bossState = 0;
							}

						}
						break;
					}
					break;
				}


				// WORM ENEMY
				// ********************************************
				// Worm Head will always towards player
				else if (enemy->type == GameObject::GO_WORMHEAD)
				{
					if (enemy->type != GameObject::GO_WORMBODY1 &&
						enemy->type != GameObject::GO_WORMBODY2 &&
						enemy->type != GameObject::GO_WORMTAIL)
					{

						if (enemy->timer > 4)
						{
							enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
							enemy->direction = enemy->direction.Normalized();
							enemy->vel = (enemy->direction * 15);
						}
						else if (enemy->timer < 4 && enemy->timer > 3.8)
						{
							cSoundController->StopSoundByID(12);
							cSoundController->PlaySoundByID(12);
						}
						else if (enemy->timer < 3)
						{

							enemy->vel = (enemy->direction * 80);
						}
						enemy->timer -= 0.04;
						if (enemy->timer < 0)
						{
							enemy->timer = 10;
						}
					}

					// Worm moving code
					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{

							if (go2->type == GameObject::GO_WORMHEAD)
							{
								GameObject* current = go2;
								GameObject* nextObj;
								GameObject* prevObj;

								GameObject* head;
								head = go2;

								// Worm moving code using linked list
								while (true)
								{
									nextObj = current->nextNode;
									if (current->prevNode != nullptr)
									{
										prevObj = current->prevNode;
										prevObj->timer = head->timer;
										float dis = current->pos.DistanceSquared(prevObj->pos);

										//*******************************************************************************************************************
										//	How the Worm movement works
										// ********************************************************************************************************************
										//	All the worm segments will spawn in the same position at first.
										//	A body segment will not start moving until its previous segment moves far enough from current segment.
										// 
										//	From there a body segment will set its target location based on the previous segment's location.
										//	It will only head to that target location and upon reaching it, it will reupdate its new target.
										// 
										//  This makes body segments only follow the segment that is connected instead of the head.
										if (dis > 70 && current->reachTarget == true)
										{
											current->targetPos = prevObj->pos;
											current->direction = prevObj->pos - Vector3(current->pos.x, current->pos.y, current->pos.z);
											current->vel = prevObj->vel;
											current->reachTarget = false;


										}
										// Move current body towards its target position
										if (current->reachTarget == false)
										{
											current->direction = current->targetPos - Vector3(current->pos.x, current->pos.y, current->pos.z);
											current->direction = current->direction.Normalized();
											if (head->timer > 3)
											{
												current->vel = current->direction * 15;
											}
											else
											{
												current->vel = current->direction * 80;
												if (head->timer <= 0)
												{
													current->vel = current->direction * 15;
													current->direction = current->direction.Normalized();
												}
											}

											current->angle = atan2(prevObj->pos.y - current->pos.y, prevObj->pos.x - current->pos.x);
											current->angle = (current->angle / Math::PI) * 180.0 - 90.f;

											float dis2 = current->pos.DistanceSquared(current->targetPos);
											if (dis2 < 100)
											{
												current->reachTarget = true;
											}
										}
									}
									if (current->nextNode != nullptr)
									{
										current = current->nextNode;
									}
									else
									{
										break;
									}
								}
							}

						}

					}
				}
				else if (enemy->type == GameObject::GO_SHOP)
				{
					// Move towards player
					//enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
					//enemy->direction = enemy->direction.Normalized();
					shopposx = enemy->pos.x;
					shopposy = enemy->pos.y;
					shopposz = enemy->pos.z;
					//enemy->vel = (enemy->direction * 40);
				}
				else if (enemy->type == GameObject::GO_BDEMON)
				{
					float diff = elapsedTime - enemy->prevEnemyBullet;
					// Enemy ship shooting at player
					if (diff > 1)
					{
						GameObject* go2 = FetchGO();
						go2->type = GameObject::GO_ENEMYBULLET;
						go2->scale.Set(4.0f, 4.0f, 4.0f);
						go2->pos = enemy->pos;
						go2->angle = enemy->angle;
						go2->enemyDamage = 4;
						go2->hitboxSizeDivider = 2;
						go2->timer = 6;

						go2->direction = go2->pos - m_ship->pos;
						go2->direction = -go2->direction.Normalized();
						go2->vel = go2->direction * BULLET_SPEED * 0.8;
						enemy->prevEnemyBullet = elapsedTime;


					}
					enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
					enemy->direction = enemy->direction.Normalized();
					enemy->vel = (enemy->direction * 6);
				}

				else if (enemy->type == GameObject::GO_GHOST ||
					enemy->type == GameObject::GO_NIGHTMARE)
				{
					enemy->direction = m_ship->pos - Vector3(enemy->pos.x, enemy->pos.y, enemy->pos.z);
					enemy->direction = enemy->direction.Normalized();
					enemy->vel = (enemy->direction * 10);
				}


				else if (enemy->type == GameObject::GO_CHEST)
				{
					if (enemy->timer > 0)
					{
						enemy->timer -= 1 * dt;
						if (enemy->timer <= 0)
						{
							enemy->active = false;
						}
					}
				}

				//// unspawn offscreen
				else if (enemy->type == GameObject::GO_ENEMYBULLET ||
					enemy->type == GameObject::GO_LASER ||
					enemy->type == GameObject::GO_WORMTAIL)
				{

					// Enemy projectile despawns after a certain lifetime
					if (enemy->type != GameObject::GO_WORMTAIL)
					{
						enemy->timer -= 1 * dt;
						if (enemy->timer <= 0)
						{
							enemy->active = false;
						}
					}

					if (enemy->pos.x > m_worldWidth
						|| enemy->pos.x < 0
						|| enemy->pos.y > m_worldHeight
						|| enemy->pos.y < 0)
					{
						if (enemy->type == GameObject::GO_WORMTAIL)
						{
							if (enemy->prevNode == nullptr &&
								enemy->nextNode == nullptr)
							{
								enemy->active = false;
							}
						}
						else
						{
							enemy->active = false;
						}
					}
				}
			}
		}

		//********************************************************************************************************************************************

		// Health regen
		if (healthRegen == true)
		{
			diff = elapsedTime - prevHealthRegen;
			if (diff > 1)
			{
				m_ship->hp += healthRegenAmount;
				if (m_ship->hp > m_ship->maxHP)
				{
					m_ship->hp = m_ship->maxHP;
				}
				prevHealthRegen = elapsedTime;
			}
		}

		// Triple shot powerup
		if (tripleShot == true)
		{
			tripleShotTimer -= 1 * dt;
			if (tripleShotTimer <= 0)
			{
				tripleShot = false;
			}
		}

		// Invincibility frames
		if (iFrames > 0)
		{
			iFrames -= 1 * dt;
		}

		if (fireTimer > 0)
		{
			fireTimer -= 1 * dt;
		}

		if (ringauraTimer > 0)
		{
			ringauraTimer -= 1 * dt;
		}

		if (ringauraTimer <= 0)
		{
			ringauraTimer = 0.75;
		}

		if (explosionTimer > 0)
		{
			explosionTimer -= 1 * dt;
		}

		if (chestTimer > 0)
		{
			chestTimer -= 1 * dt;
		}

		// If health reaches zero
		if (m_ship->hp <= 0 && isAlive == true)
		{
			isAlive = false;
			gameOverTimer = 3;
			cSoundController->StopSoundByID(1);
			cSoundController->StopSoundByID(2);
			cSoundController->StopSoundByID(3);
			cSoundController->StopSoundByID(4);
			cSoundController->StopSoundByID(7);
			cSoundController->StopSoundByID(8);
			cSoundController->StopSoundByID(13);
			cSoundController->PlaySoundByID(19);
		}


		if (SceneBase::restartGame)
		{
			RestartGame();
			SceneBase::restartGame = false;
			SceneBase::menuType = M_NONE;
			SceneManager::activeScene = S_ASSIGNMENT1;
		}

		if (SceneBase::resetGame)
		{
			RestartGame();
			SceneBase::resetGame = false;
			SceneBase::menuType = M_MAIN;
			SceneManager::activeScene = S_ASSIGNMENT1;
		}
	}
	else if (isAlive == false)
	{
		if (gameOverTimer > 0)
		{
			gameOverTimer -= 1 * dt;

			HeroSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERODEATH]);
			HeroSprite->PlayAnimation("Death", 0, 2.0f);
			HeroSprite->Update(dt);
		}
	}
}

void Assignment1::Collision(GameObject* go)
{
	// Collision check
	float dis = go->pos.DistanceSquared(m_ship->pos);
	float cRad = (m_ship->scale.x / go->hitboxSizeDivider + go->scale.x) * (m_ship->scale.x / go->hitboxSizeDivider + go->scale.x) / go->hitboxSizeDivider;

	if (dis < cRad)
	{
		if (go->type == GameObject::GO_TRIPLESHOT)
		{
			if (dis < cRad)
			{
				tripleShot = true;
				tripleShotTimer = 6;
			}
		}

		// Access upgrade screen
		if (Application::IsKeyPressed('E') && go->type == GameObject::GO_SHOP)
		{
			upgradescreen = true;
			menuType = M_UPGRADE;
			cSoundController->PlaySoundByID(17);
		}

		if (go->type == GameObject::GO_EXPLODER)
		{
			GameObject* explosion = FetchGO();
			explosion->type = GameObject::GO_ENEMYEXPLOSION;
			explosion->pos = go->pos;
			explosion->scale.Set(20, 20, 10);
			explosion->vel = 0;
			explosion->explosionScale = 0;
			explosion->scaleDown = false;
			explosionTimer = 0.1;
			EnemyExplosionSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_ENEMYEXPLOSION]);
			EnemyExplosionSprite->PlayAnimation("Explode", -1, 1.0f);
		}


		if (go->type == GameObject::GO_TRIPLESHOT ||
			go->type == GameObject::GO_HEAL ||
			go->type == GameObject::GO_ENEMYBULLET ||
			go->type == GameObject::GO_EXPLODER
			)
		{
			go->active = false;
		}

		if (go->type == GameObject::GO_TREE)
		{
			m_ship->pos = m_ship->previousPos;
		}

		// Chest gold can only be obtained once per chest
		if (go->type == GameObject::GO_CHEST &&
			go->moneyDrop > 0)
		{
			m_money += go->moneyDrop;
			go->moneyDrop = 0;

			ChestSprite->Reset();
			ChestSprite->PlayAnimation("OPEN", 0, 2.0f);
			go->timer = 1;
			chestTimer = 3;
			cSoundController->PlaySoundByID(16);

			GameObject* chestparticle = FetchGO();
			chestparticle->type = GameObject::GO_CHEST_PARTICLE;
			chestparticle->pos = go->pos;
			chestparticle->pos.x = go->pos.x - 2;
			chestparticle->scale.Set(20, 20, 0.5);
			chestparticle->vel.SetZero();
			chestparticle->timer = 1;

		}

		if (go->enemyDamage <= 0) // Healing items and buffs
		{
			m_ship->hp -= go->enemyDamage;
		}
		else if (iFrames <= 0)
		{
			m_ship->hp -= go->enemyDamage;
			iFrames = 1;

			displayDamage.push_back(go->enemyDamage);
			damageTextX.push_back((go->pos.x - camera.position.x) * 80 / (192 * (m_worldHeight / (4 * 100))));
			damageTextY.push_back((go->pos.y - camera.position.y) * 65 / (100 * (m_worldHeight / (4 * 100))));
			scaleText.push_back(1);
			translateTextY.push_back(0);
			damageTimer.push_back(elapsedTime);
			damageEnemy.push_back(false);

			cSoundController->StopSoundByID(8);
			cSoundController->PlaySoundByID(8);
		}


		if (m_ship->hp > m_ship->maxHP)
		{
			m_ship->hp = m_ship->maxHP;
		}

	}
	//Exercise 13: asteroids should wrap around the screen like the ship
	//Wrap(go->pos.x, m_worldWidth);
	//Wrap(go->pos.y, m_worldHeight);

}
void Assignment1::HitEnemy(GameObject* bullet, GameObject* target)
{
	if (bullet->type == GameObject::GO_CARD || bullet->type == GameObject::GO_BOMB ||
		bullet->type == GameObject::GO_BULLET || bullet->type == GameObject::GO_MOLOTOV ||
		bullet->type == GameObject::GO_ARROW || bullet->type == GameObject::GO_FLAMINGARROW ||
		bullet->type == GameObject::GO_TREE)
	{
		float dis = bullet->pos.DistanceSquared(target->pos);
		float rad = (bullet->scale.x + target->scale.x / 4) * (bullet->scale.x + target->scale.x / 4);

		if (dis < rad && target->type == GameObject::GO_TREE)
		{
			if (bullet->type == GameObject::GO_CARD)
			{
				GameObject* explosion = FetchGO();
				explosion->type = GameObject::GO_EXPLOSION;
				explosion->pos = target->pos;
				explosion->scale.Set(20, 20, 10);
				explosion->vel = 0;
				explosion->explosionScale = 0;
				explosion->scaleDown = false;
				explosionTimer = 0.1;
				ExplosionSprite->PlayAnimation("Explode", 1, 1.0f);
				bullet->active = false;
			}

			if (bullet->type == GameObject::GO_BOMB)
			{
				GameObject* explosion = FetchGO();
				explosion->type = GameObject::GO_EXPLOSION;
				explosion->pos = target->pos;
				explosion->scale.Set(40, 40, 10);
				explosion->vel = 0;
				explosion->explosionScale = 0;
				explosion->scaleDown = false;
				explosionTimer = 0.1;
				ExplosionSprite->PlayAnimation("Explode", 1, 1.0f);
				bullet->active = false;
			}

			if (bullet->type == GameObject::GO_MOLOTOV)
			{
				GameObject* fire = FetchGO();
				fire->type = GameObject::GO_FIRE;
				fire->pos = target->pos;
				fire->scale.Set(14, 14, 14);
				fire->direction = Vector3(0, 1, 0);
				fire->vel = 0;
				fire->timer = 5;
				FireSprite->PlayAnimation("Fire", 1, 1.0f);
				bullet->active = false;
			}

			if (bullet->type == GameObject::GO_FLAMINGARROW)
			{
				GameObject* fire = FetchGO();
				fire->type = GameObject::GO_FIRE;
				fire->pos = target->pos;
				fire->scale.Set(14, 14, 14);
				fire->direction = Vector3(0, 1, 0);
				fire->vel = 0;
				fire->timer = 8;
				fireTimer = 0.75;
				FireSprite->PlayAnimation("Fire", 1, 1.0f);
				bullet->active = false;
			}
			else
			{
				bullet->active = false;
			}
			return;
		}



		if (dis < rad && target->type != GameObject::GO_ENEMYBULLET)
		{
			if (bullet->type != GameObject::GO_EXPLOSION)
			{
				target->isHit = true;
			}
			if (bullet->type == GameObject::GO_CARD)
			{
				int damageDealt = round(basicBulletDamage * 2 * Math::RandFloatMinMax(0.7, 1.5));
				target->hp -= damageDealt;
				GameObject* explosion = FetchGO();
				explosion->type = GameObject::GO_EXPLOSION;
				explosion->pos = target->pos;
				explosion->scale.Set(20, 20, 10);
				explosion->vel = 0;
				explosion->explosionScale = 0;
				explosion->scaleDown = false;
				explosionTimer = 0.1;
				ExplosionSprite->PlayAnimation("Explode", 1, 1.0f);
				bullet->active = false;

				displayDamage.push_back(damageDealt);
				damageTextX.push_back((target->pos.x - camera.position.x) * 85 / (192 * (m_worldHeight / (4 * 100))));
				damageTextY.push_back((target->pos.y - camera.position.y) * 65 / (100 * (m_worldHeight / (4 * 100))));
				scaleText.push_back(0);
				translateTextY.push_back(0);
				damageTimer.push_back(elapsedTime);
				damageEnemy.push_back(true);

			}

			if (bullet->type == GameObject::GO_BOMB)
			{
				int damageDealt = round(basicBulletDamage * 2 * Math::RandFloatMinMax(0.7, 1.5));
				target->hp -= damageDealt;
				GameObject* explosion = FetchGO();
				explosion->type = GameObject::GO_EXPLOSION;
				explosion->pos = target->pos;
				explosion->scale.Set(40, 40, 10);
				explosion->vel = 0;
				explosion->explosionScale = 0;
				explosion->scaleDown = false;
				explosionTimer = 0.1;
				ExplosionSprite->PlayAnimation("Explode", 1, 1.0f);
				bullet->active = false;

				displayDamage.push_back(damageDealt);
				damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
				damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
				scaleText.push_back(0);
				translateTextY.push_back(0);
				damageTimer.push_back(elapsedTime);
				damageEnemy.push_back(true);
			}

			if (bullet->type == GameObject::GO_MOLOTOV)
			{
				GameObject* fire = FetchGO();
				fire->type = GameObject::GO_FIRE;
				fire->pos = target->pos;
				fire->scale.Set(14, 14, 14);
				fire->direction = Vector3(0, 1, 0);
				fire->vel = 0;
				fire->timer = 5;
				FireSprite->PlayAnimation("Fire", 1, 1.0f);
				bullet->active = false;
			}

			if (bullet->type == GameObject::GO_BULLET)
			{
				int damageDealt = round(basicBulletDamage * Math::RandFloatMinMax(0.7, 1.5));
				target->hp -= damageDealt;
				cSoundController->StopSoundByID(7);
				cSoundController->PlaySoundByID(7);
				bullet->active = false;

				displayDamage.push_back(damageDealt);
				damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
				damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
				scaleText.push_back(0);
				translateTextY.push_back(0);
				damageTimer.push_back(elapsedTime);
				damageEnemy.push_back(true);


			}

			if (bullet->type == GameObject::GO_ARROW)
			{
				int damageDealt = round(basicBulletDamage * Math::RandFloatMinMax(0.7, 1.5));
				target->hp -= damageDealt;
				bullet->active = false;

				displayDamage.push_back(damageDealt);
				damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
				damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
				scaleText.push_back(0);
				translateTextY.push_back(0);
				damageTimer.push_back(elapsedTime);
				damageEnemy.push_back(true);
			}

			if (bullet->type == GameObject::GO_FLAMINGARROW)
			{
				int damageDealt = round(basicBulletDamage * Math::RandFloatMinMax(0.7, 1.5));
				target->hp -= damageDealt;
				GameObject* fire = FetchGO();
				fire->type = GameObject::GO_FIRE;
				fire->pos = target->pos;
				fire->scale.Set(14, 14, 14);
				fire->direction = Vector3(0, 1, 0);
				fire->vel = 0;
				fire->timer = 8;
				fireTimer = 0.75;
				FireSprite->PlayAnimation("Fire", 1, 1.0f);
				bullet->active = false;

				displayDamage.push_back(damageDealt);
				damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
				damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
				scaleText.push_back(0);
				translateTextY.push_back(0);
				damageTimer.push_back(elapsedTime);
				damageEnemy.push_back(true);
			}

			// Asteroid HP reaches 0
			if (target->hp <= 0)
			{

				if (target->type == GameObject::GO_BOSS)
				{
					bossspawned = false;
				}

				// Money gained
				enemycount--;
				killcount++;
				m_money += 1 + bonusMoney;
				target->active = false;
				if (target->type == GameObject::GO_NIGHTMARE)
				{
					for (int i = 0; i < 4; ++i)
					{
						GameObject* go = FetchGO();
						go->type = GameObject::GO_GHOST;
						go->hp = round(1 * hpFactor);
						go->scale.Set(10, 10, 1);
						go->pos.Set(target->pos.x + Math::RandFloatMinMax(-10, 10), target->pos.y + Math::RandFloatMinMax(-10, 10), go->pos.z);
						go->hitboxSizeDivider = 3.5;
						go->enemyDamage = 2;
						go->angle = 0;
						go->maxHP = go->hp;
					}
				}

				if (target->type == GameObject::GO_BOSS)
				{
					tempSpawnCount--;
				}

				//*** Worm Segment seperation code*********************************
				if (target->type == GameObject::GO_WORMHEAD)
				{
					if (target->nextNode != nullptr)
					{
						target->nextNode->type = GameObject::GO_WORMHEAD;
						target->nextNode->prevNode = nullptr;
						target->prevNode = nullptr;
						target->nextNode->enemyDamage = 35;
						target->nextNode->hitboxSizeDivider = 1.8;
					}
				}
				else if (target->type == GameObject::GO_WORMBODY1 || target->type == GameObject::GO_WORMBODY2)
				{
					if (target->nextNode != nullptr)
					{
						target->nextNode->type = GameObject::GO_WORMHEAD;
						target->nextNode->prevNode = nullptr;
						target->nextNode->timer = 10;
						target->nextNode->enemyDamage = 35;
						target->nextNode->hitboxSizeDivider = 1.8;
					}

					if (target->prevNode != nullptr)
					{
						target->prevNode->type = GameObject::GO_WORMTAIL;
						target->prevNode->nextNode = nullptr;
						target->prevNode->enemyDamage = 20;
					}
				}
				else if (target->type == GameObject::GO_WORMTAIL)
				{
					if (target->prevNode != nullptr)
					{
						target->prevNode->type = GameObject::GO_WORMTAIL;
						target->prevNode->nextNode = nullptr;
						target->prevNode->enemyDamage = 20;
					}
				}


				// Drop  Item
				int random = rand() % 18;
				if (random == 0)
				{
					float maxVel = 0.8;

					GameObject* go3 = FetchGO();
					go3->type = GameObject::GO_TRIPLESHOT;
					go3->vel = 0;
					go3->pos.Set(target->pos.x, target->pos.y, 8);
					go3->scale.Set(7, 7, 1);
					go3->enemyDamage = 0;
					go3->hitboxSizeDivider = 3;

				}
				else if (random < 3)
				{
					float maxVel = 0.8;

					GameObject* go3 = FetchGO();
					go3->type = GameObject::GO_HEAL;
					go3->vel = 0;
					go3->pos.Set(target->pos.x, target->pos.y, target->pos.z);
					go3->scale.Set(5, 5, 1);
					go3->enemyDamage = -5;
					go3->hitboxSizeDivider = 3;
				}
			}
		}
	}


	else if (bullet->type == GameObject::GO_RINGAURA)
	{
		float dis = bullet->pos.DistanceSquared(target->pos);
		float rad = ((bullet->scale.x + ringAOE + target->scale.x) / 2.2) * ((bullet->scale.x + ringAOE + target->scale.x) / 2.2);
		if (dis < rad)
		{

			if (bullet->type == GameObject::GO_RINGAURA)
			{
				if (ringauraTimer == 0.75)
				{
					target->hp -= basicBulletDamage;
				}
			}

			// Asteroid HP reaches 0
			if (target->hp <= 0)
			{
				target->active = false;

				enemycount--;
				// Money gained
				if (target->type != GameObject::GO_ENEMYBULLET && target->type != GameObject::GO_TREE)
				{
					// Drop  Item
					if (target->type == GameObject::GO_NIGHTMARE)
					{
						for (int i = 0; i < 4; ++i)
						{
							GameObject* go = FetchGO();
							go->type = GameObject::GO_GHOST;
							go->hp = round(1 * hpFactor);
							go->scale.Set(10, 10, 1);
							go->pos.Set(target->pos.x + Math::RandFloatMinMax(-10, 10), target->pos.y + Math::RandFloatMinMax(-10, 10), go->pos.z);
							go->hitboxSizeDivider = 3.5;
							go->enemyDamage = 2;
							go->angle = 0;
							go->maxHP = go->hp;
						}
					}


					int random = rand() % 14;
					if (random == 0)
					{
						float maxVel = 0.8;

						GameObject* go3 = FetchGO();
						go3->type = GameObject::GO_TRIPLESHOT;
						go3->vel = 0;
						go3->pos.Set(target->pos.x, target->pos.y, target->pos.z);
						go3->scale.Set(9, 9, 1);
						go3->enemyDamage = 0;
						go3->hitboxSizeDivider = 3;

					}
					else if (random < 3)
					{
						float maxVel = 0.8;

						GameObject* go3 = FetchGO();
						go3->type = GameObject::GO_HEAL;
						go3->vel = 0;
						go3->pos.Set(target->pos.x, target->pos.y, target->pos.z);
						go3->scale.Set(5, 5, 1);
						go3->enemyDamage = -5;
						go3->hitboxSizeDivider = 3;
					}
				}
			}
		}
	}

	else if (bullet->type == GameObject::GO_EXPLOSION || bullet->type == GameObject::GO_FIRE)
	{
		float dis = bullet->pos.DistanceSquared(target->pos);
		float rad = (bullet->scale.x + target->scale.x / 4) * (bullet->scale.x + target->scale.x / 4);

		if (dis < rad && target->type == GameObject::GO_TREE)
		{
			target->active = true;
		}

		if (dis < rad && target->type != GameObject::GO_ENEMYBULLET)
		{
			if (bullet->type != GameObject::GO_EXPLOSION)
			{
				target->isHit = true;
			}

			if (bullet->type == GameObject::GO_EXPLOSION)
			{
				if (explosionTimer == 0.1)
				{
					if (target->type == GameObject::GO_WORMBODY1 || target->type == GameObject::GO_WORMBODY2 || target->type == GameObject::GO_WORMHEAD || target->type == GameObject::GO_WORMTAIL)
					{
						int damageDealt = round(basicBulletDamage * Math::RandFloatMinMax(0.7, 1.5));
						target->hp -= damageDealt;
						displayDamage.push_back(damageDealt);
						damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
						damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
						scaleText.push_back(0);
						translateTextY.push_back(0);
						damageTimer.push_back(elapsedTime);
						damageEnemy.push_back(true);
						/*	cSoundController->StopSoundByID(5);*/
						cSoundController->PlaySoundByID(5);
					}

					else
					{
						int damageDealt = round(basicBulletDamage * 3 * Math::RandFloatMinMax(0.7, 1.5));
						target->hp -= damageDealt;
						displayDamage.push_back(damageDealt);
						damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
						damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
						scaleText.push_back(0);
						translateTextY.push_back(0);
						damageTimer.push_back(elapsedTime);
						damageEnemy.push_back(true);
						/*	cSoundController->StopSoundByID(5);*/
						cSoundController->PlaySoundByID(5);
					}
				}
			}

			if (bullet->type == GameObject::GO_FIRE)
			{
				if (fireTimer <= 0)
				{
					if (target->type == GameObject::GO_BOSS)
					{
						int damageDealt = round(basicBulletDamage * 2 * Math::RandFloatMinMax(0.7, 1.5));
						target->hp -= damageDealt;
						displayDamage.push_back(damageDealt);
						damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
						damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
						scaleText.push_back(0);
						translateTextY.push_back(0);
						damageTimer.push_back(elapsedTime);
						damageEnemy.push_back(true);
						cSoundController->PlaySoundByID(5);
					}
					else
					{
						int damageDealt = round(basicBulletDamage * Math::RandFloatMinMax(0.7, 1.5));
						target->hp -= damageDealt;
						displayDamage.push_back(damageDealt);
						damageTextX.push_back((target->pos.x - camera.position.x + Math::RandFloatMinMax(-5, 5)) * 80 / (192 * (m_worldHeight / (4 * 100))));
						damageTextY.push_back((target->pos.y - camera.position.y + Math::RandFloatMinMax(-5, 5)) * 65 / (100 * (m_worldHeight / (4 * 100))));
						scaleText.push_back(0);
						translateTextY.push_back(0);
						damageTimer.push_back(elapsedTime);
						damageEnemy.push_back(true);
					}
					cSoundController->PlaySoundByID(14);
				}
			}

			// Asteroid HP reaches 0
			if (target->hp <= 0)
			{
				target->active = false;
				// Money gained
				if (target->type != GameObject::GO_ENEMYBULLET && target->type != GameObject::GO_TREE)
				{
					if (target->type == GameObject::GO_BOSS)
					{
						bossspawned = false;
					}
					// Money gained
					enemycount--;
					killcount++;
					m_money += 1 + bonusMoney;
					if (target->type == GameObject::GO_NIGHTMARE)
					{
						for (int i = 0; i < 4; ++i)
						{
							GameObject* go = FetchGO();
							go->type = GameObject::GO_GHOST;
							go->hp = round(1 * hpFactor);
							go->scale.Set(10, 10, 1);
							go->pos.Set(target->pos.x + Math::RandFloatMinMax(-10, 10), target->pos.y + Math::RandFloatMinMax(-10, 10), go->pos.z);
							go->hitboxSizeDivider = 3.5;
							go->enemyDamage = 2;
							go->angle = 0;
							go->maxHP = go->hp;
						}
					}

					if (target->type == GameObject::GO_BOSS)
					{
						tempSpawnCount--;
					}

					//*** Worm Segment seperation code*********************************
					if (target->type == GameObject::GO_WORMHEAD)
					{
						if (target->nextNode != nullptr)
						{
							target->nextNode->type = GameObject::GO_WORMHEAD;
							target->nextNode->prevNode = nullptr;
							target->prevNode = nullptr;
							target->nextNode->enemyDamage = 35;
							target->nextNode->hitboxSizeDivider = 1.8;
						}
					}
					else if (target->type == GameObject::GO_WORMBODY1 || target->type == GameObject::GO_WORMBODY2)
					{
						if (target->nextNode != nullptr)
						{
							target->nextNode->type = GameObject::GO_WORMHEAD;
							target->nextNode->prevNode = nullptr;
							target->nextNode->timer = 10;
							target->nextNode->enemyDamage = 35;
							target->nextNode->hitboxSizeDivider = 1.8;
						}

						if (target->prevNode != nullptr)
						{
							target->prevNode->type = GameObject::GO_WORMTAIL;
							target->prevNode->nextNode = nullptr;
							target->prevNode->enemyDamage = 20;
						}
					}
					else if (target->type == GameObject::GO_WORMTAIL)
					{
						if (target->prevNode != nullptr)
						{
							target->prevNode->type = GameObject::GO_WORMTAIL;
							target->prevNode->nextNode = nullptr;
							target->prevNode->enemyDamage = 20;
						}
					}


					// Drop  Item
					int random = rand() % 18;
					if (random == 0)
					{
						float maxVel = 0.8;

						GameObject* go3 = FetchGO();
						go3->type = GameObject::GO_TRIPLESHOT;
						go3->vel = 0;
						go3->pos.Set(target->pos.x, target->pos.y, 8);
						go3->scale.Set(7, 7, 1);
						go3->enemyDamage = 0;
						go3->hitboxSizeDivider = 3;

					}
					else if (random < 3)
					{
						float maxVel = 0.8;

						GameObject* go3 = FetchGO();
						go3->type = GameObject::GO_HEAL;
						go3->vel = 0;
						go3->pos.Set(target->pos.x, target->pos.y, target->pos.z);
						go3->scale.Set(5, 5, 1);
						go3->enemyDamage = -5;
						go3->hitboxSizeDivider = 3;
					}
				}
			}
		}
	}
}

float Assignment1::CalculateAdditionalForce(GameObject* go1, GameObject* go2)
{
	float radiusSquared = go1->pos.DistanceSquared(go2->pos);
	return (GRAVITY_CONSTANT * go1->mass * go2->mass) / radiusSquared;

}

void Assignment1::RenderGO(GameObject* go)
{
	float diff = elapsedTime - go->prevEnemyBullet;
	go->previousPos = go->pos;

	float renderColor;
	if (go->isHit == true)
	{
		renderColor = 100.f;
		go->isHit = false;
	}
	else
	{
		renderColor = 1.f;
	}

	switch (go->type)
	{
	case GameObject::GO_HERO:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 9);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);


		if (Application::IsMousePressed(0))
		{
			if (isAlive)
			{
				// Attack Animations
				if (worldPosX < m_ship->pos.x)
				{
					modelStack.PushMatrix();
					modelStack.Rotate(180, 0, 0, 1);
					modelStack.Scale(3, 1, 1);
					RenderMesh(meshList[GEO_HEROATTACK_LEFT], true);
					modelStack.PopMatrix();
				}
				else
				{
					modelStack.PushMatrix();
					modelStack.Scale(3, 1, 1);
					RenderMesh(meshList[GEO_HEROATTACK], true);
					modelStack.PopMatrix();
				}
			}
		}
		else
		{
			// Running Animations
			if (isAlive)
			{
				if (Application::IsKeyPressed('W') ||
					Application::IsKeyPressed('A') ||
					Application::IsKeyPressed('S') ||
					Application::IsKeyPressed('D'))
				{
					// Idle Animations
					if (heroFacingLeft == true)
					{
						modelStack.PushMatrix();
						modelStack.Rotate(180, 0, 0, 1);
						modelStack.Scale(2, 1, 1);
						RenderMesh(meshList[GEO_HERORUN_LEFT], true);
						modelStack.PopMatrix();
					}
					else
					{
						modelStack.PushMatrix();
						modelStack.Scale(2, 1, 1);
						RenderMesh(meshList[GEO_HERORUN], true);
						modelStack.PopMatrix();
					}
				}
				else
				{
					// Idle Animations
					if (heroFacingLeft == true)
					{
						modelStack.PushMatrix();
						modelStack.Rotate(180, 0, 0, 1);
						RenderMesh(meshList[GEO_HERO_LEFT], true);
						modelStack.PopMatrix();
					}
					else
					{
						RenderMesh(meshList[GEO_HERO], true);
					}
				}
			}
			else
			{
				modelStack.PushMatrix();
				modelStack.Scale(1.5, 1, 1);
				RenderMesh(meshList[GEO_HERODEATH], true);
				modelStack.PopMatrix();
			}
		}


		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (m_ship->hp / m_ship->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 1, 0.1);
			modelStack.Scale(go->scale.x * 0.002, go->scale.y * 0.003, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1., 0.2);
			modelStack.Scale(go->scale.x * 0.002 * greenHealthPercent, go->scale.y * 0.03, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();

			//modelStack.PushMatrix();
			//modelStack.Translate(0, 1., 0.2);
			//modelStack.Scale(go->scale.x * 0.002 * greenHealthPercent, go->scale.y * 0.03, go->scale.z);
			//RenderMesh(meshList[GEO_PLAYERHEALTH], false);
			//modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;
	case GameObject::GO_GHOST:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_GHOST_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_GHOST]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_GHOST_LEFT], true);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_GHOST], true);
			modelStack.PopMatrix();
		}

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.1);
			modelStack.Scale(go->scale.x * 0.08, go->scale.y * 0.015, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.2);
			modelStack.Scale(go->scale.x * 0.0008 * greenHealthPercent, go->scale.y * 0.015, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BDEMON:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);

		// Rotate to player
		modelStack.PushMatrix();
		go->angle = atan2(m_ship->pos.y - go->pos.y, m_ship->pos.x - go->pos.x);
		go->angle = (go->angle / Math::PI) * 180.0 - 90.0f;
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		meshList[GEO_BDEMON_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_BDEMON]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_BDEMON_LEFT], true);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_BDEMON], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();


		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.3, 1);
			modelStack.Scale(go->scale.x * 0.6, go->scale.y * 0.13, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.3, 1.1);
			modelStack.Scale(go->scale.x * 0.006 * greenHealthPercent, go->scale.y * 0.13, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FLAMEDEMON:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);

		// Rotate to player
		modelStack.PushMatrix();

		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		meshList[GEO_FDEMON_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_FDEMON]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_FDEMON_LEFT], true);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_FDEMON], true);
			modelStack.PopMatrix();
		}

		modelStack.PopMatrix();

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 8, 1);
			modelStack.Scale(go->scale.x * 0.6, go->scale.y * 0.13, go->scale.z + 100);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 8, 1.1);
			modelStack.Scale(go->scale.x * 0.006 * greenHealthPercent, go->scale.y * 0.13, go->scale.z + 100);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_NIGHTMARE:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		meshList[GEO_NIGHTMARE_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_NIGHTMARE]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_NIGHTMARE_LEFT], true);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_NIGHTMARE], true);
			modelStack.PopMatrix();
		}

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0);
			modelStack.Scale(go->scale.x * 0.04, go->scale.y * 0.008, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.1);
			modelStack.Scale(go->scale.x * 0.0004 * greenHealthPercent, go->scale.y * 0.008, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_EXPLODER:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		meshList[GEO_EXPLODER_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_EXPLODER]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_EXPLODER_LEFT], true);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_EXPLODER], true);
			modelStack.PopMatrix();
		}

		RenderMesh(meshList[GEO_EXPLODER], false);
		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.1);
			modelStack.Scale(go->scale.x * 0.08, go->scale.y * 0.015, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.2);
			modelStack.Scale(go->scale.x * 0.0008 * greenHealthPercent, go->scale.y * 0.015, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_WORMHEAD:
	case GameObject::GO_WORMBODY1:
	case GameObject::GO_WORMBODY2:
	case GameObject::GO_WORMTAIL:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);

		meshList[GEO_WORMHEAD]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_WORMBODY1]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_WORMBODY2]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_WORMTAIL]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		// Rotate to player
		modelStack.PushMatrix();
		if (go->type == GameObject::GO_WORMHEAD)
		{
			if (go->timer > 4)
			{
				go->angle = atan2(m_ship->pos.y - go->pos.y, m_ship->pos.x - go->pos.x);
				go->angle = (go->angle / Math::PI) * 180.0 - 90.0f;
			}
		}
		else
		{
			if (go->prevNode != nullptr)
			{
				go->angle = atan2(go->prevNode->pos.y - go->pos.y, go->prevNode->pos.x - go->pos.x);
				go->angle = (go->angle / Math::PI) * 180.0 - 90.f;
			}
		}

		modelStack.Rotate(go->angle, 0, 0, 1);

		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		if (go->type == GameObject::GO_WORMHEAD)
		{
			RenderMesh(meshList[GEO_WORMHEAD], true);
		}
		else if (go->type == GameObject::GO_WORMBODY1)
		{
			RenderMesh(meshList[GEO_WORMBODY1], true);
		}
		else if (go->type == GameObject::GO_WORMBODY2)
		{
			RenderMesh(meshList[GEO_WORMBODY2], true);
		}
		else if (go->type == GameObject::GO_WORMTAIL)
		{
			RenderMesh(meshList[GEO_WORMTAIL], true);
		}
		modelStack.PopMatrix();


		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, -5, 1);
			modelStack.Scale(go->scale.x * 0.4, go->scale.y * 0.13, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, -5, 1.1);
			modelStack.Scale(go->scale.x * 0.004 * greenHealthPercent, go->scale.y * 0.13, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;


	case GameObject::GO_SHOP:
		cout << go->timer << endl;
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		
		if (go->facingLeft == true)
		{
			RenderMesh(meshList[GEO_SHREK_LEFT], false);
		}
		else
		{
			modelStack.PushMatrix();
			modelStack.Rotate(180, 0, 0, 1);
			RenderMesh(meshList[GEO_SHREK], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SHOP_POINTER:
		modelStack.PushMatrix();
		go->pos = m_ship->pos;
		go->pos.y = m_ship->pos.y - 10;
		go->angle = atan2(shopposy - go->pos.y, shopposx - go->pos.x);
		go->angle = (go->angle / Math::PI) * 180.0 - 90.0f;
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHREK_POINTER], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BOSS:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		meshList[GEO_BOSSATTACK_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_BOSSATTACK]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_BOSS_LEFT]->material.kAmbient.Set(renderColor, renderColor, renderColor);
		meshList[GEO_BOSS]->material.kAmbient.Set(renderColor, renderColor, renderColor);

		if (bossState == 1)
		{
			modelStack.PushMatrix();
			if (go->facingLeft == true)
			{
				modelStack.PushMatrix();
				modelStack.Rotate(180, 0, 0, 1);
				RenderMesh(meshList[GEO_BOSSATTACK_LEFT], true);
				modelStack.PopMatrix();
			}
			else
			{
				RenderMesh(meshList[GEO_BOSSATTACK], true);
			}
			modelStack.PopMatrix();
		}
		else
		{
			if (go->facingLeft == true)
			{
				modelStack.PushMatrix();
				modelStack.Rotate(180, 0, 0, 1);
				RenderMesh(meshList[GEO_BOSS_LEFT], true);
				modelStack.PopMatrix();

			}
			else
			{
				RenderMesh(meshList[GEO_BOSS], true);
			}
		}


		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0);
			modelStack.Scale(go->scale.x * 0.03, go->scale.y * 0.003, go->scale.z + 10);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.5, 0.1);
			modelStack.Scale(go->scale.x * 0.0003 * greenHealthPercent, go->scale.y * 0.003, go->scale.z + 10);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}

		if (bossspawned == true)
		{
			if (go->active && go->hp != 0)
			{

				RenderTextOnScreen(meshList[GEO_TEXT], "NightBorne", Color(0, 0, 0), 2, 33, 53, false);

				RenderMeshOnScreen(meshList[GEO_HEALTHBORDER], 97, 85, 65, 6);
				RenderMeshOnScreen(meshList[GEO_HEALTHBACK], 97, 85, 65, 6);
				RenderMeshOnScreen(meshList[GEO_BOSSHEALTH], 97, 85, 65 * ((go->hp / go->maxHP)), 6);
				std::ostringstream ss;

				ss << "" << go->hp;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 38, 50, false);
			}
		}

		modelStack.PopMatrix();
		break;
	case GameObject::GO_LASER:
	case GameObject::GO_ENEMYBULLET:
		modelStack.PushMatrix();

		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(go->angle, 0, 0, 1);
		if (go->type == GameObject::GO_LASER)
		{
			go->scale.x = 15;
			go->scale.y = 15;
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_LASER], false);
		}
		else
		{
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_ENEMYBULLET], false);
		}
		modelStack.PopMatrix();

		break;

	case GameObject::GO_TREE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TREE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CHEST:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CHEST], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CHEST_PARTICLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CHEST_PARTICLE], false);
		modelStack.PopMatrix();
		break;


	case GameObject::GO_BULLET:
		go->angle += 5;
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_PRIMARY], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_HEAL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_HEAL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TRIPLESHOT:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TRIPLESHOT], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CARD:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CARDS], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_RING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_RING], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_RINGAURA:
		go->pos = m_ship->pos;
		go->scale.Set(14, 14, 1);
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x + ringAOE, go->scale.y + ringAOE, go->scale.z);
		RenderMesh(meshList[GEO_RINGAURA], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BOMB:
		go->angle += 2.5;
		go->vel.y += gravity * m_speed;
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BOMB], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_MOLOTOV:
		go->angle += 2.5;
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MOLOTOV], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FIRE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FIRE], false);
		modelStack.PopMatrix();
		break;


	case GameObject::GO_EXPLOSION:
	case GameObject::GO_ENEMYEXPLOSION:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x + go->explosionScale, go->scale.y + go->explosionScale, go->scale.z);
		if (go->type == GameObject::GO_EXPLOSION)
		{
			RenderMesh(meshList[GEO_EXPLOSION], false);
		}
		else if (go->type == GameObject::GO_ENEMYEXPLOSION)
		{
			EnemyExplosionSprite->Update(deltaTime);
			RenderMesh(meshList[GEO_ENEMYEXPLOSION], false);
		}
		modelStack.PopMatrix();

		// Scale the explosion effect
		if (go->scaleDown == false)
		{
			go->explosionScale += 0.2;
			if (go->explosionScale > 4)
			{
				go->scaleDown = true;
			}
		}

		if (go->scaleDown == true)
		{
			go->explosionScale -= 0.2;
			if (go->explosionScale <= 0)
			{
				go->active = false;
			}
		}
		break;

	case GameObject::GO_ARROW:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ARROW], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FLAMINGARROW:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FLAMINGARROW], false);
		modelStack.PopMatrix();
		break;




		//case GameObject::GO_WHITEHOLE:
		//	modelStack.PushMatrix();
		//	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		//	modelStack.Rotate(go->angle + 90, 0, 0, 1);
		//	modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		//	RenderMesh(meshList[GEO_WHITEHOLE], false);
		//	modelStack.PopMatrix();
		//	break;
	case GameObject::GO_BLACKHOLE:
		go->pos = m_ship->pos;
		break;
	}


}

void Assignment1::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::ostringstream ss;
	switch (menuType)
	{
	case M_MAIN:
		RenderMainMenu();
		break;
	case M_PAUSE:
		RenderPauseMenu();
		break;
	case M_GAMEOVER:
		RenderGameOver();
		{
			cSoundController->StopSoundByID(1);
			cSoundController->StopSoundByID(2);
			cSoundController->StopSoundByID(3);
			cSoundController->StopSoundByID(4);
			cSoundController->StopSoundByID(7);
			cSoundController->StopSoundByID(8);
			cSoundController->StopSoundByID(13);

			RenderMeshOnScreen(meshList[GEO_HEALTHBACK], 25, 85, 35, 12);
			RenderMeshOnScreen(meshList[GEO_WAVEBORDER], 25, 93, 35, 12);

			std::ostringstream ss;
			ss.str("");
			ss << "Kills:" << killcount;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 1.5, 5, 50, false);

			ss.str("");
			ss << "Wave:" << waveCount;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 5, 55, false);

		}
		break;
	case M_CHOOSE:
		RenderChoose();
		break;
	case M_ARROW:
		RenderCArrow();
		break;
	case M_BOMB:
		RenderCBomb();
		break;
	case M_CARD:
		RenderCCard();
		break;
	case M_CONTROL:
		RenderControl();
		break;
	case M_UPGRADE:
		RenderUpgrade();
		cSoundController->StopSoundByID(3);
		cSoundController->StopSoundByID(4);
		cSoundController->StopSoundByID(13);
		cSoundController->PlaySoundByID(2);

		RenderMeshOnScreen(meshList[GEO_INFOBORDER], 150, 85, 40, 10);
		ss.str("");
		ss << "$:" << m_money;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 55, 50, false);



		if (fireratelvl < 9)
		{

			ss.str("");
			ss << "LVL" << fireratelvl << " Fire Rate Up";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 22, false);

			ss.str("");
			ss << "$" << fireRateCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 22, false);

		}
		else
		{
			ss.str("");
			ss << "LVL" << fireratelvl << " Fire Rate Up";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 22, false);

			ss.str("");
			ss << "$SOLD";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 22, false);
		}

		if (damagelvl < 9)
		{
			ss.str("");
			ss << "LVL" << damagelvl << " Damage Up";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 31, false);

			ss.str("");
			ss << "$" << damageUpCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 31, false);

		}
		else
		{
			ss.str("");
			ss << "LVL" << damagelvl << " Damage Up";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 31, false);

			ss.str("");
			ss << "$SOLD";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 31, false);

		}

		if (healthRegenCost < 110)
		{
			ss.str("");
			ss << "LVL" << regenlvl << " Health regen";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 40.5, false);

			ss.str("");
			ss << "$" << healthRegenCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 40.5, false);
		}
		else
		{

			if (regenlvl >= 9)
			{
				ss.str("");
				ss << "LVL" << regenlvl << " Regen amount";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 40.5, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 40.5, false);

			}
			else
			{
				ss.str("");
				ss << "LVL" << regenlvl << " Regen amount";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 40.5, false);

				ss.str("");
				ss << "$" << healthRegenCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 40.5, false);
			}
		}


		if (cardCost < 25)
		{
			ss.str("");
			ss << "LVL" << cardlvl << " Card";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 40.5, false);

			ss.str("");
			ss << "$" << cardCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 40.5, false);
		}
		else
		{

			if (cardlvl >= 9)
			{
				ss.str("");
				ss << "LVL" << cardlvl << " Card FireRate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 40.5, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 40.5, false);

			}
			else
			{
				ss.str("");
				ss << "LVL" << cardlvl << " Card FireRate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 40.5, false);

				ss.str("");
				ss << "$" << cardCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 40.5, false);
			}
		}

		if (ringCost < 275)
		{

			ss.str("");
			ss << "LVL" << ringlvl << " Protection";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 13, false);

			ss.str("");
			ss << "$" << ringCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 13, false);
		}
		else
		{
			if (ringlvl >= 5)
			{

				ss.str("");
				ss << "LVL" << ringlvl << " Protection Range";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 47.5, 13, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 71, 13, false);
			}
			else
			{


				ss.str("");
				ss << "LVL" << ringlvl << " Protection Range";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 47.5, 13, false);

				ss.str("");
				ss << "$" << ringCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 71, 13, false);
			}
		}

		if (bombCost < 55)
		{

			ss.str("");
			ss << "LVL" << bomblvl << " Lobing Bomb";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 22, false);

			ss.str("");
			ss << "$" << bombCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 22, false);
		}
		else
		{
			if (bomblvl >= 9)
			{

				ss.str("");
				ss << "LVL" << bomblvl << " Bomb Fire Rate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 47.5, 22, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 71, 22, false);

			}

			else
			{
				ss.str("");
				ss << "LVL" << bomblvl << " Bomb Fire Rate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 47.5, 22, false);

				ss.str("");
				ss << "$" << bombCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 71, 22, false);
			}

		}

		if (molotovCost < 55)
		{

			ss.str("");
			ss << "LVL" << molotovlvl << " Molotov cocktail";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 47.5, 31, false);

			ss.str("");
			ss << "$" << molotovCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 31, false);
		}
		else
		{

			if (molotovlvl <= 3)
			{

				ss.str("");
				ss << "LVL" << molotovlvl << " Add Molotov";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 47.5, 31, false);

				ss.str("");
				ss << "$" << molotovCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 71, 31, false);
			}

			else if (molotovlvl <= 8 && molotovlvl >= 4)
			{
				ss.str("");
				ss << "LVL" << molotovlvl << " Molotov FireRate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.55, 47.5, 31, false);

				ss.str("");
				ss << "$" << molotovCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.55, 71, 31, false);
			}
			else
			{
				ss.str("");
				ss << "LVL" << molotovlvl << " Molotov FireRate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.55, 47.5, 31, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.55, 71, 31, false);
			}

		}

		if (arrowCost < 30)
		{

			ss.str("");
			ss << "LVL" << arrowlvl << " Arrow Shot";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 13, false);

			ss.str("");
			ss << "$" << arrowCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 13, false);
		}
		else
		{

			if (arrowlvl <= 3)
			{
				ss.str("");
				ss << "LVL" << arrowlvl << " Add Arrows";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 9.5, 13, false);

				ss.str("");
				ss << "$" << arrowCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.8, 33.5, 13, false);
			}

			else if (arrowlvl <= 8 && arrowlvl >= 4)
			{
				ss.str("");
				ss << "LVL" << arrowlvl << " Arrow Fire Rate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 9.5, 13, false);

				ss.str("");
				ss << "$" << arrowCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 33.5, 13, false);
			}

			else if (arrowlvl == 9 && molotovlvl == 9 && flamingarrowlvl == 0)
			{

				ss.str("");
				ss << "LVL" << flamingarrowlvl << " Flaming Arrows";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 9.5, 13, false);

				ss.str("");
				ss << "$" << flamingarrowCost;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 33.5, 13, false);
			}

			else if (arrowlvl == 9 && molotovlvl == 9 && flamingarrowlvl == 1)
			{
				ss.str("");
				ss << "LVL" << flamingarrowlvl << " Flaming Arrows";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 9.5, 13, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 33.5, 13, false);

			}

			else
			{

				ss.str("");
				ss << "LVL" << arrowlvl << "  Arrow Fire Rate";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 9.5, 13, false);

				ss.str("");
				ss << "$SOLD";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 33.5, 13, false);
			}

		}

		break;
	case M_SHOPCONTROL:
		RenderShopControl();
		break;
	case M_BOSSCONTROL:
		RenderBossControl();
		break;
	}

	if (menuType != M_NONE)
		return;

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth / 4, 0, m_worldHeight / 4, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	//RenderMesh(meshList[GEO_AXES], false);

	// Render Background (Multi-tile)
	for (float rows = 1; rows < 8; rows++)
	{
		for (float cols = 1; cols < 8; cols++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(144 * rows, 75 * cols, -5);
			modelStack.Scale(288, 150, 1);
			RenderMesh(meshList[GEO_BACKGROUND], false);
			modelStack.PopMatrix();
		}
	}





	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->pos.x < m_ship->pos.x + (m_ship->pos.x - camera.position.x)
				&& go->pos.x > camera.position.x
				&& go->pos.y < m_ship->pos.y + (m_ship->pos.y - camera.position.y)
				&& go->pos.y > camera.position.y)
			{
				RenderGO(go);
			}
		}
	}


	RenderGO(m_ship);


	// Upgrade information
	if (!gameStart)
	{

		// Enter to begin game
		switch (storystate)
		{
		case 1:

			RenderMeshOnScreen(meshList[GEO_CUBE], 100, 65, 100, 70);
			RenderMeshOnScreen(meshList[GEO_EVIL], 97.5, 65, 120, 60);

			ss.str("");
			ss << "Long ago in a distant land, I Aku the shape shifting master of darkness";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 4, 15, false);

			ss.str("");
			ss << "unleashed an unspeakable evil,";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 4, 12, false);
			ss.str("");
			ss << "Press [SPACEBAR] to continue";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 0.75, 65, 1, false);
			break;

		case 2:

			RenderMeshOnScreen(meshList[GEO_CUBE], 100, 65, 100, 70);
			RenderMeshOnScreen(meshList[GEO_PLAYER], 97.5, 65, 120, 60);

			ss.str("");
			ss << "but a foolish knight with a heart of gold stepped forth to oppose me!";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 4, 15, false);

			ss.str("");
			ss << "Press [SPACEBAR] to continue";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 0.75, 65, 1, false);
			break;

		case 3:
			RenderMeshOnScreen(meshList[GEO_CUBE], 100, 65, 100, 70);
			RenderMeshOnScreen(meshList[GEO_MAFIASHREK], 97.5, 65, 120, 60);

			ss.str("");
			ss << "The foolish knight first starts with the swamp where shrek resides,";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 4, 15, false);

			ss.str("");
			ss << "defending it together with him from my forces of evil.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 1.5, 4, 12, false);

			ss.str("");
			ss << "Press [SPACEBAR] to continue";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 0.75, 65, 1, false);
			break;

		case 4:

			gameStart = true;
			break;
		}		
	}
	if (isAlive && upgradescreen == false && gameStart)
	{

		for (int it = 0; it != damageTimer.size(); ++it)
		{
			ss.str("");
			ss << displayDamage.at(it);

			if (damageEnemy.at(it) == true) // Display enemy damage taken
			{
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 0), 2 * scaleText.at(it), damageTextX.at(it) + 0.2, damageTextY.at(it) + 0.2, true);
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2 * scaleText.at(it), damageTextX.at(it), damageTextY.at(it), true);
			}
			else // Display player damage taken
			{
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 0), 3, damageTextX.at(it) + 0.2, damageTextY.at(it) + 0.2, false);
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 3, damageTextX.at(it), damageTextY.at(it), false);
			}


			damageTextY.at(it) += 0.08;
			double diff = elapsedTime - damageTimer.at(it);

			if (scaleText.at(it) < 1)
			{
				scaleText.at(it) += 0.05;
				if (scaleText.at(it) > 1)
				{
					scaleText.at(it) = 1;
				}
			}

			if (diff > 0.4)
			{
				displayDamage.erase(displayDamage.begin() + it);
				damageTextX.erase(damageTextX.begin() + it);
				damageTextY.erase(damageTextY.begin() + it);
				translateTextY.erase(translateTextY.begin() + it);
				scaleText.erase(scaleText.begin() + it);
				damageTimer.erase(damageTimer.begin() + it);
				damageEnemy.erase(damageEnemy.begin() + it);

				if (it >= damageTimer.size())
				{
					break;
				}
			}
		}


		if (m_ship->hp >= 50)
		{
			RenderMeshOnScreen(meshList[GEO_INFOBORDER], 38, 10, 80, 30);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_INFOBORDERRED], 38, 10, 80, 30);
		}
		RenderMeshOnScreen(meshList[GEO_HEALTHBORDER], 44, 14, 60, 7);
		RenderMeshOnScreen(meshList[GEO_HEALTHBACK], 44, 14, 60, 7);
		RenderMeshOnScreen(meshList[GEO_PLAYERHEALTH], 44 - ((100 - m_ship->hp) * 0.29), 14, 60 * (m_ship->hp / 100), 7);

		RenderMeshOnScreen(meshList[GEO_HEROICON], 7, 12, 11, 11);

		ss.str("");
		ss << "" << killcount;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 1.2, 75, 58, false);

		//ss.str("");
		//ss << "EC:" << enemycount;
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 1.2, 0, 48, false);

		ss.str("");
		ss << "$: " << m_money;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 6, 3.6, false);

		ss.str("");
		ss << m_ship->hp;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.6, 27, 7.4, false);

		//ss.str("");
		//ss.precision(5);
		//ss << "FPS: " << fps;
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1.2, 0, 58, false);


		// Wave Count Display
		ss.str("");
		ss << "Wave:" << waveCount;

		if (waveCount != 5 && waveCount != 10 && waveCount != 15
			&& waveCount != 20 && waveCount != 25 && waveCount > !25)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0.5, 0), 2.5, 35, 57, false);
		}
		//Put Emphasis On Special Waves
		else if (waveCount == 5 || waveCount == 10 || waveCount == 15
			|| waveCount == 20 || waveCount == 25 || waveCount > 25)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 3.5, 33, 55, false);
		}
	}
	else if (!isAlive && gameStart && gameOverTimer <= 0)
	{
		SceneBase::menuType = M_GAMEOVER;
		cSoundController->PlaySoundByID(9);
	}


}


void Assignment1::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject* go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}

static Vector3 RotateVector(const Vector3& vec, float radian)
{
	return Vector3(vec.x * cos(radian) + vec.y * -sin(radian),
		vec.x * sin(radian) + vec.y * cos(radian), 0.f);
}
