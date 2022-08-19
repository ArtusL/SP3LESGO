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
	GameObject* FetchGO();
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	std::vector<int> displayDamage;
	std::vector<float>damageTextX;
	std::vector<float> damageTextY;
	std::vector<float> translateTextY;
	std::vector<double> damageTimer;
	CSoundController* cSoundController;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	SpriteAnimation* HeroSprite;
	SpriteAnimation* GhostSprite;
	SpriteAnimation* FdemonSprite;
	SpriteAnimation* BdemonSprite;
	SpriteAnimation* FireSprite;
	SpriteAnimation* NightmareSprite;
	SpriteAnimation* ExplosionSprite;
	SpriteAnimation* BarrierSprite;
	SpriteAnimation* PurpleShot;
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
	int missleCost;
	int ringCost;
	int misslelvl;
	int ringlvl;
	int bombCost;
	int bomblvl;
	int molotovCost;
	int molotovlvl;
	int molotovAmount;
	int arrowCost;
	int arrowlvl;
	int arrowAmount;
	int flamingarrowCost;
	int flamingarrowlvl;
	int healthRegenCost;
	int basicBulletDamage;
	int healthRegenAmount;
	float ringAOE;
	float gravity;

	double worldPosX;
	double worldPosY;

	int asteroidCount;
	int maxEnemyCount;
	int bonusMoney;

	// Timers
	double prevElapsedAsteroid;
	double prevElapsedArrow;
	double prevElapsedBomb;
	double prevElapsedBullet;
	double prevElapsedMissle;
	double prevElapsedMolotov;
	double prevHealthRegen;
	double missleRate;
	double molotovRate;
	double bombRate;
	double arrowRate;
	double flamingarrowRate;
	double keyDelay;
	double elapsedTime;
	double waveTimer;
	double tripleShotTimer;

	bool missleUse;
	bool ringUse;
	bool bombUse;
	bool flamingarrowUse;
	bool arrowUse;
	bool molotovUse;
	bool healthRegen;
	bool doubleBullet;
	bool tripleShot;
	bool isAlive;
	bool upgradeScreen;
	bool gameStart;
	bool heroFacingLeft;

	// FOR DEBUG ONLY
	int tempSpawnCount;
	int shootCount;
	int bossState;
	int laserAngle;


	char movementLastPressed;
};


static Vector3 RotateVector(const Vector3& vec, float radian);
#endif