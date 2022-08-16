#include "Assignment1.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
#include <cmath>

const float Assignment1::ROTATION_SPEED = 5.f;
const float Assignment1::MAX_ROTATION_SPEED = 0.7f;
const float Assignment1::GRAVITY_CONSTANT = 1.f;

Assignment1::Assignment1()
{
}

Assignment1::~Assignment1()
{
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

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	shipSpeed = 13.f;
	prevElapsedAsteroid = prevElapsedBullet = elapsedTime = waveTimer = prevElapsedMissle = keyDelay = prevHealthRegen = tripleShotTimer = 0.0;
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i != 100; i++)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}

	//Exercise 2b: Initialize m_hp and m_score
	m_hp= 100;
	m_money = 0;
	m_objectCount = 0;
	waveCount = 1;

	hpFactor = moneyFactor = 1;
	bonusMoney = 1;

	fireRate = 5;
	fireRateCost = 10;
	damageUpCost = 10;
	missleCost = 20;
	healthRegenCost = 20;


	basicBulletDamage = 1;
	healthRegen = 0;
	healthRegenAmount = 0;
	missleRate = 1;

	doubleBullet = false;
	tripleShot = false;
	upgradeScreen = false;
	isAlive = true;
	gameStart = false;

	movementLastPressed = ' ';

	asteroidCount = 0;
	maxEnemyCount = 10;


	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale.Set(7, 7, 1);
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

	m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;

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
	for (int i = 0; i < 10; ++i) {
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}
	m_goList.at(prevSize)->active = true;
	return m_goList.at(prevSize);
}

void Assignment1::Update(double dt)
{
	SceneBase::Update(dt);
	elapsedTime += dt;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();


	// Enter to begin game
	if (!gameStart)
	{
		if (Application::IsKeyPressed(' '))
		{
			gameStart = true;
		}
	}

	// Player ship ugrade screen
	if (upgradeScreen == true && gameStart)
	{
		if (keyDelay > 0)
		{
			keyDelay -= 1.0 * dt;
		}
		else
		{
			if (Application::IsKeyPressed('J') && fireRateCost < 60)
			{
				keyDelay = 0.3;
				if (m_money >= fireRateCost)
				{
					fireRate += 1;
					m_money -= fireRateCost;
					fireRateCost += 10;
				}
			}
			if (Application::IsKeyPressed('K'))
			{
				keyDelay = 0.3;
				if (m_money >= damageUpCost)
				{
					m_money -= damageUpCost;
					basicBulletDamage++;
					damageUpCost += 15;
				}
			}
			if (Application::IsKeyPressed('L'))
			{
				keyDelay = 0.3;
				if (m_money >= healthRegenCost)
				{
					if (healthRegenCost < 30)
					{
						healthRegen = true;
					}
					healthRegenAmount++;
					m_money -= healthRegenCost;
					healthRegenCost += 25;
				}
			}
			if (Application::IsKeyPressed('N'))
			{
				keyDelay = 0.3;
				if (m_money >= missleCost)
				{
					if (missleCost < 30)
					{
						missleUse = true;
					}
					else
					{
						missleRate += 0.5;
					}
					m_money -= missleCost;
					missleCost += 15;
				}
			}
		}
		// Upgrade ship keys
		
		if (Application::IsKeyPressed('Q'))
		{
			upgradeScreen = false;
		}
	}
	else if (gameStart)
	{
		m_force = Vector3(0,0,0);
		m_ship->vel = Vector3(0, 0, 0);

		if (Application::IsKeyPressed('9'))
		{
			m_speed = Math::Max(0.f, m_speed - 0.1f);
		}
		if (Application::IsKeyPressed('0'))
		{
			m_speed += 0.1f;
		}

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
			//m_force += m_ship->direction * ROTATION_SPEED;
			//m_torque += Vector3(-m_ship->scale.x, m_ship->scale.y, 0).Cross(Vector3(ROTATION_SPEED, 0, 0));
			//if (movementLastPressed == 'A')
			//{
			//	m_ship->angularVelocity = 0;
			//}
			//movementLastPressed = 'D';
		}

		// Access upgrade screen
		if (Application::IsKeyPressed('E'))
		{
			upgradeScreen = true;
		}




		//Exercise 8: use 2 keys to increase and decrease mass of ship
		if (Application::IsKeyPressed('O'))
		{
			m_ship->mass += 1.0f * dt;
			m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x * m_ship->scale.x;

		}
		if (Application::IsKeyPressed('P'))
		{
			//m_ship->mass -= 1.0f * dt;
			//if (m_ship->mass >= 0)
			//	m_ship->mass -= 0.1f;

			if (m_ship->mass >= 0.1f)
				m_ship->mass = 0.1f;
			m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
		}




		// Wave count increases after a certain period
		float diff = elapsedTime - waveTimer;
		if (diff > 15)
		{
			waveCount++;
			hpFactor += 1.05;
			moneyFactor += 1.05;
			bonusMoney++;
			maxEnemyCount++;
			waveTimer = elapsedTime;
		}

		// Spawn asteroids periodically
		diff = elapsedTime - prevElapsedAsteroid;
		if (diff > 1)
			if (diff > 1 && m_objectCount < maxEnemyCount)
			{
				for (int i = 0; i < 1; ++i)
				{
					GameObject* go = FetchGO();
					int randomEnemy = rand() % 100;
					int maxVel;

					// Spawn Enemy Ship
					if (randomEnemy < 10 && waveCount >= 4)
					{
						go->type = GameObject::GO_ENEMYSHIP;
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						go->hp = round(7 * hpFactor);
						go->scale.Set(10, 10, 10);
						go->prevEnemyBullet = 0.0;
						maxVel = 10;
					}
					// Spawn Big Asteroid
					else if (randomEnemy < 15)
					{
						go->type = GameObject::GO_BIGASTEROID;
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						go->hp = round(10 * hpFactor);
						go->scale.Set(20, 20, 1);
						maxVel = 5;
					}
					// Spawn Asteroids
					else
					{
						go->type = GameObject::GO_ASTEROID;
						go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						go->hp = round(1 * hpFactor);
						go->scale.Set(4, 4, 4);
						maxVel = 20;
					}
					go->angle = 0;
					go->maxHP = go->hp;

					// This spawn asteroids from the 4 sides
					int random = rand() % 4;
					switch (random)
					{
					case 0:
						go->pos.Set(m_worldWidth + 1, Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						go->vel.Set(Math::RandFloatMinMax(-maxVel, 0), Math::RandFloatMinMax(-maxVel, maxVel), 0);
						break;
					case 1:
						go->pos.Set(0 - 1, Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
						go->vel.Set(Math::RandFloatMinMax(0, maxVel), Math::RandFloatMinMax(-maxVel, maxVel), 0);
						break;
					case 2:
						go->pos.Set(Math::RandFloatMinMax(-20, 20), m_worldHeight + 1, go->pos.z);
						go->vel.Set(Math::RandFloatMinMax(-maxVel, maxVel), Math::RandFloatMinMax(-maxVel, 0), 0);
						break;
					case 3:
						go->pos.Set(Math::RandFloatMinMax(-20, 20), 0 - 1, go->pos.z);
						go->vel.Set(Math::RandFloatMinMax(-maxVel, maxVel), Math::RandFloatMinMax(0, maxVel), 0);
						break;
					}
					prevElapsedAsteroid = elapsedTime;
					m_objectCount++;
				}
			}

		if (Application::IsKeyPressed('V'))
		{
			for (int i = 0; i < 2; ++i)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_ASTEROID;
				go->pos.Set(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), go->pos.z);
				go->vel.Set(Math::RandFloatMinMax(-20, 20), Math::RandFloatMinMax(-20, 20), 0);
				go->scale.Set(7, 7, 4);
			}
		}

		//Exercise 14: use a key to spawn a bullet
		if (Application::IsKeyPressed(VK_SPACE))
		{
			//Exercise 15: limit the spawn rate of bullets
			float diff = elapsedTime - prevElapsedBullet;
			if (diff > 1 / fireRate)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_BULLET;
				if (tripleShot)
				{
					go->pos = m_ship->pos;
					go->vel = m_ship->direction * BULLET_SPEED;
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

					GameObject* go = FetchGO();
					go->type = GameObject::GO_BULLET;
					go->pos = m_ship->pos;
					go->pos.y += 4;
					go->pos.z += 1;
					go->vel = m_ship->direction * BULLET_SPEED;
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

					go = FetchGO();
					go->type = GameObject::GO_BULLET;
					go->pos = m_ship->pos;
					go->pos.x += 4;
					go->pos.z += 1;
					go->vel = m_ship->direction * BULLET_SPEED;
					go->scale.Set(4.0f, 4.0f, 4.0f);
					go->angle = m_ship->angle;

				}
				else
				{
					go->pos = m_ship->pos;
					go->vel = m_ship->direction * BULLET_SPEED;
					go->scale.Set(4.0f, 4.0f, 4.0f);
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

		//Physics Simulation Section

		//Exercise 7: Update ship's velocity based on m_force

		Vector3 acceleration = m_force * (1.0f / m_ship->mass);

		//Velocity
		m_ship->vel += acceleration * dt * shipSpeed;
		//Exercise 10: Cap Velocity magnitude (FLOAT) using MAX_SPEED (FLOAT)
		if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
		{
			m_ship->vel.Normalize() *= MAX_SPEED;
		}

		float angleInRadians = std::atan2(m_ship->vel.y, m_ship->vel.x);
		float angleInDegrees = (angleInRadians / Math::PI) * 180.0 - 90.0f;
		m_ship->angle = angleInDegrees;

		m_ship->vel += acceleration * dt * shipSpeed;
		m_ship->pos += m_ship->vel * dt * shipSpeed;

		//float angularAcceleration = m_torque.z / m_ship->momentOfInertia;
		//m_ship->angularVelocity += angularAcceleration * dt * m_speed;
		//m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
		//m_ship->direction = RotateVector(m_ship->direction, m_ship->angularVelocity * dt * shipSpeed);
		//m_ship->angle = Math::RadianToDegree(atan2(m_ship->direction.y, m_ship->direction.x));


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





		// Collision Detection
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				go->pos += go->vel * dt * m_speed;
				//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check

				if (go->type == GameObject::GO_ASTEROID)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3.5 + go->scale.x) * (m_ship->scale.x / 3.5 + go->scale.x);
					if (dis < cRad)
					{
						go->active = false;
						m_objectCount--;
						m_ship->hp -= 2;
					}
					//Exercise 13: asteroids should wrap around the screen like the ship
					Wrap(go->pos.x, m_worldWidth);
					Wrap(go->pos.y, m_worldHeight);
				}

				else if (go->type == GameObject::GO_ENEMYSHIP)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3 + go->scale.x) * (m_ship->scale.x / 3 + go->scale.x);
					if (dis < cRad)
					{
						go->active = false;
						m_objectCount--;
						m_ship->hp -= 5;

					}
					//Exercise 13: asteroids should wrap around the screen like the ship
					Wrap(go->pos.x, m_worldWidth);
					Wrap(go->pos.y, m_worldHeight);
				}


				else if (go->type == GameObject::GO_BIGASTEROID)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3 + go->scale.x / 2) * (m_ship->scale.x / 3 + go->scale.x / 2);
					if (dis < cRad)
					{
						go->active = false;
						m_objectCount--;
						m_ship->hp -= 20;
					}
					//Exercise 13: asteroids should wrap around the screen like the ship
					Wrap(go->pos.x, m_worldWidth);
					Wrap(go->pos.y, m_worldHeight);
				}

				else if (go->type == GameObject::GO_ENEMYBULLET)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3 + go->scale.x) * (m_ship->scale.x / 3 + go->scale.x);
					if (dis < cRad)
					{
						go->active = false;
						m_ship->hp -= 1;
					}

					// Enemy bullet unspawn offscreen
					if (go->pos.x > m_worldWidth
						|| go->pos.x <0
						|| go->pos.y > m_worldHeight
						|| go->pos.y < 0)
					{
						go->active = false;
						continue;
					}
				}

				else if (go->type == GameObject::GO_HEAL)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3 + go->scale.x) * (m_ship->scale.x / 3 + go->scale.x);
					if (dis < cRad)
					{
						go->active = false;
						m_ship->hp += 5;
						if (m_ship->hp > m_ship->maxHP)
						{
							m_ship->hp = m_ship->maxHP;
						}
					}

					// unspawn offscreen
					if (go->pos.x > m_worldWidth
						|| go->pos.x <0
						|| go->pos.y > m_worldHeight
						|| go->pos.y < 0)
					{
						go->active = false;
						continue;
					}
				}

				else if (go->type == GameObject::GO_TRIPLESHOT)
				{
					float dis = go->pos.DistanceSquared(m_ship->pos);
					float cRad = (m_ship->scale.x / 3 + go->scale.x) * (m_ship->scale.x / 3 + go->scale.x);
					if (dis < cRad)
					{
						go->active = false;
						tripleShot = true;
						tripleShotTimer = 6;
					}

					// unspawn offscreen
					if (go->pos.x > m_worldWidth
						|| go->pos.x <0
						|| go->pos.y > m_worldHeight
						|| go->pos.y < 0)
					{
						go->active = false;
						continue;
					}
				}

				//Exercise 16: unspawn bullets when they leave screen
				else if (go->type == GameObject::GO_BULLET || go->type == GameObject::GO_MISSLE)
				{
					if (go->pos.x > m_worldWidth
						|| go->pos.x <0
						|| go->pos.y > m_worldHeight
						|| go->pos.y < 0)
					{
						go->active = false;
						continue;
					}


					//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->type == GameObject::GO_ASTEROID && go2->active)
						{
							float dis = go->pos.DistanceSquared(go2->pos);
							float rad = (go->scale.x + go2->scale.x / 4) * (go->scale.x + go2->scale.x / 4);
							if (dis < rad)
							{
								if (go->type == GameObject::GO_MISSLE)
								{
									go2->hp -= basicBulletDamage;
									GameObject* explosion = FetchGO();
									explosion->type = GameObject::GO_EXPLOSION;
									explosion->pos = go2->pos;
									explosion->scale.Set(1, 1, 1);
									explosion->vel = 0;
									explosion->explosionScale = 0;
									explosion->scaleDown = false;
								}
								go2->hp -= basicBulletDamage;
								go->active = false;

								// Asteroid HP reaches 0
								if (go2->hp <= 0)
								{
									go2->active = false;
									m_objectCount--;
									// Money gained
									m_money += 1 + bonusMoney;

									// Drop  Item
									int random = rand() % 14;
									if (random == 0)
									{
										float maxVel = 0.8;

										GameObject* go3 = FetchGO();
										go3->type = GameObject::GO_TRIPLESHOT;
										go3->vel.Set(Math::RandFloatMinMax(-maxVel, 0), Math::RandFloatMinMax(-maxVel, maxVel), go->pos.z);
										go3->pos.Set(go2->pos.x, go2->pos.y, go2->pos.z);
										go3->scale.Set(9, 9, 1);

									}
									else if (random < 3)
									{
										float maxVel = 0.8;

										GameObject* go3 = FetchGO();
										go3->type = GameObject::GO_HEAL;
										go3->vel.Set(Math::RandFloatMinMax(-maxVel, 0), Math::RandFloatMinMax(-maxVel, maxVel), go->pos.z);
										go3->pos.Set(go2->pos.x, go2->pos.y, go2->pos.z);
										go3->scale.Set(5, 5, 1);
									}
								}
							}

						}
						// Collison with enemy ship
						else if (go2->type == GameObject::GO_ENEMYSHIP && go2->active)
						{
							float dis = go->pos.DistanceSquared(go2->pos);
							float rad = (go->scale.x + go2->scale.x / 4) * (go->scale.x + go2->scale.x / 4);
							if (dis < rad)
							{
								if (go->type == GameObject::GO_MISSLE)
								{
									go2->hp -= basicBulletDamage;
									GameObject* explosion = FetchGO();
									explosion->type = GameObject::GO_EXPLOSION;
									explosion->pos = go2->pos;
									explosion->scale.Set(1, 1, 1);
									explosion->vel = 0;
									explosion->explosionScale = 0;
									explosion->scaleDown = false;
								}
								go2->hp -= basicBulletDamage;
								go->active = false;

								// Asteroid HP reaches 0
								if (go2->hp <= 0)
								{
									go2->active = false;
									m_objectCount--;
									// Money gained
									m_money += 10 + (bonusMoney * 3);

									// Drop  Item
									int random = rand() % 14;
									if (random == 0)
									{
										float maxVel = 0.8;

										GameObject* go3 = FetchGO();
										go3->type = GameObject::GO_TRIPLESHOT;
										go3->vel.Set(Math::RandFloatMinMax(-maxVel, 0), Math::RandFloatMinMax(-maxVel, maxVel), go->pos.z);
										go3->pos.Set(go2->pos.x, go2->pos.y, go2->pos.z);
										go3->scale.Set(9, 9, 1);

									}
									else if (random < 3)
									{
										float maxVel = 0.8;

										GameObject* go3 = FetchGO();
										go3->type = GameObject::GO_HEAL;
										go3->vel.Set(Math::RandFloatMinMax(-maxVel, 0), Math::RandFloatMinMax(-maxVel, maxVel), go->pos.z);
										go3->pos.Set(go2->pos.x, go2->pos.y, go2->pos.z);
										go3->scale.Set(5, 5, 1);
									}
								}
							}
						}
						// Collision with big asteroid
						else if (go2->type == GameObject::GO_BIGASTEROID && go2->active)
						{
							float dis = go->pos.DistanceSquared(go2->pos);
							float rad = (go->scale.x + go2->scale.x / 4) * (go->scale.x + go2->scale.x / 4);
							if (dis < rad)
							{
								if (go->type == GameObject::GO_MISSLE)
								{
									go2->hp -= basicBulletDamage;
									GameObject* explosion = FetchGO();
									explosion->type = GameObject::GO_EXPLOSION;
									explosion->pos = go2->pos;
									explosion->scale.Set(1, 1, 1);
									explosion->vel = 0;
									explosion->explosionScale = 0;
									explosion->scaleDown = false;
								}
								go2->hp -= basicBulletDamage;
								go->active = false;

								// Asteroid HP reaches 0
								if (go2->hp <= 0)
								{
									go2->active = false;
									m_objectCount--;
									for (int i = 0; i < 4; ++i)
									{
										GameObject* go = FetchGO();
										go->type = GameObject::GO_ASTEROID;
										go->hp = round(1 * hpFactor);
										go->scale.Set(4, 4, 4);
										go->pos.Set(go2->pos.x, go2->pos.y, go->pos.z);
										go->vel.Set(Math::RandFloatMinMax(-20, 0), Math::RandFloatMinMax(-20, 20), 0);
									}
									// Money gained
									m_money += 10 + (bonusMoney * 3);
								}
							}
						}
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
								if (m_goList[j]->pos.DistanceSquared(go->pos) < 3600.0f)
								{
									//1 Close Destroy the object (absorb its mass)
									if (m_goList[j]->pos.DistanceSquared(go->pos) < 4.0f)
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
						}
					}
				}
			}
		}

		if (missleUse == true)
		{
			diff = elapsedTime - prevElapsedMissle;
			if (diff > 1 / missleRate)
			{
				GameObject* go = FetchGO();
				go->type = GameObject::GO_MISSLE;
				go->pos = m_ship->pos;
				go->vel = m_ship->direction * BULLET_SPEED;
				go->scale.Set(6.0f, 4.0f, 4.0f);
				go->angle = m_ship->angle;

				prevElapsedMissle = elapsedTime;
			}
		}
		// Homing missle

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active && go->type == GameObject::GO_MISSLE)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->active && go2->type == GameObject::GO_ASTEROID && it != it2 && go2->type)
					{

						float dis = go->pos.DistanceSquared(go2->pos);
						float rad = (go->scale.x + go2->scale.x * 10) * (go->scale.x + go2->scale.x * 10);
						if (dis < rad)
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

		// If health reaches zero
		if (m_ship->hp <= 0)
		{
			isAlive = false;
		}

	}
	
}

float Assignment1::CalculateAdditionalForce(GameObject* go1, GameObject* go2)
{
	float radiusSquared = go1->pos.DistanceSquared(go2->pos);
	return (GRAVITY_CONSTANT * go1->mass * go2->mass) / radiusSquared;

}

void Assignment1::RenderGO(GameObject *go)
{
	float diff = elapsedTime - go->prevEnemyBullet;
	switch (go->type)
	{
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 10);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIP], false);

		modelStack.Rotate(-go->angle, 0, 0, 1);

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (m_ship->hp / m_ship->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 1, 0.1);
			modelStack.Scale(go->scale.x * 0.2, go->scale.y * 0.03, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1., 0.2);
			modelStack.Scale(go->scale.x * 0.002 * greenHealthPercent, go->scale.y * 0.03, go->scale.z);
			RenderMesh(meshList[GEO_PLAYERHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		break;
		//Exercise 17a: render a ship texture or 3D ship model
		//Exercise 17b:	re-orientate the ship with velocity

	case GameObject::GO_ASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.3, 0.1);
			modelStack.Scale(go->scale.x * 0.5, go->scale.y * 0.1, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.3, 0.2);
			modelStack.Scale(go->scale.x * 0.005 * greenHealthPercent, go->scale.y * 0.1, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		//Exercise 4b: render a cube with length 2
		break;

	case GameObject::GO_ENEMYSHIP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);

		// Rotate to player
		modelStack.PushMatrix();
		go->angle = atan2(m_ship->pos.y - go->pos.y , m_ship->pos.x - go->pos.x);
		go->angle = (go->angle / Math::PI) * 180.0 - 90.0f;
		modelStack.Rotate(go->angle, 0, 0, 1);

		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMYSHIP], false);
		modelStack.PopMatrix();


		// Enemy ship shooting at player
		if (diff > 1)
		{
			modelStack.PushMatrix();
			GameObject* go2 = FetchGO();
			go2->type = GameObject::GO_ENEMYBULLET;
			go2->scale.Set(4.0f, 4.0f, 4.0f);
			go2->pos = go->pos;
			go2->angle = go->angle;

			go2->direction = go2->pos - m_ship->pos;
			go2->direction = -go2->direction.Normalized();
			go2->vel = go2->direction * BULLET_SPEED * 0.8;
			go->prevEnemyBullet = elapsedTime;


			modelStack.PopMatrix();
		}

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
		//Exercise 4b: render a cube with length 2
		break;

	case GameObject::GO_BIGASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BIGASTEROID], false);

		// Display health bar if asteroid is damaged
		if (go->hp < go->maxHP)
		{
			float greenHealthPercent = (go->hp / go->maxHP) * 100;
			float redHealthPercent = 100 - greenHealthPercent;

			modelStack.PushMatrix();
			modelStack.Translate(0, 1, 0);
			modelStack.Scale(go->scale.x * 0.05, go->scale.y * 0.01, go->scale.z);
			RenderMesh(meshList[GEO_REDHEALTH], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1, 0.1);
			modelStack.Scale(go->scale.x * 0.0005 * greenHealthPercent, go->scale.y * 0.01, go->scale.z);
			RenderMesh(meshList[GEO_GREENHEALTH], false);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
		//Exercise 4b: render a cube with length 2
		break;

	case GameObject::GO_ENEMYBULLET:
		modelStack.PushMatrix();

	    modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 2);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMYBULLET], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CLUB], false);
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

	case GameObject::GO_MISSLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BOW], false);
		modelStack.PopMatrix();
		break;


	case GameObject::GO_EXPLOSION:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z + 3);
		modelStack.Rotate(go->angle, 0, 0, 1);
		modelStack.Scale(go->scale.x + go->explosionScale, go->scale.y + go->explosionScale, go->scale.z);
		RenderMesh(meshList[GEO_EXPLOSION], false);
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



	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
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
	
	RenderMesh(meshList[GEO_AXES], false);



	//Render background
	modelStack.PushMatrix();
	modelStack.Translate(100, 50, -1);
	modelStack.Scale(200, 150, 1);
	RenderMesh(meshList[GEO_BACKGROUND], false);
	modelStack.PopMatrix();


	if (upgradeScreen)
	{
		modelStack.PushMatrix();
		modelStack.Translate(70, 50, 10);
		modelStack.Scale(100, 100, 1);
		RenderMesh(meshList[GEO_UPGRADESCREEN], false);
		modelStack.PopMatrix();
	}


	if (isAlive)
	{

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				RenderGO(go);
			}
		}


		RenderGO(m_ship);
	}





	//On screen text

	//Exercise 5a: Render m_hp, m_score
	std::ostringstream ss;

	//Exercise 5b: Render position, velocity & mass of ship


	
	//RenderTextOnScreen(meshList[GEO_TEXT], "Asteroid", Color(0, 1, 0), 20, 0, 0);

	// Upgrade information
	if (upgradeScreen)
	{
		// Upgrade information


		ss.str("");
		ss << "UPGRADE MENU";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 30, 50);

		ss.str("");
		ss << "--------------------------";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 16, 47);

		if (fireRateCost < 60)
		{
			ss.str("");
			ss << "[J] Fire Rate Up: $" << fireRateCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 45);

		}
		else
		{
			ss.str("");
			ss << "[J] Fire Rate Up: SOLD";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 45);

		}
		ss.str("");
		ss << "[K] Damage Up: $" << damageUpCost;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 40);

		ss.str("");
		ss << "[L] Health Regen: $" << healthRegenCost;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 35);

		if (missleCost < 25)
		{
			ss.str("");
			ss << "[N] Homing Missle: $" << missleCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 30);
		}
		else
		{
			ss.str("");
			ss << "[N] Missle Fire Rate: $" << missleCost;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 17, 30);
		}

	}

	if (!gameStart)
	{
		ss.str("");
		ss << "Asteroid Shooter";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 45);

		ss.str("");
		ss << "Press [SPACEBAR] to start";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 20);
	}

	if (isAlive  && !upgradeScreen && gameStart)
	{

		//ss.str("");
		//ss << "Fire Power: " << basicBulletDamage << " /s";
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

		//ss.str("");
		//ss << "Fire Rate: " << fireRate << " /s";
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);

		ss.str("");
		ss << "Health: " << m_ship->hp;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

		ss.str("");
		ss << "$:" << m_money;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 0);

		//Exercise 5b: Render position, velocity & mass of ship
		ss.str("");
		ss.precision(5);
		ss << "FPS: " << fps;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 0, 55);

		//RenderTextOnScreen(meshList[GEO_TEXT], "Asteroid", Color(0, 1, 0), 20, 0, 0);


		//// Time Display
		//ss.str("");
		//ss << "Time: " << trunc(elapsedTime);
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 55, 55);

		// Wave Count Display
		ss.str("");
		ss << "Wave: " << waveCount;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 55, 55);

		// For Debugging
		/*ss.str("");
		ss << "Gain: " << trunc(4 * moneyFactor);
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 55, 40);*/
	}
	else if (!isAlive && gameStart)
	{
		ss.str("");
		ss << "GAME OVER";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 5, 20, 40);

		ss.str("");
		ss << "Died at wave " << waveCount;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 20, 30);
	}
}


void Assignment1::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
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