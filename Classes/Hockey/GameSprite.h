#pragma once
#include "cocos2d.h"
#include <string_view>

USING_NS_CC;

class GameSprite : public Sprite
{
public:
	static GameSprite* createGameSprite(std::string_view fileName);
	CC_SYNTHESIZE(Touch*, touchFinger, TouchFinger);
};

