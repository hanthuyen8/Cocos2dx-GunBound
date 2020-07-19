#pragma once
#include "cocos2d.h"
USING_NS_CC;

class WorldPosition
{
public:
	Vec2 getWPosition();
	void setWPosition(Vec2 wpos);
	CC_SYNTHESIZE_READONLY(Node*, parent, Parent);
};

