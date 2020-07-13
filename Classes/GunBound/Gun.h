#pragma once
#include "cocos2d.h"

USING_NS_CC;

class Gun : public Layer
{
public:
	static Gun* createGun();
	Gun();
	CREATE_FUNC(Gun);

	Vec2 getBarrelPos();
	void setGunRotation(float angle);

private:
	Vec2 barrelPos{};
	Sprite* gunSprite{};
	Sprite* carSprite{};
};

