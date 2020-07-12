#pragma once

#include "cocos2d.h"
#include <string_view>

USING_NS_CC;

class Dino
{
private:
	const std::pair<int, int> sfRangeIdle{ 0, 3 };
	const std::pair<int, int> sfRangeMove{ 4, 9 };
	const float idleFps{ 1.0f / (sfRangeIdle.second - sfRangeIdle.first + 1) };
	const float moveFps{ 1.0f / (sfRangeMove.second - sfRangeMove.first + 1) };

public:
	Sprite* sprite{ nullptr };
	Animation* idleAnimation{ nullptr };
	Animation* moveAnimation{ nullptr };

	Dino() = default;
	explicit Dino(std::string_view dinoName);
};

