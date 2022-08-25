
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	enemyDamage(0),
	hitboxSizeDivider(1),
	isHit(false),
	mass(1.f)
{
}

GameObject::~GameObject()
{
}