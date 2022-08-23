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
		GO_BOSSHEALTH,

		GO_WORMHEAD,
		GO_WORMBODY1,
		GO_WORMBODY2,
		GO_WORMTAIL,
		
		GO_GHOST,
		GO_FLAMEDEMON,
		GO_BDEMON,
		GO_NIGHTMARE,

		GO_BLACKHOLE,
		GO_WHITEHOLE,

		GO_SHIP,

		GO_HERO,
		GO_SHOP,
		GO_HEAL,
		GO_CHEST,
		GO_HEROHEALTH,

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
		GO_ARROW,
		GO_FLAMINGARROW,
		GO_TRIPLESHOT,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 direction;
	Vector3 targetPos;
	bool active;
	float mass;
	float angle;
	float hp;
	float maxHP;
	int moneyDrop;
	float enemyDamage;
	float speedFactor;
	float prevEnemyBullet;
	float explosionScale;
	float hitboxSizeDivider;
	float timer;
	bool scaleDown;
	bool facingLeft;
	bool reachTarget;

	GameObject* nextNode;
	GameObject* prevNode;

	float momentOfInertia;
	float angularVelocity;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif