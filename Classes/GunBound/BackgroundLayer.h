#pragma once
#include "cocos2d.h"

USING_NS_CC;

class BackgroundLayer : public Layer
{
public:
	static BackgroundLayer* create();
	virtual bool init() override;

	void moveByCharacterPosition(Vec2 characterWPos);

};

