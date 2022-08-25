#ifndef ASSIGNMENT1_H
#define ASSIGNMENT1_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include"SpriteAnimation.h"
#include "SpriteAnimation.h"
#include "System\filesystem.h"
#include"..\Source\SoundController\SoundController.h"

class Assignment1 : public SceneBase
{
	static const int MAX_SPEED = 2;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

	static const float ROTATION_SPEED;
	static const float MAX_ROTATION_SPEED;
	static const float GRAVITY_CONSTANT;
public:
	Assignment1();
	~Assignment1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	float Assignment1::CalculateAdditionalForce(GameObject* go1, GameObject* go2);
	void Collision(GameObject* go1);
	void HitEnemy(GameObject* bullet, GameObject* target);
	void UpdateMenu();
	void RestartGame();
	void SpawnWorm();
	void SpawnBoss();
	GameObject* FetchGO();
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	std::vector<int> displayDamage;
	std::vector<float>damageTextX;
	std::vector<float> damageTextY;
	std::vector<float> translateTextY;
	std::vector<float> scaleText;
	std::vector<double> damageTimer;
	std::vector<bool> damageEnemy;
	CSoundController* cSoundController;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	GameObject* m_boss;

	SpriteAnimation* HeroSprite;
	SpriteAnimation* ShrekSprite;
	SpriteAnimation* ShrekSpriteLeft;
	SpriteAnimation* GhostSprite;
	SpriteAnimation* GhostSpriteLeft;
	SpriteAnimation* FdemonSprite;
	SpriteAnimation* FdemonSpriteLeft;
	SpriteAnimation* FdemonATKSprite;
	SpriteAnimation* BdemonSprite;
	SpriteAnimation* BdemonSpriteLeft;
	SpriteAnimation* NightborneSprite;
	SpriteAnimation* NightborneSpriteLeft;
	SpriteAnimation* ExploderSprite;
	SpriteAnimation* ExploderSpriteLeft;
	SpriteAnimation* EnemyExplosionSprite;
	SpriteAnimation* FireSprite;
	SpriteAnimation* NightmareSprite;
	SpriteAnimation* NightmareSpriteLeft;
	SpriteAnimation* ExplosionSprite;
	SpriteAnimation* BarrierSprite;
	SpriteAnimation* PurpleShot;
	SpriteAnimation* Chestparticlesprite;
	SpriteAnimation* ChestSprite;

	Vector3 m_force;
	float shipSpeed;

	Vector3 m_torque;

	int m_objectCount;
	int waveCount;

	float m_hp;

	// Player variable
	float hpFactor;
	int m_money;
	float moneyFactor;
	double fireRate;
	int fireRateCost;
	int damageUpCost;
	int cardCost;
	int cardlvl;
	int ringCost;
	int ringlvl;
	int bombCost;
	int bomblvl;
	int molotovCost;
	int molotovlvl;
	int molotovAmount;
	int arrowCost;
	int arrowlvl;
	int storystate;
	int arrowAmount;
	int flamingarrowCost;
	int flamingarrowlvl;
	int healthRegenCost;
	int basicBulletDamage;
	int healthRegenAmount;
	float ringAOE;
	float gravity;
	float zoomInFactor;

	double worldPosX;
	double worldPosY;

	int asteroidCount;
	int maxEnemyCount;
	int bonusMoney;

	// Timers
	double deltaTime;
	double prevElapsedAsteroid;
	double prevElapsedArrow;
	double prevElapsedBomb;
	double prevElapsedBullet;
	double prevElapsedCard;
	double prevElapsedMolotov;
	double prevHealthRegen;
	double cardRate;
	double molotovRate;
	double bombRate;
	double arrowRate;
	double flamingarrowRate;
	double keyDelay;
	double elapsedTime;
	double waveTimer;
	double tripleShotTimer;
	double fireTimer;
	double explosionTimer;
	double ringauraTimer;
	double iFrames;
	double chestTimer;

	bool shopactive;
	bool cardUse;
	bool ringUse;
	bool bombUse;
	bool flamingarrowUse;
	bool daggerUse;
	bool arrowUse;
	bool molotovUse;
	bool healthRegen;
	bool doubleBullet;
	bool tripleShot;
	bool isAlive;
	bool upgradeScreen;
	bool ChooseScreen;
	bool gameStart;
	bool heroFacingLeft;

	//enemy
	bool bossspawned;
	// FOR DEBUG ONLY
	int tempSpawnCount;
	int tempWormCount;
	int shootCount;
	int bossState;
	int laserAngle;


	char movementLastPressed;
};


static Vector3 RotateVector(const Vector3& vec, float radian);
#endif