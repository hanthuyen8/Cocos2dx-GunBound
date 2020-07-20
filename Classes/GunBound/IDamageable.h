#pragma once
#include "cocos2d.h"

class IDamageable
{
public:
	virtual void receiveDamage(const std::vector<cocos2d::Vec2>& damagedPoints) = 0;
};