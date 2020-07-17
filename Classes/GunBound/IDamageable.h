#pragma once
#include "cocos2d.h"

class IDamageable
{
public:
	virtual void setDamage(const std::vector<cocos2d::Vec2>& damagedPoints) = 0;
};