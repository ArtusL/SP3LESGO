#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,

		// Enemy types
		GO_ASTEROID,
		GO_BIGASTEROID,
		GO_ENEMYSHIP,
		GO_BOSS,
		
		GO_GHOST,
		GO_FLAMEDEMON,
		GO_BDEMON,
		GO_NIGHTMARE,

		GO_BLACKHOLE,
		GO_WHITEHOLE,

		GO_SHIP,

		GO_HERO,
		GO_HEAL,

		GO_BULLET,
		GO_SPLITBULLET,
		GO_ENEMYBULLET,
		GO_LASER,
		GO_MISSLE,
		GO_EXPLOSION,
		GO_RING,
		GO_RINGAURA,
		GO_BOMB,
		GO_MOLOTOV,
		GO_FIRE,
		GO_TRIPLESHOT,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 direction;
	bool active;
	float mass;
	float angle;
	float hp;
	float maxHP;
	float enemyDamage;
	float speedFactor;
	float prevEnemyBullet;
	float explosionScale;
	float ringaurascale;
	float hitboxSizeDivider;
	bool scaleDown;

	float momentOfInertia;
	float angularVelocity;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif