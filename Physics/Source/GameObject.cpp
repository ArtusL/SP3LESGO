
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	enemyDamage(0),
	hitboxSizeDivider(1),
	mass(1.f)
{
}

GameObject::~GameObject()
{
}