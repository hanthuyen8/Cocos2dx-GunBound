#pragma once

#include "cocos2d.h";

USING_NS_CC;

class GameTerrain: public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(GameTerrain);

private:
	DrawNode* stencil{};
	Sprite* background{};
};

